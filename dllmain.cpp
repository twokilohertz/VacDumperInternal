#include <Windows.h>
#include <psapi.h>
#include <cstdint>

#include "mem.h"
#include "hooks.h"
#include "minhook/MinHook.h"

fpLoadLibraryExW origFunc;

int setup(HMODULE hModule)
{
	// Get handle to the steamservice.dll module
	HMODULE hServiceDll = GetModuleHandle(L"steamservice.dll");
	if (!hServiceDll)
	{
		FreeLibraryAndExitThread(hModule, 1);
		return 1;
	}

	// Retrieve information about said module, namely the size
	MODULEINFO ModInfo = { 0 };
	GetModuleInformation(GetCurrentProcess(), hServiceDll, &ModInfo, sizeof(ModInfo));
	if (!ModInfo.lpBaseOfDll)
	{
		FreeLibraryAndExitThread(hModule, 1);
		return 1;
	}

	// Byte sequence to search for
	uint8_t bytes[] = { 0x74,0x47,0x6a,0x01,0x6a,00 };

	// Attempt to search for pattern
	void* target = find_pattern(bytes, sizeof(bytes), reinterpret_cast<uintptr_t>(hServiceDll), ModInfo.SizeOfImage);
	if (target == nullptr)
	{
		FreeLibraryAndExitThread(hModule, 1);
		return 1;
	}

	// Patch the jump instruction
	/*
	* 10058eee 74 47           JZ         LAB_10058f37
	* 10058ef0 6a 01           PUSH       0x1
	* 10058ef2 6a 00           PUSH       0x0
	* 
	* We will patch the jz/je to an unconditional jump (jmp/0xEB)
	* so that the steamservice.dll uses LoadLibrary to load the VAC modules
	*/

	write_byte(target, 0xEB);

	// Hook LoadLibrary

	origFunc = nullptr;

	MH_Initialize();
	MH_CreateHookApi(L"kernelbase.dll", "LoadLibraryExW", hooked_LoadLibraryExW, reinterpret_cast<LPVOID*>(&origFunc));
	MH_EnableHook(MH_ALL_HOOKS);

	while (true)
	{
		Sleep(5); // Probably a better way of doing this
	}

	// Done, remove hooks, close handles, exit thread

	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();

	CloseHandle(hServiceDll); // 0xC0000008: An invalid handle was specified - only when debugging - odd!
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved)
{
	DisableThreadLibraryCalls(hModule); // Don't care about DLL_THREAD_ATTACH or DETACH

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)setup, hModule, 0, nullptr);
		if (!hThread) return FALSE;
		CloseHandle(hThread);
	}
	//case DLL_THREAD_ATTACH:
	//case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}