#include <windows.h>

#include "resource.h"

PCWSTR g_className = L"myWindowClass";

BOOL CALLBACK AboutDlgProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch ( msg )
   {
   case WM_INITDIALOG:
      return TRUE;

   case WM_COMMAND:
      switch ( LOWORD(wParam) )
      {
      case IDOK:
         EndDialog(wnd, IDOK);
         break;

      case IDCANCEL:
         EndDialog(wnd, IDCANCEL);
         break;
      }
      break;

   default:
      return FALSE;
   }
   return TRUE;
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch ( msg )
   {
   case WM_COMMAND:
      switch ( LOWORD(wParam) )
      {
      case ID_FILE_EXIT:
         PostMessageW(wnd, WM_CLOSE, 0, 0);
         break;

      case ID_HELP_ABOUT:
      {
         int ret = (int) DialogBoxParamW(GetModuleHandleW(NULL),
                                         MAKEINTRESOURCEW(IDD_ABOUT), wnd, (DLGPROC) AboutDlgProc, 0L);

         if ( ret == IDOK )
         {
            MessageBoxW(wnd, L"Dialog exited with IDOK.", L"Notice",
                        MB_OK | MB_ICONINFORMATION);
         }
         else if ( ret == IDCANCEL )
         {
            MessageBoxW(wnd, L"Dialog exited with IDCANCEL.", L"Notice",
                        MB_OK | MB_ICONINFORMATION);
         }
         else if ( ret == -1 )
         {
            MessageBoxW(wnd, L"Dialog failed!", L"Error",
                        MB_OK | MB_ICONINFORMATION);
         }
      }
      break;
      }
      break;

   case WM_CLOSE:
      DestroyWindow(wnd);
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      break;

   default:
      return DefWindowProcW(wnd, msg, wParam, lParam);
   }
   return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE inst, _In_opt_ HINSTANCE prevInst,
                    _In_ PWSTR cmdLine, _In_ int cmdShow)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   WNDCLASSEXW wc = { 0 };
   HWND        wnd;
   MSG         msg;

   wc.cbSize        = sizeof(WNDCLASSEX);
   wc.style         = 0;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = MAKEINTRESOURCEW(IDR_MYMENU);
   wc.lpszClassName = g_className;
   wc.hIconSm       = (HICON) LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);;

   if ( !RegisterClassExW(&wc) )
   {
      MessageBoxW(NULL, L"Window Registration Failed!", L"Error!",
                  MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   wnd = CreateWindowExW(WS_EX_CLIENTEDGE,
                         g_className,
                         L"The title of my window",
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
                         NULL, NULL, inst, NULL);

   if ( wnd == NULL )
   {
      MessageBoxW(NULL, L"Window Creation Failed!", L"Error!",
                  MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   ShowWindow(wnd, cmdShow);
   UpdateWindow(wnd);

   while ( GetMessageW(&msg, NULL, 0, 0) > 0 )
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
   return (int) msg.wParam;
}
