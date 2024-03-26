#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include "app.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    App *app = App::Instance();

    app->Init(hInstance, hPrevInstance, pCmdLine, nCmdShow);
    app->Show();
    bool is_msg = true;
    while (is_msg)
    {
        is_msg = app->Update();
    }
}
