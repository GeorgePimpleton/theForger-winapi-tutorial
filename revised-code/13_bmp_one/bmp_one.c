#include <windows.h>
#include "resource.h"

const WCHAR g_szClassName[] = L"myWindowClass";

HBITMAP g_hbmBall = NULL;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch ( msg )
   {
   case WM_CREATE:
      g_hbmBall = (HBITMAP) LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDB_BALL), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
      if ( g_hbmBall == NULL )
         MessageBoxW(hwnd, L"Could not load IDB_BALL!", L"Error", MB_OK | MB_ICONEXCLAMATION);
      break;

   case WM_CLOSE:
      DestroyWindow(hwnd);
      break;

   case WM_PAINT:
   {
      // Just a note, never use a MessageBox from inside WM_PAINT
      // The box will cause more WM_PAINT messages and you'll probably end up
      // stuck in a loop

      BITMAP      bm;
      PAINTSTRUCT ps;

      HDC hdc = BeginPaint(hwnd, &ps);

      HDC hdcMem     = CreateCompatibleDC(hdc);
      HBITMAP hbmOld = (HBITMAP) SelectObject(hdcMem, g_hbmBall);

      GetObjectW(g_hbmBall, sizeof(bm), &bm);

      BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

      SelectObject(hdcMem, hbmOld);
      DeleteDC(hdcMem);

      EndPaint(hwnd, &ps);
   }
   break;

   case WM_DESTROY:
      DeleteObject(g_hbmBall);
      PostQuitMessage(0);
      break;

   default:
      return DefWindowProcW(hwnd, msg, wParam, lParam);
   }
   return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                    _In_ PWSTR lpCmdLine, _In_ int nCmdShow)
{
   WNDCLASSEXW wc;
   HWND        hwnd;
   MSG         Msg;

   wc.cbSize        = sizeof(WNDCLASSEXW);
   wc.style         = 0;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance;
   wc.hIcon         = (HICON) LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hIconSm       = (HICON) LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = g_szClassName;

   if ( !RegisterClassExW(&wc) )
   {
      MessageBoxW(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   hwnd = CreateWindowExW(WS_EX_CLIENTEDGE,
                          g_szClassName,
                          L"A Bitmap Program",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
                          NULL, NULL, hInstance, NULL);

   if ( hwnd == NULL )
   {
      MessageBoxW(NULL, L"Window Creation Failed!", L"Error!",
                  MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   ShowWindow(hwnd, nCmdShow);
   UpdateWindow(hwnd);

   while ( GetMessageW(&Msg, NULL, 0, 0) > 0 )
   {
      TranslateMessage(&Msg);
      DispatchMessageW(&Msg);
   }
   return (int) Msg.wParam;
}