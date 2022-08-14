#include <windows.h>

int WINAPI wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrev, _In_ PWSTR pszCmdLine, _In_ int iCmdShow)
{
   MessageBoxW(NULL, L"Narf!", L"Pinky says...", MB_OK | MB_ICONEXCLAMATION);
   return 0;
}