#include <sdkddkver.h>
#include <afxwin.h>

class CMainWin : public CFrameWnd
{
public:
   CMainWin( );

   DECLARE_MESSAGE_MAP( )
};

CMainWin::CMainWin( )
{
   CreateEx(WS_EX_CLIENTEDGE,
            AfxRegisterWndClass(NULL,
                                (HCURSOR) ::LoadCursorW(NULL, IDC_ARROW),
                                (HBRUSH) (COLOR_WINDOW + 1),
                                (HICON) ::LoadIconW(NULL, IDI_APPLICATION)),
            L"MFC Small Window",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            480, 240,
            NULL, NULL, 0);
}

class CApp : public CWinApp
{
public:
   BOOL InitInstance( );
};

BOOL CApp::InitInstance( )
{
   m_pMainWnd = new CMainWin;
   m_pMainWnd->ShowWindow(m_nCmdShow);
   m_pMainWnd->UpdateWindow( );

   return TRUE;
}

BEGIN_MESSAGE_MAP(CMainWin, CFrameWnd)
END_MESSAGE_MAP( )

CApp App;
