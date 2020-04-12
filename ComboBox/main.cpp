#include<Windows.h>
#include"resource.h"
#include<string>

HWND hCombo;

/*CHAR string1[] = "This";
CHAR string2[] = "is";
CHAR string3[] = "my";
CHAR string4[] = "first";
CHAR string5[] = "Combo";
CHAR string6[] = "Box";*/

const CHAR* string[] =
{
	"This",
	"is",
	"my",
	"first",
	"Check",
	"Box"
};


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
	case WM_INITDIALOG:
		hCombo = GetDlgItem(hwnd, IDC_COMBO1);
		// ƒобавл€ем строки в ComboBox
		for (int i = 0; i < sizeof(string)/sizeof(CHAR*); i++)
		{
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)string[i]);
		}
		/*SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)string1); // CTRL+D дублировать строку
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)string2);
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)string3);
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)string4);
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)string5);
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)string6);*/
		break;
	case WM_COMMAND:
	{
		CHAR buffer[25]{};
		switch (LOWORD(wParam))
		{
			case IDC_COMBO1:
			{
				if (HIWORD(wParam) == CBN_SELENDOK)
				{
					int i = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
					SendMessage(hCombo, CB_GETLBTEXT, i, (LPARAM)buffer);
					MessageBox(hwnd, buffer, "¬ыбраный пункт выпадающего списка", MB_OK | MB_ICONINFORMATION);
				}
			}
			break;
		case IDOK:
			MessageBox(hwnd, "Hello", "Hi", MB_OK | MB_ICONINFORMATION);
			break;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			MessageBox(hwnd, "Bye bye", "See you later", MB_OK | MB_ICONINFORMATION);
			break;
		}
	}
	break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	}
	return FALSE;
}
