//========================================================================
// GLFW 3.3 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2016-2017 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include "internal.h"

// Internal constants for gamepad mapping source types
#define _GLFW_JOYSTICK_AXIS     1
#define _GLFW_JOYSTICK_BUTTON   2
#define _GLFW_JOYSTICK_HATBIT   3

#define TOUCH_WIDTH             960
#define TOUCH_HEIGHT            544

SceTouchData touch;

SceTouchPanelInfo panelinfo;

float aAWidth;
float aAHeight;
float dispWidth;
float dispHeight;
float forcerange;

static const unsigned int button_map[] = {
    SCE_CTRL_CIRCLE, SCE_CTRL_CROSS, SCE_CTRL_TRIANGLE, SCE_CTRL_SQUARE,
    SCE_CTRL_LTRIGGER, SCE_CTRL_RTRIGGER,
    SCE_CTRL_START, SCE_CTRL_SELECT};

enum  	HidKeyboardScancode {
  KBD_NONE = 0x00,
  KBD_ERR_OVF = 0x01,
  KBD_A = 0x04,
  KBD_B = 0x05,
  KBD_C = 0x06,
  KBD_D = 0x07,
  KBD_E = 0x08,
  KBD_F = 0x09,
  KBD_G = 0x0a,
  KBD_H = 0x0b,
  KBD_I = 0x0c,
  KBD_J = 0x0d,
  KBD_K = 0x0e,
  KBD_L = 0x0f,
  KBD_M = 0x10,
  KBD_N = 0x11,
  KBD_O = 0x12,
  KBD_P = 0x13,
  KBD_Q = 0x14,
  KBD_R = 0x15,
  KBD_S = 0x16,
  KBD_T = 0x17,
  KBD_U = 0x18,
  KBD_V = 0x19,
  KBD_W = 0x1a,
  KBD_X = 0x1b,
  KBD_Y = 0x1c,
  KBD_Z = 0x1d,
  KBD_1 = 0x1e,
  KBD_2 = 0x1f,
  KBD_3 = 0x20,
  KBD_4 = 0x21,
  KBD_5 = 0x22,
  KBD_6 = 0x23,
  KBD_7 = 0x24,
  KBD_8 = 0x25,
  KBD_9 = 0x26,
  KBD_0 = 0x27,
  KBD_ENTER = 0x28,
  KBD_ESC = 0x29,
  KBD_BACKSPACE = 0x2a,
  KBD_TAB = 0x2b,
  KBD_SPACE = 0x2c,
  KBD_MINUS = 0x2d,
  KBD_EQUAL = 0x2e,
  KBD_LEFTBRACE = 0x2f,
  KBD_RIGHTBRACE = 0x30,
  KBD_BACKSLASH = 0x31,
  KBD_HASHTILDE = 0x32,
  KBD_SEMICOLON = 0x33,
  KBD_APOSTROPHE = 0x34,
  KBD_GRAVE = 0x35,
  KBD_COMMA = 0x36,
  KBD_DOT = 0x37,
  KBD_SLASH = 0x38,
  KBD_CAPSLOCK = 0x39,
  KBD_F1 = 0x3a,
  KBD_F2 = 0x3b,
  KBD_F3 = 0x3c,
  KBD_F4 = 0x3d,
  KBD_F5 = 0x3e,
  KBD_F6 = 0x3f,
  KBD_F7 = 0x40,
  KBD_F8 = 0x41,
  KBD_F9 = 0x42,
  KBD_F10 = 0x43,
  KBD_F11 = 0x44,
  KBD_F12 = 0x45,
  KBD_SYSRQ = 0x46,
  KBD_SCROLLLOCK = 0x47,
  KBD_PAUSE = 0x48,
  KBD_INSERT = 0x49,
  KBD_HOME = 0x4a,
  KBD_PAGEUP = 0x4b,
  KBD_DELETE = 0x4c,
  KBD_END = 0x4d,
  KBD_PAGEDOWN = 0x4e,
  KBD_RIGHT = 0x4f,
  KBD_LEFT = 0x50,
  KBD_DOWN = 0x51,
  KBD_UP = 0x52,
  KBD_NUMLOCK = 0x53,
  KBD_KPSLASH = 0x54,
  KBD_KPASTERISK = 0x55,
  KBD_KPMINUS = 0x56,
  KBD_KPPLUS = 0x57,
  KBD_KPENTER = 0x58,
  KBD_KP1 = 0x59,
  KBD_KP2 = 0x5a,
  KBD_KP3 = 0x5b,
  KBD_KP4 = 0x5c,
  KBD_KP5 = 0x5d,
  KBD_KP6 = 0x5e,
  KBD_KP7 = 0x5f,
  KBD_KP8 = 0x60,
  KBD_KP9 = 0x61,
  KBD_KP0 = 0x62,
  KBD_KPDOT = 0x63,
  KBD_102ND = 0x64,
  KBD_COMPOSE = 0x65,
  KBD_POWER = 0x66,
  KBD_KPEQUAL = 0x67,
  KBD_F13 = 0x68,
  KBD_F14 = 0x69,
  KBD_F15 = 0x6a,
  KBD_F16 = 0x6b,
  KBD_F17 = 0x6c,
  KBD_F18 = 0x6d,
  KBD_F19 = 0x6e,
  KBD_F20 = 0x6f,
  KBD_F21 = 0x70,
  KBD_F22 = 0x71,
  KBD_F23 = 0x72,
  KBD_F24 = 0x73,
  KBD_OPEN = 0x74,
  KBD_HELP = 0x75,
  KBD_PROPS = 0x76,
  KBD_FRONT = 0x77,
  KBD_STOP = 0x78,
  KBD_AGAIN = 0x79,
  KBD_UNDO = 0x7a,
  KBD_CUT = 0x7b,
  KBD_COPY = 0x7c,
  KBD_PASTE = 0x7d,
  KBD_FIND = 0x7e,
  KBD_MUTE = 0x7f,
  KBD_VOLUMEUP = 0x80,
  KBD_VOLUMEDOWN = 0x81,
  KBD_CAPSLOCK_ACTIVE = 0x82,
  KBD_NUMLOCK_ACTIVE = 0x83,
  KBD_SCROLLLOCK_ACTIVE = 0x84,
  KBD_KPCOMMA = 0x85,
  KBD_KPLEFTPAREN = 0xb6,
  KBD_KPRIGHTPAREN = 0xb7,
  KBD_LEFTCTRL = 0xe0,
  KBD_LEFTSHIFT = 0xe1,
  KBD_LEFTALT = 0xe2,
  KBD_LEFTMETA = 0xe3,
  KBD_RIGHTCTRL = 0xe4,
  KBD_RIGHTSHIFT = 0xe5,
  KBD_RIGHTALT = 0xe6,
  KBD_RIGHTMETA = 0xe7,
  KBD_MEDIA_PLAYPAUSE = 0xe8,
  KBD_MEDIA_STOPCD = 0xe9,
  KBD_MEDIA_PREVIOUSSONG = 0xea,
  KBD_MEDIA_NEXTSONG = 0xeb,
  KBD_MEDIA_EJECTCD = 0xec,
  KBD_MEDIA_VOLUMEUP = 0xed,
  KBD_MEDIA_VOLUMEDOWN = 0xee,
  KBD_MEDIA_MUTE = 0xef,
  KBD_MEDIA_WWW = 0xf0,
  KBD_MEDIA_BACK = 0xf1,
  KBD_MEDIA_FORWARD = 0xf2,
  KBD_MEDIA_STOP = 0xf3,
  KBD_MEDIA_FIND = 0xf4,
  KBD_MEDIA_SCROLLUP = 0xf5,
  KBD_MEDIA_SCROLLDOWN = 0xf6,
  KBD_MEDIA_EDIT = 0xf7,
  KBD_MEDIA_SLEEP = 0xf8,
  KBD_MEDIA_COFFEE = 0xf9,
  KBD_MEDIA_REFRESH = 0xfa,
  KBD_MEDIA_CALC = 0xfb
};

