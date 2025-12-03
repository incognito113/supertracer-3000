#pragma once

#include <functional>
#include <queue>
#include <thread>
#include <vector>

// Simple thread pool for parallel task execution
class ThreadPool {
 private:
  std::vector<std::thread> workers;         // Worker threads
  std::queue<std::function<void()>> tasks;  // Task queue
  std::mutex mtx;                           // Mutex for task queue
  std::condition_variable cv;               // Task available condition
  std::condition_variable cvFinished;       // All tasks finished condition
  std::atomic<int> active{0};               // Active task count
  std::atomic_bool abortAll{false};         // Flag to abort all tasks
  std::atomic_bool stop{false};             // Destruction flag

 public:
  ThreadPool(size_t numThreads);
  ~ThreadPool();

  template <class F>
  void enqueue(F&& f) {
    {
      std::unique_lock<std::mutex> lock(mtx);
      tasks.emplace(std::forward<F>(f));
    }
    cv.notify_one();
  }

  int size() const { return workers.size(); }
  int numTasks();

  bool shouldAbort();
  void wait();
  void clearTasks();
};