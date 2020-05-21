#pragma once
#include<Windows.h>
#include"resource.h"

CONST CHAR szFilter[] = "Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0"; // cconst filter
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL	CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL	LoadTextFileToEdit(HWND hwnd, LPCTSTR pszFileName);
BOOL	SaveTextFileFromEdit(HWND hwnd, LPCSTR pszFileName);
extern CHAR szFileName[MAX_PATH];
extern LPSTR pszText;
extern LPSTR pszFileText;

VOID DoFileOpen(HWND hwnd);
VOID DoFileSaveAs(HWND hwnd);

BOOL FileChanged(HWND hEdit);
