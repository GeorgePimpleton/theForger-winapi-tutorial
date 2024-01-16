#include <windows.h>

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       cmdShow)
{
   UNREFERENCED_PARAMETER(inst);
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);
   UNREFERENCED_PARAMETER(cmdShow);

   MessageBoxW(NULL, L"Narf!", L"Pinky says...", MB_OK | MB_ICONEXCLAMATION);

   return 0;
}
