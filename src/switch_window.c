//========================================================================
// GLFW 3.3 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2016 Google Inc.
// Copyright (c) 2016-2019 Camilla Löwy <elmindreda@glfw.org>
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


#define FB_WIDTH  1920
#define FB_HEIGHT 1080


static int resizeNativeWindow(_GLFWwindow* window, int width, int height)
{
    if (width > FB_WIDTH)
    {
        _glfwInputError(GLFW_INVALID_VALUE, "Invalid width (%d > %d)", width, FB_WIDTH);
        return GLFW_FALSE;
    }
    if (height > FB_HEIGHT)
    {
        _glfwInputError(GLFW_INVALID_VALUE, "Invalid height (%d > %d)", height, FB_HEIGHT);
        return GLFW_FALSE;
    }

    nwindowSetCrop(window->nx.nwin, 0, FB_HEIGHT-height, width, FB_HEIGHT);
    window->nx.width = width;
    window->nx.height = height;

    _glfwInputFramebufferSize(window, width, height);
    _glfwInputWindowSize(window, width, height);
    _glfwInputWindowDamage(window);

    return GLFW_TRUE;
}

static int createNativeWindow(_GLFWwindow* window,
                              const _GLFWwndconfig* wndconfig)
{
    window->nx.nwin = nwindowGetDefault();
    nwindowSetDimensions(window->nx.nwin, FB_WIDTH, FB_HEIGHT);
    return resizeNativeWindow(window, wndconfig->width, wndconfig->height);
}

__attribute__ ((weak))
int _glfwPlatformCreateContext(_GLFWwindow* window,
                                      const _GLFWctxconfig* ctxconfig,
                                      const _GLFWfbconfig* fbconfig)
{
    _glfwInputError(GLFW_API_UNAVAILABLE, "Switch: EGL library not loaded");
    return GLFW_FALSE;
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

int _glfwPlatformCreateWindow(_GLFWwindow* window,
                              const _GLFWwndconfig* wndconfig,
                              const _GLFWctxconfig* ctxconfig,
                              const _GLFWfbconfig* fbconfig)
{
    if (_glfw.nx.cur_window)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR, "Switch: Cannot create more than one window");
        return GLFW_FALSE;
    }

    if (!createNativeWindow(window, wndconfig))
        return GLFW_FALSE;

    if (ctxconfig->client != GLFW_NO_API)
    {
        if (ctxconfig->source == GLFW_NATIVE_CONTEXT_API ||
            ctxconfig->source == GLFW_EGL_CONTEXT_API)
        {
            if (!_glfwPlatformCreateContext(window, ctxconfig, fbconfig))
                return GLFW_FALSE;
        }
        else
        {
            _glfwInputError(GLFW_API_UNAVAILABLE, "Switch: OSMesa not available");
            return GLFW_FALSE;
        }
    }

    _glfw.nx.cur_window = window;
    return GLFW_TRUE;
}

void _glfwPlatformDestroyWindow(_GLFWwindow* window)
{
    if (window->context.destroy)
        window->context.destroy(window);
    _glfw.nx.cur_window = NULL;
}

void _glfwPlatformSetWindowTitle(_GLFWwindow* window, const char* title)
{
}

void _glfwPlatformSetWindowIcon(_GLFWwindow* window, int count,
                                const GLFWimage* images)
{
}

void _glfwPlatformSetWindowMonitor(_GLFWwindow* window,
                                   _GLFWmonitor* monitor,
                                   int xpos, int ypos,
                                   int width, int height,
                                   int refreshRate)
{
}

void _glfwPlatformGetWindowPos(_GLFWwindow* window, int* xpos, int* ypos)
{
}

void _glfwPlatformSetWindowPos(_GLFWwindow* window, int xpos, int ypos)
{
}

void _glfwPlatformGetWindowSize(_GLFWwindow* window, int* width, int* height)
{
    if (width)
        *width = window->nx.width;
    if (height)
        *height = window->nx.height;
}

void _glfwPlatformSetWindowSize(_GLFWwindow* window, int width, int height)
{
    if (window->resizable)
        resizeNativeWindow(window, width, height);
}

void _glfwPlatformSetWindowSizeLimits(_GLFWwindow* window,
                                      int minwidth, int minheight,
                                      int maxwidth, int maxheight)
{
}

void _glfwPlatformSetWindowAspectRatio(_GLFWwindow* window, int n, int d)
{
}

void _glfwPlatformGetFramebufferSize(_GLFWwindow* window, int* width, int* height)
{
    // Applications rely on framebuffer size == window size; so let's do it that way.
    _glfwPlatformGetWindowSize(window, width, height);
}

void _glfwPlatformGetWindowFrameSize(_GLFWwindow* window,
                                     int* left, int* top,
                                     int* right, int* bottom)
{
    _glfwPlatformGetWindowSize(window, right, bottom);
}

void _glfwPlatformGetWindowContentScale(_GLFWwindow* window,
                                        float* xscale, float* yscale)
{
    if (xscale)
        *xscale = 1.f;
    if (yscale)
        *yscale = 1.f;
}

void _glfwPlatformIconifyWindow(_GLFWwindow* window)
{
}

void _glfwPlatformRestoreWindow(_GLFWwindow* window)
{
}

void _glfwPlatformMaximizeWindow(_GLFWwindow* window)
{
}

int _glfwPlatformWindowMaximized(_GLFWwindow* window)
{
    return GLFW_FALSE; // "Full screen windows cannot be maximized"
}

