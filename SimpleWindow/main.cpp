#include<Windows.h>
#include"resource.h"

CONST CHAR szFilter[] = "Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0"; // cconst filter
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL	CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL	LoadTextFileToEdit(HWND hwnd, LPCTSTR pszFileName);
BOOL	SaveTextFileFromEdit(HWND hwnd, LPCSTR pszFileName);
//LPSTR pszText;
LPSTR pszFileText;
CHAR szFileName[MAX_PATH]{}; // Глобальная переменная имени файла

BOOL __stdcall DoFileOpen(HWND hwnd); // _stdcall ф-кция очищает стек
VOID DoFileSaveAs(HWND hwnd);

BOOL FileChanged(HWND hEdit);

VOID WatchChanges(HWND hwnd, BOOL(__stdcall* Action)(HWND))
{
	if (FileChanged(GetDlgItem(hwnd, IDC_MAIN_EDIT)))
	{
		switch (MessageBox(hwnd, "Сохранить изменения в файле?", "Не так быстро...", MB_YESNOCANCEL | MB_ICONQUESTION))
		{
		case IDYES: SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVE, 0);
		case IDNO: Action(hwnd);
		case IDCANCEL: break;
		}
	}
	else
		Action(hwnd);
}

VOID SetWindowTitle(HWND hEdit)
{
	CHAR szTitle[MAX_PATH] = "SimpleWindowEdit - ";
	LPSTR lpszNameOnly = strrchr(szFileName, '\\') + 1;
	strcat_s(szTitle, MAX_PATH, lpszNameOnly);
	HWND hwndParent = GetParent(hEdit);
	SetWindowText(hwndParent, szTitle);
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	if (lpCmdLine[0])
	{
		//strcpy_s(szFileName, MAX_PATH, lpCmdLine);
		for (int i = 0, j = 0; lpCmdLine[i]; i++)
		{
			if (lpCmdLine[i] != '\"')szFileName[j++] = lpCmdLine[i];
		}
	}
	//1) Регистрация класса окна:
	CONST CHAR SZ_CLASS_NAME[] = "myWindowClass";
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	//wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	//wc.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;//MAKEINTRESOURCE(IDR_MENU1);
	wc.lpszClassName = SZ_CLASS_NAME;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window registration failed", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	//2) Создание окна:
	HWND hwnd = CreateWindowEx
	(
		WS_EX_CLIENTEDGE | WS_EX_ACCEPTFILES,
		SZ_CLASS_NAME,
		"Title of my Window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
		NULL, NULL, hInstance, NULL
	);

	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window was not created", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	MessageBox(hwnd, lpCmdLine, "Info", MB_OK | MB_ICONINFORMATION);
	if (lpCmdLine[0])
	{
		strcpy_s(szFileName, sizeof(szFileName), lpCmdLine);
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	//3) Message loop (цикл сообщений):
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

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
		AppendMenu(hSubMenu, MF_SEPARATOR, 0, 0);
		AppendMenu(hSubMenu, MF_STRING, ID_FILE_OPEN, "&Open");
		AppendMenu(hSubMenu, MF_SEPARATOR, 0, 0);
		AppendMenu(hSubMenu, MF_STRING, ID_FILE_SAVE, "&Save");
		AppendMenu(hSubMenu, MF_STRING, ID_FILE_SAVEAS, "&SaveAs");
		AppendMenu(hSubMenu, MF_SEPARATOR, 0, 0);
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
		if (szFileName[0])
			LoadTextFileToEdit(hEdit, szFileName);

		// Красивый шрифт для написания текста в окне
		HFONT hDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hDefault, MAKELPARAM(FALSE, 0));

		if (szFileName[0])
		{
			LoadTextFileToEdit(GetDlgItem(hwnd, IDC_MAIN_EDIT), szFileName);
		}
		/////////////////////////////////////////////////////////////////
		///////////////	HOTKEYS	/////////////////////////////////////////
		/////////////////////////////////////////////////////////////////
		RegisterHotKey(hwnd, HOTKEY_NEW, MOD_CONTROL, 'N');
		RegisterHotKey(hwnd, HOTKEY_OPEN, MOD_CONTROL, 'O');
		RegisterHotKey(hwnd, HOTKEY_SAVE, MOD_CONTROL, 'S');
		RegisterHotKey(hwnd, HOTKEY_SAVEAS, MOD_CONTROL + MOD_ALT, 'S');
		RegisterHotKey(hwnd, HOTKEY_ABOUT,0, VK_F1);
		/////////////////////////////////////////////////////////////////
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
	case WM_DROPFILES:
	{
		HDROP hDrop = (HDROP)wParam;
		DragQueryFile(hDrop, 0, szFileName, MAX_PATH);
		LoadTextFileToEdit(GetDlgItem(hwnd, IDC_MAIN_EDIT), szFileName);
		DragFinish(hDrop);
	}
	break;
	case WM_HOTKEY:
		switch (wParam)
		{
		case HOTKEY_NEW:SendMessage(hwnd, WM_COMMAND, ID_FILE_NEW, 0); break;
		case HOTKEY_OPEN:SendMessage(hwnd, WM_COMMAND, ID_FILE_OPEN, 0); break;
		case HOTKEY_SAVE:SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVE, 0); break;
		case HOTKEY_SAVEAS:SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVEAS, 0); break;
		case HOTKEY_ABOUT:SendMessage(hwnd, WM_COMMAND, ID_HELP_ABOUT, 0); break;
	
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_OPEN:
		{
			WatchChanges(hwnd, DoFileOpen);
		}
		break;
		case ID_FILE_SAVE:
		{
			/*CHAR szFileName[MAX_PATH]{};
			if (szFileName[0])
				SaveTextFileFromEdit(GetDlgItem(hwnd, IDC_MAIN_EDIT), szFileName);
			else
				SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVEAS, 0);*/
			if (strlen(szFileName))
			{
				SaveTextFileFromEdit(GetDlgItem(hwnd, IDC_MAIN_EDIT), szFileName);
			}
			else
			{
				SendMessage(hwnd, WM_COMMAND, ID_FILE_SAVEAS, 0);
			}
		}
		break;
		case ID_FILE_SAVEAS:
		{
			DoFileSaveAs(hwnd);
		}
		break;
		case ID_FILE_EXIT:
			//DestroyWindow(hwnd);
			SendMessage(hwnd, WM_CLOSE, 0, 0);
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
		WatchChanges(hwnd, DestroyWindow);
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

	HANDLE hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE) //Если файл создался продолжаем работу сним
	{
		DWORD dwFileSize = GetFileSize(hFile, NULL);
		if (dwFileSize != UINT_MAX)
		{
			if (pszFileText)GlobalFree(pszFileText);
			pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if (pszFileText)
			{
				DWORD dwRead;

				if (ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
				{
					pszFileText[dwFileSize] = 0;
					if (SetWindowText(hEdit, pszFileText))
					{
						bSuccess = TRUE;
						SetWindowTitle(hEdit);
					}
				}

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
		DWORD dwTextLength = GetWindowTextLength(hEdit);
		if (dwTextLength > 0)
		{
			if (pszFileText)GlobalFree(pszFileText);
			pszFileText = (LPSTR)GlobalAlloc(GPTR, dwTextLength + 1);
			if (pszFileText != NULL)
			{
				if (GetWindowText(hEdit, pszFileText, dwTextLength + 1))
				{
					DWORD dwWritten;
					if (WriteFile(hFile, pszFileText, dwTextLength, &dwWritten, NULL))
					{
						bSuccess = TRUE;
						SetWindowTitle(hEdit);
					}
				}
			}
			CloseHandle(hFile);
		}
	}
	return bSuccess;
}


BOOL __stdcall DoFileOpen(HWND hwnd)
{
	// Создадим стандартное окно открытия файла:
	OPENFILENAME ofn;
	//CHAR szFileName[MAX_PATH]{};

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
		return TRUE;
	}
	return FALSE;
}

VOID DoFileSaveAs(HWND hwnd)
{
	OPENFILENAME ofn;
	//CHAR szFileName[MAX_PATH]{};

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
	/////////////// Точно рабочий вариант////////////////////////
	/*BOOL bFileWasChanged = FALSE;
	DWORD dwCurrentTextLength = GetWindowTextLength(hEdit);
	DWORD dwFileTextLength = pszFileText ? strlen(pszFileText) : 0;
	if (dwCurrentTextLength != dwFileTextLength)bFileWasChanged = TRUE;
	else
	{
		LPSTR pszCurrentText = (LPSTR)GlobalAlloc(GPTR, dwCurrentTextLength + 1);
		GetWindowTextA(hEdit, pszCurrentText, dwCurrentTextLength + 1);
		if (pszFileText && strcmp(pszFileText, pszCurrentText))bFileWasChanged = TRUE;
		GlobalFree(pszCurrentText);
	}
	return bFileWasChanged;*/
	///////////////////////////////////////

	BOOL bFileWasChanged = FALSE;
	DWORD dwCurrentTextLength = GetWindowTextLength(hEdit);
	DWORD dwFileTextLength = pszFileText ? strlen(pszFileText) : 0;
	if (dwCurrentTextLength != dwFileTextLength)bFileWasChanged = TRUE;
	else
	{
		//	Если строки совпадают по размеру, то их нужно сравнивать
		//	1. Выделяем память, для текста в редакторе:
		LPSTR pszCurrentText = (LPSTR)GlobalAlloc(GPTR, dwCurrentTextLength + 1);
		//	2. Загружаем текст из редактора в строку:
		GetWindowTextA(hEdit, pszCurrentText, dwCurrentTextLength + 1);
		//	3. Сравниваем переменную и строки
		if (pszFileText && strcmp(pszFileText, pszCurrentText))bFileWasChanged = TRUE;
		//	4. Очищаем память, которая использовалась для текста в редакторе
		GlobalFree(pszCurrentText);
	}
	return bFileWasChanged;
}