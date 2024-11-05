#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
 public:
  explicit ThreadPool(size_t numThreads);
  ~ThreadPool();

  ThreadPool(const ThreadPool& server) = delete;
  auto operator=(ThreadPool& other) = delete;
  ThreadPool(ThreadPool&& server) = delete;
  auto operator=(ThreadPool&& other) = delete;

  void enqueue(std::function<void()> task);

 private:
  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;

  std::mutex queueMutex_;
  std::condition_variable condition_;
  bool stop_{false};

  void workerThread();
};
