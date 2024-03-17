#pragma once

#include <Util/Logging.hpp>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace simview {
namespace util {

class StreamExecutor {
 private:
  std::vector<std::thread> _workers;
  std::queue<std::function<void()>> _tasks;

  std::mutex _queueMutex;
  std::condition_variable _condition;
  bool _stop;

 public:
  StreamExecutor(size_t threads);
  ~StreamExecutor();

  template <class F, class... Args>
  auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
};

}  // namespace util
}  // namespace simview
