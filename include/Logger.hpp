#pragma once
#include <fstream>
#include <mutex>
#include <string>

class Logger {
public:
  /* singleton access */
  static Logger& instance();

  /* simple printf‑style helpers */
  void log(const std::string& msg);
  void warn(const std::string& msg) { log("WARN: " + msg); }
  void err(const std::string& msg) { log("ERR : " + msg); }

private:
  Logger(); // opens log file
  std::ofstream _out;
  std::mutex _mtx;
};
