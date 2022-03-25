#pragma once
#include <thread>
#include <string>
#include <condition_variable>
#include <random>

class Cockroach {
  double distance = 0;
  std::string name;
  std::thread mythread;

 public:
  Cockroach() = delete;
  Cockroach(Cockroach const&) = delete;
  Cockroach(Cockroach &move);
  Cockroach& operator=(Cockroach const&) = delete;
  Cockroach& operator=(Cockroach& move) = delete;
  Cockroach(std::string const &roachName, std::condition_variable &finished,
            double threshold, std::condition_variable &startNotif,
            std::atomic<Cockroach *> &winner, bool const &started);
  inline std::string const& getName() const { return name; }
  inline double getDist() const { return distance; }
  ~Cockroach();
};