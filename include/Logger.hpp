#pragma once
#include <fstream>
#include <mutex>
#include <string>

class Logger {
public:
  /* singleton:     Logger::instance().log("msg");              */
  static Logger& instance();

  void log(const std::string& msg);
  void warn(const std::string& msg) { log("WARN: " + msg); }
  void err(const std::string& msg) { log("ERR : " + msg); }

private:
  Logger(); // opens file
  std::ofstream _out;
  std::mutex _mtx;
};
