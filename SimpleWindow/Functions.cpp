#include"Functions.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		//Creating menu:
		HMENU hMenu = CreateMenu();
		HMENU hSubMenu;

		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, ID_FILE_NEW, "&New");
		AppendMenu(hSubMenu, MF_STRING, ID_FILE_OPEN, "&Open");
		AppendMenu(hSubMenu, MF_STRING, ID_FILE_SAVE, "&Save");
		AppendMenu(hSubMenu, MF_STRING, ID_FILE_SAVEAS, "&SaveAs");
		AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, "E&xit");
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&File");

		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, ID_HELP_ABOUT, "&About");
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Help");

		SetMenu(hwnd, hMenu);

		//Adding icons:
		HICON hIcon = (HICON)LoadImage(NULL, "Document.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

		//Text editor:
		RECT rect;
		GetClientRect(hwnd, &rect);

		HWND hEdit = CreateWindowEx
		(
			WS_EX_CLIENTEDGE, "Edit", "",
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
			0, 0, rect.right, rect.bottom,
			hwnd,
			(HMENU)IDC_MAIN_EDIT,
			GetModuleHandle(NULL),
			NULL
		);
		if (hEdit == NULL)
			MessageBox(hwnd, "Can not create edit control", "Error", MB_OK | MB_ICONERROR);

		// Красивый шрифт для написания текста в окне
		HFONT hDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hDefault, MAKELPARAM(FALSE, 0));
	}
	break;

	case WM_SIZE: //для того чтобы при растягивании окна скролбары менялись с ним
	{
		RECT rcClient;
		GetClientRect(hwnd, &rcClient);
		HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_OPEN:
		{
			DoFileOpen(hwnd);
		}
		break;
		case ID_FILE_SAVE:
		{

			if (szFileName[0])
				SaveTextFileFromEdit(GetDlgItem(hwnd, IDC_MAIN_EDIT), szFileName);
			else
				SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVEAS, 0);
		}
		break;
		case ID_FILE_SAVEAS:
		{
			DoFileSaveAs(hwnd);
		}
		break;
		case ID_FILE_EXIT:
			DestroyWindow(hwnd);
			break;
		case ID_HELP_ABOUT:
		{
			switch (DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hwnd, DlgProc))
			{
			case IDOK:		MessageBox(hwnd, "Dialog ended with OK!", "Info", MB_OK | MB_ICONINFORMATION);		break;
			case IDCANCEL:	MessageBox(hwnd, "Dialog ended with Cancel!", "Info", MB_OK | MB_ICONINFORMATION);	break;
			case -1:		MessageBox(hwnd, "Dialog Failed!", "Error", MB_OK | MB_ICONERROR);		break;
			}
		}
		break;
		}
		break;
	case WM_CLOSE:
		//if (MessageBox(hwnd, "Вы действительно хотите закрыть окно", "Вы уверены?", MB_YESNO|MB_ICONQUESTION) == IDYES)
	{
		if (FileChanged(GetDlgItem(hwnd, IDC_MAIN_EDIT)))
		{
			switch (MessageBox(hwnd, "Сохранить изменения в файле?", "Не так быстро...", MB_YESNOCANCEL | MB_ICONQUESTION))
			{
			case IDYES: SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVE, 0);
			case IDNO: DestroyWindow(hwnd);
			case IDCANCEL: break;
			}
		}
		else
			DestroyWindow(hwnd);
	}
	break;
	case WM_DESTROY:
		//MessageBox(hwnd, "От странные, лчше б дверь закрыли", "Возмущение", MB_OK | MB_ICONINFORMATION);
		PostQuitMessage(0);
		break;
	default:return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

BOOL	CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hwnd, IDOK);
			break;
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
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

BOOL	LoadTextFileToEdit(HWND hEdit, LPCTSTR pszFileName)
{
	BOOL bSuccess = FALSE;
	LPSTR pszFileText = NULL;
	HANDLE hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE) //Если файл создался продолжаем работу сним
	{
		DWORD dwFileSize = GetFileSize(hFile, NULL);
		if (dwFileSize != UINT_MAX)GlobalFree(pszFileText);
		pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
		if (pszFileText)
		{
			DWORD dwRead;

			if (ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
			{
				pszFileText[dwFileSize] = 0;
				if (SetWindowText(hEdit, pszFileText))bSuccess = TRUE;
			}

		}

	}
	CloseHandle(hFile);
	return bSuccess;
}

BOOL	SaveTextFileFromEdit(HWND hEdit, LPCSTR pszFileName)
{
	BOOL bSuccess = FALSE;
	HANDLE hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		LPSTR pszText;
		DWORD dwTextLength = GetWindowTextLength(hEdit);
		if (dwTextLength > 0)
		{
			pszText = (LPSTR)GlobalAlloc(GPTR, dwTextLength + 1);
			if (pszText != NULL)
			{
				if (GetWindowText(hEdit, pszText, dwTextLength + 1))
				{
					DWORD dwWritten;
					if (WriteFile(hFile, pszText, dwTextLength, &dwWritten, NULL))bSuccess = TRUE;
				}
				GlobalFree(pszText);
			}
			CloseHandle(hFile);
		}
	}
	return bSuccess;
}


VOID DoFileOpen(HWND hwnd)
{
	// Создадим стандартное окно открытия файла:
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = szFilter; // filter
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH; // максим. длинна пути
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	//ofn.lpstrDefExt = "txt";
	if (GetOpenFileName(&ofn))
	{
		// Происходит загрузка файла...
		HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
		LoadTextFileToEdit(hEdit, szFileName);
	}
}

VOID DoFileSaveAs(HWND hwnd)
{
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = "txt";

	if (GetSaveFileName(&ofn))
	{
		HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
		SaveTextFileFromEdit(hEdit, szFileName);
	}
}

BOOL FileChanged(HWND hEdit)
{
	BOOL bFileWasChanged = FALSE;
	DWORD dwCurrentTextLength = GetWindowTextLength(hEdit);
	DWORD dwFileTextLength = pszFileText ? strlen(pszFileText) : 0;
	if (dwCurrentTextLength != dwFileTextLength)bFileWasChanged = TRUE;
	else
	{
		LPSTR pszCurrentText = (LPSTR)GlobalAlloc(GPTR, dwCurrentTextLength + 1);
		GetWindowTextA(hEdit, pszCurrentText, dwCurrentTextLength + 1);
		if (pszText && strcmp(pszFileText, pszCurrentText))bFileWasChanged = TRUE;
		GlobalFree(pszCurrentText);
	}
	return bFileWasChanged;
}