#include <windows.h>
#include "resource.h"

const WCHAR g_szClassName[] = L"myWindowClass";
const int   ID_TIMER        = 1;

const int   BALL_MOVE_DELTA = 2;

typedef struct _BALLINFO
{
   int width;
   int height;
   int x;
   int y;

   int dx;
   int dy;
} BALLINFO;

BALLINFO g_ballInfo;
HBITMAP  g_hbmBall = NULL;
HBITMAP  g_hbmMask = NULL;

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

void DrawBall(HDC hdc, RECT* prc)
{
   HDC hdcBuffer        = CreateCompatibleDC(hdc);
   HBITMAP hbmBuffer    = CreateCompatibleBitmap(hdc, prc->right, prc->bottom);
   HBITMAP hbmOldBuffer = (HBITMAP) SelectObject(hdcBuffer, hbmBuffer);

   HDC hdcMem     = CreateCompatibleDC(hdc);
   HBITMAP hbmOld = (HBITMAP) SelectObject(hdcMem, g_hbmMask);

   FillRect(hdcBuffer, prc, (HBRUSH) GetStockObject(WHITE_BRUSH));

   BitBlt(hdcBuffer, g_ballInfo.x, g_ballInfo.y, g_ballInfo.width, g_ballInfo.height, hdcMem, 0, 0, SRCAND);

   SelectObject(hdcMem, g_hbmBall);
   BitBlt(hdcBuffer, g_ballInfo.x, g_ballInfo.y, g_ballInfo.width, g_ballInfo.height, hdcMem, 0, 0, SRCPAINT);

   BitBlt(hdc, 0, 0, prc->right, prc->bottom, hdcBuffer, 0, 0, SRCCOPY);

   SelectObject(hdcMem, hbmOld);
   DeleteDC(hdcMem);

   SelectObject(hdcBuffer, hbmOldBuffer);
   DeleteDC(hdcBuffer);
   DeleteObject(hbmBuffer);
}

void UpdateBall(RECT* prc)
{
   g_ballInfo.x += g_ballInfo.dx;
   g_ballInfo.y += g_ballInfo.dy;

   if ( g_ballInfo.x < 0 )
   {
      g_ballInfo.x  = 0;
      g_ballInfo.dx = BALL_MOVE_DELTA;
   }
   else if ( g_ballInfo.x + g_ballInfo.width > prc->right )
   {
      g_ballInfo.x  = prc->right - g_ballInfo.width;
      g_ballInfo.dx = -BALL_MOVE_DELTA;
   }

   if ( g_ballInfo.y < 0 )
   {
      g_ballInfo.y  = 0;
      g_ballInfo.dy = BALL_MOVE_DELTA;
   }
   else if ( g_ballInfo.y + g_ballInfo.height > prc->bottom )
   {
      g_ballInfo.y  = prc->bottom - g_ballInfo.height;
      g_ballInfo.dy = -BALL_MOVE_DELTA;
   }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch ( msg )
   {
   case WM_CREATE:
   {
      UINT ret;
      BITMAP bm;

      g_hbmBall = (HBITMAP) LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDB_BALL),
                                       IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
      if ( g_hbmBall == NULL )
         MessageBoxW(hwnd, L"Could not load IDB_BALL!", L"Error", MB_OK | MB_ICONEXCLAMATION);

      g_hbmMask = CreateBitmapMask(g_hbmBall, RGB(0, 0, 0));
      if ( g_hbmMask == NULL )
         MessageBoxW(hwnd, L"Could not create mask!", L"Error", MB_OK | MB_ICONEXCLAMATION);

      #pragma warning (disable : 6387)
      GetObjectW(g_hbmBall, sizeof(bm), &bm);

      ZeroMemory(&g_ballInfo, sizeof(g_ballInfo));
      g_ballInfo.width = bm.bmWidth;
      g_ballInfo.height = bm.bmHeight;

      g_ballInfo.dx = BALL_MOVE_DELTA;
      g_ballInfo.dy = BALL_MOVE_DELTA;

      ret = (UINT) SetTimer(hwnd, ID_TIMER, 50, NULL);
      if ( ret == 0 )
         MessageBoxW(hwnd, L"Could not SetTimer()!", L"Error", MB_OK | MB_ICONEXCLAMATION);
   }
   break;

   case WM_CLOSE:
      DestroyWindow(hwnd);
      break;

   case WM_PAINT:
   {
      RECT        rcClient;
      PAINTSTRUCT ps;
      HDC         hdc = BeginPaint(hwnd, &ps);

      GetClientRect(hwnd, &rcClient);
      DrawBall(hdc, &rcClient);

      EndPaint(hwnd, &ps);
   }
   break;

   case WM_TIMER:
   {
      RECT rcClient;
      HDC  hdc = GetDC(hwnd);

      GetClientRect(hwnd, &rcClient);

      UpdateBall(&rcClient);
      DrawBall(hdc, &rcClient);

      ReleaseDC(hwnd, hdc);
   }
   break;

   case WM_DESTROY:
      KillTimer(hwnd, ID_TIMER);

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
                          L"An Animation Program",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 320, 240,
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