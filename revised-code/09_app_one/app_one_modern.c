#include <windows.h>

#define IDC_MAIN_EDIT 101

LRESULT CALLBACK WndProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam );

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR    cmdLine,
                     _In_     int      cmdShow )
{
   UNREFERENCED_PARAMETER( prevInst );

   WNDCLASSEXW wc        = { 0 };
   HWND        wnd;
   MSG         msg;
   PCWSTR      className = L"myWindowClass";

   wc.cbSize        = sizeof( WNDCLASSEXW );
   wc.style         = 0;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = ( HICON ) LoadImageW( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR );
   wc.hIconSm       = ( HICON ) LoadImageW( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR );
   wc.hCursor       = ( HCURSOR ) LoadImageW( NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED );
   wc.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = className;

   if ( !RegisterClassExW( &wc ) )
   {
      MessageBoxW( NULL, L"Window Registration Failed!", L"Error!",
                   MB_ICONEXCLAMATION | MB_OK );
      return 0;
   }

   wnd = CreateWindowExW( 0,
                          className,
                          L"theForger's Tutorial Application",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 480, 320,
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
      HFONT fontDefault;
      HWND  edit;

      edit = CreateWindowExW( WS_EX_CLIENTEDGE, L"EDIT", L"",
                              WS_CHILD | WS_VISIBLE |
                              WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                              0, 0, 100, 100, wnd, ( HMENU ) IDC_MAIN_EDIT, GetModuleHandleW( NULL ), NULL );

      if ( edit == NULL )
      {
         MessageBoxW( wnd, L"Could not create edit box.", L"Error", MB_OK | MB_ICONERROR );
      }

      fontDefault = ( HFONT ) GetStockObject( DEFAULT_GUI_FONT );

   // suppress one time a warning
   // https://learn.microsoft.com/en-us/cpp/preprocessor/warning?view=msvc-170
   #pragma warning (suppress : 6387)
      SendMessageW( edit, WM_SETFONT, ( WPARAM ) fontDefault, MAKELPARAM( FALSE, 0 ) );
   }
   break;

   case WM_SIZE:
   {
      HWND edit;
      RECT rcClient;

      GetClientRect( wnd, &rcClient );

      edit = GetDlgItem( wnd, IDC_MAIN_EDIT );
      SetWindowPos( edit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER );
   }
   break;

   case WM_CLOSE:
      DestroyWindow( wnd );
      break;

   case WM_DESTROY:
      PostQuitMessage( 0 );
      break;

   default:
      return DefWindowProcW( wnd, msg, wParam, lParam );
   }
   return 0;
}
