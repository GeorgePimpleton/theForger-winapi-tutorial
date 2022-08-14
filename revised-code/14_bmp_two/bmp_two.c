#include <windows.h>
#include "resource.h"

const WCHAR g_szClassName[] = L"myWindowClass";

HBITMAP g_hbmBall = NULL;
HBITMAP g_hbmMask = NULL;

HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
   HDC     hdcMem;
   HDC     hdcMem2;
   HBITMAP hbmMask;
   BITMAP  bm;

   GetObjectW(hbmColour, sizeof(BITMAP), &bm);
   hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

   hdcMem  = CreateCompatibleDC(0);
   hdcMem2 = CreateCompatibleDC(0);

   SelectObject(hdcMem, hbmColour);
   SelectObject(hdcMem2, hbmMask);

   SetBkColor(hdcMem, crTransparent);

   BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

   BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

   DeleteDC(hdcMem);
   DeleteDC(hdcMem2);

   return hbmMask;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch ( msg )
   {
   case WM_CREATE:
      g_hbmBall = (HBITMAP) LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDB_BALL), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
      if ( g_hbmBall == NULL )
         MessageBoxW(hwnd, L"Could not load IDB_BALL!", L"Error", MB_OK | MB_ICONEXCLAMATION);

      g_hbmMask = CreateBitmapMask(g_hbmBall, RGB(0, 0, 0));
      if ( g_hbmMask == NULL )
         MessageBoxW(hwnd, L"Could not create mask!", L"Error", MB_OK | MB_ICONEXCLAMATION);
      break;

   case WM_CLOSE:
      DestroyWindow(hwnd);
      break;

   case WM_PAINT:
   {
      BITMAP      bm;
      RECT        rcClient;
      PAINTSTRUCT ps;

      HDC hdc = BeginPaint(hwnd, &ps);

      HDC hdcMem = CreateCompatibleDC(hdc);
      HBITMAP hbmOld = (HBITMAP) SelectObject(hdcMem, g_hbmMask);

      GetObjectW(g_hbmBall, sizeof(bm), &bm);

      GetClientRect(hwnd, &rcClient);
      FillRect(hdc, &rcClient, (HBRUSH) GetStockObject(LTGRAY_BRUSH));

      BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
      BitBlt(hdc, bm.bmWidth, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCAND);
      BitBlt(hdc, bm.bmWidth * 2, bm.bmHeight * 2, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCAND);

      SelectObject(hdcMem, g_hbmBall);
      BitBlt(hdc, 0, bm.bmHeight, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
      BitBlt(hdc, bm.bmWidth, bm.bmHeight, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCPAINT);
      BitBlt(hdc, bm.bmWidth * 2, bm.bmHeight * 2, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCPAINT);

      SelectObject(hdcMem, hbmOld);
      DeleteDC(hdcMem);

      EndPaint(hwnd, &ps);
   }
   break;

   case WM_DESTROY:
      DeleteObject(g_hbmBall);
      DeleteObject(g_hbmMask);

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
                          L"Another Bitmap Program",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 240, 160,
                          NULL, NULL, hInstance, NULL);

   if ( hwnd == NULL )
   {
      MessageBoxW(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
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