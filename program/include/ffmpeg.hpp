#pragma once

#include <cstdarg>
#include <string>

extern "C"
{
#include "libavutil/error.h"
}

namespace tuim
{
  class FFmpeg
  {
  public:
    struct Tags
    {
      std::string artist = "";
      std::string title = "";
    };

  public:
    FFmpeg();

    Tags get_tags(const std::string &path);
    void update_mean_volume(const std::string &path);

  private:
    void handle_averror(const std::string &message, const int &code);
    static void custom_log_callback(void *ptr, int level, const char *fmt, va_list vargs);

  public:
    inline static double last_mean_volume = 0.0;

  private:
    char error_buffer[AV_ERROR_MAX_STRING_SIZE] = {};
  };

  inline FFmpeg ffmpeg;
}
