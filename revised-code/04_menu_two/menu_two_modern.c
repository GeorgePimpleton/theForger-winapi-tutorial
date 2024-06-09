#include <windows.h>

#define ID_FILE_EXIT 9001
#define ID_STUFF_GO  9002

PCWSTR g_className = L"myWindowClass";

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR    cmdLine,
                     _In_     int      cmdShow )
{
   UNREFERENCED_PARAMETER( prevInst );

   WNDCLASSEXW wc = { 0 };
   HWND        wnd;
   MSG         msg;

   wc.cbSize        = sizeof( WNDCLASSEXW );
   wc.style         = 0;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = NULL;
   wc.hCursor       = ( HCURSOR ) LoadImageW( NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED );
   wc.hbrBackground = ( HBRUSH )  ( COLOR_WINDOW + 1 );
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = g_className;
   wc.hIconSm       = NULL;

   if ( !RegisterClassExW( &wc ) )
   {
      MessageBoxW( NULL, L"Window Registration Failed!", L"Error!",
                   MB_ICONEXCLAMATION | MB_OK );
      return 0;
   }

   wnd = CreateWindowExW( WS_EX_CLIENTEDGE,
                          g_className,
                          L"A Menu #2",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
                          NULL, NULL, inst, NULL );

   if ( wnd == NULL )
   {
      MessageBoxW( NULL, L"Window Creation Failed!", L"Error!",
                   MB_ICONEXCLAMATION | MB_OK );
      return 0;
   }

   ShowWindow( wnd, cmdShow );
   UpdateWindow( wnd );

   while ( GetMessageW( &msg, NULL, 0, 0 ) > 0 )
   {
      TranslateMessage( &msg );
      DispatchMessageW( &msg );
   }

   return ( int ) msg.wParam;
}

LRESULT CALLBACK WndProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   switch ( msg )
   {
   case WM_CREATE:
   {
      HMENU hMenu;
      HMENU hSubMenu;
      HICON hIcon;
      HICON hIconSm;

      hMenu = CreateMenu( );

      hSubMenu = CreatePopupMenu( );
      AppendMenuW( hSubMenu, MF_STRING, ID_FILE_EXIT, L"E&xit" );
      AppendMenuW( hMenu, MF_STRING | MF_POPUP, ( UINT_PTR ) hSubMenu, L"&File" );

      hSubMenu = CreatePopupMenu( );
      AppendMenuW( hSubMenu, MF_STRING, ID_STUFF_GO, L"&Go" );
      AppendMenuW( hMenu, MF_STRING | MF_POPUP, ( UINT_PTR ) hSubMenu, L"&Stuff" );

      SetMenu( wnd, hMenu );

      hIcon = ( HICON ) LoadImageW( NULL, L"menu_two.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE );

      if ( hIcon )
      {
         SendMessageW( wnd, WM_SETICON, ICON_BIG, ( LPARAM ) hIcon );
      }
      else
      {
         MessageBoxW( wnd, L"Could not load large icon! Is it in the current working directory?", L"Error", MB_OK | MB_ICONERROR );
      }

      hIconSm = ( HICON ) LoadImageW( NULL, L"menu_two.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE );

      if ( hIconSm )
      {
         SendMessageW( wnd, WM_SETICON, ICON_SMALL, ( LPARAM ) hIconSm );
      }
      else
      {
         MessageBoxW( wnd, L"Could not load small icon! Is it in the current working directory?", L"Error", MB_OK | MB_ICONERROR );
      }
   }
   break;

   case WM_COMMAND:
      switch ( LOWORD( wParam ) )
      {
      case ID_FILE_EXIT:
         PostMessageW( wnd, WM_CLOSE, 0, 0 );
         break;

      case ID_STUFF_GO:
         MessageBoxW( wnd, L"You clicked Go!", L"Woo!", MB_ICONINFORMATION | MB_OK );
         break;
      }
      break;

   case WM_CLOSE:
      DestroyWindow( wnd );
      break;

   case WM_DESTROY:
      PostQuitMessage( 0 );
      break;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}
