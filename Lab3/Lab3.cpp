#include <Windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <stdio.h>
#include <thread>

#include "Lab3.h"
#include <string>

using std::string;


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshows) {
	WNDCLASS SoftwareMainClass = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(NULL, IDI_APPLICATION), L"MainWndClass", SoftwareMainProcedure);

	if (!RegisterClass(&SoftwareMainClass)) { return -1; }

	MSG SoftwareMainMessage = { 0 };

	CreateWindow(L"MainWndClass", L"File manager", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 250, NULL, NULL, NULL, NULL);

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
		case OnFileButtonClicked:
			if (GetOpenFileNameW(&ofnFile)) {
				ChangeSourceText();
				isFileChosen = true;
				CheckCopyAndMoveButtons();
				
			}
			break;
		case OnFolderButtonClicked:
			if (GetFolder(hWnd)) {
				ChangeDestinationText();
				isFolderChosen = true;
				CheckCopyAndMoveButtons();
			}
			break;
		case OnCopyButtonClicked:
			if (!
				CreateThread(
					NULL,
					0,
					CopyFunction,
					NULL,
					0,
					NULL)) {
				MessageBox(hWnd, L"Can't handle operation!", L"Error", MB_OK | MB_ICONERROR);
			}
			else {
				currentProccesses++;
			}
			break;
		case OnMoveButtonClicked:
			if (!
				CreateThread(
					NULL,
					0,
					MoveFunction,
					NULL,
					0,
					NULL)) {
				MessageBox(hWnd, L"Can't handle operation!", L"Error", MB_OK | MB_ICONERROR);
			}
			else {
				currentProccesses++;
			}
			
			break;
		}

		break;
	case WM_CREATE: {
		CreateButtons(hWnd);
		SetOpenFilesParams(hWnd);

		HRESULT hr = CoInitialize(NULL);
		 

		if (SUCCEEDED(hr)) {
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog));
			if (SUCCEEDED(hr)) {
				folderHR = pFileDialog->GetOptions(&folderOptions);
				if (SUCCEEDED(folderHR)) {
					pFileDialog->SetOptions(folderOptions | FOS_PICKFOLDERS);
				}
			}
		}
		else {
			MessageBox(hWnd, L"Can't create file dialog", L"Error", MB_OK | MB_ICONERROR);
			PostQuitMessage(0);
		}

		break;
	}
	case WM_SIZE:
		ChangeButtonsSizes(hWnd);
		break;
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lp;
		lpMMI->ptMinTrackSize.x = 1000;
		lpMMI->ptMinTrackSize.y = 500;
		break;
	}
	case WM_CLOSE:
		if (currentProccesses > 0) {
			MessageBox(hWnd, L"Can't close application while proccessing operation!", L"Error", MB_OK | MB_ICONERROR);
		}
		else {
			DestroyWindow(hWnd);
		}
		break;
	case WM_DESTROY:
		
		pFileDialog->Release();
		CoUninitialize();
		PostQuitMessage(0);
		
		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
}

void CreateButtons(HWND hWnd) {
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	fileButton = CreateWindow(
		L"BUTTON", 
		L"Choose source file",   
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_BORDER, 
		10,
		10,
		(clientRect.right - 30) / 2,
		50,
		hWnd,    
		(HMENU)OnFileButtonClicked,
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);    

	folderButton = CreateWindow(
		L"BUTTON",
		L"Choose destination folder",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_BORDER,
		20 + (clientRect.right - 30) / 2,
		10,
		(clientRect.right - 30) / 2,
		50,
		hWnd,
		(HMENU)OnFolderButtonClicked,
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);

	sourceText = CreateWindow(
		L"STATIC",         
		L"Source file path: ",  
		WS_VISIBLE | WS_CHILD,  
		10,              
		70,                 
		(clientRect.right - 20),
		20,                
		hWnd,              
		NULL,      
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);

	destinationText = CreateWindow(
		L"STATIC",
		L"Destination folder path: ",
		WS_VISIBLE | WS_CHILD,
		10,
		100,
		(clientRect.right - 20),
		20,
		hWnd,
		NULL,
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);

	copyButton = CreateWindow(
		L"BUTTON",
		L"Copy chosen file",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_BORDER | WS_DISABLED,
		10,
		clientRect.bottom - 60,
		(clientRect.right - 30) / 2,
		50,
		hWnd,
		(HMENU)OnCopyButtonClicked,
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);

	moveButton = CreateWindow(
		L"BUTTON",
		L"Move chosen file",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_BORDER | WS_DISABLED,
		20 + (clientRect.right - 30) / 2,
		clientRect.bottom - 60,
		(clientRect.right - 30) / 2,
		50,
		hWnd,
		(HMENU)OnMoveButtonClicked,
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);

	hEdit = CreateWindow(
		L"EDIT",           
		L"",   
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOVSCROLL | WS_DISABLED | ES_MULTILINE | WS_VSCROLL,
		10,               
		130,               
		(clientRect.right - 20),        
		clientRect.bottom - 200,         
		hWnd,              
		NULL,             
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), 
		NULL              
	);
}

