#include <windows.h>

#include "resource.h"

HBRUSH g_hbrBackground = NULL;

BOOL CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch (Message)
   {
   case WM_INITDIALOG:
      g_hbrBackground = CreateSolidBrush(RGB(55, 55, 55));

      SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM) LoadIconW(NULL, MAKEINTRESOURCEW(IDI_APPLICATION)));
      SendMessageW(hwnd, WM_SETICON, ICON_BIG,   (LPARAM) LoadIconW(NULL, MAKEINTRESOURCEW(IDI_APPLICATION)));
      break;

   case WM_CLOSE:
      EndDialog(hwnd, 0);
      break;

   case WM_CTLCOLORDLG:
      return (LONG) g_hbrBackground;

   case WM_CTLCOLORSTATIC:
   {
      HDC hdcStatic = (HDC) wParam;
      SetTextColor(hdcStatic, RGB(255, 255, 255));
      SetBkMode(hdcStatic, TRANSPARENT);
      return (LONG) (HBRUSH) g_hbrBackground;
   }
   break;

   case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      case IDOK:
         EndDialog(hwnd, 0);
         break;
      }
      break;

   case WM_DESTROY:
      DeleteObject(g_hbrBackground);
      break;

   default:
      return FALSE;
   }
   return TRUE;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                    _In_ PWSTR lpCmdLine, _In_ int nCmdShow)
{
   return (int) DialogBoxW(hInstance, MAKEINTRESOURCEW(IDD_MAIN), NULL, (DLGPROC) DlgProc);
}