enum
{
    _VITA_AXIS_LEFT_X,
    _VITA_AXIS_LEFT_Y,
    _VITA_AXIS_RIGHT_X,
    _VITA_AXIS_RIGHT_Y,

    _VITA_AXIS_COUNT
};

enum
{
    _VITA_BUTTON_CIRCLE,
    _VITA_BUTTON_CROSS,
    _VITA_BUTTON_TRIANGLE,
    _VITA_BUTTON_SQUARE,
    _VITA_BUTTON_L,
    _VITA_BUTTON_R,
    _VITA_BUTTON_START,
    _VITA_BUTTON_SELECT,

    _VITA_BUTTON_COUNT
};

enum
{
    _VITA_HAT_DPAD,

    _VITA_HAT_COUNT
};

static _GLFWmapping s_vitaMapping =
{
    .name = "Playstation Vita Controller",
    .guid = "",
    .buttons =
    {
        [GLFW_GAMEPAD_BUTTON_A]            = { .type = _GLFW_JOYSTICK_BUTTON, .index = _VITA_BUTTON_CROSS },
        [GLFW_GAMEPAD_BUTTON_B]            = { .type = _GLFW_JOYSTICK_BUTTON, .index = _VITA_BUTTON_CIRCLE },
        [GLFW_GAMEPAD_BUTTON_X]            = { .type = _GLFW_JOYSTICK_BUTTON, .index = _VITA_BUTTON_SQUARE },
        [GLFW_GAMEPAD_BUTTON_Y]            = { .type = _GLFW_JOYSTICK_BUTTON, .index = _VITA_BUTTON_TRIANGLE },
        [GLFW_GAMEPAD_BUTTON_BACK]         = { .type = _GLFW_JOYSTICK_BUTTON, .index = _VITA_BUTTON_SELECT },
        [GLFW_GAMEPAD_BUTTON_START]        = { .type = _GLFW_JOYSTICK_BUTTON, .index = _VITA_BUTTON_START },
        //[GLFW_GAMEPAD_BUTTON_GUIDE]        = { .type = _GLFW_JOYSTICK_BUTTON, .index = 0 },
        [GLFW_GAMEPAD_BUTTON_LEFT_BUMPER]  = { .type = _GLFW_JOYSTICK_BUTTON, .index = _VITA_BUTTON_L },
        [GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] = { .type = _GLFW_JOYSTICK_BUTTON, .index = _VITA_BUTTON_R },
        [GLFW_GAMEPAD_BUTTON_DPAD_UP]      = { .type = _GLFW_JOYSTICK_HATBIT, .index = (_VITA_HAT_DPAD<<4) | 0x1 },
        [GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]   = { .type = _GLFW_JOYSTICK_HATBIT, .index = (_VITA_HAT_DPAD<<4) | 0x2 },
        [GLFW_GAMEPAD_BUTTON_DPAD_DOWN]    = { .type = _GLFW_JOYSTICK_HATBIT, .index = (_VITA_HAT_DPAD<<4) | 0x4 },
        [GLFW_GAMEPAD_BUTTON_DPAD_LEFT]    = { .type = _GLFW_JOYSTICK_HATBIT, .index = (_VITA_HAT_DPAD<<4) | 0x8 },
    },
    .axes =
    {
        [GLFW_GAMEPAD_AXIS_LEFT_X]         = { .type = _GLFW_JOYSTICK_AXIS,   .index = _VITA_AXIS_LEFT_X,  .axisScale = 1, .axisOffset = 0 },
        [GLFW_GAMEPAD_AXIS_LEFT_Y]         = { .type = _GLFW_JOYSTICK_AXIS,   .index = _VITA_AXIS_LEFT_Y,  .axisScale = 1, .axisOffset = 0 },
        [GLFW_GAMEPAD_AXIS_RIGHT_X]        = { .type = _GLFW_JOYSTICK_AXIS,   .index = _VITA_AXIS_RIGHT_X, .axisScale = 1, .axisOffset = 0 },
        [GLFW_GAMEPAD_AXIS_RIGHT_Y]        = { .type = _GLFW_JOYSTICK_AXIS,   .index = _VITA_AXIS_RIGHT_Y, .axisScale = 1, .axisOffset = 0 },
    },
};

