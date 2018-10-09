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
	struct mp_decode m_v;///<��Ƶ������
	struct mp_decode m_a;///<��Ƶ������

public:
	//setting
	char *m_path; //���ŵ�url����·��
	char *m_format_name = nullptr;//���ŵĸ�ʽ����"MP4"�ȣ�ûдnull��Ĭ��ȥ��
	int m_buffering = 0xffff;///<������ǲ��ű����ļ�������������û�������С������0������
	bool m_is_local_file;///<�ÿ��Ƿ��Ǳ����ļ�����Ҳ����m_formatContext�Ƿ��Ǳ����ļ��ĻỰ
	bool m_hw = false;///<�Ƿ�ʹ��Ӳ������

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
	int m_scale_linesizes[4] = {0};///<��Ӧscale_pic������scale_pic[0]��ʾ��һ��ͨ����ͼ��һ�е����ݣ�scale_pic[0][y * scale_linesizes��0��+x]�����0��ͨ����(x,y)������ֵ
							 ///<�������scale_linesizes[0]�Ǹ�������ʾ���ͼ���Ƿ�ת�ģ�Ҳ����scale_pic[0]��Ӧ���½ǵ����أ����һ�����ض�Ӧ���Ͻ�
	uint8_t *m_scale_pic[4] = {0};///<������Ƶת�루���Բ����˵ģ��������ָ�룬4�����ĸ�ͼ��ͨ��������scale_pic[0]��ʾ��һ��ͨ����ͼ������
    //av setting
	AVPixelFormat m_dstFormat = AV_PIX_FMT_NONE;///<Ŀ��ͼ���ʽ
	int m_dstHeight = 0;///<Ŀ��ͼ��߶�
	int m_dstWidth = 0;///<Ŀ��ͼ����


    //ad
	struct SwrContext   *m_adswscale = nullptr;///<��Ƶת�����ĻỰ���
	uint8_t             *m_output_buffer[AV_NUM_DATA_POINTERS] = {0};///<�����Ƶ�����ݸ�ͨ��ָ�룬���������audio_resampler_resample���ú���Ч����ʼΪnull
	int                 m_output_size = 0;///<�����Ƶ�����ݸ��������������audio_resampler_resample���ú���Ч����ʼ��Ϊ0
    //ad setting
	int            m_output_layout = 0;///<�����Ƶ��ͨ��������
	enum AVSampleFormat m_output_format = AV_SAMPLE_FMT_NONE;///<�����Ƶ�Ĳ������ݽṹ
	
	uint32_t            m_output_ch = 0;///<�����Ƶ��ͨ����
	uint32_t            m_output_freq = 0;///<�����Ƶ��ÿ�������
	uint32_t            m_output_planes = 0;


	std::unique_ptr<boost::thread> m_mainThread;
};

