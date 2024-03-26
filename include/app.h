#include <windows.h>
#include "benchmark.h"
#define test 1

enum Action
{
    OnStart = 1,
    OnStop
};

struct BenchSettings{
    size_t chunkSize;
    size_t fileSize;
};

class App
{
public:
    void Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);
    bool Update();
    void Show();
    static App *Instance()
    {
        static App instance;
        return &instance;
    }
    void InputAction(WPARAM action);

protected:
    App(){};
    Writer writer;
    HWND windowHandle;
    HWND startBtn;
    HWND stopBtn;
    HWND text_field;
    HWND hChunkSizeEdit;
    HWND hFileSizeEdit;
    HWND hProgressBar;
    int nCmdShow;
    WNDCLASS wc;
    BenchSettings lastSettings;
};

LRESULT CALLBACK AppProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
