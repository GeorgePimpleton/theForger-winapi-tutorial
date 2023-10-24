#include <windows.h>

#include "resource.h"

const WCHAR g_szClassName[ ] = L"myWindowClass";

HWND g_hToolbar = NULL;

BOOL CALLBACK ToolDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch (Message)
   {
   case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      case IDC_PRESS:
         MessageBoxW(hwnd, L"Hi!", L"This is a message",
                     MB_OK | MB_ICONEXCLAMATION);
         break;

      case IDC_OTHER:
         MessageBoxW(hwnd, L"Bye!", L"This is also a message",
                     MB_OK | MB_ICONEXCLAMATION);
         break;
      }
      break;

   default:
      return FALSE;
   }
   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch (Message)
   {
   case WM_CREATE:
      g_hToolbar = CreateDialogW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDD_TOOLBAR),
                                 hwnd, (DLGPROC) ToolDlgProc);
      if (g_hToolbar != NULL)
      {
         ShowWindow(g_hToolbar, SW_SHOW);
      }
      else
      {
         MessageBoxW(hwnd, L"CreateDialog returned NULL", L"Warning!",
                    MB_OK | MB_ICONINFORMATION);
      }
      break;

   case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      case ID_FILE_EXIT:
         PostMessageW(hwnd, WM_CLOSE, 0, 0);
         break;

      case ID_DIALOG_SHOW:
         ShowWindow(g_hToolbar, SW_SHOW);
         break;

      case ID_DIALOG_HIDE:
         ShowWindow(g_hToolbar, SW_HIDE);
         break;
      }
      break;

   case WM_CLOSE:
      DestroyWindow(hwnd);
      break;

   case WM_DESTROY:
      DestroyWindow(g_hToolbar);
      PostQuitMessage(0);
      break;
   default:
      return DefWindowProc(hwnd, Message, wParam, lParam);
   }
   return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                    _In_ PWSTR lpCmdLine, _In_ int nCmdShow)
{
   WNDCLASSEXW wc;
   HWND hwnd;
   MSG Msg;

   wc.cbSize        = sizeof(WNDCLASSEXW);
   wc.style         = 0;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hIconSm       = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
   wc.lpszMenuName  = MAKEINTRESOURCEW(IDR_MYMENU);
   wc.lpszClassName = g_szClassName;

   if (!RegisterClassExW(&wc))
   {
      MessageBoxW(NULL, L"Window Registration Failed!", L"Error!",
                  MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   hwnd = CreateWindowExW(WS_EX_CLIENTEDGE,
                          g_szClassName,
                          L"The title of my window",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
                          NULL, NULL, hInstance, NULL);

   if (hwnd == NULL)
   {
      MessageBoxW(NULL, L"Window Creation Failed!", L"Error!",
                  MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   ShowWindow(hwnd, nCmdShow);
   UpdateWindow(hwnd);

   while (GetMessageW(&Msg, NULL, 0, 0) > 0)
   {
      if (!IsDialogMessageW(g_hToolbar, &Msg))
      {
         TranslateMessage(&Msg);
         DispatchMessageW(&Msg);
      }
   }
   return (int) Msg.wParam;
}
