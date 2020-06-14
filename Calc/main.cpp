#include<Windows.h>
#include<CommCtrl.h>
#include"resource.h"
#include<stdio.h>
#include<cmath>

LPWSTR lpszCurrentText = NULL;

BOOL	CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DOUBLE val1 = NULL;
DOUBLE val2 = NULL;
CHAR Pointer;

DOUBLE GetEditText(HWND hEdit);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hEdit1 = GetDlgItem(hwnd, IDC_EDIT1);
		switch (uMsg)
		{
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON1:
			{
				SendMessage(hEdit1, WM_CHAR, 0x31, 0);
			}
			break;
			case IDC_BUTTON2:
			{
				SendMessage(hEdit1, WM_CHAR, 0x32, 0);
			}
			break;
			case IDC_BUTTON3:
			{
				SendMessage(hEdit1, WM_CHAR, 0x33, 0);
			}
			break;
			case IDC_BUTTON4:
			{
				SendMessage(hEdit1, WM_CHAR, 0x34, 0);
			}
			break;
			case IDC_BUTTON5:
			{
				SendMessage(hEdit1, WM_CHAR, 0x35, 0);
			}
			break;
			case IDC_BUTTON6:
			{
				SendMessage(hEdit1, WM_CHAR, 0x36, 0);
			}
			break;
			case IDC_BUTTON7:
			{
				SendMessage(hEdit1, WM_CHAR, 0x37, 0);
			}
			break;
			case IDC_BUTTON8:
			{
				SendMessage(hEdit1, WM_CHAR, 0x38, 0);
			}
			break;
			case IDC_BUTTON9:
			{
				SendMessage(hEdit1, WM_CHAR, 0x39, 0);
			}
			break;
			case IDC_BUTTON10:
			{
				SendMessage(hEdit1, WM_CHAR, 0x30, 0);
			}
			break;
			case IDC_BUTTON_PLUS:
			{
				val1 = GetEditText(hEdit1);
				SetWindowText(hEdit1, NULL);
				Pointer = '+';
			}
			break;
			case IDC_BUTTON_MINUS:
			{
				val1 = GetEditText(hEdit1);
				SetWindowText(hEdit1, NULL);
				Pointer = '-';
			}
			break;
			case IDC_BUTTON_MULTIPLICATION:
			{
				val1 = GetEditText(hEdit1);
				SetWindowText(hEdit1, NULL);
				Pointer = 'x';
			}
			break;
			case IDC_BUTTON_DIVISION:
			{
				val1 = GetEditText(hEdit1);
				SetWindowText(hEdit1, NULL);
				Pointer = '/';
			}
			break;
			case IDC_BUTTON_POWER:
			{
				val1 = GetEditText(hEdit1);
				SetWindowText(hEdit1, NULL);
				Pointer = '^';
			}
			break;
			case IDC_BUTTON_EQUALLY:
			{
				CHAR buffer[255]{};

				val2 = GetEditText(hEdit1);
				SetWindowText(hEdit1, NULL);
				switch (Pointer)
				{
				case '+':sprintf_s(buffer, "%.f", (val1 + val2));
					break;
				case '-':sprintf_s(buffer, "%.f", (val1 - val2));
					break;
				case 'x':sprintf_s(buffer, "%.f", (val1 * val2));
					break;
				case '/':sprintf_s(buffer, "%.2f", (val1 / val2));
					break;
				case '^':sprintf_s(buffer, "%.f", double (pow(val1, val2)));
					break;
				}
				SetWindowText(hEdit1, buffer);
			}
			break;
			case IDC_BUTTON_CLEAR:
			{
				SetWindowText(hEdit1, NULL);
			}
			break;
			}
			break;
		case WM_CLOSE:
			EndDialog(hwnd, 0);
			break;
		default:return FALSE;
		}
	return TRUE;
}

DOUBLE GetEditText(HWND hEdit)
{
	CHAR buffer[255]{};
	int text = GetWindowText(hEdit, buffer, 255);
	return atof(buffer);
}