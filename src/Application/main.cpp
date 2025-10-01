#pragma once
#include "GameApplication.h"
#include <Windows.h>

using namespace ArenaFighter;

// Global game instance
GameApplication* g_Game = nullptr;

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Register window class
    const wchar_t CLASS_NAME[] = L"DFRArenaFighterWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    if (!RegisterClass(&wc)) {
        MessageBox(nullptr, L"Failed to register window class", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Calculate window size to achieve desired client area
    RECT windowRect = {0, 0, 1920, 1080};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    // Create window
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"DFR - Anime Arena Fighter",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hwnd) {
        MessageBox(nullptr, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Create and initialize game
    g_Game = new GameApplication();
    if (!g_Game->initialize(hwnd)) {
        MessageBox(hwnd, L"Failed to initialize game", L"Error", MB_OK | MB_ICONERROR);
        delete g_Game;
        return 1;
    }

    // Run game
    g_Game->run();

    // Cleanup
    delete g_Game;
    g_Game = nullptr;

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            if (g_Game) {
                g_Game->shutdown();
            }
            PostQuitMessage(0);
            return 0;

        case WM_SIZE:
            // Handle window resize if needed
            return 0;

        case WM_MOUSEMOVE:
            if (g_Game) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                g_Game->onMouseMove(x, y);
            }
            return 0;
            
        case WM_LBUTTONDOWN:
            if (g_Game) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                g_Game->onMouseDown(x, y);
            }
            return 0;
            
        case WM_LBUTTONUP:
            if (g_Game) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                g_Game->onMouseUp(x, y);
            }
            return 0;
            
        case WM_KEYDOWN:
            if (g_Game && !(lParam & 0x40000000)) { // Check if not repeat
                g_Game->onKeyDown(static_cast<int>(wParam));
            }
            return 0;
            
        case WM_KEYUP:
            if (g_Game) {
                g_Game->onKeyUp(static_cast<int>(wParam));
            }
            return 0;

        case WM_CHAR:
            // Handle character input for text entry if needed
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}