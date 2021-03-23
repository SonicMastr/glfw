//========================================================================
// GLFW 3.3 POSIX - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2017 Camilla Löwy <elmindreda@glfw.org>
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
// It is fine to use C99 in this file because it will not be built with VS
//========================================================================

#include "internal.h"

#include <assert.h>
#include <string.h>
#include <kernel.h>

typedef struct
{
  int count;
  pthread_mutex_t mutex;
  void (*dtor)(void*);  
} pthread_key;

#define PTHREAD_MUTEX_INITIALIZER_ { STATIC_INIT_ID_, (pthread_t)MUTEX_SIG_, 0, 0, (char)PTHREAD_MUTEX_NORMAL,     0, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} }

static pthread_key key_table[PTHREAD_KEYS_MAX];
static pthread_mutex_t key_mutex;
static pthread_storage_t UserMainThreadStorage;
static pthread_t UserMainThread = NULL;

void storeThread()
{
    UserMainThread = &UserMainThreadStorage;
    UserMainThread->id = sceKernelGetThreadId();
	key_mutex.id = sceKernelCreateMutex("GLFW TLS", SCE_KERNEL_MUTEX_ATTR_TH_FIFO, 1, NULL);
	printf("Stored Thread\n");
}

pthread_t getSelf(void)
{
  	SceUID id;

  	id = sceKernelGetThreadId();
  	if (id <= 0)
    	return NULL;
  	if (UserMainThread != NULL && UserMainThread->id == id)
    	return UserMainThread;

    return NULL;
}

//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

GLFWbool _glfwPlatformCreateTls(_GLFWtls* tls)
{
    assert(tls->posix.allocated == GLFW_FALSE);

    int kix;
    int res;
    sceKernelLockMutex(key_mutex.id, 1, NULL);
	printf("Locked TLS Mutex\n");
    for (kix = 0; kix < PTHREAD_KEYS_MAX; kix++) 
        if (key_table[kix].count == 0) 
        {
            key_table[kix].count++;
            key_table[kix].dtor = NULL;
            key_table[kix].mutex.id = sceKernelCreateMutex("GLFW KEY", SCE_KERNEL_MUTEX_ATTR_TH_FIFO, 1, NULL);
            tls->posix.key.key = kix;
            res = 0;
            tls->posix.allocated = GLFW_TRUE;
            sceKernelUnlockMutex(key_mutex.id, 1);
			printf("Unlocked TLS Mutex\n");
            return GLFW_TRUE;
        }
    sceKernelUnlockMutex(key_mutex.id, 1);
	printf("Unlocked TLS Mutex But Error\n");
    _glfwInputError(GLFW_PLATFORM_ERROR,
                        "POSIX: Failed to create context TLS");
    return GLFW_FALSE;
}

void _glfwPlatformDestroyTls(_GLFWtls* tls)
{
    if (tls->posix.allocated) {
        int k = tls->posix.key.key;
        int c;

        if (k < 0 || k >= PTHREAD_KEYS_MAX) 
            goto einval;

        sceKernelLockMutex(key_table[k].mutex.id, 1, NULL);
        if ((c = key_table[k].count) == 0)
            goto exit;
        
        if (c == 1) {
            // The key is there but there is no thread specific 
            // data associated to it.  We can safely delete
            key_table[k].dtor = NULL;
            key_table[k].count = 0;
            goto exit;
        }
exit:
        sceKernelDeleteMutex(key_table[k].mutex.id);
    }
einval:
    memset(tls, 0, sizeof(_GLFWtls));
}

void* _glfwPlatformGetTls(_GLFWtls* tls)
{
    assert(tls->posix.allocated == GLFW_TRUE);
	void *ret;
	pthread_t me = getSelf();
	int k = tls->posix.key.key;

	if (me->specific_data == NULL ||
		k < 0 || k >= PTHREAD_KEYS_MAX)
		return NULL;

	if (key_table[k].count) {
		ret = (void *)me->specific_data[k];
	} 
	else {
		ret = NULL;
	}

	return(ret);
}

void _glfwPlatformSetTls(_GLFWtls* tls, void* value)
{
    assert(tls->posix.allocated == GLFW_TRUE);
	pthread_t me = getSelf();
	printf("Got Self: 0x%08X\n", me);
	int k = tls->posix.key.key;

	if (k < 0 || k >= PTHREAD_KEYS_MAX)
		return EINVAL;

	sceKernelLockMutex(key_table[k].mutex.id, 1, NULL);
	printf("Locked Set TLS Mutex\n");
	if (key_table[k].count == 0)
		goto exit;

	if (me->specific_data[k] == NULL) 
		{
		if (value != NULL) 
			{
			me->specific_data_count++;
			key_table[k].count++;
			}
		} 
	else 
		{
		if (value == NULL) 
			{
			me->specific_data_count--;
			key_table[k].count--;
			}
		}
	me->specific_data[k] = value;
exit:
  	sceKernelUnlockMutex(key_table[k].mutex.id, 1);
	printf("Unlocked Set TLS Mutex\n");
}

GLFWbool _glfwPlatformCreateMutex(_GLFWmutex* mutex)
{
    assert(mutex->posix.allocated == GLFW_FALSE);

    mutex->posix.handle.id = sceKernelCreateMutex("GLFW Mutex", SCE_KERNEL_MUTEX_ATTR_TH_FIFO, 1, NULL);
    if (mutex->posix.handle.id <= 0) {
        _glfwInputError(GLFW_PLATFORM_ERROR, "POSIX: Failed to create mutex");
        return GLFW_FALSE;
    }
    else {
        return mutex->posix.allocated = GLFW_TRUE;
    }

}

void _glfwPlatformDestroyMutex(_GLFWmutex* mutex)
{
    if (mutex->posix.allocated) {
        sceKernelDeleteMutex(mutex->posix.handle.id);

    }
    memset(mutex, 0, sizeof(_GLFWmutex));
}

void _glfwPlatformLockMutex(_GLFWmutex* mutex)
{
    assert(mutex->posix.allocated == GLFW_TRUE);
    sceKernelLockMutex(mutex->posix.handle.id, 1, NULL);
}

void _glfwPlatformUnlockMutex(_GLFWmutex* mutex)
{
    assert(mutex->posix.allocated == GLFW_TRUE);
    sceKernelUnlockMutex(mutex->posix.handle.id, 1);
}

