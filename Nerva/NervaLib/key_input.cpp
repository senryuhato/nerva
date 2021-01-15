#include	<Windows.h>
#include	"key_input.h"

__int64 KeyInput::state = 0;
__int64 KeyInput::trigger = 0;
__int64 KeyInput::release = 0;


void KeyInput::update()
{
	__int64		old_state;

	old_state = state;	// 前回のkeyStateを退避させる
	state = 0;				// 現在の状態を取得するまえにゼロクリアする

	if (GetKeyState(VK_UP) < 0) state |= KEY_UP;		// ↑をKEY_UPに割り当て
	if (GetKeyState(VK_DOWN) < 0) state |= KEY_DOWN;		// ↓をKEY_DOWN
	if (GetKeyState(VK_RIGHT) < 0) state |= KEY_RIGHT;		// →をKEY_RIGHT
	if (GetKeyState(VK_LEFT) < 0) state |= KEY_LEFT;		// ←をKEY_LEFT
	if (GetKeyState(VK_SPACE) < 0) state |= KEY_START;		// スペースをKEY_START
	if (GetKeyState(VK_RETURN) < 0) state |= KEY_ENTER;	// ENTERをKEY_ENTER

	if (GetKeyState('A') < 0) state |= KEY_A;
	if (GetKeyState('Z') < 0) state |= KEY_Z;
	if (GetKeyState('X') < 0) state |= KEY_X;
	if (GetKeyState('C') < 0) state |= KEY_C;
	if (GetKeyState('D') < 0) state |= KEY_D;
	if (GetKeyState('E') < 0) state |= KEY_E;
	if (GetKeyState('F') < 0) state |= KEY_F;
	if (GetKeyState('G') < 0) state |= KEY_G;
	if (GetKeyState('H') < 0) state |= KEY_H;
	if (GetKeyState('I') < 0) state |= KEY_I;
	if (GetKeyState('J') < 0) state |= KEY_J;
	if (GetKeyState('K') < 0) state |= KEY_K;
	if (GetKeyState('L') < 0) state |= KEY_L;
	if (GetKeyState('M') < 0) state |= KEY_M;
	if (GetKeyState('N') < 0) state |= KEY_N;
	if (GetKeyState('O') < 0) state |= KEY_O;
	if (GetKeyState('P') < 0) state |= KEY_P;
	if (GetKeyState('Q') < 0) state |= KEY_Q;
	if (GetKeyState('R') < 0) state |= KEY_R;
	if (GetKeyState('S') < 0) state |= KEY_S;
	if (GetKeyState('T') < 0) state |= KEY_T;
	if (GetKeyState('U') < 0) state |= KEY_U;
	if (GetKeyState('V') < 0) state |= KEY_V;
	if (GetKeyState('W') < 0) state |= KEY_W;
	if (GetKeyState('X') < 0) state |= KEY_X;
	if (GetKeyState('Y') < 0) state |= KEY_Y;
	if (GetKeyState('Z') < 0) state |= KEY_Z;

	if (GetKeyState('1') < 0) state |= KEY_1;
	if (GetKeyState('2') < 0) state |= KEY_2;
	if (GetKeyState('3') < 0) state |= KEY_3;
	if (GetKeyState('4') < 0) state |= KEY_4;
	if (GetKeyState('5') < 0) state |= KEY_5;
	if (GetKeyState('6') < 0) state |= KEY_6;
	if (GetKeyState('7') < 0) state |= KEY_7;
	if (GetKeyState('8') < 0) state |= KEY_8;
	if (GetKeyState('9') < 0) state |= KEY_9;
	if (GetKeyState('0') < 0) state |= KEY_0;

	if (GetKeyState(VK_LBUTTON) < 0) state |= KEY_LBUTTON;
	if (GetKeyState(VK_RBUTTON) < 0) state |= KEY_RBUTTON;
	if (GetKeyState(VK_MBUTTON) < 0) state |= KEY_MBUTTON;

	if (GetKeyState(VK_MENU) < 0) state |= KEY_ALT;


	trigger = (~old_state) & state;							// トリガー入力の計算
	__int64 now_state = state;
	release = old_state & (~now_state);						// リリースの計算
}