int _glfwPlatformWindowHovered(_GLFWwindow* window)
{
    return _glfw.nx.is_focused;
}

int _glfwPlatformFramebufferTransparent(_GLFWwindow* window)
{
    return GLFW_FALSE;
}

void _glfwPlatformSetWindowResizable(_GLFWwindow* window, GLFWbool enabled)
{
}

void _glfwPlatformSetWindowDecorated(_GLFWwindow* window, GLFWbool enabled)
{
}

void _glfwPlatformSetWindowFloating(_GLFWwindow* window, GLFWbool enabled)
{
}

float _glfwPlatformGetWindowOpacity(_GLFWwindow* window)
{
    return 1.f;
}

void _glfwPlatformSetWindowOpacity(_GLFWwindow* window, float opacity)
{
}

void _glfwPlatformSetRawMouseMotion(_GLFWwindow *window, GLFWbool enabled)
{
}

GLFWbool _glfwPlatformRawMouseMotionSupported(void)
{
    return GLFW_FALSE;
}

void _glfwPlatformShowWindow(_GLFWwindow* window)
{
}

void _glfwPlatformRequestWindowAttention(_GLFWwindow* window)
{
}

void _glfwPlatformUnhideWindow(_GLFWwindow* window)
{
}

void _glfwPlatformHideWindow(_GLFWwindow* window)
{
}

void _glfwPlatformFocusWindow(_GLFWwindow* window)
{
}

int _glfwPlatformWindowFocused(_GLFWwindow* window)
{
    return GLFW_TRUE; // We always have input focus
}

int _glfwPlatformWindowIconified(_GLFWwindow* window)
{
    return !_glfw.nx.is_focused;
}

int _glfwPlatformWindowVisible(_GLFWwindow* window)
{
    return _glfw.nx.is_focused;
}

void _glfwPlatformPollEvents(void)
{
    u32 msg;
    Result res;

    if (!_glfw.nx.cur_window)
        return;

    int events;
    do
    {
        res = appletGetMessage(&msg);
        if (R_SUCCEEDED(res))
        {
            bool should_close = !appletProcessMessage(msg);
            if (should_close)
            {
                _glfwInputWindowCloseRequest(_glfw.nx.cur_window);
                return;
            }
        }

        events = _glfw.nx.event_mask;
        _glfw.nx.event_mask = 0;
        if (events & _GLFW_SWITCH_EVENT_FOCUS_CHANGED)
        {
            _glfwInputWindowIconify(_glfw.nx.cur_window, !_glfw.nx.is_focused);
            if (_glfw.nx.is_focused)
                appletSetFocusHandlingMode(AppletFocusHandlingMode_NoSuspend);
            else
                appletSetFocusHandlingMode(AppletFocusHandlingMode_SuspendHomeSleepNotify);
        }
        if (events & _GLFW_SWITCH_EVENT_SCREEN_SIZE_CHANGED)
            _glfwPlatformSetWindowSize(_glfw.nx.cur_window, _glfw.nx.scr_width, _glfw.nx.scr_height);
    } while (events);

    // Update joysticks
    _glfwUpdateSwitchJoysticks();
}

void _glfwPlatformWaitEvents(void)
{
    _glfwPlatformPollEvents();
}

void _glfwPlatformWaitEventsTimeout(double timeout)
{
    _glfwPlatformPollEvents();
}

void _glfwPlatformPostEmptyEvent(void)
{
}

void _glfwPlatformGetCursorPos(_GLFWwindow* window, double* xpos, double* ypos)
{
}

void _glfwPlatformSetCursorPos(_GLFWwindow* window, double x, double y)
{
}

void _glfwPlatformSetCursorMode(_GLFWwindow* window, int mode)
{
}

int _glfwPlatformCreateCursor(_GLFWcursor* cursor,
                              const GLFWimage* image,
                              int xhot, int yhot)
{
    return GLFW_TRUE;
}

int _glfwPlatformCreateStandardCursor(_GLFWcursor* cursor, int shape)
{
    return GLFW_TRUE;
}

void _glfwPlatformDestroyCursor(_GLFWcursor* cursor)
{
}

void _glfwPlatformSetCursor(_GLFWwindow* window, _GLFWcursor* cursor)
{
}

void _glfwPlatformSetClipboardString(const char* string)
{
}

const char* _glfwPlatformGetClipboardString(void)
{
    return NULL;
}

const char* _glfwPlatformGetScancodeName(int scancode)
{
    return "";
}

int _glfwPlatformGetKeyScancode(int key)
{
    return -1;
}

void _glfwPlatformGetRequiredInstanceExtensions(char** extensions)
{
    extensions[0] = "VK_KHR_surface";
    extensions[1] = "VK_NN_vi_surface";
}

int _glfwPlatformGetPhysicalDevicePresentationSupport(VkInstance instance,
                                                      VkPhysicalDevice device,
                                                      uint32_t queuefamily)
{
    return GLFW_FALSE;
}

VkResult _glfwPlatformCreateWindowSurface(VkInstance instance,
                                          _GLFWwindow* window,
                                          const VkAllocationCallbacks* allocator,
                                          VkSurfaceKHR* surface)
{
    // TODO: Actually use VK_NN_vi_surface to create a VkSurfaceKHR from a NWindow
    // (wishful thinking though - I have no idea when will Vulkan be supported)
    return VK_ERROR_INITIALIZATION_FAILED;
}

