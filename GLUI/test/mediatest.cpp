#include "test.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../media/Decoder.h"
#include "../media/AudioPlayerAl.h"
#include "../tool/tasks.h"
#include "../UI/ButtonViews.h"
#include "../UI/glfwindows.h"
#include "../UI/Glwindows.h"
#include "../tool/path.h"
#include "../UI/TextView.h"
#include "../UI/layoutview.h"

//light test
namespace mediatest
{

	std::unique_ptr<boost::thread> m_audioThread;
	GLUI::TaskList tasklist;
	GLUI::imageview* m_view;
	FFmedia m_FFmedia;
	AudioPlayerAl m_AudioPlayerAl;
	std::vector<uint8_t> audiodata;
	boost::mutex m_dataMutex;
	boost::mutex m_condition_dataMutex;
	boost::condition_variable m_condition_writedata;
	GLUI::GLwindow windows;

	class mediahelper : public GLUI::ReceiveObject
	{
	public:
		void audiothread()
		{

			for (;;)
			{
				boost::unique_lock<boost::mutex> locker(m_condition_dataMutex);
				m_condition_writedata.wait(locker);

				FFmedia* media = &m_FFmedia;
				if (!m_AudioPlayerAl.isOpen() && !m_AudioPlayerAl.Open(av_get_bytes_per_sample(media->m_output_format), media->m_output_freq, media->m_output_ch)) {
					BOOST_LOG_TRIVIAL(error) << "failed to open openal";
					return;
				}
				{
					boost::unique_lock<boost::mutex> locker(m_dataMutex);
					m_AudioPlayerAl.WriteAudio(&audiodata[0], audiodata.size());
				}
			}

		}
		void OnVedioUpdate(FFmedia* media)
		{
			tasklist.PostTask(std::bind([](FFmedia* media)
			{
				if (!m_view->tex().get() ||
					!m_view->tex()->is_available()|| 
					m_view->tex()->Width != media->m_dstWidth||
					m_view->tex()->Height != media->m_dstHeight)
				{
					m_view->tex().reset(new GLUI::Texture2D);
					m_view->tex()->Width = media->m_dstWidth;
					m_view->tex()->Height = media->m_dstHeight;
					m_view->tex()->setInternalFormat(GL_RGB);
					m_view->tex()->bind_data(NULL);
					glfwSetWindowSize(windows.windows(), media->m_dstWidth, media->m_dstHeight);
					m_view->set_rect(0, 0, media->m_dstWidth, media->m_dstHeight);
				}

				if (!m_view->tex()->is_available())
				{
					BOOST_LOG_TRIVIAL(error) << "texture error";
					return;
				}
				m_view->tex()->Copy(0,(char*) media->m_scale_pic[0], 0, 0, media->m_dstWidth, media->m_dstHeight);
				m_view->repaint();
                
			}, media));
		}
		void OnAudioUpdate(FFmedia* media)
		{
			const int original_buffer_size = av_samples_get_buffer_size(
				nullptr, media->m_output_ch,
				media->m_output_size, media->m_output_format, 1);

			//这里是设成单通道，多通道要一个左边一个右边作为一组排列
			if (media->m_output_buffer[0])
			{
				boost::unique_lock<boost::mutex> locker(m_dataMutex);
				if (audiodata.size() < original_buffer_size) {
					audiodata.resize(original_buffer_size);
				}
				memcpy(&audiodata[0], media->m_output_buffer[0], original_buffer_size);
				m_condition_writedata.notify_all();
				
			}
		}
		void OnEof()
		{
			 
		}

 

	};

	mediahelper helper;
	
	int main()
	{

		int screenWidth = 1024;
		int screenHeight = 1024;
		//InitGL();
		glfwInit();

		
		windows.CreateWindows(screenWidth, screenHeight, "hello");
		glewInit();
		m_view = new GLUI::imageview;
		m_view->set_rect(0, 0, screenWidth, screenHeight);
		windows.rootview()->insert_view(m_view);

		m_FFmedia.m_path = "test.mp4";//"rtmp://live.hkstv.hk.lxdns.com/live/hks";
		m_FFmedia.m_is_local_file = true;
		m_FFmedia.m_dstFormat = AV_PIX_FMT_RGB24;
		m_FFmedia.m_output_format = AV_SAMPLE_FMT_S16;
		m_FFmedia.start_decode_thread();
		m_audioThread.reset(new boost::thread(&mediahelper::audiothread, &helper));
		m_FFmedia.Connectsafe(&FFmedia::OnVedioUpdate, &helper, &mediahelper::OnVedioUpdate);
		m_FFmedia.Connectsafe(&FFmedia::OnAudioUpdate, &helper, &mediahelper::OnAudioUpdate);
		//m_FFmedia.Connect()

		while (!glfwWindowShouldClose(windows.windows())) {
			//glfwWaitEvents();
			Sleep(20);
			glfwPollEvents();
			tasklist.Run();

			if (GLFW_PRESS == glfwGetKey(windows.windows(), GLFW_KEY_ESCAPE)) {
				glfwSetWindowShouldClose(windows.windows(), 1);
			}

		}
		glfwTerminate();
		return 0;
	}
}