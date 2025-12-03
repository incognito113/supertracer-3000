#include "pool.hpp"

#include <condition_variable>
#include <iostream>

// Initialize pool with given number of threads
ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
  for (size_t i = 0; i < numThreads; ++i) {
    workers.emplace_back([this] {
      while (true) {
        std::function<void()> task;
        {
          // Acquire lock and wait for tasks
          std::unique_lock<std::mutex> lock(this->mtx);
          cv.wait(lock, [this] { return stop || !tasks.empty(); });
          if (this->stop && this->tasks.empty()) return;
          task = std::move(this->tasks.front());
          this->tasks.pop();
          active++;
        }
        task();
        // Mark task as done
        {
          std::unique_lock<std::mutex> lock(this->mtx);
          active--;
          if (active == 0 && tasks.empty()) cvFinished.notify_all();
        }
      }
    });
  }
}

// Destructor: join all threads
ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(mtx);
    stop = true;
  }
  cv.notify_all();
  for (std::thread& worker : workers) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

// Get number of pending tasks
int ThreadPool::numTasks() {
  std::unique_lock<std::mutex> lock(mtx);
  return tasks.size();
}

// Check if abort flag is set
bool ThreadPool::shouldAbort() {
  return abortAll.load(std::memory_order_acquire);
}

// Wait for all tasks to finish
void ThreadPool::wait() {
  std::unique_lock<std::mutex> lock(mtx);
  cvFinished.wait(lock, [this] { return tasks.empty() && active == 0; });
}

// Clear all pending tasks and abort active ones
void ThreadPool::clearTasks() {
  {
    std::unique_lock<std::mutex> lock(mtx);
    abortAll = true;
    // Clear pending tasks
    std::queue<std::function<void()>> empty;
    std::swap(tasks, empty);
  }

  // Wake all workers so they notice abortAll
  cv.notify_all();

  // Wait until all active tasks exit early
  {
    std::unique_lock<std::mutex> lock(mtx);
    cvFinished.wait(lock, [this] { return active == 0; });
  }

  // reset abort flag; pool can accept new tasks
  {
    std::unique_lock<std::mutex> lock(mtx);
    abortAll = false;
  }
}