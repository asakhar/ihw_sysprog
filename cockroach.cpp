#include "cockroach.hpp"

Cockroach::Cockroach(Cockroach &move)
    : distance(move.distance),
      name{std::move(move.name)},
      mythread(std::move(move.mythread)) {}
Cockroach::Cockroach(std::string const &roachName,
                     std::condition_variable &finished, double threshold,
                     std::condition_variable &startNotif,
                     std::atomic<Cockroach *> &winner, bool const &started)
    : name{roachName},
      mythread{[&startNotif, &finished, threshold, this, &winner, &started]() {
        std::mt19937 twister{std::random_device{}()};
        std::uniform_real_distribution<double> distribution{-.0000005, .000001};
        {
          std::mutex startMut;
          std::unique_lock<std::mutex> lock{startMut};
          startNotif.wait(lock, [&started] { return started; });
        }
        while (distance < threshold && !winner.load()) {
          distance += distribution(twister);
        }
        Cockroach *old = nullptr;
        winner.compare_exchange_strong(old, this, std::memory_order::seq_cst);
        finished.notify_one();
      }} {}
Cockroach::~Cockroach() { mythread.join(); }