#include "ffmpeg.hpp"

#include <cinttypes>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

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
#include "libavutil/error.h"
#include "libavutil/frame.h"
#include "libavutil/log.h"
#include "libavutil/samplefmt.h"
}

namespace tuim
{
  FFmpeg::FFmpeg() { av_log_set_callback(custom_log_callback); }

  void FFmpeg::get_tags(const std::string &path)
  {
    AVFormatContext *fmt_ctx = nullptr;
    AVDictionary *options = nullptr;
    av_dict_set(&options, "scan_all_pmts", "0", 0);
    av_dict_set(&options, "analyzeduration", "0", 0);
    av_dict_set(&options, "probesize", "32", 0);
    if (int code = avformat_open_input(&fmt_ctx, path.c_str(), nullptr, nullptr); code != 0)
      handle_averror("Failed to open input: " + path, code);
    av_dict_free(&options);

    AVDictionaryEntry *artist_tag = av_dict_get(fmt_ctx->metadata, "artist", nullptr, 0);
    AVDictionaryEntry *title_tag = av_dict_get(fmt_ctx->metadata, "title", nullptr, 0);
    last_artist = artist_tag ? artist_tag->value : "";
    last_title = title_tag ? title_tag->value : "";

    avformat_close_input(&fmt_ctx);
  }

  void FFmpeg::get_mean_volume(const std::string &path)
  {
    AVFormatContext *fmt_ctx = nullptr;
    if (int code = avformat_open_input(&fmt_ctx, path.c_str(), nullptr, nullptr); code != 0)
      handle_averror("Failed to open input: " + path, code);
    if (int code = avformat_find_stream_info(fmt_ctx, nullptr); code < 0)
      handle_averror("Failed to find stream info: " + path, code);

    int audio_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    AVStream *audio_stream = fmt_ctx->streams[audio_stream_index];
    const AVCodec *decoder = avcodec_find_decoder(audio_stream->codecpar->codec_id);
    AVCodecContext *dec_ctx = avcodec_alloc_context3(decoder);
    if (int code = avcodec_parameters_to_context(dec_ctx, audio_stream->codecpar); code < 0)
      handle_averror("Failed to copy codec parameters: " + path, code);
    if (int code = avcodec_open2(dec_ctx, decoder, nullptr); code < 0)
      handle_averror("Failed to open codec: " + path, code);

    AVChannelLayout layout = {};
    bool used_default_channel_layout = false;
    if (audio_stream->codecpar->ch_layout.order == AV_CHANNEL_ORDER_NATIVE)
      layout = audio_stream->codecpar->ch_layout;
    else
    {
      av_channel_layout_default(&layout, audio_stream->codecpar->ch_layout.nb_channels);
      used_default_channel_layout = true;
    }
    uint64_t channel_layout = 0;
    if (layout.order == AV_CHANNEL_ORDER_NATIVE) channel_layout = layout.u.mask;

    char args[512];
    snprintf(args, sizeof(args), "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%" PRIx64,
             audio_stream->time_base.num, audio_stream->time_base.den, dec_ctx->sample_rate,
             av_get_sample_fmt_name(dec_ctx->sample_fmt), channel_layout);

    AVFilterGraph *filter_graph = avfilter_graph_alloc();
    const AVFilter *abuffer = avfilter_get_by_name("abuffer");
    const AVFilter *abuffersink = avfilter_get_by_name("abuffersink");
    const AVFilter *volumedetect = avfilter_get_by_name("volumedetect");
    AVFilterContext *buffersrc_ctx = nullptr;
    AVFilterContext *buffersink_ctx = nullptr;
    AVFilterContext *vol_ctx = nullptr;
    if (int code = avfilter_graph_create_filter(&buffersrc_ctx, abuffer, "in", args, nullptr, filter_graph); code < 0)
      handle_averror("Failed to create buffer filter: " + path, code);
    if (int code = avfilter_graph_create_filter(&buffersink_ctx, abuffersink, "out", nullptr, nullptr, filter_graph);
        code < 0)
      handle_averror("Failed to create buffersink filter: " + path, code);
    if (int code = avfilter_graph_create_filter(&vol_ctx, volumedetect, "volume", nullptr, nullptr, filter_graph);
        code < 0)
      handle_averror("Failed to create volume filter: " + path, code);
    if (int code = avfilter_link(buffersrc_ctx, 0, vol_ctx, 0); code != 0)
      handle_averror("Failed to link buffer filter: " + path, code);
    if (int code = avfilter_link(vol_ctx, 0, buffersink_ctx, 0); code != 0)
      handle_averror("Failed to link volume filter: " + path, code);
    if (int code = avfilter_graph_config(filter_graph, nullptr); code < 0)
      handle_averror("Failed to configure filter graph: " + path, code);

    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    AVFrame *filt_frame = av_frame_alloc();
    while (av_read_frame(fmt_ctx, pkt) >= 0)
    {
      if (pkt->stream_index == audio_stream_index)
      {
        if (avcodec_send_packet(dec_ctx, pkt) >= 0)
          while (avcodec_receive_frame(dec_ctx, frame) >= 0)
          {
            if (int code = av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF); code < 0)
              handle_averror("Failed to add flags to buffer: " + path, code);
            while (av_buffersink_get_frame(buffersink_ctx, filt_frame) >= 0);
            av_frame_unref(frame);
          }
      }
      av_packet_unref(pkt);
    }
    avcodec_send_packet(dec_ctx, nullptr);
    while (avcodec_receive_frame(dec_ctx, frame) >= 0)
    {
      if (int code = av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF); code < 0)
        handle_averror("Failed to add flags to buffer: " + path, code);
      while (av_buffersink_get_frame(buffersink_ctx, filt_frame) >= 0);
      av_frame_unref(frame);
    }

    av_frame_free(&filt_frame);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avfilter_graph_free(&filter_graph);
    if (used_default_channel_layout) av_channel_layout_uninit(&layout);
    avcodec_free_context(&dec_ctx);
    avformat_close_input(&fmt_ctx);
  }

  void FFmpeg::handle_averror(const std::string &message, const int &code)
  {
    av_strerror(code, error_buffer, sizeof(error_buffer));
    std::cerr << message << std::endl;
    std::cerr << "Error code: " << std::string(error_buffer) << std::endl;
    exit(EXIT_FAILURE);
  }

#pragma warning(suppress : 4068)
#pragma clang diagnostic push
#pragma warning(suppress : 4068)
#pragma clang diagnostic ignored "-Wformat-nonliteral"
  extern "C" void FFmpeg::custom_log_callback(void *ptr, int level, const char *fmt, va_list vargs)
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
      if (sscanf_s(line.c_str() + pos, "mean_volume: %f dB", &val) == 1) { last_mean_volume = val; }
    }

    // fprintf(stderr, "%s", buffer);
  }
#pragma warning(suppress : 4068)
#pragma clang diagnostic pop
}
