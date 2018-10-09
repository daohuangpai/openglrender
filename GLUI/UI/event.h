#pragma once

namespace GLUI
{



	class event {
	public:
		enum {
			LEFT_BUTTON = 0x0004,
			MIDDLE_BUTTON= 0x0008,
			RIGHT_BUTTON= 0x0010
		};
		typedef int mouse_event;
		enum {
			CLICK = 0x0001,
			RELEASE= 0x0002,
			//DOUBLECLICK
		};
		typedef int mouse_statue;
		typedef int key_statue;


		enum {
			LB_CLICK = 0x0005,
			LB_RELEASE = 0x0006,
			MB_CLICK = 0x0009,
			MB_RELEASE = 0x000a,
			RB_CLICK = 0x0011,
			RB_RELEASE = 0x0012
		};
		 
		

		 enum EventType{
			 MOUSE = 0,
		 };
		 virtual EventType type() = 0;
	};


	class MouseEvent : public event{
	public:
		virtual EventType type() override { return MOUSE; }
	private:
		int x, y;


	};
















}