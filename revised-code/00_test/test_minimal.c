#include <windows.h>

int WINAPI
WinMain(HINSTANCE inst, HINSTANCE prevInst, LPSTR CmdLine, int cmdShow)
{
	MessageBox(NULL, TEXT("Narf!"), TEXT("Pinky says..."), MB_OK | MB_ICONEXCLAMATION);
	return 0;
}