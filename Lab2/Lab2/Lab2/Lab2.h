#pragma once

#define OnSaveFile 1
#define OnSaveFileAs 2
#define OnLoadFile 3
#define OnBold 4
#define OnItalic 5

#define OnTextColor 6
#define OnBgColor 7

#define TextBufferSize 256

char Buffer[TextBufferSize];

bool isFileLoaded = false;
bool isBold = false;
bool isItalic = false;

HWND hEditControl;

char filename[260];
OPENFILENAMEA ofn;

CHARFORMAT cf{ };

COLORREF textColor = RGB(0, 0, 0);
COLORREF bgColor = RGB(255, 255, 255);

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