#include <Windows.h>
#include <thread>
#include <string>

#include "Lab4.h"


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshows) {
	WNDCLASS SoftwareMainClass = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(NULL, IDI_APPLICATION), L"MainWndClass", SoftwareMainProcedure);

	if (!RegisterClass(&SoftwareMainClass)) { return -1; }

	MSG SoftwareMainMessage = { 0 };

	HWND hWnd = CreateWindow(L"MainWndClass", L"CPU and Memory usage", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 250, NULL, NULL, NULL, NULL);


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

	HPEN hpen, hpenCPUGraph, hpenMemGraph;
	RECT rect, rectCPUGraph, rectMemGraph, rectWindow;

	switch (msg) {
	case WM_CREATE: {

		for (int i = 400 - 1; i >= 0; i--) {
			cpuArray[i] = -1;
			memoryArray[i] = -1;
		}

		handleThread = CreateThread(NULL, 0, ThreadProc, NULL, NULL, NULL);

		CreateWindow(
			L"STATIC",
			L"CPU:",
			WS_VISIBLE | WS_CHILD,
			10,
			40,
			80,
			20,
			hWnd,
			NULL,
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL);

		cpuText = CreateWindow(
			L"STATIC",
			L"",
			WS_VISIBLE | WS_CHILD,
			10,
			60,
			80,
			20,
			hWnd,
			NULL,
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL);

		CreateWindow(
			L"STATIC",
			L"MEMORY:",
			WS_VISIBLE | WS_CHILD,
			10,
			170,
			80,
			20,
			hWnd,
			NULL,
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL);

		memoryText = CreateWindow(
			L"STATIC",
			L"",
			WS_VISIBLE | WS_CHILD,
			10,
			190,
			80,
			20,
			hWnd,
			NULL,
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL);

		break;
	}
	case WM_PAINT: {
		GetClientRect(hWnd, &rectWindow);

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		SetRect(&rectCPUGraph, 100, 10, rectWindow.right - 10, 110);
		SetRect(&rectMemGraph, 100, 140, rectWindow.right - 10, 240);
		FillRect(hdc, &rectCPUGraph, CreateSolidBrush(RGB(220, 220, 220)));
		FillRect(hdc, &rectMemGraph, CreateSolidBrush(RGB(220, 220, 220)));
		FrameRect(hdc, &rectCPUGraph, CreateSolidBrush(RGB(0, 0, 0)));
		FrameRect(hdc, &rectMemGraph, CreateSolidBrush(RGB(0, 0, 0)));

		hpen = CreatePen(PS_SOLID, 0, RGB(128, 128, 128));
		SelectObject(hdc, hpen);
		for (int i = 10; i < 100; i += 10) {
			MoveToEx(hdc, rectCPUGraph.left, rectCPUGraph.bottom - i, 0);
			LineTo(hdc, rectCPUGraph.right, rectCPUGraph.bottom - i);

			MoveToEx(hdc, rectMemGraph.left, rectMemGraph.bottom - i, 0);
			LineTo(hdc, rectMemGraph.right, rectMemGraph.bottom - i);
		}
		DeleteObject(hpen);

		hpenCPUGraph = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
		SelectObject(hdc, hpenCPUGraph);

		for (int i = 1, j = 10; i < 400; i++, j += 10)
		{
			if ((cpuArray[i + 1] >= 0) && ((rectWindow.right - j) > rectCPUGraph.left + 10))
			{
				MoveToEx(hdc, rectWindow.right - j, 110 - cpuArray[i], 0);
				LineTo(hdc, rectWindow.right - j - 10, 110 - cpuArray[i + 1]);

			}
		}
		DeleteObject(hpenCPUGraph);

		hpenMemGraph = CreatePen(PS_SOLID, 1, RGB(153, 50, 204));
		SelectObject(hdc, hpenMemGraph);
		for (int i = 0, j = 10; i < 400; i++, j += 10)
		{
			if ((memoryArray[i + 1] >= 0) && ((rectWindow.right - j) > rectMemGraph.left + 10))
			{
				MoveToEx(hdc, rectWindow.right - j, rectMemGraph.bottom - memoryArray[i], 0);
				LineTo(hdc, rectWindow.right - j - 10, rectMemGraph.bottom - memoryArray[i + 1]);
			}
		}
		DeleteObject(hpenMemGraph);

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lp;
		lpMMI->ptMinTrackSize.x = 650;
		lpMMI->ptMinTrackSize.y = 300;
		lpMMI->ptMaxTrackSize.x = 650;
		lpMMI->ptMaxTrackSize.y = 300;
		break;
	}
	case WM_DESTROY:
		CloseHandle(handleThread);
		PostQuitMessage(0);

		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
}


DWORD ThreadProc(LPVOID lpParam)
{
	GetSystemTimes(&last_idleTime, &last_kernelTime, &last_userTime);
	for (;;)
	{
		if (GetSystemTimes(&idleTime, &kernelTime, &userTime) != 0)
		{

			double usr = userTime.dwLowDateTime - last_userTime.dwLowDateTime;
			double ker = kernelTime.dwLowDateTime - last_kernelTime.dwLowDateTime;
			double idl = idleTime.dwLowDateTime - last_idleTime.dwLowDateTime;

			double sys = ker + usr;

			last_idleTime = idleTime;
			last_userTime = userTime;
			last_kernelTime = kernelTime;

			if (sys != 0) {
				cpu = (sys - idl) / sys * 100;

				for (int i = 400 - 1; i > 0; i--) {
					cpuArray[i] = cpuArray[i - 1];
				}
				cpuArray[0] = cpu;

				int cpuValue = static_cast<int>(cpu);
				std::wstring cpuString = std::to_wstring(cpuValue) + L"%";

				SetWindowText(cpuText, cpuString.c_str());
			}
		}

		MEMORYSTATUSEX statex;

		statex.dwLength = sizeof(statex);

		GlobalMemoryStatusEx(&statex);

		memory = statex.dwMemoryLoad;
		for (int i = 400 - 1; i > 0; i--) {
			memoryArray[i] = memoryArray[i - 1];
		}
		memoryArray[0] = memory;

		int memoryValue = static_cast<int>(memory);
		std::wstring memoryString = std::to_wstring(memoryValue) + L"%";

		SetWindowText(memoryText, memoryString.c_str());

		InvalidateRect(FindWindow(L"MainWndClass", NULL), NULL, TRUE);
		Sleep(1000);
	}

}