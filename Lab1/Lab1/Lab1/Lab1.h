#pragma once

#define OnSaveFile 1
#define OnSaveFileAs 2
#define OnLoadFile 3

#define TextBufferSize 256

char Buffer[TextBufferSize];

bool isFileLoaded = false;

HWND hEditControl;

char filename[260];
OPENFILENAMEA ofn;

LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure);

void MainWndMenus(HWND hWnd);
void MainWndAddWidgets(HWND hWnd);
void SaveData(LPCSTR path);
void LoadData(LPCSTR path);
void SetOpenFileParams(HWND hWnd);
LPCSTR GetFileName(LPCSTR filepath);
bool FileExists(LPCSTR path);
bool HasTextChanged(HWND hEditControl, LPCSTR filepath);
void LoadFile(HWND hWnd);
void ChangeWindowTitle(HWND hWnd);