#include "main.hpp"

#include <cinttypes>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include "SDL2/SDL_timer.h"
#include "taglib/fileref.h"
#include "taglib/tstring.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavcodec/codec.h"
#include "libavcodec/packet.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/channel_layout.h"
#include "libavutil/dict.h"
#include "libavutil/frame.h"
#include "libavutil/log.h"
#include "libavutil/samplefmt.h"
}

#include "database.hpp"
#include "player.hpp"
#include "song.hpp"

static float g_mean_volume_db = 0.0f;
#pragma warning(suppress : 4068)
#pragma clang diagnostic push
#pragma warning(suppress : 4068)
#pragma clang diagnostic ignored "-Wformat-nonliteral"
extern "C" void custom_log_callback(void *ptr, int level, const char *fmt, va_list vargs)
{
  if (ptr == nullptr || level < 0) {}
  char buffer[1024];
  std::string line;

  va_list vargs_copy1, vargs_copy2;
  va_copy(vargs_copy1, vargs);
  va_copy(vargs_copy2, vargs);
  int length = vsnprintf(buffer, sizeof(buffer), fmt, vargs_copy1);
  va_end(vargs_copy1);

  if ((unsigned long long)length < sizeof(buffer))
    line = buffer;
  else
  {
    std::vector<char> dynamic_buffer((size_t)length + 1);
    vsnprintf(dynamic_buffer.data(), dynamic_buffer.size(), fmt, vargs_copy2);
    line = dynamic_buffer.data();
  }
  va_end(vargs_copy2);

  size_t pos = line.find("mean_volume:");
  if (pos != std::string::npos)
  {
    float val = 0.0f;
    if (sscanf_s(line.c_str() + pos, "mean_volume: %f dB", &val) == 1) { g_mean_volume_db = val; }
  }

  // fprintf(stderr, "%s", buffer);
}
#pragma warning(suppress : 4068)
#pragma clang diagnostic pop

