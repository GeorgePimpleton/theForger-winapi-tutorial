#include <windows.h>

// Step 5: the Window Procedure
LRESULT CALLBACK WndProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam );

// Step 1: Desktop WinAPI app entry point
// added SAL (source-code annotation language) to WinMain
// https://learn.microsoft.com/en-us/cpp/code-quality/understanding-sal?view=msvc-170
int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR     cmdLine,
                     _In_     int       cmdShow )
{
   UNREFERENCED_PARAMETER( prevInst );

   // WinAPI has several pointer to string data types
   // this type is a constant ptr-to-str
   // https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types
   PCWSTR className = L"myWindowClass";  // <-- no need for this to be a global

   WNDCLASSEXW wc = { 0 };
   HWND        wnd;
   MSG         msg;

   // Step 2: Registering the Window Class
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
   wc.lpszClassName = className;
   wc.hIconSm       = ( HICON ) LoadImageW( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR );

   if ( !RegisterClassExW( &wc ) )
   {
      MessageBoxW( NULL, L"Window Registration Failed!", L"Critical Error", MB_OK | MB_ICONERROR );
      return 0;
   }

   // Step 3: Creating (and showing) the Window
   wnd = CreateWindowExW( WS_EX_CLIENTEDGE,
                          className,
                          L"The title of my window",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          240, 120,
                          NULL, NULL, inst, NULL );

   if ( wnd == NULL )
   {
      MessageBoxW( NULL, L"Window Creation Failed!", L"Critical Error", MB_OK | MB_ICONERROR );
      return 0;
   }

   ShowWindow( wnd, cmdShow );
   UpdateWindow( wnd );

   // Step 4: The Message Loop
   while ( GetMessageW( &msg, NULL, 0, 0 ) > 0 )
   {
      TranslateMessage( &msg );
      DispatchMessageW( &msg );
   }

   // needed to stop VS from whinging about possible loss of data
   return ( int ) msg.wParam;
}

// Step 5: the Window Procedure
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
