#include <windows.h>
#include "resource.h"

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

BALLINFO ballInfo;
HBITMAP  bmBall = NULL;
HBITMAP  bmMask = NULL;

HBITMAP CreateBitmapMask(HBITMAP bmColor, COLORREF transparent)
{
   HDC     dcMem;
   HDC     dcMem2;
   HBITMAP bmMask;
   BITMAP  bm;

   GetObjectW(bmColor, sizeof(BITMAP), &bm);
   bmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

   dcMem  = CreateCompatibleDC(0);
   dcMem2 = CreateCompatibleDC(0);

   SelectObject(dcMem, bmColor);
   SelectObject(dcMem2, bmMask);

   SetBkColor(dcMem, transparent);

   BitBlt(dcMem2, 0, 0, bm.bmWidth, bm.bmHeight, dcMem, 0, 0, SRCCOPY);

   BitBlt(dcMem, 0, 0, bm.bmWidth, bm.bmHeight, dcMem2, 0, 0, SRCINVERT);

   DeleteDC(dcMem);
   DeleteDC(dcMem2);

   return bmMask;
}

void DrawBall(HDC dc, RECT* rect)
{
   HDC     dcBuffer    = CreateCompatibleDC(dc);
   HBITMAP bmBuffer    = CreateCompatibleBitmap(dc, rect-> right, rect-> bottom);
   HBITMAP bmOldBuffer = (HBITMAP) SelectObject(dcBuffer, bmBuffer);
   HDC     dcMem       = CreateCompatibleDC(dc);
   HBITMAP bmOld       = (HBITMAP) SelectObject(dcMem, bmMask);

   FillRect(dcBuffer, rect, (HBRUSH) GetStockObject(WHITE_BRUSH));

   BitBlt(dcBuffer, ballInfo.x, ballInfo.y, ballInfo.width, ballInfo.height, dcMem, 0, 0, SRCAND);

   SelectObject(dcMem, bmBall);
   BitBlt(dcBuffer, ballInfo.x, ballInfo.y, ballInfo.width, ballInfo.height, dcMem, 0, 0, SRCPAINT);

   BitBlt(dc, 0, 0, rect->right, rect->bottom, dcBuffer, 0, 0, SRCCOPY);

   SelectObject(dcMem, bmOld);
   DeleteDC(dcMem);

   SelectObject(dcBuffer, bmOldBuffer);
   DeleteDC(dcBuffer);
   DeleteObject(bmBuffer);
}

void UpdateBall(RECT* rect)
{
   ballInfo.x += ballInfo.dx;
   ballInfo.y += ballInfo.dy;

   if ( ballInfo.x < 0 )
   {
      ballInfo.x  = 0;
      ballInfo.dx = BALL_MOVE_DELTA;
   }
   else if ( ballInfo.x + ballInfo.width > rect->right )
   {
      ballInfo.x  = rect->right - ballInfo.width;
      ballInfo.dx = -BALL_MOVE_DELTA;
   }

   if ( ballInfo.y < 0 )
   {
      ballInfo.y  = 0;
      ballInfo.dy = BALL_MOVE_DELTA;
   }
   else if ( ballInfo.y + ballInfo.height > rect->bottom )
   {
      ballInfo.y  = rect->bottom - ballInfo.height;
      ballInfo.dy = -BALL_MOVE_DELTA;
   }
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch ( msg )
   {
   case WM_CREATE:
   {
      UINT   ret;
      BITMAP bm;

      bmBall = (HBITMAP) LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDB_BALL),
                                    IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);

      if ( bmBall == NULL )
      {
         MessageBoxW(wnd, L"Could not load IDB_BALL!", L"Error", MB_OK | MB_ICONEXCLAMATION);
      }

      bmMask = CreateBitmapMask(bmBall, RGB(0, 0, 0));

      if ( bmMask == NULL )
      {
         MessageBoxW(wnd, L"Could not create mask!", L"Error", MB_OK | MB_ICONEXCLAMATION);
      }

      #pragma warning (disable : 6387)
      GetObjectW(bmBall, sizeof(bm), &bm);

      ZeroMemory(&ballInfo, sizeof(ballInfo));
      ballInfo.width  = bm.bmWidth;
      ballInfo.height = bm.bmHeight;

      ballInfo.dx = BALL_MOVE_DELTA;
      ballInfo.dy = BALL_MOVE_DELTA;

      ret = (UINT) SetTimer(wnd, ID_TIMER, 15, NULL);

      if ( ret == 0 )
      {
         MessageBoxW(wnd, L"Could not SetTimer()!", L"Error", MB_OK | MB_ICONEXCLAMATION);
      }
   }
   break;

   case WM_CLOSE:
      DestroyWindow(wnd);
      break;

   case WM_PAINT:
   {
      RECT        client;
      PAINTSTRUCT ps;
      HDC         dc = BeginPaint(wnd, &ps);

      GetClientRect(wnd, &client);
      DrawBall(dc, &client);

      EndPaint(wnd, &ps);
   }
   break;

   case WM_TIMER:
   {
      RECT client;
      HDC  dc = GetDC(wnd);

      GetClientRect(wnd, &client);

      UpdateBall(&client);
      DrawBall(dc, &client);

      ReleaseDC(wnd, dc);
   }
   break;

   case WM_DESTROY:
      KillTimer(wnd, ID_TIMER);

      DeleteObject(bmBall);
      DeleteObject(bmMask);

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
                         L"An Animation Program",
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, 320, 240,
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