void ChangeButtonsSizes(HWND hWnd) {
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	MoveWindow(
		fileButton,
		10,
		10,
		(clientRect.right - 30) / 2,
		50,
		TRUE);

	MoveWindow(
		folderButton,
		20 + (clientRect.right - 30) / 2,
		10,
		(clientRect.right - 30) / 2,
		50,
		TRUE);

	MoveWindow(
		copyButton,
		10,
		clientRect.bottom - 60,
		(clientRect.right - 30) / 2,
		50,
		TRUE);

	MoveWindow(
		moveButton,
		20 + (clientRect.right - 30) / 2,
		clientRect.bottom - 60,
		(clientRect.right - 30) / 2,
		50,
		TRUE);


	MoveWindow(
		hEdit,
		10,              
		130,              
		(clientRect.right - 20),               
		clientRect.bottom - 200,
		TRUE);
}


void SetOpenFilesParams(HWND hWnd) {
	ZeroMemory(&ofnFile, sizeof(ofnFile));
	ofnFile.lStructSize = sizeof(ofnFile);
	ofnFile.hwndOwner = hWnd;
	ofnFile.lpstrFile = fileName;
	ofnFile.nMaxFile = sizeof(fileName);
	ofnFile.lpstrFilter = L"All Files\0*.*\0";
	ofnFile.lpstrFileTitle = NULL;
	ofnFile.nMaxFileTitle = 0;
	ofnFile.lpstrInitialDir = NULL;
	ofnFile.lpstrDefExt = NULL;
	ofnFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}

bool GetFolder(HWND hwnd) {
	bool isSuccess = false;
	folderHR = pFileDialog->Show(NULL);
	if (SUCCEEDED(folderHR)) {
		IShellItem* pItem;
		folderHR = pFileDialog->GetResult(&pItem); 
		if (SUCCEEDED(folderHR)) {
			folderHR = pItem->GetDisplayName(SIGDN_FILESYSPATH, &folderName);
			if (SUCCEEDED(folderHR)) {
				isSuccess = true;
			}
			pItem->Release(); 
		}
	}
	return isSuccess;
}

void ChangeSourceText() {
	WCHAR newSource[300];
	wcscpy_s(newSource, L"Source file path: ");
	wcscat_s(newSource, fileName);
	
	SetWindowText(sourceText, (LPWSTR)newSource);
}


void ChangeDestinationText() {
	WCHAR newDestination[300];
	wcscpy_s(newDestination, L"Destination folder path: ");
	wcscat_s(newDestination, folderName);

	SetWindowText(destinationText, (LPWSTR)newDestination);
}

void CheckCopyAndMoveButtons() {
	if (isFileChosen && isFolderChosen) {
		EnableWindow(copyButton, TRUE);
		EnableWindow(moveButton, TRUE);
	}
	else {
		EnableWindow(copyButton, FALSE);
		EnableWindow(moveButton, FALSE);
	}
}

DWORD CopyFunction(LPVOID lpParam) {

	WCHAR path[MAX_PATH];
	WCHAR filePath[MAX_PATH];
	WCHAR folder[MAX_PATH];
	wcscpy_s(filePath, fileName);
	wcscpy_s(path, fileName);
	wcscpy_s(folder, folderName);

	WCHAR name[MAX_PATH];
	PathStripPath(filePath);
	lstrcpyW(name, filePath);

	WCHAR newPath[MAX_PATH];
	PathCombine(newPath, folder, name);
	SendMessage(hEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);

	WCHAR message[MAX_PATH];
	wcscpy_s(message, L"Started coping file to ");
	wcscat_s(message, newPath);
	wcscat_s(message, L"...\r\n");

	SendMessage(hEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
	SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)message);

	Sleep(10000);
	if (CopyFile(path, newPath, TRUE)) {

		SendMessage(hEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
		wcscpy_s(message, L"Copied file successfully to ");
		wcscat_s(message, newPath);
		wcscat_s(message, L"!\r\n");
		SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)message);

		currentProccesses--;
		ExitThread(0);
		return 0;
	}

	SendMessage(hEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
	wcscpy_s(message, L"Error while coping file to ");
	wcscat_s(message, newPath);
	wcscat_s(message, L"! :(\r\n");
	SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)message);
	
	currentProccesses--;
	ExitThread(1);
	return 1;
}


DWORD MoveFunction(LPVOID lpParam) {
	WCHAR path[MAX_PATH];
	WCHAR folder[MAX_PATH];
	WCHAR filePath[MAX_PATH];
	wcscpy_s(filePath, fileName);
	wcscpy_s(path, fileName);
	wcscpy_s(folder, folderName);

	WCHAR name[MAX_PATH];
	PathStripPath(filePath);
	lstrcpyW(name, filePath);

	WCHAR newPath[MAX_PATH];
	PathCombine(newPath, folder, name);

	WCHAR message[MAX_PATH];
	wcscpy_s(message, L"Started moving file to ");
	wcscat_s(message, newPath);
	wcscat_s(message, L"...\r\n");

	SendMessage(hEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
	SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)message);

	Sleep(10000);
	if (MoveFileW(path, newPath)) {

		SendMessage(hEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
		wcscpy_s(message, L"Moved file successfully to ");
		wcscat_s(message, newPath);
		wcscat_s(message, L"!\r\n");
		SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)message);

		currentProccesses--;
		ExitThread(0);
		return 0;
	}

	SendMessage(hEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
	wcscpy_s(message, L"Error while moving file to ");
	wcscat_s(message, newPath);
	wcscat_s(message, L"! :(\r\n");
	SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)message);

	currentProccesses--;
	ExitThread(1);
	return 1;
	
}
