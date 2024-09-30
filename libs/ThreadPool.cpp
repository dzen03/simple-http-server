#include "ThreadPool.h"

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

ThreadPool::ThreadPool(size_t numThreads) {
  for (size_t i = 0; i < numThreads; ++i) {
    workers_.emplace_back([this] { workerThread(); });
  }
}

ThreadPool::~ThreadPool() {
  {
    const std::unique_lock<std::mutex> lock(queueMutex_);
    stop_ = true;
  }
  condition_.notify_all();
  for (std::thread& worker : workers_) {
    worker.join();
  }
}

void ThreadPool::enqueue(std::function<void()> task) {
  {
    const std::unique_lock<std::mutex> lock(queueMutex_);
    tasks_.push(std::move(task));
  }
  condition_.notify_one();
}

void ThreadPool::workerThread() {
  while (true) {
    std::function<void()> task;
    {
      std::unique_lock<std::mutex> lock(queueMutex_);
      condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

      if (stop_ && tasks_.empty()) {
        return;
      }

      task = std::move(tasks_.front());
      tasks_.pop();
    }
    task();
  }
}
