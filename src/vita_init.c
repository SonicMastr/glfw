//========================================================================
// GLFW 3.3 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2016 Google Inc.
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

static void _glfwVitaRefreshFocusState(void)
{
    _glfw.vita.event_mask |= _GLFW_VITA_EVENT_FOCUS_CHANGED;
    _glfw.vita.is_focused = 1;
}

static void _glfwVitaRefreshScreenSize(void)
{
    _glfw.vita.event_mask |= _GLFW_VITA_EVENT_SCREEN_SIZE_CHANGED;
    _glfw.vita.scr_width = 960;
    _glfw.vita.scr_height = 544;
}

__attribute__ ((weak))
void _glfwPlatformTerminateContextApi(void)
{
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

int _glfwPlatformInit(void)
{
    int ret;
    if(ret = pibInit(PIB_SHACCCG), ret)
    {
        printf("PIB ERROR %d\n", ret);
        return GLFW_FALSE;
    }
    _glfwVitaRefreshFocusState();
    _glfwVitaRefreshScreenSize();

    _glfwInputMonitor(_glfwAllocMonitor("Default", 960, 544), GLFW_CONNECTED, _GLFW_INSERT_FIRST);
    _glfwInitVitaJoysticks();
    return GLFW_TRUE;
}

void _glfwPlatformTerminate(void)
{
    _glfwPlatformTerminateContextApi();
}

const char* _glfwPlatformGetVersionString(void)
{
    return _GLFW_VERSION_NUMBER " PSVita Pigs in a Blanket GLES 2.0 Driver by CBPS";
}

