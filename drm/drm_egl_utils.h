/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 Linaro Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Author: Zoltan Kuscsik <zoltan.kuscsik@linaro.org>
 */

#ifndef WESTEROS_DRM_EGL_UTILS_H_
#define WESTEROS_DRM_EGL_UTILS_H_
#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gbm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>


#ifndef DEFAULT_DRM_CARD
#define DEFAULT_DRM_CARD "/dev/dri/card0"
#endif

#define EGL_UTILS_DEBUG 1

#define EGL_UTILS_FAILURE -1
#define EGL_UTILS_SUCCESS 0

static PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC g_create_platform_window_surface_extension = NULL;
static PFNEGLGETPLATFORMDISPLAYEXTPROC egl_helper_get_platform_display = NULL;

static const EGLint egl_utils_opaque_attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RED_SIZE, 1,
    EGL_GREEN_SIZE, 1,
    EGL_BLUE_SIZE, 1,
    EGL_ALPHA_SIZE, 0,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_NONE
};

typedef enum {
    EGL_OPAQUE_ATTRIBS,
    EGL_TRANSPARENT_ATTRIBS
} configTypes;

const EGLint* egl_helper_get_config_type(configTypes type);

#ifdef EGL_UTILS_DEBUG
void DEBUG_PRINT(const char *, ...);
#else
static inline void DEBUG_PRINT(const char *fmt, ...) {};
#endif

/**
* Print details of available EGL configs;
*/
void egl_helper_print_eglConfigs(EGLDisplay dpy,
        const EGLConfig* configs, const EGLint matched);


 /*
 If the platform provisuales the eglCreatePlatformWindowSurfaceEXT
 * use it.
 */

void egl_helper_setup_egl_client_extensions();

/**
 * Initialize DRM display and EGL
 * Returns EGL_UTILS_SUCCESS on success and EGL_UTILS_FAILURE
 * on failure.
 * Input variables:
 *
 *  @attrs - EGL attributes
 *  @fd    - file descriptor pointing to DRM device. If NULL,
 *           default DRM device is opened.
 *
 * Output variables:
 *
 *  @con     - DRM connector
 *  @gbm     - gbm device
 *  @display - EGL display
 *  @config  - EGL configuration
 */

int egl_helper_init_drm_display(
        const EGLint attrs[],
        int *fd,
        drmModeConnector** con,
        struct gbm_device** gbm,
        EGLDisplay* display,
        EGLConfig* config);

#endif //WESTEROS_DRM_EGL