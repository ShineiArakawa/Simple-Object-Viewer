#include <SimView/Util/StreamExecutor.hpp>

namespace simview {
namespace util {

StreamExecutor::StreamExecutor(const ui32& thread_count)
    : thread_count_{thread_count ? thread_count : std::thread::hardware_concurrency()} {
  threads.reset(new std::thread[thread_count_]);

  for (ui32 i = 0; i < thread_count_; ++i) {
    threads[i] = std::thread(&StreamExecutor::worker, this);
  }
}

StreamExecutor::~StreamExecutor() {
  {
    // Lock task queue to prevent adding new task.
    std::lock_guard<std::mutex> lock(tasks_mutex);
    running = false;
  }

  // Wake up all threads so that they may exist
  condition.notify_all();

  for (ui32 i = 0; i < thread_count_; ++i) {
    threads[i].join();
  }
}

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
template <typename F, typename... Args, typename R>
#else
template <typename F, typename... Args, typename R>
#endif
std::future<R> StreamExecutor::submit(F&& func, const Args&&... args) {
  auto task = std::make_shared<std::packaged_task<R()>>([func, args...]() {
    return func(args...);
  });
  auto future = task->get_future();

  push_task([task]() { (*task)(); });
  return future;
}

template <typename F>
void StreamExecutor::push_task(const F& task) {
  {
    const std::lock_guard<std::mutex> lock(tasks_mutex);

    if (!running) {
      throw std::runtime_error("Cannot schedule new task after shutdown.");
    }

    tasks.push(std::function<void()>(task));
  }

  condition.notify_one();
}

void StreamExecutor::worker() {
  for (;;) {
    std::function<void()> task;

    {
      std::unique_lock<std::mutex> lock(tasks_mutex);
      condition.wait(lock, [&] { return !tasks.empty() || !running; });

      if (!running && tasks.empty()) {
        return;
      }

      task = std::move(tasks.front());
      tasks.pop();
    }

    task();
  }
}

}  // namespace util
}  // namespace simview