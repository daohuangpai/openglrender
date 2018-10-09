#pragma once
#include <memory>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavdevice/avdevice.h>
#include <libavutil/imgutils.h>
}
#include "../ThridUtil/circlebuf.h"
#include "../ThridUtil/decode.h"
#include "../common/Signal.h"


class FFmedia : public GLUI::SendObject{
public:
	void OnVedioUpdate(FFmedia* media)
	{
		Run(&FFmedia::OnVedioUpdate, std::move(media));
	}
	void OnAudioUpdate(FFmedia* media)
	{
		Run(&FFmedia::OnAudioUpdate, std::move(media));
	}
	void OnEof()
	{
		Run(&FFmedia::OnEof);
	}
 

	virtual ~FFmedia();
private:
	bool Init();
	bool reset(int64_t seek = -1);
	bool runonthread();
	int prepare_packet();
	bool initVedioScaling();
	bool vedio_rescaling();
	bool initAudioScaling();
	bool audio_resample();
public:
	void start_decode_thread();
	void pause();
	void stop();
	//void resume();
	void clear();


private:
	AVFormatContext* m_formatContext = nullptr;
	struct mp_decode m_v;///<视频解码器
	struct mp_decode m_a;///<音频解码器

public:
	//setting
	char *m_path; //播放的url数据路径
	char *m_format_name = nullptr;//播放的格式，如"MP4"等，没写null则默认去找
	int m_buffering = 0xffff;///<如果不是播放本地文件，这个用来设置缓冲流大小，大于0才设置
	bool m_is_local_file;///<该块是否是本地文件来的也就是m_formatContext是否是本地文件的会话
	bool m_hw = false;///<是否使用硬件解码

	//contrl
	bool m_stop = false;
	bool m_pause = false;
	int m_seek_pos = -1;

	boost::mutex m_contrlMutex;
	boost::mutex m_PausedMutex;
	boost::condition_variable m_PausedCV;


	//info
	bool eof = false;

    //av
	struct SwsContext *m_avswscale = nullptr;
	int m_scale_linesizes[4] = {0};///<对应scale_pic，比如scale_pic[0]表示第一个通道的图像一行的数据，scale_pic[0][y * scale_linesizes【0】+x]代表第0个通道第(x,y)个像素值
							 ///<如果这里scale_linesizes[0]是负数，表示这边图像是反转的，也就是scale_pic[0]对应右下角的像素，最后一个像素对应左上角
	uint8_t *m_scale_pic[4] = {0};///<其中视频转码（可以播放了的）后的数据指针，4代表四个图像通道，比如scale_pic[0]表示第一个通道的图像数据
    //av setting
	AVPixelFormat m_dstFormat = AV_PIX_FMT_NONE;///<目标图像格式
	int m_dstHeight = 0;///<目标图像高度
	int m_dstWidth = 0;///<目标图像宽度


    //ad
	struct SwrContext   *m_adswscale = nullptr;///<音频转化器的会话句柄
	uint8_t             *m_output_buffer[AV_NUM_DATA_POINTERS] = {0};///<输出音频的数据各通道指针，这个参数在audio_resampler_resample设置后有效，初始为null
	int                 m_output_size = 0;///<输出音频的数据个数，这个参数在audio_resampler_resample设置后有效，初始设为0
    //ad setting
	int            m_output_layout = 0;///<输出音频的通道数类型
	enum AVSampleFormat m_output_format = AV_SAMPLE_FMT_NONE;///<输出音频的采样数据结构
	
	uint32_t            m_output_ch = 0;///<输出音频的通道数
	uint32_t            m_output_freq = 0;///<输出音频的每秒采样率
	uint32_t            m_output_planes = 0;


	std::unique_ptr<boost::thread> m_mainThread;
};

