#include "Logger.hpp"
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <sstream>

using clock = std::chrono::system_clock;

static std::string today() {
  auto now   = clock::now();
  auto t     = clock::to_time_t(now);
  std::tm tm = *std::localtime(&t);
  std::ostringstream os;
  os << std::put_time(&tm, "%Y-%m-%d");
  return os.str();
}

Logger& Logger::instance() {
  static Logger s;
  return s;
}

Logger::Logger() {
  namespace fs = std::filesystem;
  fs::create_directories("documents/logs");
  _out.open("documents/logs/" + today() + ".log", std::ios::app);
}

void Logger::log(const std::string& m) {
  std::lock_guard<std::mutex> lk(_mtx);
  auto now = clock::now();
  auto ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) %
      1000;

  std::time_t tt = clock::to_time_t(now);
  std::tm tm     = *std::localtime(&tt);

  _out << std::put_time(&tm, "%H:%M:%S") << '.' << std::setw(3) << std::setfill('0')
       << ms.count() << "  " << m << '\n';
}
