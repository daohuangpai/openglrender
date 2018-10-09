#include "Decoder.h"
#include <mutex>
#include <windows.h>
#include "../common/StringStream.h"
#include <boost/log/trivial.hpp>

#include <memory>
#include <utility>

template<typename T, typename D>
inline auto MakeGuard(T* t, D d)
{
	return std::unique_ptr<T, D>(t, std::move(d));
}
static bool have_clockfreq = false;
static LARGE_INTEGER clock_freq;

static inline uint64_t get_clockfreq(void)
{
	if (!have_clockfreq) {
		QueryPerformanceFrequency(&clock_freq);
		have_clockfreq = true;
	}

	return clock_freq.QuadPart;
}

/**
* @brief 得到系统运行到现在的纳秒数
*/
uint64_t os_gettime_ns(void)
{
	LARGE_INTEGER current_time;
	double time_val;

	QueryPerformanceCounter(&current_time);
	time_val = (double)current_time.QuadPart;
	time_val *= 1000000000.0;
	time_val /= (double)get_clockfreq();

	return (uint64_t)time_val;
}

bool os_sleepto_ns(uint64_t time_target)
{
	uint64_t t = os_gettime_ns();
	uint32_t milliseconds;

	if (t >= time_target)
		return false;

	milliseconds = (uint32_t)((time_target - t) / 1000000);
	if (milliseconds > 1)
		Sleep(milliseconds - 1);

	for (;;) {
		t = os_gettime_ns();
		if (t >= time_target)
			return true;

		Sleep(0);
	}
}

void os_sleep_ms(uint32_t duration)
{
	Sleep(duration);
}



bool FFmedia::Init()
{
	//唯一初始化ffmpeg
	std::once_flag flag;
	std::call_once(flag, [] {
		av_register_all();
		avdevice_register_all();
		avcodec_register_all();
		avformat_network_init();
	});


	AVInputFormat *format = NULL;
	//如果m->format_name有标识输入格式的话（比如"mp4"等），那么就查找这个格式的AVInputFormat
	//AVInputFormat其实就是解码器的函数表
	if (m_format_name && *m_format_name) {
		format = av_find_input_format(m_format_name);
		if (!format)
			BOOST_LOG_TRIVIAL(error) << StringStream::StringPrintf("MP: Unable to find input format for "
				"'%s'", m_path);
	}
	AVDictionary *opts = NULL;
	
	if (m_buffering && !m_is_local_file)//如果是网络文件，设置缓冲流大小
		av_dict_set_int(&opts, "buffer_size", m_buffering, 0);
	av_dict_set(&opts, "stimeout", "10000000", 0);

	m_formatContext = avformat_alloc_context(); //初始化mmfpeg的流媒体句柄，管理整个流
	//auto videoCodecContextGuard = MakeGuard(&m_formatContext, avformat_close_input);	//错误释放保证

	int ret = avformat_open_input(&m_formatContext, m_path, format,
		opts ? &opts : NULL);
	av_dict_free(&opts);
	if (ret < 0) {
		BOOST_LOG_TRIVIAL(error) << StringStream::StringPrintf("MP: Failed to open media: '%s'", m_path);
		return false;
	}

	if (avformat_find_stream_info(m_formatContext, NULL) < 0) {
		BOOST_LOG_TRIVIAL(error) << StringStream::StringPrintf("MP: Failed to find stream info for '%s'",
			m_path);
		return false;
	}

	if (!mp_decode_init(m_formatContext, &m_v, AVMEDIA_TYPE_VIDEO,m_hw)) {
		BOOST_LOG_TRIVIAL(error) << "failed to Get AvDecoder";
		return false;
	}

	if (!mp_decode_init(m_formatContext, &m_a, AVMEDIA_TYPE_AUDIO, m_hw)) {
		BOOST_LOG_TRIVIAL(error) << "failed to Get AdDecoder";
		return false;
	}
    
	if (!m_v.available && !m_a.available) {//如果同时没有音频和视频，那么失败
		BOOST_LOG_TRIVIAL(error) << "failed to Get Decoder";
		return false;
	}
	//videoCodecContextGuard.release();//保证成功不被删
	return true;
}

 
bool FFmedia::reset(int64_t seek)
{
	//设置播放位置
	AVStream *stream = m_formatContext->streams[0];
	int64_t seek_pos;
	int seek_flags;
	
	if (m_formatContext->duration == AV_NOPTS_VALUE) {
		//如果没有持续时间则一般是网络直播视频，起始的start_time都是很大，这边需要重设，如果是网络直播视频统一设置为起点0
		seek_pos = 0;
		seek_flags = AVSEEK_FLAG_FRAME;
	}
	else {
		//有持续时间一般是固定视频，网络的可以是MP4或者本地文件，有起始时间
		seek_pos = (seek == -1)?m_formatContext->start_time: seek;
		seek_flags = AVSEEK_FLAG_BACKWARD;
	}
	/*
	AVSEEK_FLAG_BACKWARD：若你设置seek时间为1秒，但是只有0秒和2秒上才有I帧，则时间从0秒开始。
	AVSEEK_FLAG_ANY：若你设置seek时间为1秒，但是只有0秒和2秒上才有I帧，则时间从2秒开始。
	AVSEEK_FLAG_FRAME：若你设置seek时间为1秒，但是只有0秒和2秒上才有I帧，则时间从2秒开始。
	设置如果是有start_time的则从start_time开始播放
	*/
	int64_t seek_target = seek_flags == AVSEEK_FLAG_BACKWARD
		? av_rescale_q(seek_pos, AVRational{ 1, AV_TIME_BASE }, stream->time_base)
		: seek_pos;


	int ret = av_seek_frame(m_formatContext, 0, seek_target, seek_flags);
	if (ret < 0) {
		char err[AV_ERROR_MAX_STRING_SIZE] = { 0 };
		av_make_error_string(err, AV_ERROR_MAX_STRING_SIZE, ret);
		BOOST_LOG_TRIVIAL(info) << StringStream::StringPrintf("MP: Failed to seek:%s", err);
		return false;
	}

	if (m_v.available)
		mp_decode_flush(&m_v);
	if (m_a.available)
		mp_decode_flush(&m_a);

	return true;

}


