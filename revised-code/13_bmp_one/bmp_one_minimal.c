#include <windows.h>
#include "resource.h"

const TCHAR g_szClassName[ ] = TEXT( "myWindowClass" );

HBITMAP g_hbmBall = NULL;

LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   switch ( msg )
   {
   case WM_CREATE:
      g_hbmBall = LoadBitmap( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDB_BALL ) );
      if ( g_hbmBall == NULL )
         MessageBox( hwnd, TEXT( "Could not load IDB_BALL!" ), TEXT( "Error" ), MB_OK | MB_ICONEXCLAMATION );
      break;
   case WM_CLOSE:
      DestroyWindow( hwnd );
      break;
   case WM_PAINT:
   {
      // Just a note, never use a MessageBox from inside WM_PAINT
      // The box will cause more WM_PAINT messages and you'll probably end up
      // stuck in a loop

      BITMAP bm;
      PAINTSTRUCT ps;

      HDC hdc = BeginPaint( hwnd, &ps );

      HDC hdcMem = CreateCompatibleDC( hdc );
      HBITMAP hbmOld = ( HBITMAP ) SelectObject( hdcMem, g_hbmBall );

      GetObject( g_hbmBall, sizeof( bm ), &bm );

      BitBlt( hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY );

      SelectObject( hdcMem, hbmOld );
      DeleteDC( hdcMem );

      EndPaint( hwnd, &ps );
   }
   break;
   case WM_DESTROY:
      DeleteObject( g_hbmBall );
      PostQuitMessage( 0 );
      break;
   default:
      return DefWindowProc( hwnd, msg, wParam, lParam );
   }
   return 0;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow )
{
   WNDCLASSEX wc;
   HWND hwnd;
   MSG Msg;

   wc.cbSize		 = sizeof( WNDCLASSEX );
   wc.style		 = 0;
   wc.lpfnWndProc	 = WndProc;
   wc.cbClsExtra	 = 0;
   wc.cbWndExtra	 = 0;
   wc.hInstance	 = hInstance;
   wc.hIcon		 = LoadIcon( NULL, IDI_APPLICATION );
   wc.hCursor		 = LoadCursor( NULL, IDC_ARROW );
   wc.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = g_szClassName;
   wc.hIconSm		 = LoadIcon( NULL, IDI_APPLICATION );

   if ( !RegisterClassEx( &wc ) )
   {
      MessageBox( NULL, TEXT( "Window Registration Failed!" ), TEXT( "Error!" ),
                  MB_ICONEXCLAMATION | MB_OK );
      return 0;
   }

   hwnd = CreateWindowEx(
      WS_EX_CLIENTEDGE,
      g_szClassName,
      TEXT( "A Bitmap Program" ),
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
      NULL, NULL, hInstance, NULL );

   if ( hwnd == NULL )
   {
      MessageBox( NULL, TEXT( "Window Creation Failed!" ), TEXT( "Error!" ),
                  MB_ICONEXCLAMATION | MB_OK );
      return 0;
   }

   ShowWindow( hwnd, nCmdShow );
   UpdateWindow( hwnd );

   while ( GetMessage( &Msg, NULL, 0, 0 ) > 0 )
   {
      TranslateMessage( &Msg );
      DispatchMessage( &Msg );
   }
   return Msg.wParam;
}
