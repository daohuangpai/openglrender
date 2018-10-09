#include "test.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../UI/ButtonViews.h"
#include "../UI/glfwindows.h"
#include "../UI/Glwindows.h"
#include "../tool/path.h"
#include "../UI/TextView.h"

namespace uitest
{

	class printer : public GLUI::ReceiveObject {
	public:
		void printfbutton(GLUI::buttonimageview* view)
		{
			printf("press button:%d\r\n", view);
		}
	};

	


	int main() {
		printer pr;

		int screenWidth = 1000;
		int screenHeight = 1000;
		//InitGL();
		glfwInit();

		GLUI::GLwindow windows;
		windows.CreateWindows(screenWidth, screenHeight, "hello");
		glewInit();
		GLUI::imageview* m_view = new GLUI::imageview;
		m_view->setimage((PATH::imagepath()+"2007_001698.jpg").data());
		m_view->set_rect(0, 0, screenWidth, screenHeight);

		
		
		int beginx = 10;
		int width = 200;
		int height = 200;
		for (int i = 0; i < 3; i++)
		{
			
			GLUI::buttonimageview* m_sonview1 = new GLUI::buttonimageview;
			m_sonview1->setimage(GLUI::buttonimageview::BUTTON_NOMAL, (PATH::imagepath() + "2007_001733.jpg").data());
			m_sonview1->setimage(GLUI::buttonimageview::BUTTON_HOVEL, (PATH::imagepath() + "2007_001761.jpg").data());
			m_sonview1->setimage(GLUI::buttonimageview::BUTTON_HOT, (PATH::imagepath() + "2007_001763.jpg").data());
			m_sonview1->set_rect(beginx, 10, width, height);
			beginx += width;
			m_view->insert_view(m_sonview1);
			m_sonview1->Connectsafe(&GLUI::buttonimageview::click, &pr, &printer::printfbutton);
		}

		/*GLUI::textview* texview = new GLUI::textview;
		texview->init("simkai.ttf");
		texview->setfontsize(32);
		texview->set_rect(beginx, 0, screenWidth - beginx, height);
		texview->settext(std::wstring(L"awjdhkajhdkljad1213546879的积分灰色空间单峰函数绝地反击是都awd否计算都会覅u网易或暗示集454分成计算都会夫斯基发你的"));
		m_view->insert_view(texview);*/
	
		windows.rootview()->insert_view(m_view);
		while (!glfwWindowShouldClose(windows.windows())) {
			glfwWaitEvents();
			glfwPollEvents();
			if (GLFW_PRESS == glfwGetKey(windows.windows(), GLFW_KEY_ESCAPE)) {
				glfwSetWindowShouldClose(windows.windows(), 1);
			}

		}
		glfwTerminate();
		return 0;
	}
}