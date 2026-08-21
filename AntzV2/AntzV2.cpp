// AntzV2.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "AntzV2.h"
#include "GameTypes.h"
#include "Setup.h"
#include "Game.h"
#include <ctime>
#include <cstdlib>


#define MAX_LOADSTRING 100

constexpr UINT GAME_TIMER = 1;
constexpr UINT ATTACK_TIMER = 2;
constexpr UINT AI_TIMER = 3;

int x = 100;
int y = 80;

int gClientWidth = 1100;
int gClientHeight = 900;

HDC gMemDC = nullptr;
HBITMAP gBackBuffer = nullptr;
HBITMAP gOldBitmap = nullptr;
HWND gHwnd = nullptr;

int gWidth = 1100;
int gHeight = 900;
int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
std::mt19937 gRNG;

Setup gSetup;
Game gGame;
GameResult g_Result;

AppState gAppState = APP_TITLE;

HBITMAP gTitleBmp = nullptr;
HDC gTitleDC = nullptr;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

POINT ClientToGamePoint(LPARAM lParam)
{
    POINT pt;

    pt.x = LOWORD(lParam);
    pt.y = HIWORD(lParam);

    if (gClientWidth != 0 && gClientHeight != 0)
    {
        pt.x = pt.x * gWidth / gClientWidth;
        pt.y = pt.y * gHeight / gClientHeight;
    }

    return pt;
}
UINT GetInterval(Timer timer, GameSpeed speed)
{
    switch (timer)
    {
    case Timer::Battle:
        switch (speed)
        {
        case GameSpeed::Half:     return 2500;
        case GameSpeed::Normal:   return 1250;
        case GameSpeed::Fast:     return 625;
        case GameSpeed::Fastest:  return 312;
        }
        break;

    case Timer::AI:
        switch (speed)
        {
        case GameSpeed::Half:     return 1000;
        case GameSpeed::Normal:   return 500;
        case GameSpeed::Fast:     return 250;
        case GameSpeed::Fastest:  return 125;
        }
        break;
    }

    return 1000;
}
void RepaintGame()
{
    InvalidateRect(gHwnd, nullptr, FALSE);
}
void StartGameTimer()
{
    SetTimer(gHwnd, GAME_TIMER, 2000, nullptr);
}
void StopGameTimer()
{
    KillTimer(gHwnd, GAME_TIMER);
}
void StartTimer(Timer timer, GameSpeed speed)
{
    UINT gSpeed = GetInterval(timer, speed);

    switch (timer)
    {
    case Timer::Battle:
        SetTimer(gHwnd, ATTACK_TIMER, gSpeed, nullptr);
        break;

    case Timer::AI:
        SetTimer(gHwnd, AI_TIMER, gSpeed, nullptr);
        break;
    }
}
void StopTimer(Timer timer, GameSpeed speed)
{
    switch (timer)
    {
    case Timer::Battle:
        KillTimer(gHwnd, ATTACK_TIMER);
        break;

    case Timer::AI:
        KillTimer(gHwnd, AI_TIMER);
        break;
    }
}
void GameWin(const GameResult& result)
{
    g_Result = result;
    gAppState = APP_WIN;
    InvalidateRect(gHwnd, nullptr, FALSE);
}
void GetExeDirectory(wchar_t* outPath, int size)
{
    GetModuleFileNameW(nullptr, outPath, size);

    wchar_t* lastSlash = wcsrchr(outPath, L'\\');
    if (lastSlash)
        *(lastSlash + 1) = 0; // keep trailing slash
}
bool FileExists(const wchar_t* path)
{
    DWORD attrs = GetFileAttributesW(path);

    return (attrs != INVALID_FILE_ATTRIBUTES) &&
        !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}
void LoadTitleScreen()
{
    wchar_t path[MAX_PATH];
    GetExeDirectory(path, MAX_PATH);
    wcscat_s(path, L"Title.bmp");

    if (!FileExists(path))
        return;

    gTitleBmp = (HBITMAP)LoadImage(
        nullptr, path, IMAGE_BITMAP,
        0, 0, LR_LOADFROMFILE);

    if (!gTitleBmp)
        return;

    HDC hdc = GetDC(nullptr);
    gTitleDC = CreateCompatibleDC(hdc);
    ReleaseDC(nullptr, hdc);

    SelectObject(gTitleDC, gTitleBmp);
}
void FreeTitleScreen()
{
    if (gTitleDC)
    {
        DeleteDC(gTitleDC);
        gTitleDC = nullptr;
    }

    if (gTitleBmp)
    {
        DeleteObject(gTitleBmp);
        gTitleBmp = nullptr;
    }
}
void InitBackBuffer(HWND hwnd) {
    if (gMemDC != nullptr)
        return; // already initialized (prevents double creation)

    HDC hdc = GetDC(hwnd);

    gMemDC = CreateCompatibleDC(hdc);
    gBackBuffer = CreateCompatibleBitmap(hdc, gWidth, gHeight);
    gOldBitmap = (HBITMAP)SelectObject(gMemDC, gBackBuffer);

    RECT rect = { 0, 0, gWidth, gHeight };
    HBRUSH brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    FillRect(gMemDC, &rect, brush);

    ReleaseDC(hwnd, hdc);
}

