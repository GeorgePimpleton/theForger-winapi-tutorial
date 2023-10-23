#include <windows.h>
#include <tchar.h>

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, PTSTR pszCmdLine, int iCmdShow)
{
	MessageBox(NULL, TEXT("Narf!"), TEXT("Pinky says..."), MB_OK | MB_ICONEXCLAMATION);
	return 0;
}