/**
* @brief  得到下一个packet块，并且把他压入到对应的解码器的解码队列
* @return
*       -# 小于0  失败
*       -#  大于0的数  成功并且是d->pkt解码的
*/
int FFmedia::prepare_packet()
{
	AVPacket new_pkt;
	AVPacket pkt;
	av_init_packet(&pkt);
	new_pkt = pkt;

	int ret = av_read_frame(m_formatContext, &pkt);
	if (ret < 0) {
		if (ret != AVERROR_EOF) {
			char err[AV_ERROR_MAX_STRING_SIZE] = { 0 };
			av_make_error_string(err, AV_ERROR_MAX_STRING_SIZE, ret);
			BOOST_LOG_TRIVIAL(info) << StringStream::StringPrintf("MP: av_read_frame failed: %s (%d)",
				err, ret);
		}
		return ret;
	}
	//得到pkt音频或者视频的解码器
	struct mp_decode *d = nullptr;
	if (m_v.available && pkt.stream_index == m_v.stream->index)
		d = &m_v;
	else if (m_a.available && pkt.stream_index == m_a.stream->index)
		d = &m_a;;
	if (d && pkt.size) {
		//https://blog.csdn.net/fernandowei/article/details/50492815
		//意思就是把pkt的数据拷贝到new_pkt，然后压入解码的队列
		//如果某个packet有多个ref，那么这个packet的数据其实有两份存储位置，一份就是这个packet自己data指针所指向，另一份是这个packet的其他所有ref
		//而且当有多个ref时，buf->data数据部分是不可写、只能读的；
		//解码队列本来就只去读
		av_packet_ref(&new_pkt, &pkt);
		mp_decode_push_packet(d, &new_pkt);
	}
	//删除引用，就类似于ffmpeg的内部智能计数指针，这边已经把他放到解码队列了，那么就可以把原来的指针计数减1
	//之后再解码中这个块被解码后av_packet_unref(&new_pkt)；之后就再减1为0，ffmpeg就会清除这块的内存了
	av_packet_unref(&pkt);
	return ret;
}


