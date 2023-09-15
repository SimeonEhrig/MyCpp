#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

struct Job {
  int thread_id;
  std::vector<int> &data;

  void operator()(int const sleep_time) {
    std::cout << "Thread " << thread_id << ": start work\n";
    for (auto i = 0; i < data.size(); ++i) {
      data[i] += thread_id;
    }
    std::cout << "Thread " << thread_id << ": sleep " << sleep_time << "s.\n";
    std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
    std::cout << "Thread " << thread_id << ": Work is done\n";
  }
};

int main(int argc, char **argv) {
  std::vector<int> d1(10, 0);
  std::iota(d1.begin(), d1.end(), 1);
  std::vector<int> d2(d1), d3(d1);

  Job j1{0, d1};
  Job j2{1, d2};
  Job j3{2, d3};

  auto t1 = std::thread(j1, 5);
  auto t2 = std::thread(j2, 12);
  auto t3 = std::thread(j3, 7);

  t1.join();
  std::cout << "t1.join() done\n";
  t2.join();
  std::cout << "t2.join() done\n";
  t3.join();
  std::cout << "t3.join() done\n";

  return 0;
}
