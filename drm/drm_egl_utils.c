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

#include "drm_egl_utils.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>

void DEBUG_PRINT(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, fmt, ap);
  va_end(ap);
}

static int select_egl_config(EGLDisplay dpy, const EGLint* attribs, EGLConfig* config) {

  EGLint matched;

	if (!eglChooseConfig(dpy, attribs, config, 1, &matched) || matched < 1 ){
        fprintf(stderr, "** Failed to get working egl config. eglError = %x **\n", eglGetError());
        return -1;
      }

  return 0;
}

const EGLint* egl_helper_get_config_type(configTypes type) {
    switch(type) {
        case(EGL_OPAQUE_ATTRIBS):
            return egl_utils_opaque_attribs;
        break;
        default:
            return egl_utils_opaque_attribs;
    }
}

void egl_helper_print_eglConfigs(EGLDisplay dpy,
        const EGLConfig *configs, const EGLint matched)
{
    int i;
    EGLint id, size, level;
    EGLint red, green, blue, alpha;
    EGLint depth, stencil;
    EGLint surfaces;
    EGLint visualId, samples, sampleBuffers;
    char surfString[256] = "";

    printf("         Total number of configs = %d\n", matched);
    printf("                 colorbuffer\n");
    printf("  id sz  l   r  g  b  a depth stencil ns b samples samplbuf [visual id] surfaces \n");
    printf("-----------------------------------------------0---------------------------------\n");

    for (i = 0; i < matched; i++)
    {

        eglGetConfigAttrib(dpy, configs[i], EGL_CONFIG_ID, &id);
        eglGetConfigAttrib(dpy, configs[i], EGL_BUFFER_SIZE, &size);
        eglGetConfigAttrib(dpy, configs[i], EGL_LEVEL, &level);

        eglGetConfigAttrib(dpy, configs[i], EGL_RED_SIZE, &red);
        eglGetConfigAttrib(dpy, configs[i], EGL_GREEN_SIZE, &green);
        eglGetConfigAttrib(dpy, configs[i], EGL_BLUE_SIZE, &blue);
        eglGetConfigAttrib(dpy, configs[i], EGL_ALPHA_SIZE, &alpha);
        eglGetConfigAttrib(dpy, configs[i], EGL_DEPTH_SIZE, &depth);
        eglGetConfigAttrib(dpy, configs[i], EGL_STENCIL_SIZE, &stencil);
        eglGetConfigAttrib(dpy, configs[i], EGL_NATIVE_VISUAL_ID, &visualId);
        eglGetConfigAttrib(dpy, configs[i], EGL_SURFACE_TYPE, &surfaces);
        eglGetConfigAttrib(dpy, configs[i], EGL_SAMPLES, &samples);
        eglGetConfigAttrib(dpy, configs[i], EGL_SAMPLE_BUFFERS, &sampleBuffers);

        if (surfaces & EGL_WINDOW_BIT)
            strcat(surfString, "win,");
        if (surfaces & EGL_PBUFFER_BIT)
            strcat(surfString, "pbf,");
        if (surfaces & EGL_PIXMAP_BIT)
            strcat(surfString, "pix,");

        if (strlen(surfString) > 0)
            surfString[strlen(surfString) - 1] = 0;

        printf("0x%02x %2d %2d %2d %2d %2d %2d %2d %2d %2d%2d      0x%02x   %-12s\n",
               id, size, level,
               red, green, blue, alpha,
               depth, stencil,
               samples, sampleBuffers, visualId, surfString);
    }
}

void egl_helper_setup_egl_client_extensions()
{
    const char *extensions;

    extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);

    if (!extensions)
    {
        fprintf(stderr, "Unable to get EGL client extension string.\n");
        return;
    }

    if (strstr(extensions, "EGL_EXT_platform_base"))
        g_create_platform_window_surface_extension =
            (void *)eglGetProcAddress("eglCreatePlatformWindowSurfaceEXT");
    else
        g_create_platform_window_surface_extension = NULL;
}

int egl_helper_init_drm_display(
        const EGLint attrs[],
        int *fd,
        drmModeConnector **con,
        struct gbm_device** gbm,
        EGLDisplay* display,
        EGLConfig* config)
{
    int i, res_fd;
    EGLint minor, major;
    drmModeConnector *connector;
    struct gbm_device* gbmRes;
    drmModeRes *res;

    if(config == NULL) {
        DEBUG_PRINT("Config must be not NULL\n");
        return EGL_UTILS_FAILURE;
    }

    if(*fd == 0) {
        printf("Opening default DRM card  %s\n", DEFAULT_DRM_CARD);
        res_fd = open(DEFAULT_DRM_CARD, O_RDWR);
    }
    else {
        printf("Using DRM card %x\n", *fd);
        res_fd = *fd;
    }

    if (res_fd < 0){
        printf("ERROR: Failed to open DRM device.\n");
        return EGL_UTILS_FAILURE;
    }

    res = drmModeGetResources(res_fd);

    if (!res) {
        DEBUG_PRINT("ERROR: Failed get DRM mode resources.\n");
        return EGL_UTILS_FAILURE;
    }
    DEBUG_PRINT("Number of connectors %d \n", res->count_connectors);

    for (int j = 0; j < res->count_connectors; j++)
    {
        DEBUG_PRINT("Testing connector %d\n", j);
        connector = drmModeGetConnector(res_fd, res->connectors[j]);

        if (!connector)
            continue;

        if (connector->connection == DRM_MODE_CONNECTED && connector->count_modes > 0) {
            printf("Found a working connector. Number of modes %d\n", connector->count_modes);
            break;
        }

        drmModeFreeConnector(connector);
    }

    if (connector != NULL)
    {
        *con = connector;
        DEBUG_PRINT("Using connector id: %d. Number of modes: %s", connector->connector_id, connector->count_modes);
    }
    else
    {
        DEBUG_PRINT("ERORR: No valid connector found\n");
        return EGL_UTILS_FAILURE;
    }

    gbmRes = gbm_create_device(res_fd);

    if (!gbmRes)
    {
        DEBUG_PRINT("ERROR: Failed to create gbm device.");
        return EGL_UTILS_FAILURE;
    } else {
        *gbm = gbmRes;
    }

    /* Egl platform init */

    if (!egl_helper_get_platform_display)
    {
        egl_helper_get_platform_display = (void *)eglGetProcAddress(
            "eglGetPlatformDisplayEXT");
    }

    if (egl_helper_get_platform_display != NULL)
    {
        DEBUG_PRINT("Getting platform display\n");
        display = egl_helper_get_platform_display(EGL_PLATFORM_GBM_KHR,
                                           gbmRes,
                                           NULL);
    }
    else
    {
        display = eglGetDisplay(gbmRes);
    }
    if(display == EGL_NO_DISPLAY) {
        DEBUG_PRINT("ERROR: Failed to get EGL display\n");
        return EGL_UTILS_FAILURE;
    }

    if (!eglInitialize(display, &major, &minor))
    {
        DEBUG_PRINT("Failed to initialize display\n");
        return EGL_UTILS_FAILURE;
    }

    fprintf(stderr, "Getting EGL config. display = %d \n", display);
    select_egl_config(display, attrs, config);

    /* Set output vars */
    *fd = res_fd;
    return EGL_UTILS_SUCCESS;
}