static void _initVitaTouch(void)
{
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
	sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);

	SceTouchPanelInfo panelinfo;
	sceTouchGetPanelInfo(SCE_TOUCH_PORT_FRONT, &panelinfo);
	dispWidth = (float)(panelinfo.maxDispX - panelinfo.minDispX);
	dispHeight = (float)(panelinfo.maxDispY - panelinfo.minDispY);
	forcerange = (float)(panelinfo.maxForce - panelinfo.minForce);
}

static void _convertTouchXYToGLFWXY(float *glfw_x, float *glfw_y, int vita_x, int vita_y) {
	float x = 0;
	float y = 0;
	x = (vita_x - panelinfo.minDispX) / dispWidth;
	y = (vita_y - panelinfo.minDispY) / dispHeight;
	if (x < 0.0) {
		x = 0.0;
	} else if (x > 1.0) {
		x = 1.0;
	}
	if (y < 0.0) {
		y = 0.0;
	} else if (y > 1.0) {
		y = 1.0;
	}
	*glfw_x = x;
	*glfw_y = y;
}

void _glfwInitVitaJoysticks(void)
{
    _GLFWjoystick* js = _glfwAllocJoystick(s_vitaMapping.name, s_vitaMapping.guid,
        _VITA_AXIS_COUNT, _VITA_BUTTON_COUNT, _VITA_HAT_COUNT);

    js->mapping = &s_vitaMapping;
    js->vita.id = 0;
    _initVitaTouch();
}

