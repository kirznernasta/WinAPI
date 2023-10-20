#pragma once

LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure);

HWND hEdit;
HANDLE hFile;
void EnumerateRegistryKeys(HKEY hKey, const std::wstring& path, HWND hEdit);