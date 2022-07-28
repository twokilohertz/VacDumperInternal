#include "hooks.h"
#include <Windows.h>
#include <Shlwapi.h>
#include <string>

extern fpLoadLibraryExW origFunc;

HMODULE WINAPI hooked_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE  hFile, DWORD dwFlags)
{
	// Store original LoadLibrary return value
	HMODULE retval = origFunc(lpLibFileName, hFile, dwFlags);

	// If the library doesn't have this exported, don't bother dumping
	// NOTE: Could also check the DOS stub for the "VLV" characters, todo?
	if (!GetProcAddress(retval, "_runfunc@20"))
	{
		return retval;
	}

	wchar_t filename[MAX_PATH] = { 0 };		// Empty string buffer
	wcscpy_s(filename, lpLibFileName);		// Copy full path to buffer (hopefully it's not bigger than MAX_PATH!)
	PathStripPath(filename);				// Strip the path, just leave the filename

	// Crafting the new path
	std::wstring newpath = L"C:\\DumpoMode\\"; // Hard-coded path right now
	newpath.append(filename);

	// Copy the file to the new location
	CopyFile(lpLibFileName, newpath.c_str(), FALSE);

	return retval;
}