int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    for (int i = 1; i < argc; i++) std::cerr << "Unexpected argument: " << argv[i] << std::endl;
    exit(EXIT_FAILURE);
  }

  std::filesystem::path path = "C:/Users/conno/Music/Songs";
  if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
  {
    std::cerr << "Invalid path: " << path << std::endl;
    exit(EXIT_FAILURE);
  }

  tuim::database.execute("CREATE TABLE IF NOT EXISTS " + tuim::Song::table_definition + ";");
  tuim::database.execute("BEGIN TRANSACTION;");
  for (const auto &entry : std::filesystem::directory_iterator(path))
  {
    if (!entry.is_regular_file() || entry.path().extension() != ".mp3") continue;

    TagLib::FileRef file_reference(entry.path().string().c_str());
    if (file_reference.isNull())
    {
      std::cerr << "FileRef Error: " << entry.path().string() << " could not be loaded!" << std::endl;
      exit(EXIT_FAILURE);
    }
    TagLib::String artist_tag = file_reference.tag()->artist();
    std::string artist = artist_tag.to8Bit(true);
    TagLib::String title_tag = file_reference.tag()->title();
    std::string title = title_tag.to8Bit(true);

    tuim::database.execute("INSERT OR IGNORE INTO " + tuim::Song::table_reference + " VALUES (?, ?, ?, ?);",
                           {entry.path().string(), artist, title, 0.0});
  }
  tuim::database.execute("COMMIT;");

  std::vector<tuim::Song> target_songs =
    tuim::database.query<tuim::Song>("SELECT * FROM " + tuim::Song::table_name + " ORDER BY RANDOM() LIMIT 1;");
  if (target_songs.empty())
  {
    std::cerr << "No songs found!" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << target_songs.size() << std::endl;
  std::cout << target_songs[0].path.string() << std::endl;
  std::cout << target_songs[0].artist << std::endl;
  std::cout << target_songs[0].title << std::endl;
  std::cout << target_songs[0].mean_volume << std::endl;

  AVFormatContext *fmt_ctx = nullptr;
  AVCodecContext *dec_ctx = nullptr;
  AVFilterGraph *filter_graph = nullptr;
  AVFilterContext *buffersrc_ctx = nullptr;
  AVFilterContext *buffersink_ctx = nullptr;

  av_log_set_callback(custom_log_callback);

  avformat_open_input(&fmt_ctx, target_songs[0].path.string().c_str(), nullptr, nullptr);
  avformat_find_stream_info(fmt_ctx, nullptr);

  AVDictionaryEntry *title_tag = av_dict_get(fmt_ctx->metadata, "title", nullptr, 0);
  AVDictionaryEntry *artist_tag = av_dict_get(fmt_ctx->metadata, "artist", nullptr, 0);
  std::string title = title_tag ? title_tag->value : "";
  std::string artist = artist_tag ? artist_tag->value : "";

  int audio_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
  AVStream *audio_stream = fmt_ctx->streams[audio_stream_index];

  const AVCodec *decoder = avcodec_find_decoder(audio_stream->codecpar->codec_id);
  dec_ctx = avcodec_alloc_context3(decoder);
  avcodec_parameters_to_context(dec_ctx, audio_stream->codecpar);
  avcodec_open2(dec_ctx, decoder, nullptr);

  AVChannelLayout layout;
  if (audio_stream->codecpar->ch_layout.order == AV_CHANNEL_ORDER_NATIVE)
    layout = audio_stream->codecpar->ch_layout;
  else
    av_channel_layout_default(&layout, audio_stream->codecpar->ch_layout.nb_channels);

  uint64_t channel_layout = 0;
  if (layout.order == AV_CHANNEL_ORDER_NATIVE) { channel_layout = layout.u.mask; }

  char args[512];
  snprintf(args, sizeof(args), "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%" PRIx64,
           audio_stream->time_base.num, audio_stream->time_base.den, dec_ctx->sample_rate,
           av_get_sample_fmt_name(dec_ctx->sample_fmt), channel_layout);

  filter_graph = avfilter_graph_alloc();

  const AVFilter *abuffer = avfilter_get_by_name("abuffer");
  const AVFilter *volumedetect = avfilter_get_by_name("volumedetect");
  const AVFilter *abuffersink = avfilter_get_by_name("abuffersink");

  avfilter_graph_create_filter(&buffersrc_ctx, abuffer, "in", args, nullptr, filter_graph);
  avfilter_graph_create_filter(&buffersink_ctx, abuffersink, "out", nullptr, nullptr, filter_graph);

  AVFilterContext *vol_ctx = nullptr;
  avfilter_graph_create_filter(&vol_ctx, volumedetect, "volume", nullptr, nullptr, filter_graph);

  avfilter_link(buffersrc_ctx, 0, vol_ctx, 0);
  avfilter_link(vol_ctx, 0, buffersink_ctx, 0);

  avfilter_graph_config(filter_graph, nullptr);

  AVPacket *pkt = av_packet_alloc();
  AVFrame *frame = av_frame_alloc();
  AVFrame *filt_frame = av_frame_alloc();

  while (av_read_frame(fmt_ctx, pkt) >= 0)
  {
    if (pkt->stream_index == audio_stream_index)
    {
      if (avcodec_send_packet(dec_ctx, pkt) >= 0)
      {
        while (avcodec_receive_frame(dec_ctx, frame) >= 0)
        {
          av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF);

          while (av_buffersink_get_frame(buffersink_ctx, filt_frame) >= 0);
          av_frame_unref(frame);
        }
      }
    }
    av_packet_unref(pkt);
  }

  avcodec_send_packet(dec_ctx, nullptr);
  while (avcodec_receive_frame(dec_ctx, frame) >= 0)
  {
    av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF);
    while (av_buffersink_get_frame(buffersink_ctx, filt_frame) >= 0);
    av_frame_unref(frame);
  }

  av_frame_free(&frame);
  av_frame_free(&filt_frame);
  av_packet_free(&pkt);
  avcodec_free_context(&dec_ctx);
  avformat_close_input(&fmt_ctx);
  avfilter_graph_free(&filter_graph);

  std::cout << "Mean: " << g_mean_volume_db << std::endl;
  std::cout << "Artist: " << artist << std::endl;
  std::cout << "Title: " << title << std::endl;

  tuim::Player player;
  player.set_volume(20);
  player.load(target_songs[0].path);
  player.play();
  while (player.music_active()) SDL_Delay(1000);
  player.unload();

  std::cout << "Done!" << std::endl;
  return 0;
}
