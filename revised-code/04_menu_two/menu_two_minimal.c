#include <windows.h>

#define ID_FILE_EXIT 9001
#define ID_STUFF_GO  9002

const TCHAR g_szClassName[ ] = TEXT( "myWindowClass" );

LRESULT CALLBACK WndProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
   switch ( Message )
   {
   case WM_CREATE:
   {
      HMENU hMenu, hSubMenu;
      HICON hIcon, hIconSm;

      hMenu = CreateMenu( );

      hSubMenu = CreatePopupMenu( );
      AppendMenu( hSubMenu, MF_STRING, ID_FILE_EXIT, TEXT( "E&xit" ) );
      AppendMenu( hMenu, MF_STRING | MF_POPUP, ( UINT ) hSubMenu, TEXT( "&File" ) );

      hSubMenu = CreatePopupMenu( );
      AppendMenu( hSubMenu, MF_STRING, ID_STUFF_GO, TEXT( "&Go" ) );
      AppendMenu( hMenu, MF_STRING | MF_POPUP, ( UINT ) hSubMenu, TEXT( "&Stuff" ) );

      SetMenu( hwnd, hMenu );

      hIcon = ( HICON ) LoadImage( NULL, TEXT( "menu_two.ico" ), IMAGE_ICON, 32, 32, LR_LOADFROMFILE );
      if ( hIcon )
      {
         SendMessage( hwnd, WM_SETICON, ICON_BIG, ( LPARAM ) hIcon );
      }
      else
      {
         MessageBox( hwnd, TEXT( "Could not load large icon! Is it in the current working directory?" ), TEXT( "Error" ), MB_OK | MB_ICONERROR );
      }

      hIconSm = ( HICON ) LoadImage( NULL, TEXT( "menu_two.ico" ), IMAGE_ICON, 16, 16, LR_LOADFROMFILE );
      if ( hIconSm )
      {
         SendMessage( hwnd, WM_SETICON, ICON_SMALL, ( LPARAM ) hIconSm );
      }
      else
      {
         MessageBox( hwnd, TEXT( "Could not load small icon! Is it in the current working directory?" ), TEXT( "Error" ), MB_OK | MB_ICONERROR );
      }
   }
   break;
   case WM_COMMAND:
      switch ( LOWORD( wParam ) )
      {
      case ID_FILE_EXIT:
         PostMessage( hwnd, WM_CLOSE, 0, 0 );
         break;
      case ID_STUFF_GO:
         MessageBox( hwnd, TEXT( "You clicked Go!" ), TEXT( "Woo!" ), MB_OK );
         break;
      }
      break;
   case WM_CLOSE:
      DestroyWindow( hwnd );
      break;
   case WM_DESTROY:
      PostQuitMessage( 0 );
      break;
   default:
      return DefWindowProc( hwnd, Message, wParam, lParam );
   }
   return 0;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow )
{
   WNDCLASSEX wc;
   HWND hwnd;
   MSG Msg;

   wc.cbSize        = sizeof( WNDCLASSEX );
   wc.style         = 0;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance;
   wc.hIcon         = NULL;
   wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
   wc.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = g_szClassName;
   wc.hIconSm       = NULL;

   if ( !RegisterClassEx( &wc ) )
   {
      MessageBox( NULL, TEXT( "Window Registration Failed!" ), TEXT( "Error!" ),
                  MB_ICONEXCLAMATION | MB_OK );
      return 0;
   }

   hwnd = CreateWindowEx(
      WS_EX_CLIENTEDGE,
      g_szClassName,
      TEXT( "A Menu #2" ),
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
