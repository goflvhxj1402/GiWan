#include"GameFrame.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCSTR lpszClass = TEXT("MapTool");
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
GameFrame g_GameFrame;

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInst;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = g_hInst;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		WIN_X + 16, WIN_Y + 58, NULL, (HMENU)0, g_hInst, NULL);
	ShowWindow(hWnd, nCmdShow);
	HDC hdc;
	hdc = GetDC(hWnd);
	g_GameFrame.SetGame(hdc);
	while (true)
	{
		if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
		{
			if (Message.message == WM_QUIT)
				break;
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		else
		{
			g_GameFrame.UpdateGame(hdc);
		}
	}
	ReleaseDC(hWnd, hdc);
	return(int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_MAPTOOL_CREATEMAP:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, AboutDlgProc);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	PAINTSTRUCT ps;
	HDC hdc;
	switch (iMessage)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			MapTool::GetInstance()->Save(hDlg);
			break;
		case IDC_BUTTON2:
			MapTool::GetInstance()->Reset();
			InvalidateRect(hDlg, NULL, TRUE);
			break;
		case IDC_BUTTON3:
			MapTool::GetInstance()->Load(hDlg);
			InvalidateRect(hDlg, NULL, TRUE);
			break;
		}
		return TRUE;
	case WM_LBUTTONDOWN:
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		if (MapTool::GetInstance()->Click(pt))
			InvalidateRect(hDlg, NULL, TRUE);
		return TRUE;
	case WM_PAINT:
		hdc = BeginPaint(hDlg, &ps);
		MapTool::GetInstance()->RoomDraw(hdc);
		EndPaint(hDlg, &ps);
		return TRUE;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		break;
	}
	return FALSE;
}