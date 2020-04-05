#include<Windows.h>
#include"resource.h"

HWND hEdit1;
HWND hEdit2;

CHAR str1[] = { "Hello!..." };
CHAR str2[] = { 0 };

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
	case WM_INITDIALOG: // ������������� ����������� ����
	{
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		SendMessage(hwnd, WM_SETICON, 0, LOWORD(hIcon));

		hEdit1 = GetDlgItem(hwnd, IDC_EDIT1);
		hEdit2 = GetDlgItem(hwnd, IDC_EDIT2);

		SendMessage(hEdit1, WM_SETTEXT, 0, (LPARAM)str1);
		SetFocus(GetDlgItem(hwnd, IDC_COPY));
	}
	break;
	case WM_COMMAND:	// ��������� ������ �������� �� ������ � �.�.
		switch (LOWORD(wParam))
		{
		case IDC_COPY:
			SendMessage(hEdit1, WM_GETTEXT, 255, (LPARAM)str2);
			SendMessage(hEdit2, WM_SETTEXT, 0, (LPARAM)str2);
			break;
		case IDOK:
			MessageBox(hwnd, "���� ��������� ������ ���", "Info", MB_OK | MB_ICONINFORMATION);
		}
		break;
	case IDCANCEL:
	{
		MessageBox(hwnd, "���� ��������� ������ Ͳ", "Info", MB_OK | MB_ICONINFORMATION);
		EndDialog(hwnd, 0);
		break;
	}
	break;
	case WM_CLOSE:	// �������� ����������� ���� ������� "X"
		EndDialog(hwnd, 0);
	}
	return FALSE;
}