#ifndef LIBAV_STREAMERS_H_
#define LIBAV_STREAMERS_H_

#include <image_transport/image_transport.h>
#include "web_video_server/image_streamer.h"
#include "async_web_server_cpp/http_request.hpp"
#include "async_web_server_cpp/http_connection.hpp"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/intreadwrite.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
}

namespace web_video_server
{

class LibavStreamer : public ImageStreamer
{
public:
  LibavStreamer(const async_web_server_cpp::HttpRequest &request, async_web_server_cpp::HttpConnectionPtr connection,
                image_transport::ImageTransport it, const std::string &format_name, const std::string &codec_name,
                const std::string &content_type);

  ~LibavStreamer();

  static void SetupAVLibrary();
protected:
  virtual void initializeEncoder();
  virtual void sendImage(const cv::Mat&, const ros::WallTime& time);
  virtual void initialize(const cv::Mat&);
  AVOutputFormat* output_format_;
  AVFormatContext* format_context_;
  AVCodec* codec_;
  AVCodecContext* codec_context_;
  AVStream* video_stream_;

private:
  AVFrame* frame_;
  AVPicture* picture_;
  AVPicture* tmp_picture_;
  struct SwsContext* sws_context_;
  ros::WallTime first_image_timestamp_;
  boost::mutex encode_mutex_;

  std::string format_name_;
  std::string codec_name_;
  std::string content_type_;
  int bitrate_;
  int qmin_;
  int qmax_;
  int gop_;
};

class LibavStreamerType : public ImageStreamerType
{
public:
  LibavStreamerType(const std::string &format_name, const std::string &codec_name, const std::string &content_type);

  boost::shared_ptr<ImageStreamer> create_streamer(const async_web_server_cpp::HttpRequest &request,
                                                   async_web_server_cpp::HttpConnectionPtr connection,
                                                   image_transport::ImageTransport it);

  std::string create_viewer(const async_web_server_cpp::HttpRequest &request);

private:
  const std::string format_name_;
  const std::string codec_name_;
  const std::string content_type_;
};

}

#endif
