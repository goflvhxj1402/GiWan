#include"GameFrame.h"
#include"resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCSTR lpszClass = TEXT("BattleCity");
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
GameFrame g_GameFrame;

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	hInst = g_hInst;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
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

	srand((unsigned)time(NULL));
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
			g_GameFrame.UpdateGame(hWnd, hdc);
		}
	}
	g_GameFrame.EndGame();
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
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
			break;
		case  ID_SELECTMAP_SETSTAGE1:
			g_GameFrame.SetCustomMap(hWnd, 0);
			break;
		case  ID_SELECTMAP_SETSTAGE2:
			g_GameFrame.SetCustomMap(hWnd, 1);
			break;
		case  ID_SELECTMAP_SETSTAGE3:
			g_GameFrame.SetCustomMap(hWnd, 2);
			break;
		case  ID_SELECTMAP_SETSTAGE4:
			g_GameFrame.SetCustomMap(hWnd, 3);
			break;
		case  ID_SELECTMAP_SETSTAGE5:
			g_GameFrame.SetCustomMap(hWnd, 4);
			break;
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	POINT pt;
	switch (iMessage)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_RESET:
			MapTool::GetInstance()->Reset();
			InvalidateRect(hDlg, NULL, true);
			break;
		case IDC_SAVE:
			MapTool::GetInstance()->Save(hDlg);
			InvalidateRect(hDlg, NULL, true);
			break;
		case IDC_LOAD:
			MapTool::GetInstance()->Load(hDlg);
			InvalidateRect(hDlg, NULL, true);
			break;
		}
		return TRUE;
	case WM_LBUTTONDOWN:
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		if (MapTool::GetInstance()->Click(pt))
			InvalidateRect(hDlg, NULL, true);
		return TRUE;
	case WM_PAINT:
		hdc = BeginPaint(hDlg, &ps);
		MapTool::GetInstance()->DrawMapTool(hdc);
		EndPaint(hDlg, &ps);
		return TRUE;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	}
	return FALSE;
}