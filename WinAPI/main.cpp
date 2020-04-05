#include<Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCCmdline, int nCmdShow)
{
	if (MessageBox(NULL, "Hello WinAPI ;~)", "Header", MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2 | MB_SYSTEMMODAL) == IDOK)
	{
		MessageBox(NULL, "It`s work", "Заголовок 2 окна", MB_OK | MB_ICONQUESTION);
	}
}