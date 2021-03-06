#pragma once

//////// キー入力ビット割り当て ////////
#define KEY_UP			0x000000000000001
#define KEY_DOWN		0x000000000000002
#define KEY_RIGHT		0x000000000000004
#define KEY_LEFT		0x000000000000008
#define KEY_START		0x000000000000010
#define KEY_ENTER		0x000000000000020
#define KEY_A			0x000000000000040
#define KEY_B			0x000000000000080
#define KEY_C			0x000000000000100
#define KEY_D			0x000000000000200
#define KEY_E			0x000000000000400
#define KEY_F			0x000000000000800
#define KEY_G			0x000000000001000
#define KEY_H			0x000000000002000
#define KEY_I			0x000000000004000
#define KEY_J			0x000000000008000
#define KEY_K			0x000000000010000
#define KEY_L			0x000000000020000
#define KEY_M			0x000000000040000
#define KEY_N			0x000000000080000
#define KEY_O			0x000000000100000
#define KEY_P			0x000000000200000
#define KEY_Q			0x000000000400000
#define KEY_R			0x000000000800000
#define KEY_S			0x000000001000000
#define KEY_T			0x000000002000000
#define KEY_U			0x000000004000000
#define KEY_V			0x000000008000000
#define KEY_W			0x000000010000000
#define KEY_X			0x000000020000000
#define KEY_Y			0x000000040000000
#define KEY_Z			0x000000080000000

#define KEY_1			0x000000100000000
#define KEY_2			0x000000200000000
#define KEY_3			0x000000400000000
#define KEY_4			0x000000800000000
#define KEY_5			0x000001000000000
#define KEY_6			0x000002000000000
#define KEY_7			0x000004000000000
#define KEY_8			0x000008000000000
#define KEY_9			0x000010000000000
#define KEY_0			0x000020000000000

#define KEY_LBUTTON     0x000100000000000
#define KEY_RBUTTON     0x000200000000000
#define KEY_MBUTTON     0x000400000000000

#define KEY_ALT         0x001000000000000

class KeyInput
{
private:
	static __int64 state;
	static __int64 trigger;
	static __int64 release;

public:
	static void update();
	static __int64 key_state() { return state; }
	static __int64 key_trigger() { return trigger; }
	static __int64 key_release() { return release; }
};