/*
*@brief 保证如果同时有音视频那么就同时加载完毕这两个
*/
inline bool mp_media_ready_to_start(mp_decode* v, mp_decode* a)
{
	if (v->available && !v->frame_ready)
		return false;
	if (a->available && !a->frame_ready)
		return false;
	return true;
}


/**
* @brief 视频输出的初始化，根据我们定义的m->scale_format设定图像格式，然后申请格式的一帧的图像缓存，地址在scale_pic，行数据大小在scale_linesizes
* @note m->v.decoder必须已经初始化过也就是已经调用mp_open _codec (struct mp_decode*d)了
*/
bool FFmedia::initVedioScaling()
{
	if (m_dstFormat == AV_PIX_FMT_NONE) {
		m_dstFormat = m_v.decoder->pix_fmt;
	}

	if (m_dstWidth == 0) {
		m_dstWidth = m_v.decoder->width;
	}

	if (m_dstHeight == 0) {
		m_dstHeight = m_v.decoder->height;
	}

	/*
	srcW：源图像的宽
	srcH：源图像的高
	srcFormat：源图像的像素格式
	dstW：目标图像的宽
	dstH：目标图像的高
	dstFormat：目标图像的像素格式
	flags：设定图像拉伸使用的算法
	这里把要输出的图像格式定义为m->scale_format，图像大小则不变
	*/
	m_avswscale = sws_getCachedContext(NULL,
		m_v.decoder->width, m_v.decoder->height,
		m_v.decoder->pix_fmt,
		m_dstWidth, m_dstHeight,
		m_dstFormat,
		SWS_FAST_BILINEAR, NULL, NULL, NULL);
	if (!m_avswscale) {
		BOOST_LOG_TRIVIAL(error) << "MP: Failed to initialize scaler";
		return false;
	}

	//为从原视频流转化成我们需求的图像格式的数据流分配空间
	//m->scale_pic, m->scale_linesizes分别是我们要求格式的图像数据的指针和每一行的数据大小
	int ret = av_image_alloc(m_scale_pic, m_scale_linesizes,
		m_v.decoder->width, m_v.decoder->height,
		m_dstFormat, 4);
	if (ret < 0) {
		BOOST_LOG_TRIVIAL(error) << "MP: Failed to create scale pic data";
		return false;
	}

	return true;
}


bool FFmedia::initAudioScaling()
{
	if (m_output_layout == 0) {
		m_output_layout = m_a.decoder->channel_layout;
	}
	
	if (m_output_format == AV_SAMPLE_FMT_NONE) {
		m_output_format = m_a.decoder->sample_fmt;
	}

	if (m_output_freq == 0) {
		m_output_freq = m_a.decoder->sample_rate;
	}

	if (m_output_ch == 0) {
		m_output_ch = m_a.decoder->channels;
	}

	//m_bytesample = m_a.decoder->

	m_adswscale = swr_alloc_set_opts(NULL,
		m_output_layout, m_output_format, m_output_freq,
		m_a.decoder->channel_layout, m_a.decoder->sample_fmt, m_a.decoder->sample_rate,
		0, NULL);

	if (!m_adswscale) {
		BOOST_LOG_TRIVIAL(error) << "swr_alloc_set_opts failed";
		return false;
	}

	int errcode = swr_init(m_adswscale);
	if (errcode != 0) {
		BOOST_LOG_TRIVIAL(error) << StringStream::StringPrintf("avresample_open failed: error code %d",errcode);
		return false;
	}

	return true;
}


