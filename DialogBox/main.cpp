#include<Windows.h>
#include"resource.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG: // Инициализация диалогового окна
	{
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		SendMessage(hwnd, WM_SETICON, 0, LOWORD(hIcon));
	}
		break;
	case WM_COMMAND:	// Обработка команд нажатием на кнопки и т.д.
		switch (LOWORD(wParam))
		{
		case IDOK:
			MessageBox(hwnd, "Була натиснута кнопка ТАК", "Info", MB_OK | MB_ICONINFORMATION);
			break;
		}
	case IDCANCEL:
	{
		MessageBox(hwnd, "Була натиснута кнопка НІ", "Info", MB_OK | MB_ICONINFORMATION);
		break;
	}
		break;
	case WM_CLOSE:	// Закрытие диалогового окна кнопкой "X"
		EndDialog(hwnd, 0);
	}
	return FALSE;
}