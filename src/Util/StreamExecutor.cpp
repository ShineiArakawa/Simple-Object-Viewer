#include <SimView/Util/StreamExecutor.hpp>

namespace simview {
namespace util {

StreamExecutor::StreamExecutor(size_t threads)
    : _stop(false) {
  // Generate execution threads

  for (size_t iThread = 0; iThread < threads; ++iThread) {  // for-loop for threads
    _workers.emplace_back([this] {                          // Lambda func
      while (true) {                                        // while for tasks
        std::function<void()> task;

        {
          // Execution
          std::unique_lock<std::mutex> lock(this->_queueMutex);

          this->_condition.wait(lock, [this] { return this->_stop || !this->_tasks.empty(); });

          if (this->_stop && this->_tasks.empty()) {
            return;
          }

          task = std::move(this->_tasks.front());
          this->_tasks.pop();
        }

        task();
      }  // end of while-loop for tasks
    }    // end of lambda func
    );   // end of emplace_back
  }      // end of for-loop for threads

  return;
}

StreamExecutor::~StreamExecutor() {
  {
    std::unique_lock<std::mutex> lock(_queueMutex);
    _stop = true;
  }

  _condition.notify_all();

  for (std::thread& worker : _workers) {
    // Wait until all jobs done
    worker.join();
  }

  return;
}

template <class F, class... Args>
auto StreamExecutor::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
  using return_type = typename std::result_of<F(Args...)>::type;
  auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<return_type> res = task->get_future();
  {
    std::unique_lock<std::mutex> lock(_queueMutex);

    // don't allow enqueueing after stopping the pool
    if (_stop) {
      throw std::runtime_error("enqueue on stopped ThreadPool");
    }

    _tasks.emplace([task]() { (*task)(); });
  }
  _condition.notify_one();

  return res;
}

}  // namespace util
}  // namespace simview