#ifndef SCREENRECORDER_H
#define SCREENRECORDER_H
#define AUDIO 0
#define QT 1
#include "ffmpeg.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <math.h>
#include <string>
#include <thread>
#include <mutex>
#include <iomanip>
#if WIN32
#include <Windows.h>
#include <WinUser.h>
#elif linux
#include <condition_variable>
#endif


#define __STDC_CONSTANT_MACROS

//FFMPEG LIBRARIES
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavcodec/avfft.h"

#include "libavdevice/avdevice.h"

#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"

#include "libavformat/avformat.h"
#include "libavformat/avio.h"

	// libav resample
#include "libavutil/avutil.h"
#include "libavutil/opt.h"
#include "libavutil/common.h"
#include "libavutil/channel_layout.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
#include "libavutil/time.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "libavutil/file.h"

// lib swresample

#include "libswscale/swscale.h"

}



class ScreenRecorder
{
private:
	AVInputFormat* pAVInputFormat;
	AVInputFormat* audioInputFormat;
	AVOutputFormat* output_format;

	AVCodecContext* pAVCodecContext;
	AVCodecContext* inAudioCodecContext;
	AVCodecContext* outAudioCodecContext;
	AVFormatContext* pAVFormatContext;

	AVFrame* pAVFrame;
	AVFrame* outFrame;

	AVCodec* outVideoCodec;
	AVCodec* pAVCodec;
	AVCodec* outAVCodec;
	AVCodec* pLocalCodec;
	AVCodec* inAudioCodec;
	AVCodec* outAudioCodec;
	AVCodecParameters* pCodecParameters;
	AVCodecParameters* pAVCodecParameters;

	AVPacket* pAVPacket;
	AVPacket* packet;

	AVDictionary* options;
	AVDictionary* audioOptions;
	AVOutputFormat* outputAVFormat;
	AVFormatContext* outAVFormatContext;
	AVFormatContext* inAudioFormatContext;
	AVCodecContext* outAVCodecContext;
	AVCodecContext* outVideoCodecContext;
	AVAudioFifo* fifo;
	AVStream* videoSt;
	AVFrame* outAVFrame;
	std::mutex mu;
	std::mutex write_lock;
	std::condition_variable cv;
	const char* dev_name;
	const char* output_file;
	
	double video_pts;
	int magicNumber = 3000;
	int cropX = 0;
	int cropY = 0;
	int cropH = 1080;
	int cropW = 1920;
	int out_size;
	int codec_id;
	int value;
	int value2;
	int value3;
	int VideoStreamIndx;
	int audioStreamIndx;
	int outVideoStreamIndex;
	int outAudioStreamIndex;
	bool threading;
	std::thread* demux;
	std::thread* rescale;
	std::thread* mux;
	bool pauseCapture;
	bool stopCapture;
	bool started;
	bool activeMenu;
	int width, height;
	int w, h;


public:

	ScreenRecorder();
	ScreenRecorder(std::string RecPath);
	ScreenRecorder(const ScreenRecorder& p1);
	~ScreenRecorder();

	/* function to initiate communication with display library */
	int openVideoDevice();
	int openAudioDevice();
	int initOutputFile();
	void generateVideoStream();
	void generateAudioStream();
	int init_fifo();
	int add_samples_to_fifo(uint8_t** converted_input_samples, const int frame_size);
	int initConvertedSamples(uint8_t*** converted_input_samples, AVCodecContext* output_codec_context, int frame_size);
	void captureAudio();
	int captureVideoFrames();
	void CreateThreads();
	void SetUpScreenRecorder();
	void StopRecording();
	void InnerSetup();
	void PauseRecording();
#if WIN32
	std::string RecordingPath = "..\\media\\output.mp4";
#elif
	std::string RecordingPath = "../media//output.mp4";
#endif
};

#endif