/* 
* @brief 转化音频格式到m_output_buffer
*/
bool FFmedia::audio_resample()
{
	uint32_t in_frames = m_a.frame->nb_samples;
	//在audio_resampler_create之后已经设置了要转化的格式和转化后的格式
	//因为转化不是那么完美，不可能完美的从一种帧率转成另一种总是有一定的误差
	//swr_get_delay代表将输入格式的数据转成输出的格式的数据和理论上的误差时间，理论上就是把
	//m_a.decoder->sample_rate代表以什么基准，如果设置成rm_a.decoder->sample_rate那么输出就是延迟是多少个输入的采样点
	int64_t delay = swr_get_delay(m_adswscale, m_a.decoder->sample_rate);
	//把理论上的采样点数加上实际增加的采样点数然后转成输出的采样点数
	int estimated = (int)av_rescale_rnd(
		delay + (int64_t)in_frames,
		(int64_t)m_output_freq, (int64_t)m_a.decoder->sample_rate,
		AV_ROUND_UP);

	/* resize the buffer if bigger */
	//如果输出的采样点数不同了则重新分配空间防止溢出
	if (estimated > m_output_size) {
		if (m_output_buffer[0])
			av_freep(&m_output_buffer[0]);

		av_samples_alloc(m_output_buffer, NULL, m_output_ch,
			estimated, m_output_format, 0);

		m_output_size = estimated;
	}

	int ret = swr_convert(m_adswscale,
		(uint8_t**)m_output_buffer, m_output_size,
		(const uint8_t**)m_a.frame->data, in_frames);

	if (ret < 0) {
		BOOST_LOG_TRIVIAL(error) << StringStream::StringPrintf("swr_convert failed: %d", ret);
		return false;
	}

	return true;
}



bool FFmedia::vedio_rescaling()
{
	int ret = sws_scale(m_avswscale,
		m_v.frame->data, (const int *)m_v.frame->linesize,
		0, m_v.frame->height,
		m_scale_pic, (const int *)m_scale_linesizes);
	if (ret <= 0) {
		BOOST_LOG_TRIVIAL(error) << StringStream::StringPrintf("video_scaler_scale: sws_scale failed: %d",
			ret);
		return false;
	}
	return true;
}


inline bool FrameReady(mp_decode* d)
{
	return d->available && d->frame_ready;
}

/*@brief 得到本帧播放最早播放完的流（视频或者音频），以取得下一帧的最小延迟时间 \n
通过本帧的视频流播放时间和音频流播放时间对比，取最小的那个值返回
*/
inline int64_t get_next_min_pts(mp_decode* a, mp_decode* v)
{
	int64_t min_next_ns = 0x7FFFFFFFFFFFFFFFLL;

	if (FrameReady(a)) {
		if (a->frame_pts < min_next_ns)
			min_next_ns = a->frame_pts;
	}
	if (FrameReady(v)) {
		if (v->frame_pts < min_next_ns)
			min_next_ns = v->frame_pts;
	}

	return min_next_ns;
}


void FFmedia::pause()
{
	boost::unique_lock<boost::mutex> locker(m_contrlMutex);
	m_pause = !m_pause;
	m_PausedCV.notify_all();

}
void FFmedia::stop()
{
	boost::unique_lock<boost::mutex> locker(m_contrlMutex);
	m_stop = true;
	m_PausedCV.notify_all();
}


