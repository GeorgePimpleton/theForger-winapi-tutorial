#include <windows.h>
#include "resource.h"

HBITMAP CreateBitmapMask(HBITMAP bmColour, COLORREF transparent)
{
   HDC     dcMem;
   HDC     dcMem2;
   HBITMAP bmMask;
   BITMAP  bm;

   GetObjectW(bmColour, sizeof(BITMAP), &bm);
   bmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

   dcMem  = CreateCompatibleDC(0);
   dcMem2 = CreateCompatibleDC(0);

   SelectObject(dcMem, bmColour);
   SelectObject(dcMem2, bmMask);

   SetBkColor(dcMem, transparent);

   BitBlt(dcMem2, 0, 0, bm.bmWidth, bm.bmHeight, dcMem, 0, 0, SRCCOPY);

   BitBlt(dcMem, 0, 0, bm.bmWidth, bm.bmHeight, dcMem2, 0, 0, SRCINVERT);

   DeleteDC(dcMem);
   DeleteDC(dcMem2);

   return bmMask;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static HBITMAP bmBall = NULL;
   static HBITMAP bmMask = NULL;

   switch ( msg )
   {
   case WM_CREATE:
      bmBall = (HBITMAP) LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDB_BALL), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);

      if ( bmBall == NULL )
      {
         MessageBoxW(hwnd, L"Could not load IDB_BALL!", L"Error", MB_OK | MB_ICONEXCLAMATION);
      }

      bmMask = CreateBitmapMask(bmBall, RGB(0, 0, 0));

      if ( bmMask == NULL )
      {
         MessageBoxW(hwnd, L"Could not create mask!", L"Error", MB_OK | MB_ICONEXCLAMATION);
      }
      break;

   case WM_CLOSE:
      DestroyWindow(hwnd);
      break;

   case WM_PAINT:
   {
      BITMAP      bm;
      RECT        client;
      PAINTSTRUCT ps;

      HDC dc = BeginPaint(hwnd, &ps);

      HDC     dcMem = CreateCompatibleDC(dc);
      HBITMAP bmOld = (HBITMAP) SelectObject(dcMem, bmMask);

      GetObjectW(bmBall, sizeof(bm), &bm);

      GetClientRect(hwnd, &client);
      FillRect(dc, &client, (HBRUSH) GetStockObject(LTGRAY_BRUSH));

      BitBlt(dc, 0, 0, bm.bmWidth, bm.bmHeight, dcMem, 0, 0, SRCCOPY);
      BitBlt(dc, bm.bmWidth, 0, bm.bmWidth, bm.bmHeight, dcMem, 0, 0, SRCAND);
      BitBlt(dc, bm.bmWidth * 2, bm.bmHeight * 2, bm.bmWidth, bm.bmHeight, dcMem, 0, 0, SRCAND);

      SelectObject(dcMem, bmBall);
      BitBlt(dc, 0, bm.bmHeight, bm.bmWidth, bm.bmHeight, dcMem, 0, 0, SRCCOPY);
      BitBlt(dc, bm.bmWidth, bm.bmHeight, bm.bmWidth, bm.bmHeight, dcMem, 0, 0, SRCPAINT);
      BitBlt(dc, bm.bmWidth * 2, bm.bmHeight * 2, bm.bmWidth, bm.bmHeight, dcMem, 0, 0, SRCPAINT);

      SelectObject(dcMem, bmOld);
      DeleteDC(dcMem);

      EndPaint(hwnd, &ps);
   }
   break;

   case WM_DESTROY:
      DeleteObject(bmBall);
      DeleteObject(bmMask);

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
                         L"Another Bitmap Program",
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, 240, 160,
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
