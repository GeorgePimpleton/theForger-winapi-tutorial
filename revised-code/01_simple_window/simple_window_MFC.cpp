#include <sdkddkver.h>
#include <afxwin.h>

// application class
class CTheApp : public CWinApp
{
public:
   BOOL InitInstance( );
};

// main window class
class CMainWnd : public CFrameWnd
{
public:
   CMainWnd( );

protected:
   DECLARE_MESSAGE_MAP( )
};

// instantiate the application
CTheApp theApp;

// construct a window
CMainWnd::CMainWnd( )
{
   Create(NULL, L"The title of my window");
}

// initalize the application
BOOL CTheApp::InitInstance( )
{
   m_pMainWnd = new CMainWnd;

   m_pMainWnd->ShowWindow(m_nCmdShow);
   m_pMainWnd->UpdateWindow( );

   return TRUE;
}

// application's message map
BEGIN_MESSAGE_MAP(CMainWnd, CFrameWnd)
END_MESSAGE_MAP( )
