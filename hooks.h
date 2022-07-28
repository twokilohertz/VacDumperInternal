#pragma once
#include <Windows.h>

typedef HMODULE(WINAPI* fpLoadLibraryExW)(LPCWSTR, HANDLE, DWORD);

HMODULE WINAPI hooked_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE  hFile, DWORD dwFlags);