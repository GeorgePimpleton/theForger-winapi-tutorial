#include <windows.h>

// WinAPI has several pointer to string data types
// this type is a constant ptr-to-str
// https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types
PCWSTR g_className = L"myWindowClass";

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

// added a generic error reporting function that terminates the app
void ErrorQuit( PCWSTR );

// Step 0: Desktop WinAPI app entry point
// added SAL (source-code annotation language) to WinMain
// https://learn.microsoft.com/en-us/cpp/code-quality/understanding-sal?view=msvc-170
int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR     cmdLine,
                     _In_     int       cmdShow )
{
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );

   WNDCLASSEXW wc = { 0 };
   HWND        wnd;
   MSG         msg;

   // Step 1: Registering the Window Class
   wc.cbSize        = sizeof( WNDCLASSEXW );
   wc.style         = 0;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = ( HICON ) LoadImageW( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR );
   wc.hCursor       = ( HCURSOR ) LoadImageW( NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED );
   wc.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = g_className;
   wc.hIconSm       = ( HICON ) LoadImageW( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR );

   if ( !RegisterClassExW( &wc ) )
   {
      ErrorQuit( L"Window Registration Failed!" );
   }

   // Step 2: Creating (and showing) the Window
   wnd = CreateWindowExW( WS_EX_CLIENTEDGE,
                          g_className,
                          L"The title of my window",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          240, 120,
                          NULL, NULL, inst, NULL );

   if ( wnd == NULL )
   {
      ErrorQuit( L"Window Creation Failed!" );
   }

   ShowWindow( wnd, cmdShow );
   UpdateWindow( wnd );

   // Step 3: The Message Loop
   while ( GetMessageW( &msg, NULL, 0, 0 ) > 0 )
   {
      TranslateMessage( &msg );
      DispatchMessageW( &msg );
   }

   // needed to stop VS from whinging about possible loss of data from a conversion
   return ( int ) msg.wParam;
}

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   switch ( msg )
   {
   case WM_CLOSE:
      DestroyWindow( wnd );
      break;

   case WM_DESTROY:
      PostQuitMessage( 0 );
      break;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}

void ErrorQuit( PCWSTR errMsg )
{
   MessageBoxW( NULL, errMsg, L"Critical Error", MB_OK | MB_ICONERROR );
   PostQuitMessage( 0 );
}
