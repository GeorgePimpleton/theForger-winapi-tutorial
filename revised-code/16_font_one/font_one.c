#include <windows.h>

#include "resource.h"

const WCHAR g_szClassName[] = L"myWindowClass";

HFONT    g_hfFont        = NULL;
BOOL     g_bOpaque       = TRUE;
COLORREF g_rgbText       = RGB(0, 0, 0);
COLORREF g_rgbBackground = RGB(55, 55, 55);

COLORREF g_rgbCustom[ 16 ] = { 0 };

void DoSelectFont(HWND hwnd)
{
   CHOOSEFONTW cf = { sizeof(CHOOSEFONTW) };
   LOGFONTW    lf;

   GetObjectW(g_hfFont, sizeof(LOGFONTW), &lf);

   cf.Flags     = CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
   cf.hwndOwner = hwnd;
   cf.lpLogFont = &lf;
   cf.rgbColors = g_rgbText;

   if ( ChooseFontW(&cf) )
   {
      HFONT hf = CreateFontIndirectW(&lf);
      if ( hf )
      {
         g_hfFont = hf;
      }
      else
      {
         MessageBoxW(hwnd, L"Font creation failed!", L"Error", MB_OK | MB_ICONEXCLAMATION);
      }

      g_rgbText = cf.rgbColors;
   }
}

void DoSelectColour(HWND hwnd)
{
   CHOOSECOLORW cc = { sizeof(CHOOSECOLORW) };

   cc.Flags        = CC_RGBINIT | CC_FULLOPEN | CC_ANYCOLOR;
   cc.hwndOwner    = hwnd;
   cc.rgbResult    = g_rgbBackground;
   cc.lpCustColors = g_rgbCustom;

   if ( ChooseColorW(&cc) )
   {
      g_rgbBackground = cc.rgbResult;
   }
}

void DrawClientSize(HDC hdc, RECT* prc, HFONT hf)
{
   WCHAR szSize[ 100 ];
   WCHAR szTitle[] = L"These are the dimensions of your client area:";
   HFONT hfOld     = (HFONT) SelectObject(hdc, hf);

   SetBkColor(hdc, g_rgbBackground);
   SetTextColor(hdc, g_rgbText);

   if ( g_bOpaque )
   {
      SetBkMode(hdc, OPAQUE);
   }
   else
   {
      SetBkMode(hdc, TRANSPARENT);
   }

   DrawTextW(hdc, szTitle, -1, prc, DT_WORDBREAK);

   wsprintfW(szSize, L"{%d, %d, %d, %d}", prc->left, prc->top, prc->right, prc->bottom);

   DrawTextW(hdc, szSize, -1, prc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

   SelectObject(hdc, hfOld);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch ( msg )
   {
   case WM_CREATE:
      g_hfFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
      break;

   case WM_CLOSE:
      DestroyWindow(hwnd);
      break;

   case WM_COMMAND:
      switch ( LOWORD(wParam) )
      {
      case ID_FILE_EXIT:
         PostMessageW(hwnd, WM_CLOSE, 0, 0);
         break;

      case ID_FORMAT_FONT:
         DoSelectFont(hwnd);

         InvalidateRect(hwnd, NULL, TRUE);
         UpdateWindow(hwnd);
         break;

      case ID_FORMAT_DEFAULTGUIFONT:
         DeleteObject(g_hfFont);
         g_hfFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);

         InvalidateRect(hwnd, NULL, TRUE);
         UpdateWindow(hwnd);
         break;

      case ID_FORMAT_TEST:
      {
         HFONT hf;
         HDC   hdc;
         long  lfHeight;

         hdc = GetDC(NULL);
         lfHeight = -MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72);
         ReleaseDC(NULL, hdc);

         hf = CreateFontW(lfHeight, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, L"Times New Roman");

         if ( hf )
         {
            DeleteObject(g_hfFont);
            g_hfFont = hf;
         }
         else
         {
            MessageBoxW(hwnd, L"Font creation failed!", L"Error", MB_OK | MB_ICONEXCLAMATION);
         }
         InvalidateRect(hwnd, NULL, TRUE);
         UpdateWindow(hwnd);
      }
      break;

      case ID_FORMAT_BACKGROUNDCOLOUR:
         DoSelectColour(hwnd);

         InvalidateRect(hwnd, NULL, TRUE);
         UpdateWindow(hwnd);
         break;

      case ID_FORMAT_OPAQUE:
         g_bOpaque = !g_bOpaque;

         InvalidateRect(hwnd, NULL, TRUE);
         UpdateWindow(hwnd);
         break;
      }
      break;

   case WM_INITMENUPOPUP:
      CheckMenuItem((HMENU) wParam, ID_FORMAT_OPAQUE, MF_BYCOMMAND | (g_bOpaque ? MF_CHECKED : MF_UNCHECKED));
      break;

   case WM_PAINT:
   {
      RECT        rcClient;
      PAINTSTRUCT ps;
      HDC         hdc = BeginPaint(hwnd, &ps);

      GetClientRect(hwnd, &rcClient);

      DrawClientSize(hdc, &rcClient, g_hfFont);

      EndPaint(hwnd, &ps);
   }
   break;

   case WM_DESTROY:
      DeleteObject(g_hfFont);

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
   wc.style         = CS_VREDRAW | CS_HREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance;
   wc.hIcon         = (HICON) LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hIconSm       = (HICON) LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
   wc.lpszMenuName  = MAKEINTRESOURCEW(IDR_MENU);
   wc.lpszClassName = g_szClassName;

   if ( !RegisterClassExW(&wc) )
   {
      MessageBoxW(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   hwnd = CreateWindowExW(WS_EX_CLIENTEDGE,
                          g_szClassName,
                          L"A Font Program",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
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