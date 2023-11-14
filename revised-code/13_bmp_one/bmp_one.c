#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static HBITMAP ball = NULL;

   switch ( msg )
   {
   case WM_CREATE:
      ball = (HBITMAP) LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDB_BALL), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);

      if ( ball == NULL )
      {
         MessageBoxW(hwnd, L"Could not load IDB_BALL!", L"Error", MB_OK | MB_ICONEXCLAMATION);
      }
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

      HDC dc = BeginPaint(hwnd, &ps);

      HDC     dcMem = CreateCompatibleDC(dc);
      HBITMAP bmOld = (HBITMAP) SelectObject(dcMem, ball);

      GetObjectW(ball, sizeof(bm), &bm);

      BitBlt(dc, 0, 0, bm.bmWidth, bm.bmHeight, dcMem, 0, 0, SRCCOPY);

      SelectObject(dcMem, bmOld);
      DeleteDC(dcMem);

      EndPaint(hwnd, &ps);
   }
   break;

   case WM_DESTROY:
      DeleteObject(ball);
      PostQuitMessage(0);
      break;

   default:
      return DefWindowProcW(hwnd, msg, wParam, lParam);
   }
   return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE inst, _In_opt_ HINSTANCE prevInst,
                    _In_ PWSTR cmdLine, _In_ int cmdShow)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   WNDCLASSEXW wc        = { 0 };
   HWND        wnd;
   MSG         msg;
   PCWSTR      className = L"myWindowClass";

   wc.cbSize        = sizeof(WNDCLASSEXW);
   wc.style         = 0;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hIconSm       = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = className;

   if ( !RegisterClassExW(&wc) )
   {
      MessageBoxW(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   wnd = CreateWindowExW(WS_EX_CLIENTEDGE,
                         className,
                         L"A Bitmap Program",
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
                         NULL, NULL, inst, NULL);

   if ( wnd == NULL )
   {
      MessageBoxW(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
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