void RenderTitle()
{
    if (!gTitleBmp || !gTitleDC)
        return;

    BITMAP bm;
    GetObject(gTitleBmp, sizeof(bm), &bm);

    StretchBlt(
        gMemDC, 0, 0, gWidth, gHeight, gTitleDC,
        0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
}

void RenderBackBuffer() {
    if (!gMemDC)
        return;

    //1.Clear screen
    RECT rc = { 0, 0, gClientWidth, gClientHeight };
    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(gMemDC, &rc, brush);
    DeleteObject(brush);

	// 2. DRAW TITLE
    switch (gAppState)
    {
    case APP_TITLE:
        RenderTitle();
        break;

    case APP_SETUP:
        gSetup.RenderSetup(gMemDC);
        break;

    case APP_GAME:
        gGame.Render();
		break;

    case APP_WIN:
        gSetup.RenderWin(gMemDC);
		break;

    case APP_GAMEOVER:
      //  RenderGameOver();
        break;
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    gRNG.seed(static_cast<std::mt19937::result_type>(time(nullptr)));
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ANTZV2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ANTZV2));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ANTZV2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = nullptr;
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ANTZV2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   RECT rc = { 0, 0, gClientWidth, gClientHeight };
   AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, TRUE);
   int windowWidth = rc.right - rc.left;
   int windowHeight = rc.bottom - rc.top;

   int x = (screenWidth - windowWidth) / 2;
   int y = (screenHeight - windowHeight) / 2;
   HWND hWnd = CreateWindowW( szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       x, y, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr,
       hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_CREATE:
    {
        gHwnd = hWnd;
        InitBackBuffer(hWnd);
        LoadTitleScreen();
        gSetup.Initialize(hWnd);
        gGame.Initialize(gMemDC);
        gGame.SetRepaintCallback(RepaintGame);
        gGame.SetTimerCallbacks(StartTimer, StopTimer);
		gGame.SetGameWinCallback(GameWin);
        return 0;
    }

    case WM_SIZE:
        gClientWidth = LOWORD(lParam);
        gClientHeight = HIWORD(lParam);
        return 0;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            RenderBackBuffer();

            StretchBlt( hdc, 0, 0, gClientWidth, gClientHeight,
                gMemDC, 0, 0, gWidth, gHeight, SRCCOPY);

            EndPaint(hWnd, &ps);
        }
        break;

    case WM_LBUTTONDOWN:
    {
        POINT pt = ClientToGamePoint(lParam);
        switch (gAppState)
        {
        case APP_TITLE:
            gAppState = APP_SETUP;
            InvalidateRect(hWnd, nullptr, TRUE);
            return 0;

        case APP_SETUP:
            gSetup.OnMouseDown(pt.x, pt.y);
            gSetup.HandleMouseClick(hWnd, pt.x, pt.y);
            if (gSetup.IsReady())
            {
                gGame.SetupGame(gSetup.GetConfig(), gSetup.GetPlayers());
                gAppState = APP_GAME;
                SetTimer(hWnd, GAME_TIMER, 1000, nullptr);
            }

            InvalidateRect(hWnd, nullptr, TRUE);
            return 0;

        case APP_WIN:
            // handle replay/new game buttons
            gSetup.HandleMouseClick(hWnd, pt.x, pt.y);

            if (gAppState == APP_GAMEOVER)
            {
                DestroyWindow(hWnd);
                return 0;
            }

            if (gSetup.IsReady())
            {
                gGame.SetupGame(gSetup.GetConfig(), gSetup.GetPlayers());
                gSetup.ClearReady();
                gAppState = APP_GAME;

                SetTimer(hWnd, GAME_TIMER, 1000, nullptr);
            }

            InvalidateRect(hWnd, nullptr, TRUE);
            return 0;

        case APP_GAME:

            gGame.OnMouseDown(pt.x, pt.y);
            return 0;

        case APP_GAMEOVER:
            DestroyWindow(hWnd);
            return 0;
        }
    }
    case WM_TIMER:
    {
        switch (wParam)
        {
        case GAME_TIMER:
            KillTimer(hWnd, GAME_TIMER);
            gGame.UpdateTurn();
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        case ATTACK_TIMER:
            gGame.UpdateAttack();
            InvalidateRect(hWnd, nullptr, FALSE);
            break;

        case AI_TIMER:
            gGame.UpdateAIturn();
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        }
        return 0;
    }
    case WM_CHAR:
    {
        if (gAppState == APP_SETUP)
        {
            gSetup.OnChar((char)wParam);

            InvalidateRect(hWnd, nullptr, FALSE);
        }
        else if (gAppState == APP_GAME)
        {
            gGame.OnChar((char)wParam);

            InvalidateRect(hWnd, nullptr, FALSE);
        }
        return 0;
    }
	case WM_MOUSEMOVE:
    {
        //POINT pt = ClientToGamePoint(lParam);
        //if (gAppState == APP_GAME)
        //gGame.HandleMouseMove(pt.x, pt.y);
        return 0;
    }

    case WM_ERASEBKGND:
        return TRUE;

    case WM_DESTROY:
		FreeTitleScreen();
        gSetup.Shutdown();
        if (gMemDC)
        {
            SelectObject(gMemDC, gOldBitmap);
            DeleteObject(gBackBuffer);
            DeleteDC(gMemDC);
        }
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
};
