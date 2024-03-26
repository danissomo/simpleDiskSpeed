#include <thread>
#include <string>
#include <chrono>
#include "semaphore.h"

struct WorkerReport
{
    semaphore readySignal;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    int curChunk = 0;
};

struct WorkerTask
{
    semaphore stopSignal;
    std::string path;
    size_t cunksize;
    size_t size_kb;
};

enum State
{
    idle,
    started,
    completed
};

class Writer
{
public:
    Writer();
    ~Writer();
    bool Start(std::string path, size_t chunk, size_t size);
    bool Stop();
    bool IsReady();
    float GetStatus();
    int GetProgress();

private:
    WorkerReport report;
    WorkerTask task;
    std::thread writingThread;
    static void writerWorker(WorkerTask *task, WorkerReport *report);
};
