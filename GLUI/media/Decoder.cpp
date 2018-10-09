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
* @brief �õ�ϵͳ���е����ڵ�������
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
	//Ψһ��ʼ��ffmpeg
	std::once_flag flag;
	std::call_once(flag, [] {
		av_register_all();
		avdevice_register_all();
		avcodec_register_all();
		avformat_network_init();
	});


	AVInputFormat *format = NULL;
	//���m->format_name�б�ʶ�����ʽ�Ļ�������"mp4"�ȣ�����ô�Ͳ��������ʽ��AVInputFormat
	//AVInputFormat��ʵ���ǽ������ĺ�����
	if (m_format_name && *m_format_name) {
		format = av_find_input_format(m_format_name);
		if (!format)
			BOOST_LOG_TRIVIAL(error) << StringStream::StringPrintf("MP: Unable to find input format for "
				"'%s'", m_path);
	}
	AVDictionary *opts = NULL;
	
	if (m_buffering && !m_is_local_file)//����������ļ������û�������С
		av_dict_set_int(&opts, "buffer_size", m_buffering, 0);
	av_dict_set(&opts, "stimeout", "10000000", 0);

	m_formatContext = avformat_alloc_context(); //��ʼ��mmfpeg����ý����������������
	//auto videoCodecContextGuard = MakeGuard(&m_formatContext, avformat_close_input);	//�����ͷű�֤

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
    
	if (!m_v.available && !m_a.available) {//���ͬʱû����Ƶ����Ƶ����ôʧ��
		BOOST_LOG_TRIVIAL(error) << "failed to Get Decoder";
		return false;
	}
	//videoCodecContextGuard.release();//��֤�ɹ�����ɾ
	return true;
}

 
bool FFmedia::reset(int64_t seek)
{
	//���ò���λ��
	AVStream *stream = m_formatContext->streams[0];
	int64_t seek_pos;
	int seek_flags;
	
	if (m_formatContext->duration == AV_NOPTS_VALUE) {
		//���û�г���ʱ����һ��������ֱ����Ƶ����ʼ��start_time���Ǻܴ������Ҫ���裬���������ֱ����Ƶͳһ����Ϊ���0
		seek_pos = 0;
		seek_flags = AVSEEK_FLAG_FRAME;
	}
	else {
		//�г���ʱ��һ���ǹ̶���Ƶ������Ŀ�����MP4���߱����ļ�������ʼʱ��
		seek_pos = (seek == -1)?m_formatContext->start_time: seek;
		seek_flags = AVSEEK_FLAG_BACKWARD;
	}
	/*
	AVSEEK_FLAG_BACKWARD����������seekʱ��Ϊ1�룬����ֻ��0���2���ϲ���I֡����ʱ���0�뿪ʼ��
	AVSEEK_FLAG_ANY����������seekʱ��Ϊ1�룬����ֻ��0���2���ϲ���I֡����ʱ���2�뿪ʼ��
	AVSEEK_FLAG_FRAME����������seekʱ��Ϊ1�룬����ֻ��0���2���ϲ���I֡����ʱ���2�뿪ʼ��
	�����������start_time�����start_time��ʼ����
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
* @brief  �õ���һ��packet�飬���Ұ���ѹ�뵽��Ӧ�Ľ������Ľ������
* @return
*       -# С��0  ʧ��
*       -#  ����0����  �ɹ�������d->pkt�����
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
	//�õ�pkt��Ƶ������Ƶ�Ľ�����
	struct mp_decode *d = nullptr;
	if (m_v.available && pkt.stream_index == m_v.stream->index)
		d = &m_v;
	else if (m_a.available && pkt.stream_index == m_a.stream->index)
		d = &m_a;;
	if (d && pkt.size) {
		//https://blog.csdn.net/fernandowei/article/details/50492815
		//��˼���ǰ�pkt�����ݿ�����new_pkt��Ȼ��ѹ�����Ķ���
		//���ĳ��packet�ж��ref����ô���packet��������ʵ�����ݴ洢λ�ã�һ�ݾ������packet�Լ�dataָ����ָ����һ�������packet����������ref
		//���ҵ��ж��refʱ��buf->data���ݲ����ǲ���д��ֻ�ܶ��ģ�
		//������б�����ֻȥ��
		av_packet_ref(&new_pkt, &pkt);
		mp_decode_push_packet(d, &new_pkt);
	}
	//ɾ�����ã���������ffmpeg���ڲ����ܼ���ָ�룬����Ѿ������ŵ���������ˣ���ô�Ϳ��԰�ԭ����ָ�������1
	//֮���ٽ���������鱻�����av_packet_unref(&new_pkt)��֮����ټ�1Ϊ0��ffmpeg�ͻ���������ڴ���
	av_packet_unref(&pkt);
	return ret;
}


/*
*@brief ��֤���ͬʱ������Ƶ��ô��ͬʱ�������������
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
* @brief ��Ƶ����ĳ�ʼ�����������Ƕ����m->scale_format�趨ͼ���ʽ��Ȼ�������ʽ��һ֡��ͼ�񻺴棬��ַ��scale_pic�������ݴ�С��scale_linesizes
* @note m->v.decoder�����Ѿ���ʼ����Ҳ�����Ѿ�����mp_open _codec (struct mp_decode*d)��
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
	srcW��Դͼ��Ŀ�
	srcH��Դͼ��ĸ�
	srcFormat��Դͼ������ظ�ʽ
	dstW��Ŀ��ͼ��Ŀ�
	dstH��Ŀ��ͼ��ĸ�
	dstFormat��Ŀ��ͼ������ظ�ʽ
	flags���趨ͼ������ʹ�õ��㷨
	�����Ҫ�����ͼ���ʽ����Ϊm->scale_format��ͼ���С�򲻱�
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

	//Ϊ��ԭ��Ƶ��ת�������������ͼ���ʽ������������ռ�
	//m->scale_pic, m->scale_linesizes�ֱ�������Ҫ���ʽ��ͼ�����ݵ�ָ���ÿһ�е����ݴ�С
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
* @brief ת����Ƶ��ʽ��m_output_buffer
*/
bool FFmedia::audio_resample()
{
	uint32_t in_frames = m_a.frame->nb_samples;
	//��audio_resampler_create֮���Ѿ�������Ҫת���ĸ�ʽ��ת����ĸ�ʽ
	//��Ϊת��������ô�����������������Ĵ�һ��֡��ת����һ��������һ�������
	//swr_get_delay���������ʽ������ת������ĸ�ʽ�����ݺ������ϵ����ʱ�䣬�����Ͼ��ǰ�
	//m_a.decoder->sample_rate������ʲô��׼��������ó�rm_a.decoder->sample_rate��ô��������ӳ��Ƕ��ٸ�����Ĳ�����
	int64_t delay = swr_get_delay(m_adswscale, m_a.decoder->sample_rate);
	//�������ϵĲ�����������ʵ�����ӵĲ�������Ȼ��ת������Ĳ�������
	int estimated = (int)av_rescale_rnd(
		delay + (int64_t)in_frames,
		(int64_t)m_output_freq, (int64_t)m_a.decoder->sample_rate,
		AV_ROUND_UP);

	/* resize the buffer if bigger */
	//�������Ĳ���������ͬ�������·���ռ��ֹ���
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

/*@brief �õ���֡�������粥�����������Ƶ������Ƶ������ȡ����һ֡����С�ӳ�ʱ�� \n
ͨ����֡����Ƶ������ʱ�����Ƶ������ʱ��Աȣ�ȡ��С���Ǹ�ֵ����
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

	//��ʼ���������
	if (!Init())
		return false;

	//��Ϊseek��ͷ����
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


		//��ͣ
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

		//��֮ͣ�£������ͣ��ֹͣ������ֹͣ
		if (stop)
			break;

		//seek
		if (seek_pos != -1) {
			if (!reset(seek_pos))
				return false;
		}

		//ֻҪ�ļ�û�н�������ô�ͱ�֤����Ƶ��Ҫ���ص�
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
		

		//��ʼ��һ����Ƶת����
		if (!m_avswscale && FrameReady(&m_v)) {
			if (!initVedioScaling()) {
				return false;
			}
		}

		//��ʼ��һ����Ƶת����
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
		//���ʱ��̫�����ߴ����ֱ�Ӻ���
		if (!((delay < 0) || (delay > 3000000000))) {
			os_sleepto_ns(now_min_pts + play_sys_start_ts);
		}
		old_min_pts = now_min_pts;
		//��ʽ����Ƶ
		if (FrameReady(&m_v) && m_v.frame_pts+ play_sys_start_ts <= os_gettime_ns()) {
			if (!vedio_rescaling())
				return false;
			 //ͼ�����
			OnVedioUpdate(this);
			m_v.frame_ready = false;
		}

		//��ʽ����Ƶ
		if (FrameReady(&m_a) && m_a.frame_pts + play_sys_start_ts <= os_gettime_ns()) {
			if (!audio_resample())
				return false;
		   //��Ƶ����
			OnAudioUpdate(this);
			m_a.frame_ready = false;
		}

		
		//����������һ���ļ�����ȫ����frame���õ������ˣ���ô�ͽ���
		if (eof && !FrameReady(&m_v) && !FrameReady(&m_a)) {
			OnEof();
			break;
		}
	}
	//��������
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