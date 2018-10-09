#pragma once
namespace GLUI
{
#define GLUI_MOD_SHIFT           0x0001
	/*! @brief If this bit is set one or more Control keys were held down.
	*/
#define GLUI_MOD_CONTROL         0x0002
	/*! @brief If this bit is set one or more Alt keys were held down.
	*/
#define GLUI_MOD_ALT             0x0004
	/*! @brief If this bit is set one or more Super keys were held down.
	*/
#define GLUI_MOD_SUPER           0x0008


	/* The unknown key */
#define GLUI_KEY_UNKNOWN            -1

/* Printable keys */
#define GLUI_KEY_SPACE              32
#define GLUI_KEY_APOSTROPHE         39  /* ' */
#define GLUI_KEY_COMMA              44  /* , */
#define GLUI_KEY_MINUS              45  /* - */
#define GLUI_KEY_PERIOD             46  /* . */
#define GLUI_KEY_SLASH              47  /* / */
#define GLUI_KEY_0                  48
#define GLUI_KEY_1                  49
#define GLUI_KEY_2                  50
#define GLUI_KEY_3                  51
#define GLUI_KEY_4                  52
#define GLUI_KEY_5                  53
#define GLUI_KEY_6                  54
#define GLUI_KEY_7                  55
#define GLUI_KEY_8                  56
#define GLUI_KEY_9                  57
#define GLUI_KEY_SEMICOLON          59  /* ; */
#define GLUI_KEY_EQUAL              61  /* = */
#define GLUI_KEY_A                  65
#define GLUI_KEY_B                  66
#define GLUI_KEY_C                  67
#define GLUI_KEY_D                  68
#define GLUI_KEY_E                  69
#define GLUI_KEY_F                  70
#define GLUI_KEY_G                  71
#define GLUI_KEY_H                  72
#define GLUI_KEY_I                  73
#define GLUI_KEY_J                  74
#define GLUI_KEY_K                  75
#define GLUI_KEY_L                  76
#define GLUI_KEY_M                  77
#define GLUI_KEY_N                  78
#define GLUI_KEY_O                  79
#define GLUI_KEY_P                  80
#define GLUI_KEY_Q                  81
#define GLUI_KEY_R                  82
#define GLUI_KEY_S                  83
#define GLUI_KEY_T                  84
#define GLUI_KEY_U                  85
#define GLUI_KEY_V                  86
#define GLUI_KEY_W                  87
#define GLUI_KEY_X                  88
#define GLUI_KEY_Y                  89
#define GLUI_KEY_Z                  90
#define GLUI_KEY_LEFT_BRACKET       91  /* [ */
#define GLUI_KEY_BACKSLASH          92  /* \ */
#define GLUI_KEY_RIGHT_BRACKET      93  /* ] */
#define GLUI_KEY_GRAVE_ACCENT       96  /* ` */
#define GLUI_KEY_WORLD_1            161 /* non-US #1 */
#define GLUI_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define GLUI_KEY_ESCAPE             256
#define GLUI_KEY_ENTER              257
#define GLUI_KEY_TAB                258
#define GLUI_KEY_BACKSPACE          259
#define GLUI_KEY_INSERT             260
#define GLUI_KEY_DELETE             261
#define GLUI_KEY_RIGHT              262
#define GLUI_KEY_LEFT               263
#define GLUI_KEY_DOWN               264
#define GLUI_KEY_UP                 265
#define GLUI_KEY_PAGE_UP            266
#define GLUI_KEY_PAGE_DOWN          267
#define GLUI_KEY_HOME               268
#define GLUI_KEY_END                269
#define GLUI_KEY_CAPS_LOCK          280
#define GLUI_KEY_SCROLL_LOCK        281
#define GLUI_KEY_NUM_LOCK           282
#define GLUI_KEY_PRINT_SCREEN       283
#define GLUI_KEY_PAUSE              284
#define GLUI_KEY_F1                 290
#define GLUI_KEY_F2                 291
#define GLUI_KEY_F3                 292
#define GLUI_KEY_F4                 293
#define GLUI_KEY_F5                 294
#define GLUI_KEY_F6                 295
#define GLUI_KEY_F7                 296
#define GLUI_KEY_F8                 297
#define GLUI_KEY_F9                 298
#define GLUI_KEY_F10                299
#define GLUI_KEY_F11                300
#define GLUI_KEY_F12                301
#define GLUI_KEY_F13                302
#define GLUI_KEY_F14                303
#define GLUI_KEY_F15                304
#define GLUI_KEY_F16                305
#define GLUI_KEY_F17                306
#define GLUI_KEY_F18                307
#define GLUI_KEY_F19                308
#define GLUI_KEY_F20                309
#define GLUI_KEY_F21                310
#define GLUI_KEY_F22                311
#define GLUI_KEY_F23                312
#define GLUI_KEY_F24                313
#define GLUI_KEY_F25                314
#define GLUI_KEY_KP_0               320
#define GLUI_KEY_KP_1               321
#define GLUI_KEY_KP_2               322
#define GLUI_KEY_KP_3               323
#define GLUI_KEY_KP_4               324
#define GLUI_KEY_KP_5               325
#define GLUI_KEY_KP_6               326
#define GLUI_KEY_KP_7               327
#define GLUI_KEY_KP_8               328
#define GLUI_KEY_KP_9               329
#define GLUI_KEY_KP_DECIMAL         330
#define GLUI_KEY_KP_DIVIDE          331
#define GLUI_KEY_KP_MULTIPLY        332
#define GLUI_KEY_KP_SUBTRACT        333
#define GLUI_KEY_KP_ADD             334
#define GLUI_KEY_KP_ENTER           335
#define GLUI_KEY_KP_EQUAL           336
#define GLUI_KEY_LEFT_SHIFT         340
#define GLUI_KEY_LEFT_CONTROL       341
#define GLUI_KEY_LEFT_ALT           342
#define GLUI_KEY_LEFT_SUPER         343
#define GLUI_KEY_RIGHT_SHIFT        344
#define GLUI_KEY_RIGHT_CONTROL      345
#define GLUI_KEY_RIGHT_ALT          346
#define GLUI_KEY_RIGHT_SUPER        347
#define GLUI_KEY_MENU               348

#define GLUI_KEY_LAST               GLUI_KEY_MENU

}