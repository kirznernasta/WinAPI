
#include <windows.h>
#include <richedit.h>
#include <commdlg.h>
#include "Lab2.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshows) {
	WNDCLASS SoftwareMainClass = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(NULL, IDI_QUESTION), L"MainWndClass", SoftwareMainProcedure);

	if (!RegisterClass(&SoftwareMainClass)) { return -1; }

	MSG SoftwareMainMessage = { 0 };

	CreateWindow(L"MainWndClass", L"Text editor", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 250, NULL, NULL, NULL, NULL);

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
	case WM_COMMAND:
		switch (wp) {
		case OnLoadFile:
			if (HasTextChanged(hEditControl, filename)) {
				int result = MessageBoxA(hWnd, "Do you want to save changes?", "Confirmation", MB_YESNOCANCEL | MB_APPLMODAL);
				if (result == IDYES) {
					SendMessage(hWnd, WM_COMMAND, isFileLoaded ? OnSaveFile : OnSaveFileAs, 0);
					LoadFile(hWnd);
				}
				else if (result == IDNO) {
					LoadFile(hWnd);
				}
			}
			else {
				LoadFile(hWnd);
			}

			break;
		case OnSaveFile:
			if (FileExists(filename)) {
				SaveData(filename);
			}
			break;
		case OnSaveFileAs:
			while (true) {
				if (GetSaveFileNameA(&ofn)) {
					if (FileExists(filename)) {
						int result = MessageBoxA(hWnd, "File already exists. Do you want to overwrite it?", "Confirmation", MB_YESNO | MB_ICONQUESTION);
						if (result == IDYES) {
							SaveData(filename);
							ChangeWindowTitle(hWnd);
							break;
						}
					}
					else {
						SaveData(filename);
						ChangeWindowTitle(hWnd);
						break;
					}
				}
				else {
					break;
				}
			}
			break;

		case OnBold: {
			MENUITEMINFO menuItemInfo = { 0 };
			menuItemInfo.cbSize = sizeof(MENUITEMINFO);
			menuItemInfo.fMask = MIIM_STRING;
			UINT menuItemID = OnBold;
			const char* narrowString;
			if (isBold) {
				cf.dwEffects &= ~CFE_BOLD;
				narrowString = "Bold   ";
			}
			else {
				cf.dwEffects |= CFE_BOLD;
				narrowString = "Bold * ";
			}
			isBold = !isBold;
			SendMessage(hEditControl, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
			int bufferSize = MultiByteToWideChar(CP_UTF8, 0, narrowString, -1, NULL, 0);
			wchar_t* wideString = new wchar_t[bufferSize];
			MultiByteToWideChar(CP_UTF8, 0, narrowString, -1, wideString, bufferSize);
			menuItemInfo.dwTypeData = wideString;
			SetMenuItemInfo(GetMenu(hWnd), menuItemID, FALSE, &menuItemInfo);
			break;
		}


		case OnItalic: {
			MENUITEMINFO menuItemInfo = { 0 };
			menuItemInfo.cbSize = sizeof(MENUITEMINFO);
			menuItemInfo.fMask = MIIM_STRING;
			UINT menuItemID = OnItalic;
			const char* narrowString;
			if (isItalic) {
				cf.dwEffects &= ~CFE_ITALIC;
				narrowString = "Italic   ";
			}
			else {
				cf.dwEffects |= CFE_ITALIC;
				narrowString = "Italic * ";
			}
			isItalic = !isItalic;
			SendMessage(hEditControl, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
			int bufferSize = MultiByteToWideChar(CP_UTF8, 0, narrowString, -1, NULL, 0);
			wchar_t* wideString = new wchar_t[bufferSize];
			MultiByteToWideChar(CP_UTF8, 0, narrowString, -1, wideString, bufferSize);
			menuItemInfo.dwTypeData = wideString;
			SetMenuItemInfo(GetMenu(hWnd), menuItemID, FALSE, &menuItemInfo);
			delete[] wideString;
			break;
		}
		case OnTextColor: {
			CHOOSECOLOR cc;           
			COLORREF acrCustClr[6]; 

			ZeroMemory(&cc, sizeof(cc));
			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hWnd;
			cc.lpCustColors = (LPDWORD)acrCustClr;
			cc.rgbResult = textColor;

			if (ChooseColor(&cc) == TRUE) {
				COLORREF chosenColor = cc.rgbResult;
				cf.crTextColor = chosenColor;
				textColor = chosenColor;
				SendMessage(hEditControl, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
			}
			break;
		}
		case OnBgColor: {
			CHOOSECOLOR bgcc;
			COLORREF acrCustClr[6];

			ZeroMemory(&bgcc, sizeof(bgcc));
			bgcc.lStructSize = sizeof(bgcc);
			bgcc.hwndOwner = hWnd;
			bgcc.lpCustColors = (LPDWORD)acrCustClr;
			bgcc.rgbResult = bgColor;

			if (ChooseColor(&bgcc) == TRUE) {
				COLORREF chosenColor = bgcc.rgbResult;
				bgColor = chosenColor;
				SendMessage(hEditControl, EM_SETBKGNDCOLOR, 0, chosenColor);
			}
			break;
		}
		}
		break;
	case WM_CREATE:
		MainWndMenus(hWnd);
		MainWndAddWidgets(hWnd);
		SetOpenFileParams(hWnd);
		break;
	case WM_SIZE:
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		MoveWindow(hEditControl, 10, 10, clientRect.right - 20, clientRect.bottom - 20, TRUE);
		break;
	case WM_DESTROY:
		if (HasTextChanged(hEditControl, filename)) {
			int result = MessageBoxA(hWnd, "Do you want to save changes?", "Confirmation", MB_YESNO | MB_APPLMODAL);
			if (result == IDYES) {
				SendMessage(hWnd, WM_COMMAND, isFileLoaded ? OnSaveFile : OnSaveFileAs, 0);
			}
			PostQuitMessage(0);
		}
		else {
			PostQuitMessage(0);
		}
		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
}


void MainWndMenus(HWND hWnd)
{
	HMENU RootMenu = CreateMenu();
	HMENU SubMenu = CreateMenu();

	AppendMenu(SubMenu, MF_STRING, OnLoadFile, L"Load");
	AppendMenu(SubMenu, MF_STRING | MF_GRAYED, OnSaveFile, L"Save");
	AppendMenu(SubMenu, MF_STRING, OnSaveFileAs, L"Save as");

	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"File");

	AppendMenu(RootMenu, MF_STRING, OnBold, L"Bold   ");
	AppendMenu(RootMenu, MF_STRING, OnItalic, L"Italic   ");

	AppendMenu(RootMenu, MF_STRING, OnTextColor, L"Text color");
	AppendMenu(RootMenu, MF_STRING, OnBgColor, L"Background color");

	SetMenu(hWnd, RootMenu);
}

void MainWndAddWidgets(HWND hWnd) {
	LoadLibrary(TEXT("Msftedit.dll"));

	hEditControl = CreateWindowEx(0, MSFTEDIT_CLASS, TEXT(""),
		ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VSCROLL,
		0, 0, 200, 200,
		hWnd, NULL, NULL, NULL);

	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR | CFM_SIZE | CFM_BOLD | CFM_ITALIC;
	cf.crTextColor = RGB(0, 0, 0);
	cf.yHeight = (12 * 1440) / 72;

	SendMessage(hEditControl, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
	SendMessage(hEditControl, EM_SETBKGNDCOLOR, 0, RGB(255, 255, 255));
}

void SaveData(LPCSTR path) {
	HANDLE FileToSave = CreateFileA(
		path,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	int saveLength = GetWindowTextLength(hEditControl) + 1;
	char* data = new char[saveLength];

	saveLength = GetWindowTextA(hEditControl, data, saveLength);

	DWORD bytesIterated;
	WriteFile(FileToSave, data, saveLength, &bytesIterated, NULL);

	CloseHandle(FileToSave);
	delete[] data;
}

void LoadData(LPCSTR path) {
	SetWindowTextA(hEditControl, "");
	HANDLE FileToLoad = CreateFileA(
		path,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	DWORD fileSize = GetFileSize(FileToLoad, NULL);
	char* data = new char[fileSize + 1];

	DWORD bytesRead = 0;
	ReadFile(FileToLoad, data, fileSize, &bytesRead, NULL);
	data[bytesRead] = '\0';

	CloseHandle(FileToLoad);

	SetWindowTextA(hEditControl, data);


	delete[] data;
}

void SetOpenFileParams(HWND hWnd) {
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	ofn.lpstrFilter = ".txt\0*.txt\0";
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrDefExt = "txt";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}

LPCSTR GetFileName(LPCSTR filepath) {
	LPCSTR filename = strrchr(filepath, '\\');
	if (filename) {
		filename++;
	}
	else {
		filename = filepath;
	}
	return filename;
}

bool FileExists(LPCSTR path) {
	DWORD attributes = GetFileAttributesA(path);
	return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

bool HasTextChanged(HWND hEditControl, LPCSTR filepath) {
	bool result;

	int editLength = GetWindowTextLength(hEditControl) + 1;
	char* editText = new char[editLength];
	GetWindowTextA(hEditControl, editText, editLength);


	if (FileExists(filepath)) {
		HANDLE FileToCheck = CreateFileA(
			filepath,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		DWORD fileSize = GetFileSize(FileToCheck, NULL);
		char* fileData = new char[fileSize + 1];

		DWORD bytesRead = 0;
		ReadFile(FileToCheck, fileData, fileSize, &bytesRead, NULL);
		fileData[bytesRead] = '\0';

		CloseHandle(FileToCheck);

		int editLength = GetWindowTextLength(hEditControl) + 1;
		char* editText = new char[editLength];
		GetWindowTextA(hEditControl, editText, editLength);

		result = strcmp(fileData, editText) != 0;

		delete[] fileData;
		delete[] editText;

		return result;
	}
	else {
		result = strcmp("", editText) != 0;
	}

	delete[] editText;
	return result;
}

void ChangeWindowTitle(HWND hWnd) {
	char newTitle[300];
	strcpy_s(newTitle, GetFileName(filename));
	strcat_s(newTitle, " - Text editor");
	SetWindowTextA(hWnd, newTitle);
}

void LoadFile(HWND hWnd) {
	if (GetOpenFileNameA(&ofn)) {
		LoadData(filename);
		ChangeWindowTitle(hWnd);
		EnableMenuItem(GetMenu(hWnd), OnSaveFile, MF_ENABLED);
		isFileLoaded = true;
	}
}