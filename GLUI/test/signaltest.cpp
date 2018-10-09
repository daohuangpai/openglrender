#include "test.h"
#include "../common/Signal.h"

namespace signaltest
{
	class test1 :public GLUI::SendObject {
	public:
		void Send(int a, int b)
		{
			Run(&test1::Send, std::move(a), std::move(b));
		}
	private:
	};

	class test2 :public GLUI::ReceiveObject {
	public:
		void recvie(int a, int b)
		{
			printf("a:%d,b:%d\r\n", a, b);
		}
	};


	int main()
	{
		test1 testq1;
		test2 testq2;
		test2 testq3;


		testq1.Connectsafe(&test1::Send, &testq2, &test2::recvie);
		testq1.Connectsafe(&test1::Send, &testq3, &test2::recvie);
		{
			test2 testq4;
			testq1.Connectsafe(&test1::Send, &testq4, &test2::recvie);
			printf("before delete testq4....................\r\n");
			testq1.Send(100, 200);
		}
		printf("after delete testq4....................\r\n");
		testq1.Send(100, 200);
		Sleep(10000);

		return 0;
	}
}