void _glfwUpdateVitaJoysticks(void)
{
    static unsigned int old_buttons;
    SceUInt64 down, up;

    // Read input state
    SceCtrlData *pad = NULL;
    sceCtrlPeekBufferPositive(0, pad, 1);

    down = pad->buttons;
    up   = old_buttons ^ pad->buttons;

#define MAP_KEY(_vita_key, _glfw_key, _scancode) \
    do { \
        if (down & (_vita_key)) _glfwInputKey(_glfw.vita.cur_window, (_glfw_key), (_scancode), GLFW_PRESS, 0); \
        else if (up & (_vita_key)) { if (down & ~(_vita_key)) _glfwInputKey(_glfw.vita.cur_window, (_glfw_key), (_scancode), GLFW_RELEASE, 0);} \
    } while (0)

    // Map common keyboard keys to the controller
    // TODO: Only do this mapping if a keyboard isn't connected
    MAP_KEY(SCE_CTRL_UP, GLFW_KEY_UP, KBD_UP);
    MAP_KEY(SCE_CTRL_DOWN, GLFW_KEY_DOWN, KBD_DOWN);
    MAP_KEY(SCE_CTRL_LEFT, GLFW_KEY_LEFT, KBD_LEFT);
    MAP_KEY(SCE_CTRL_RIGHT, GLFW_KEY_RIGHT, KBD_RIGHT);
    MAP_KEY(SCE_CTRL_CROSS, GLFW_KEY_X, KBD_X);
    MAP_KEY(SCE_CTRL_CIRCLE, GLFW_KEY_Z, KBD_Z);
    MAP_KEY(SCE_CTRL_SQUARE, GLFW_KEY_S, KBD_S);
    MAP_KEY(SCE_CTRL_TRIANGLE, GLFW_KEY_A, KBD_A);
    MAP_KEY(SCE_CTRL_START, GLFW_KEY_ENTER, KBD_ENTER);
    MAP_KEY(SCE_CTRL_SELECT, GLFW_KEY_ESCAPE, KBD_ESC);

    // Report touch inputs as mouse clicks
    if (sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1), touch.reportNum > 0)
    {
        float x, y;
        _convertTouchXYToGLFWXY(&x, &y, touch.report[0].x, touch.report[0].y);
        float scaledXPos = (float)x / TOUCH_WIDTH * _glfw.vita.cur_window->vita.width;
        float scaledYPos = (float)y / TOUCH_HEIGHT * _glfw.vita.cur_window->vita.height;

        _glfwInputCursorPos(_glfw.vita.cur_window, scaledXPos, scaledYPos);

        if (_glfw.vita.cur_window->mouseButtons[GLFW_MOUSE_BUTTON_LEFT] == GLFW_RELEASE)
            _glfwInputMouseClick(_glfw.vita.cur_window, GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);

    } else if (_glfw.vita.cur_window->mouseButtons[GLFW_MOUSE_BUTTON_LEFT] == GLFW_PRESS)
        _glfwInputMouseClick(_glfw.vita.cur_window, GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE, 0);
}

//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

int _glfwPlatformPollJoystick(_GLFWjoystick* js, int mode)
{
    // Detect presence - we assume ID is always 0
    if (js->vita.id != 0)
        return GLFW_FALSE;

    SceCtrlData *pad = NULL;
    sceCtrlPeekBufferPositive(js->vita.id, pad, 1);

    if (mode & _GLFW_POLL_AXES)
    {
        unsigned char lx, ly, rx, ry;
        lx = pad->lx;
        ly = pad->ly;
        rx = pad->rx;
        ry = pad->ry;
        _glfwInputJoystickAxis(js, _VITA_AXIS_LEFT_X, lx / 32768.0f);
        _glfwInputJoystickAxis(js, _VITA_AXIS_LEFT_Y, ly / 32768.0f);
        _glfwInputJoystickAxis(js, _VITA_AXIS_RIGHT_X, rx / 32768.0f);
        _glfwInputJoystickAxis(js, _VITA_AXIS_RIGHT_Y, ry / 32768.0f);
    }

    if (mode & _GLFW_POLL_BUTTONS)
    {
        int i;
        for (i = 0; i < _VITA_BUTTON_COUNT; i++)
            _glfwInputJoystickButton(js, i, (pad->buttons & button_map[i]) ? GLFW_PRESS : GLFW_RELEASE);
        // Extract hat values
        char hatbits = (pad->buttons >> 4) & 0xF;
        _glfwInputJoystickHat(js, i, hatbits);
    }

    return GLFW_TRUE;
}


void _glfwPlatformUpdateGamepadGUID(char* guid)
{
}

