#include "GameApplicationAdapter.h"
#include <Windows.h>

using namespace ArenaFighter;

// Global game instance
DFRGameApplication* g_Game = nullptr;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            if (g_Game) g_Game->Shutdown();
            PostQuitMessage(0);
            return 0;
            
        case WM_KEYDOWN:
            if (g_Game && !(lParam & 0x40000000)) { // Ignore key repeat
                g_Game->OnKeyDown(static_cast<int>(wParam));
            }
            return 0;
            
        case WM_KEYUP:
            if (g_Game) {
                g_Game->OnKeyUp(static_cast<int>(wParam));
            }
            return 0;
            
        case WM_MOUSEMOVE:
            if (g_Game) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                g_Game->OnMouseMove(x, y);
            }
            return 0;
            
        case WM_LBUTTONDOWN:
            if (g_Game) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                g_Game->OnMouseClick(0, x, y);
            }
            return 0;
            
        case WM_RBUTTONDOWN:
            if (g_Game) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                g_Game->OnMouseClick(1, x, y);
            }
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"DFRArenaFighterWindow";
    
    // Register window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    
    if (!RegisterClass(&wc)) {
        MessageBox(nullptr, L"Failed to register window class", L"Error", MB_OK);
        return 1;
    }
    
    // Create window
    int screenWidth = 1920;
    int screenHeight = 1080;
    
    RECT windowRect = { 0, 0, screenWidth, screenHeight };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;
    
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"DFR - Anime Arena Fighter (Lost Saga Style)",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowWidth, windowHeight,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
    
    if (!hwnd) {
        MessageBox(nullptr, L"Failed to create window", L"Error", MB_OK);
        return 1;
    }
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // Create and initialize game
    g_Game = new DFRGameApplication();
    if (!g_Game->Initialize(hwnd, screenWidth, screenHeight)) {
        MessageBox(hwnd, L"Failed to initialize game", L"Error", MB_OK);
        delete g_Game;
        return 1;
    }
    
    // Show instructions
    MessageBox(hwnd, 
        L"DFR Arena Fighter Controls:\n\n"
        L"Movement: Arrow Keys or WASD\n"
        L"Special Moves: S + Direction (Uses MANA only)\n"
        L"Gear Skills: AS, AD, SD, ASD (Have cooldowns)\n"
        L"Block: Hold S for 1 second\n"
        L"Stance Switch: S + Down\n\n"
        L"Press Enter to start a match!",
        L"Welcome to DFR", MB_OK);
    
    // Run game
    g_Game->Run();
    
    // Cleanup
    delete g_Game;
    g_Game = nullptr;
    
    return 0;
}