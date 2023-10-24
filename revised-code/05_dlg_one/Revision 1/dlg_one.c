#include <windows.h>
#include <tchar.h>

#include "resource.h"

const TCHAR g_szClassName[ ] = TEXT("myWindowClass");

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch ( Message )
   {
   case WM_INITDIALOG:

      return TRUE;
   case WM_COMMAND:
      switch ( LOWORD(wParam) )
      {
      case IDOK:
         EndDialog(hwnd, IDOK);
         break;

      case IDCANCEL:
         EndDialog(hwnd, IDCANCEL);
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
   switch ( Message )
   {
   case WM_COMMAND:
      switch ( LOWORD(wParam) )
      {
      case ID_FILE_EXIT:
         PostMessage(hwnd, WM_CLOSE, 0, 0);
         break;

      case ID_HELP_ABOUT:
      {
         int ret = DialogBox(GetModuleHandle(NULL),
                             MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlgProc);

         if ( ret == IDOK )
         {
            MessageBox(hwnd, TEXT("Dialog exited with IDOK."), TEXT("Notice"),
                       MB_OK | MB_ICONINFORMATION);
         }
         else if ( ret == IDCANCEL )
         {
            MessageBox(hwnd, TEXT("Dialog exited with IDCANCEL."), TEXT("Notice"),
                       MB_OK | MB_ICONINFORMATION);
         }
         else if ( ret == -1 )
         {
            MessageBox(hwnd, TEXT("Dialog failed!"), TEXT("Error"),
                       MB_OK | MB_ICONINFORMATION);
         }
      }
      break;
      }
      break;

   case WM_CLOSE:
      DestroyWindow(hwnd);
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      break;

   default:
      return DefWindowProc(hwnd, Message, wParam, lParam);
   }
   return 0;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     PTSTR lpCmdLine, int nCmdShow)
{
   WNDCLASSEX wc;
   HWND       hwnd;
   MSG        Msg;

   wc.cbSize        = sizeof(WNDCLASSEX);
   wc.style         = 0;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance;
   wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
   wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
   wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MYMENU);
   wc.lpszClassName = g_szClassName;
   wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

   if ( !RegisterClassEx(&wc) )
   {
      MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("Error!"),
                 MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,
                         g_szClassName, TEXT("The title of my window"),
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
                         NULL, NULL, hInstance, NULL);

   if ( hwnd == NULL )
   {
      MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error!"),
                 MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   ShowWindow(hwnd, nCmdShow);
   UpdateWindow(hwnd);

   while ( GetMessage(&Msg, NULL, 0, 0) > 0 )
   {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
   }
   return Msg.wParam;
}