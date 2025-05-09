#include "Logger.hpp"
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <sstream>

using sys_clock = std::chrono::system_clock;

/* --- helpers --- */
static std::string now_ymd_hms() {
  auto tp    = sys_clock::now();
  auto tt    = sys_clock::to_time_t(tp);
  std::tm tm = *std::localtime(&tt);

  std::ostringstream os;
  os << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
  return os.str();
}

static std::string today_dir() {
  auto tp    = sys_clock::now();
  auto tt    = sys_clock::to_time_t(tp);
  std::tm tm = *std::localtime(&tt);

  std::ostringstream os;
  os << std::put_time(&tm, "%Y-%m-%d");
  return os.str();
}

/* --- Logger --- */
Logger& Logger::instance() {
  static Logger L;
  return L;
}

Logger::Logger() {
  namespace fs = std::filesystem;
  fs::create_directories("documents/logs");

  const char* tag   = std::getenv("SAT_LOG_TAG"); // optional
  std::string fname = "documents/logs/" + today_dir() + "/";
  fs::create_directories(fname); // date sub‑dir

  fname += now_ymd_hms();
  if (tag && *tag)
    fname += '_' + std::string(tag);
  fname += ".log";

  _out.open(fname, std::ios::app);
}

void Logger::log(const std::string& m) {
  std::lock_guard<std::mutex> g(_mtx);

  auto tp = sys_clock::now();
  auto ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000;
  auto tt    = sys_clock::to_time_t(tp);
  std::tm tm = *std::localtime(&tt);

  _out << std::put_time(&tm, "%H:%M:%S") << '.' << std::setw(3) << std::setfill('0')
       << ms.count() << ' ' << m << '\n';
}
