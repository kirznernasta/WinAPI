#pragma once

HANDLE handleThread;

HWND cpuText;
HWND memoryText;

double cpu = -1;
double memory = -1;
int cpuArray[400];
int memoryArray[400];

FILETIME idleTime;
FILETIME kernelTime;
FILETIME userTime;

FILETIME last_idleTime;
FILETIME last_kernelTime;
FILETIME last_userTime;

LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure);

DWORD ThreadProc(LPVOID lpParam);
