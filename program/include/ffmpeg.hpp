#pragma once

#include <cstdarg>
#include <string>

namespace tuim
{
  class FFmpeg
  {
  public:
    FFmpeg();

    void get_tags(const std::string &path);
    void get_mean_volume(const std::string &path);

  private:
    static void custom_log_callback(void *ptr, int level, const char *fmt, va_list vargs);

  public:
    inline static float last_mean_volume = 0.0f;
    inline static std::string last_artist = "";
    inline static std::string last_title = "";
  };

  inline FFmpeg ffmpeg;
}