bool FFmedia::runonthread()
{

	//初始句柄解码器
	if (!Init())
		return false;

	//设为seek从头启动
	if (!reset()) 
		return false;

	int64_t play_sys_start_ts = os_gettime_ns();
	int64_t now_pts = 0;
	int64_t old_min_pts = 0;
	bool init = false;

	for (;;)
	{
		if (boost::this_thread::interruption_requested())
		{
			break;
		}

		bool pause, stop;
		int seek_pos = -1;
		{
			boost::unique_lock<boost::mutex> locker(m_contrlMutex);
			pause = m_pause;
			stop = m_stop;
			
			if (m_seek_pos != -1) { 
				seek_pos = m_seek_pos; 
				m_seek_pos = -1; 
			}
		}


		//暂停
		if (pause)
		{
			int64_t pausetime = os_gettime_ns();
			boost::unique_lock<boost::mutex> locker(m_PausedMutex);
			m_PausedCV.wait(locker, [this](){
				boost::unique_lock<boost::mutex> locker(m_contrlMutex);
				return !m_pause &&m_stop;
			});
			pausetime = os_gettime_ns() - pausetime;
			play_sys_start_ts = play_sys_start_ts + pausetime;
		}

		//暂停之下，如果暂停中停止则立刻停止
		if (stop)
			break;

		//seek
		if (seek_pos != -1) {
			if (!reset(seek_pos))
				return false;
		}

		//只要文件没有结束，那么就保证音视频都要加载到
		while (!mp_media_ready_to_start(&m_v, &m_a)&&!eof) {
			int ret = prepare_packet();
			if (ret == AVERROR_EOF)
				eof = true;
			else if (ret < 0)
				return false;

			if (m_v.available && !m_v.frame_ready && !mp_decode_next(&m_v, eof)) {
				BOOST_LOG_TRIVIAL(error) << "decode vedio failed";
				return false;
			}
				
			if (m_a.available && !m_a.frame_ready && !mp_decode_next(&m_a, eof)) {
				BOOST_LOG_TRIVIAL(error) << "decode avdio failed";
				return false;
			}
				
		}
		

		//初始化一次视频转化器
		if (!m_avswscale && FrameReady(&m_v)) {
			if (!initVedioScaling()) {
				return false;
			}
		}

		//初始化一次音频转化器
		if (!m_adswscale && FrameReady(&m_a)) {
			if (!initAudioScaling())
				return false;
		}

		if (!init) {
			play_sys_start_ts = os_gettime_ns();
			init = true;
	    }
		int64_t now_min_pts = get_next_min_pts(&m_a, &m_v);
		int64_t delay = now_min_pts - old_min_pts;
		//如果时间太长或者错误就直接忽略
		if (!((delay < 0) || (delay > 3000000000))) {
			os_sleepto_ns(now_min_pts + play_sys_start_ts);
		}
		old_min_pts = now_min_pts;
		//格式化视频
		if (FrameReady(&m_v) && m_v.frame_pts+ play_sys_start_ts <= os_gettime_ns()) {
			if (!vedio_rescaling())
				return false;
			 //图像更新
			OnVedioUpdate(this);
			m_v.frame_ready = false;
		}

		//格式化音频
		if (FrameReady(&m_a) && m_a.frame_pts + play_sys_start_ts <= os_gettime_ns()) {
			if (!audio_resample())
				return false;
		   //音频更新
			OnAudioUpdate(this);
			m_a.frame_ready = false;
		}

		
		//如果读到最后一个文件并且全部的frame都得到处理了，那么就结束
		if (eof && !FrameReady(&m_v) && !FrameReady(&m_a)) {
			OnEof();
			break;
		}
	}
	//结束清理
	clear();
	return true;
}


void FFmedia::clear()
{
	mp_decode_free(&m_a);
	mp_decode_free(&m_v);

	if(m_formatContext)
	  avformat_close_input(&m_formatContext);

	if (m_avswscale) {
		sws_freeContext(m_avswscale);
		m_avswscale = nullptr;
	}
	if (m_scale_pic[0])
		av_freep(&m_scale_pic[0]);

	if (m_adswscale) {
		swr_free(&m_adswscale);
		m_adswscale = nullptr;
	}
		
	if (m_output_buffer[0])
		av_freep(&m_output_buffer[0]);
}

void FFmedia::start_decode_thread()
{
	if (m_mainThread.get()) {
		m_mainThread->interrupt();
		m_mainThread->join();
		m_mainThread.reset(nullptr);
	}
	m_mainThread.reset(new boost::thread(&FFmedia::runonthread, this));
}

FFmedia::~FFmedia()
{
	if (m_mainThread.get()) {
		m_mainThread->interrupt();
		m_mainThread->join();
		m_mainThread.reset(nullptr);
	}
}