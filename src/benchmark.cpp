#include "benchmark.h"
#include <iostream>
#include <fstream>

Writer::Writer()
{
}

bool Writer::Start(std::string path, size_t chunk, size_t size)
{
    this->task.path = path;
    this->task.size_kb = size;
    this->task.cunksize = chunk;
    this->task.stopSignal.release();
    this->writingThread = std::thread(Writer::writerWorker, &this->task, &this->report);
    return true;
}
bool Writer::Stop()
{
    if (this->writingThread.joinable())
    {
        this->task.stopSignal.acquire();
        this->writingThread.join();
        this->task.stopSignal.release();
        return true;
    }
    return false;
}

float Writer::GetStatus()
{
    auto duration = this->report.endTime - this->report.startTime;
    float sec_count = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return sec_count;
}

bool Writer::IsReady()
{
    bool rs = this->report.readySignal.try_acquire();
    return rs;
}

int Writer::GetProgress()
{
    return this->report.curChunk;
}

Writer::~Writer()
{
    this->Stop();
}

void Writer::writerWorker(WorkerTask *task, WorkerReport *report)
{
    std::ofstream file;
    file.rdbuf()->pubsetbuf(0, 0);
    size_t chunk_count = task->size_kb / task->cunksize;
    size_t chunk_len = task->cunksize * 1024 / sizeof(char);
    char *chunk = new char[chunk_len];
    chunk[chunk_len - 1] = '\0';
    file.open(task->path);
    report->startTime = std::chrono::steady_clock::now();
    report->curChunk = 0;
    for (int i = 0; i < chunk_count && task->stopSignal.try_acquire(); i++)
    {
        task->stopSignal.release();
        file.write(chunk, sizeof(char) * chunk_len);
        report->curChunk++;
    }
    report->endTime = std::chrono::steady_clock::now();
    file.close();
    task->stopSignal.release();
    report->readySignal.release();
    delete chunk;
}
