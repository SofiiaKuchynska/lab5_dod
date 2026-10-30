#include <iostream>
#include <string>
#include <chrono>
#include <future>
#include <syncstream>
#include <thread>

// slow computation simulator (7 seconds)
void run_slow_task(const std::string& task_name) {
  std::this_thread::sleep_for(std::chrono::seconds(7));
  std::osyncstream bout(std::cout);
  bout << "Slow task completed: " << task_name << "\n";
}

// quick computation simulator (1 second)
void run_quick_task(const std::string& task_name) {
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::osyncstream bout(std::cout);
  bout << "Quick task completed: " << task_name << "\n";
}

// optimal execution scheme for variant 7
void work() {
  auto start_time = std::chrono::high_resolution_clock::now();

  // Stage 1 [0s - 7s]:
  // Launch A2 (slow, 7s) and B2 (slow, 7s) asynchronously.
  // Main thread executes A1 (1s), A3 (1s), A4 (1s) sequentially (takes 3s total).
  auto fut_a2 = std::async(std::launch::async, run_slow_task, "A2");
  auto fut_b2 = std::async(std::launch::async, run_slow_task, "B2");

  run_quick_task("A1");
  run_quick_task("A3");
  run_quick_task("A4");

  fut_a2.get(); // wait for A2 at t=7s
  fut_b2.get(); // wait for B2 at t=7s

  // Stage 2 [7s - 8s]:
  // B1 depends on A1, A2. Both are done! Run B1 on main thread.
  run_quick_task("B1");

  // Stage 3 [8s - 9s]:
  // C depends on B1, B2. Both are done! Run C on main thread.
  run_quick_task("C");

  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> total_duration = end_time - start_time;

  std::osyncstream bout(std::cout);
  bout << "Execution duration: " << total_duration.count() << " seconds\n";
  bout << "Work is done!\n";
}

int main() {
  work();
  return 0;
}