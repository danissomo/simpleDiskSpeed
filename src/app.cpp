#ifndef UNICODE
#define UNICODE
#endif
#include "app.h"
#include <stdlib.h>
#include <CommCtrl.h>

void App::Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    this->nCmdShow = nCmdShow;
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    this->wc = {};
    this->wc.lpfnWndProc = AppProc;
    this->wc.hInstance = hInstance;
    this->wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    this->windowHandle = CreateWindowEx(
        0,                   // Optional window styles.
        CLASS_NAME,          // Window class
        L"SimpleDiskSpeed",  // Window text
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, // Window style
        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT,
        310, 170,
        NULL,      // Parent window
        NULL,      // Menu
        hInstance, // Instance handle
        NULL       // Additional application data
    );

    this->startBtn = CreateWindow(
        L"BUTTON",                         // Predefined class; Unicode assumed
        L"start",                          // Button text
        WS_VISIBLE | WS_CHILD | ES_CENTER, // Styles
        10,                                // x position
        10,                                // y position
        50,                                // Button width
        30,                                // Button height
        this->windowHandle,                // Parent window
        (HMENU)Action::OnStart,            // No menu.
        (HINSTANCE)GetWindowLongPtr(this->windowHandle, GWLP_HINSTANCE),
        NULL);
    this->stopBtn = CreateWindow(
        L"BUTTON",                         // Predefined class; Unicode assumed
        L"stop",                           // Button text
        WS_VISIBLE | WS_CHILD | ES_CENTER, // Styles
        65,                                // x position
        10,                                // y position
        50,                                // Button width
        30,                                // Button height
        this->windowHandle,                // Parent window
        (HMENU)Action::OnStop,             // No menu.
        (HINSTANCE)GetWindowLongPtr(this->windowHandle, GWLP_HINSTANCE),
        // NULL,
        NULL);

    int width = 10, chFw = 50, textChFw = 70, sFw = 40, textSFw = 90;
    int y = 70;
    CreateWindow(L"static", L"chunk(KB)", WS_VISIBLE | WS_CHILD, width, y, textChFw, 20, this->windowHandle, NULL, NULL, NULL);
    width += textChFw + 5;
    this->hChunkSizeEdit = CreateWindow(L"edit", L"10", WS_VISIBLE | WS_CHILD | ES_NUMBER | WS_BORDER, width, y, chFw, 20, this->windowHandle, NULL, NULL, NULL);
    width += chFw + 5;
    CreateWindow(L"static", L"test size(MB)", WS_VISIBLE | WS_CHILD, width, y, textSFw, 20, this->windowHandle, NULL, NULL, NULL);
    width += textSFw + 5;
    this->hFileSizeEdit = CreateWindow(L"edit", L"100", WS_VISIBLE | WS_CHILD | ES_NUMBER | WS_BORDER, width, y, sFw, 20, this->windowHandle, NULL, NULL, NULL);
    EnableWindow(this->stopBtn, false);
    width += sFw;
    this->text_field = CreateWindow(L"static", L"speed: 0.0", WS_VISIBLE | WS_CHILD, 10, 45, width, 20, this->windowHandle, NULL, NULL, NULL);
    y += 25;
    this->hProgressBar = CreateWindow(PROGRESS_CLASS, NULL, WS_VISIBLE | WS_CHILD, 10, y, width, 20, this->windowHandle, NULL, NULL, NULL);
    SendMessage(this->hProgressBar, PBM_SETRANGE, NULL, (LPARAM)MAKELONG(0, 100));
}

void App::Show()
{
    ShowWindow(this->windowHandle, this->nCmdShow);
}

bool App::Update()
{
    MSG msg = {};
    bool rs = GetMessage(&msg, NULL, 0, 0) > 0;
    if (!rs)
        return false;
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    if (writer.IsReady())
    {
        writer.Stop();
        std::string s = "speed: ";
        float sec_taken = writer.GetStatus() / 1000.0f;
        float speed = (float(lastSettings.fileSize)) / sec_taken;
        s = s + std::to_string(speed);
        auto text = std::wstring(s.begin(), s.end());
        SetWindowText(this->text_field, (LPCWSTR)text.c_str());
        EnableWindow(this->startBtn, true);
        EnableWindow(this->stopBtn, false);
    }
    int progress = writer.GetProgress();
    float step = 100.0f * float(progress) * float(this->lastSettings.chunkSize) / float(this->lastSettings.fileSize * 1024);
    SendMessage(this->hProgressBar, PBM_SETPOS, int(round(step)), NULL);
    return true;
}

void App::InputAction(WPARAM action)
{
    LPWSTR s_chunkSize = new wchar_t[20];
    LPWSTR s_fileSize = new wchar_t[20];
    int i_chunkSize_kb = 0;
    _wtoi(s_chunkSize);
    int i_fileSize_mb = 0;
    _wtoi(s_fileSize);
    switch (action)
    {
    case Action::OnStop:
        this->writer.Stop();
        EnableWindow(this->startBtn, true);
        break;
    case Action::OnStart:
        EnableWindow(this->startBtn, false);
        GetWindowText(this->hChunkSizeEdit, s_chunkSize, 20);
        GetWindowText(this->hFileSizeEdit, s_fileSize, 20);
        i_chunkSize_kb = _wtoi(s_chunkSize);
        i_fileSize_mb = _wtoi(s_fileSize);
        this->lastSettings.chunkSize = i_chunkSize_kb;
        this->lastSettings.fileSize = i_fileSize_mb;
        this->writer.Start("test.txt", i_chunkSize_kb, i_fileSize_mb * 1024);
        EnableWindow(this->stopBtn, true);
        break;

    default:
        break;
    }
    delete s_chunkSize, s_chunkSize;
}

LRESULT CALLBACK AppProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_COMMAND:
        App::Instance()->InputAction(wParam);

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}