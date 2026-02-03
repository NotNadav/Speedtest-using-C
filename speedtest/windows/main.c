#include <windows.h>
#include <stdio.h>
#include "speedtest.h"

#define ID_START_BUTTON 1001
#define ID_STATUS_TEXT 1002
#define ID_PING_LABEL 1003
#define ID_DOWNLOAD_LABEL 1004
#define ID_UPLOAD_LABEL 1005
#define ID_PING_VALUE 1006
#define ID_DOWNLOAD_VALUE 1007
#define ID_UPLOAD_VALUE 1008

HWND hwndStatus;
HWND hwndPingValue;
HWND hwndDownloadValue;
HWND hwndUploadValue;
HWND hwndButton;

DWORD WINAPI SpeedTestThread(LPVOID lpParam) {
    HWND hwnd = (HWND)lpParam;
    SpeedTestResult result = {0};
    
    EnableWindow(hwndButton, FALSE);
    
    SetWindowText(hwndStatus, "Testing ping...");
    if (test_ping(&result.ping_ms) == 0) {
        char buffer[64];
        sprintf(buffer, "%.0f ms", result.ping_ms);
        SetWindowText(hwndPingValue, buffer);
    } else {
        SetWindowText(hwndPingValue, "Error");
    }
    
    SetWindowText(hwndStatus, "Testing download speed...");
    if (test_download(&result.download_mbps) == 0) {
        char buffer[64];
        sprintf(buffer, "%.2f Mbps", result.download_mbps);
        SetWindowText(hwndDownloadValue, buffer);
    } else {
        SetWindowText(hwndDownloadValue, "Error");
    }
    
    SetWindowText(hwndStatus, "Testing upload speed...");
    if (test_upload(&result.upload_mbps) == 0) {
        char buffer[64];
        sprintf(buffer, "%.2f Mbps", result.upload_mbps);
        SetWindowText(hwndUploadValue, buffer);
    } else {
        SetWindowText(hwndUploadValue, "Error");
    }
    
    SetWindowText(hwndStatus, "Test complete!");
    
    EnableWindow(hwndButton, TRUE);
    
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            hwndButton = CreateWindow(
                "BUTTON", "Start Test",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                20, 20, 120, 40,
                hwnd, (HMENU)ID_START_BUTTON, NULL, NULL
            );
            
            hwndStatus = CreateWindow(
                "STATIC", "Ready to test",
                WS_VISIBLE | WS_CHILD,
                20, 80, 360, 20,
                hwnd, (HMENU)ID_STATUS_TEXT, NULL, NULL
            );
            
            CreateWindow(
                "STATIC", "Ping:",
                WS_VISIBLE | WS_CHILD,
                20, 120, 100, 20,
                hwnd, (HMENU)ID_PING_LABEL, NULL, NULL
            );
            
            hwndPingValue = CreateWindow(
                "STATIC", "-",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                130, 120, 150, 20,
                hwnd, (HMENU)ID_PING_VALUE, NULL, NULL
            );
            
            CreateWindow(
                "STATIC", "Download:",
                WS_VISIBLE | WS_CHILD,
                20, 150, 100, 20,
                hwnd, (HMENU)ID_DOWNLOAD_LABEL, NULL, NULL
            );
            
            hwndDownloadValue = CreateWindow(
                "STATIC", "-",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                130, 150, 150, 20,
                hwnd, (HMENU)ID_DOWNLOAD_VALUE, NULL, NULL
            );
            
            CreateWindow(
                "STATIC", "Upload:",
                WS_VISIBLE | WS_CHILD,
                20, 180, 100, 20,
                hwnd, (HMENU)ID_UPLOAD_LABEL, NULL, NULL
            );
            
            hwndUploadValue = CreateWindow(
                "STATIC", "-",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                130, 180, 150, 20,
                hwnd, (HMENU)ID_UPLOAD_VALUE, NULL, NULL
            );
            
            break;
        }
        
        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_START_BUTTON) {
                HANDLE hThread = CreateThread(NULL, 0, SpeedTestThread, hwnd, 0, NULL);
                if (hThread) {
                    CloseHandle(hThread);
                }
            }
            break;
        }
        
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "SpeedTestWindow";
    
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    RegisterClass(&wc);
    
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Speedtest",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 280,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    
    if (hwnd == NULL) {
        return 0;
    }
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}
