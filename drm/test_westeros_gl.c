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

#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gbm.h>
#include "westeros-gl.h"
#include "drm_egl_utils.h"

#define RESULT_TEST_PASS 0
#define RESULT_TEST_FAIL 1

#define XRES 1280
#define YRES 720

static EGLContext context;
static EGLSurface egl_surface;


int TEST_eglGetDisplay_Overload()
{
    printf("TEST_eglGetDisplay_Overload ... \n");
    EGLConfig config;

    EGLint num_config;

    EGLint redSize, greenSize, blueSize, alphaSize, depthSize;
    EGLint count = 0;
    EGLint ctxAttrib[3];

    ctxAttrib[0] = EGL_CONTEXT_CLIENT_VERSION;
    ctxAttrib[1] = 2; // ES2
    ctxAttrib[2] = EGL_NONE;

    WstGLCtx *ctx = WstGLInit();

    if(ctx == NULL) {
        DEBUG_PRINT("failed to init GBM\n");
        return RESULT_TEST_FAIL;
    }

    egl_helper_setup_egl_client_extensions();
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (dpy == EGL_NO_DISPLAY)
        return RESULT_TEST_FAIL;

    if (!eglInitialize(dpy, NULL, NULL))
    {
        printf("Failed to initialize display\n");
        return RESULT_TEST_FAIL;
    }

    EGLint num_configs;

    if (!eglGetConfigs(dpy, NULL, 0, &num_configs))
    {
        return RESULT_TEST_FAIL;
    }

    EGLConfig *configs = (EGLConfig *)malloc(num_configs * sizeof(EGLConfig));
    EGLint matched;

    eglChooseConfig(dpy, egl_helper_get_config_type(EGL_OPAQUE_ATTRIBS),
                    configs, num_configs, &matched);

    for (int i = 0; i < num_configs; ++i)
    {
        EGLint gbm_format;

        if (!eglGetConfigAttrib(dpy, configs[i],
                                EGL_NATIVE_VISUAL_ID, &gbm_format))
        {
            continue;
        }

        if (gbm_format == GBM_FORMAT_XRGB8888)
        {
            printf("Using config %d\n", i);
            config = configs[i];
            break;
        }
    }
    /* Create EGL Context */
    if (!eglBindAPI(EGL_OPENGL_ES_API))
    {
        printf("failed to bind EGL_OPENGL_ES_API\n");
        return -1;
    }

    context = eglCreateContext(dpy, config, EGL_NO_CONTEXT, ctxAttrib);

    if (context == EGL_NO_CONTEXT)
    {
        printf("Failed to get EGL context. Error = %x\n", eglGetError());
        return -1;
    }
    /* Create GBM surface */


    void *gbm_surface = WstGLCreateNativeWindow(ctx, 0, 0, XRES, YRES);

    printf("GBM surface is %p\n", gbm_surface);

    if (gbm_surface == NULL)
    {
        printf("Failed to get gbm_surface. Error: %x\n", eglGetError());
        return -1;
    }

    egl_surface = eglCreateWindowSurface(dpy, config, (void*) gbm_surface, NULL);

    if (egl_surface == EGL_NO_SURFACE) {
        printf("Failed to get surface. Error: %x \n", eglGetError());
        return -1;
    }
    EGLint ret = eglMakeCurrent(dpy, egl_surface, egl_surface, context);
    if (ret == EGL_FALSE)
        printf("Failed to get context. Current error %x\n", eglGetError());

    return 0;
}

int TEST_WstGLInit()
{
    printf("TEST_WstGLInit\n");
    WstGLCtx *ctx = WstGLInit();


    return RESULT_TEST_FAIL;

    WstGLTerm(ctx);
    return RESULT_TEST_PASS;
}

int TEST_WstGLTerm()
{
    printf("Test: WstGLTerm\n");
    int result = 0;
    return result;
}

int TEST_WstGLCreateNativeWindow()
{
    printf("TEST_WstGLCreateNativeWindow\n");
    int result = 0;
    void *nw;
    WstGLCtx *ctx = WstGLInit();
    printf("Run\n");
    nw = WstGLCreateNativeWindow(ctx, 0, 0, XRES, YRES);
    printf("Done\n");
    return result;
}

int TEST_WstGLDestroyNativeWindow()
{
    printf("TEST_WstGLDestroyNativeWindow\n");
    int result = 0;
    return result;
}

int TEST_WstGLGetNativePixmap()
{
    printf("Test: TEST_WstGLGetNativePixmap\n");
    int result = 0;
    return result;
}

int TEST_WstGLGetNativePixmapDimensions()
{
    printf("TEST_WstGLGetNativePixmapDimensions");
    int result = 0;
    return result;
}

int TEST_WstGLReleaseNativePixmap()
{
    printf("TEST_WstGLReleaseNativePixmap\n");
    int result = 0;
    return result;
}

int TEST_WstGLGetEGLNativePixmap()
{
    printf("TEST_WstGLGetEGLNativePixmap\n");
    int result = 0;
    return result;
}

void printFailSuccess(int i)
{
    printf("%s\n", i == RESULT_TEST_PASS ? "PASS" : "FAIL");
}
int main()
{
    printFailSuccess(TEST_eglGetDisplay_Overload());
    /*
    TEST_WstGLInit();
    TEST_WstGLTerm();
    TEST_WstGLCreateNativeWindow();
    TEST_WstGLDestroyNativeWindow();
    TEST_WstGLGetNativePixmap();
    TEST_WstGLGetNativePixmapDimensions();
    TEST_WstGLReleaseNativePixmap();
    TEST_WstGLGetEGLNativePixmap();
    */
    return 0;
}
