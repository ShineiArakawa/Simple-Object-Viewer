#pragma once

#include <SimView/Util/Logging.hpp>
#include <atomic>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>

namespace simview {
namespace util {

class StreamExecutor {
  using ui32 = std::uint_fast32_t;
  using ui64 = std::uint_fast64_t;

 public:
  StreamExecutor(const ui32& thread_count = std::thread::hardware_concurrency());
  ~StreamExecutor();

  ui32 thread_count() const {
    return thread_count_;
  }

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
  template <typename F, typename... Args, typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
#else
  template <typename F, typename... Args, typename R = typename std::result_of<std::decay_t<F>(std::decay_t<Args>...)>::type>
#endif
  std::future<R> submit(F&& func, const Args&&... args);

 private:
  template <typename F>
  void push_task(const F& task);
  void worker();

 private:
  mutable std::mutex tasks_mutex{};
  std::atomic<bool> running{true};
  std::queue<std::function<void()>> tasks{};
  const ui32 thread_count_;
  std::unique_ptr<std::thread[]> threads;
  std::condition_variable condition;
};

}  // namespace util
}  // namespace simview
