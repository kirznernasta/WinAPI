#include <Windows.h>
#include <thread>
#include <string>

#include "Lab5.h"


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshows) {
	WNDCLASS SoftwareMainClass = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(NULL, IDI_APPLICATION), L"MainWndClass", SoftwareMainProcedure);

	if (!RegisterClass(&SoftwareMainClass)) { return -1; }

	MSG SoftwareMainMessage = { 0 };

	HWND hWnd = CreateWindow(L"MainWndClass", L"Registry analysis", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 250, NULL, NULL, NULL, NULL);


	ShowWindow(hWnd, ncmdshows);
	UpdateWindow(hWnd);

	while (GetMessage(&SoftwareMainMessage, NULL, NULL, NULL)) {
		TranslateMessage(&SoftwareMainMessage);
		DispatchMessage(&SoftwareMainMessage);
	}
	return 0;
}

WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure) {
	WNDCLASS NWC = { 0 };

	NWC.hCursor = Cursor;
	NWC.hIcon = Icon;
	NWC.hInstance = hInst;
	NWC.lpszClassName = Name;
	NWC.hbrBackground = BGColor;
	NWC.lpfnWndProc = Procedure;

	return NWC;
}

LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_CREATE: {
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);

		hEdit = CreateWindowW(
			L"EDIT",
			L"",
			WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOVSCROLL | ES_READONLY  | ES_MULTILINE | WS_VSCROLL,
			10,
			10,
			(clientRect.right - 10),
			clientRect.bottom - 20,
			hWnd,
			NULL,
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL
		);


		HKEY hKey;
		// HKEY_CLASSES_ROOT
		// HKEY_CURRENT_USER
		// HKEY_LOCAL_MACHINE
		// HKEY_USERS
		// HKEY_CURRENT_CONFIG
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
			EnumerateRegistryKeys(hKey, L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft", hEdit);
			RegCloseKey(hKey);
		}

		break;
	}
	case WM_SIZE:
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		
		MoveWindow(
			hEdit,
			10,
			10,
			(clientRect.right - 10),
			clientRect.bottom - 20,
			TRUE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);

		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
}

void EnumerateRegistryKeys(HKEY hKey, const std::wstring& path, HWND hEdit) {

	DWORD dwBytesWritten;

	DWORD subKeys;
	DWORD maxSubKeyLen;
	DWORD values;
	DWORD maxValueNameLen;
	DWORD maxValueDataLen;

	if (RegQueryInfoKey(hKey, NULL, NULL, NULL, &subKeys, &maxSubKeyLen, NULL, &values, &maxValueNameLen, &maxValueDataLen, NULL, NULL) != ERROR_SUCCESS) {
		return;
	}

	if (values == 0 && subKeys == 0) {

		SendMessageW(hEdit, EM_SETSEL, -1, -1);
		SendMessageW(hEdit, EM_REPLACESEL, 0, (LPARAM)(path + L"\r\n").c_str());

		return;
	}

	for (DWORD i = 0; i < subKeys; ++i) {
		WCHAR subKeyName[MAX_PATH];
		DWORD subKeyNameSize = sizeof(subKeyName);

		if (RegEnumKeyEx(hKey, i, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
			HKEY subKey;
			if (RegOpenKeyEx(hKey, subKeyName, 0, KEY_READ, &subKey) == ERROR_SUCCESS) {
				EnumerateRegistryKeys(subKey, path + L"\\" + subKeyName, hEdit);
				RegCloseKey(subKey);
			}
		}
	}
}
