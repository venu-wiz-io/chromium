#include "vizio_sdk_logger.h"

namespace viziosdk {

    std_out_logger::std_out_logger(const logging_config_t& config) : logger(config), levels(config.find("color") != config.end() ? colored : uncolored) {}
    std_out_logger::~std_out_logger() {}
    void std_out_logger::log(const std::string& message, const log_level level) {
      if(level < LOG_LEVEL_CUTOFF)
        return;
      std::string output;
      output.reserve(message.length() + 64);
      output.append(timestamp());
      output.append(levels.find(level)->second);
      output.append(message);
      output.push_back('\n');
      log(output);
    }
    void std_out_logger::log(const std::string& message) {
      //cout is thread safe, to avoid multiple threads interleaving on one line
      //though, we make sure to only call the << operator once on std::cout
      //otherwise the << operators from different threads could interleave
      //obviously we dont care if flushes interleave
      //std::lock_guard<std::mutex> lk{lock};
      std::cout << message;
      std::cout.flush();
    }

    file_logger::file_logger(const logging_config_t& config):logger(config) {
      //grab the file name
      auto name = config.find("file_name");
      if(name == config.end())
#ifdef NO_EXCEPTIONS
        return;
#else
        throw std::runtime_error("No output file provided to file logger");
#endif
      file_name = name->second;

      //if we specify an interval
      reopen_interval = std::chrono::seconds(300);
      auto interval = config.find("reopen_interval");
      if(interval != config.end())
      {
#ifdef NO_EXCEPTIONS
          reopen_interval = std::chrono::seconds(std::stoul(interval->second));
#else
        try {
          reopen_interval = std::chrono::seconds(std::stoul(interval->second));
        }
        catch(...) {
          throw std::runtime_error(interval->second + " is not a valid reopen interval");
        }
#endif
      }

      //crack the file open
      reopen();
    }
    file_logger::~file_logger() {}
    void file_logger::log(const std::string& message, const log_level level) {
      if(level < LOG_LEVEL_CUTOFF)
        return;
      std::string output;
      output.reserve(message.length() + 64);
      output.append(timestamp());
      output.append(uncolored.find(level)->second);
      output.append(message);
      output.push_back('\n');
      log(output);
    }
    void file_logger::log(const std::string& message) {
      lock.lock();
      file << message;
      file.flush();
      lock.unlock();
      reopen();
    }


    logger_factory::logger_factory() {
      creators.emplace("", [](const logging_config_t& config)->logger*{return new logger(config);});
      creators.emplace("std_out", [](const logging_config_t& config)->logger*{return new std_out_logger(config);});
      creators.emplace("file", [](const logging_config_t& config)->logger*{return new file_logger(config);});
    }
    logger_factory::~logger_factory() {}

  //statically get a factory
#if 0
  inline logger_factory& get_factory() {
    static logger_factory factory_singleton{};
    return factory_singleton;
  }

  //get at the singleton
  inline logger& get_logger(const logging_config_t& config = { {"type", "std_out"}, {"color", ""} }) {
    static std::unique_ptr<logger> singleton(get_factory().produce(config));
    return *singleton;
  }

  //configure the singleton (once only)
  inline void configure(const logging_config_t& config) {
    get_logger(config);
  }

  //statically log manually without the macros below
  inline void log(const std::string& message, const log_level level) {
    get_logger().log(message, level);
  }

  //statically log manually without a level or maybe with a custom one
  inline void log(const std::string& message) {
    get_logger().log(message);
  }

  //these standout when reading code
  inline void TRACE(const std::string& message) {
    get_logger().log(message, log_level::TRACE);
  };
  inline void DEBUG(const std::string& message) {
    get_logger().log(message, log_level::DEBUG);
  };
  inline void INFO(const std::string& message) {
    get_logger().log(message, log_level::INFO);
  };
  inline void WARN(const std::string& message) {
    get_logger().log(message, log_level::WARN);
  };
  inline void ERROR(const std::string& message) {
    get_logger().log(message, log_level::ERROR);
  };
#endif
}

