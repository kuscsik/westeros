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
#include <dlfcn.h>
#include <stdbool.h>
#include <unistd.h>
#include <gbm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include "drm_egl_utils.h"
#include "westeros-gl.h"

#define LIBEGL_DEFAULT_LIBRARY "libEGL.so"
#define CONNECTOR_ID 1;

static void *g_libegl = NULL;
static WstGLCtx* g_wstCtx = NULL;

typedef struct _WstGLCtx
{
    EGLDisplay dpy;
    struct gbm_device* gbm;
    drmModeRes *res;
    drmModeConnector *connector;
    drmModeEncoder *encoder;
    drmModeModeInfo *modeInfo;
    EGLImageKHR image;
    EGLConfig config;
    int fd;
} WstGLCtx;

/* EGL library overload, based on the work
 * of Takanari Hayama <taki@igel.co.jp>
 * https://github.com/thayama/libegl/blob/master/egl.c
 */
static void _init_egl_library()
{
	const char *filename = (getenv("LIBEGL_LIBRARY_NAME")) ? getenv("LIBEGL_LIBRARY_NAME") : LIBEGL_DEFAULT_LIBRARY;
	g_libegl = dlopen(filename, RTLD_LAZY);
}
static EGLDisplay(*_eglGetDisplay) (EGLNativeDisplayType display_id);

#define CREATE_EGL_SYMBOL(f) \
	if (!g_libegl) _init_egl_library();	\
	if (!_##f) _##f = dlsym(g_libegl, #f);	\

/*End of EGL overload section */

/* EGL overload implementations */
EGLDisplay eglGetDisplay(EGLNativeDisplayType display_id)
{
    EGLint major, minor, n, count;
    CREATE_EGL_SYMBOL(eglGetDisplay);
    if(g_wstCtx == NULL) {
        g_wstCtx = WstGLInit();
    }

    g_wstCtx->dpy = _eglGetDisplay((NativeDisplayType) g_wstCtx->gbm);

    if (!g_wstCtx->dpy)
        return EGL_NO_DISPLAY;

    return g_wstCtx->dpy;
}

WstGLCtx* WstGLInit() {
    int i;

    if(g_wstCtx != NULL)
        return g_wstCtx;

    g_wstCtx = (WstGLCtx*) malloc(sizeof(WstGLCtx));

    g_wstCtx->modeInfo = NULL;
    g_wstCtx->image = NULL;

    if(egl_helper_init_drm_display(
            egl_helper_get_config_type(EGL_OPAQUE_ATTRIBS),
            &g_wstCtx->fd,          /* Get the DRM file descriptor */
            &g_wstCtx->connector,   /* Get DRM mode connector */
            &g_wstCtx->gbm,         /* Get a GBM device */
            &g_wstCtx->dpy,         /* Get the egl Display */
            &g_wstCtx->config       /* Read a valid display config */
            ) == EGL_UTILS_FAILURE )
    {
        goto fail;
    }

    drmModeRes* resources = drmModeGetResources(fd);

    for (i = 0; i < resources->count_encoders; i++) {
        g_wstCtx->encoder = drmModeGetEncoder(g_wstCtx->fd, resources->encoders[i]);

        if (encoder == NULL)
            continue;

        if (g_wstCtx->encoder->encoder_id == g_wstCtx->connector->encoder_id)
            break;

        drmModeFreeEncoder(g_wstCtx->encoder);
    }

    return g_wstCtx;
fail:
    free(g_wstCtx);
    return NULL;
}

void WstGLTerm( WstGLCtx *ctx ) {
    if(ctx == NULL)
        return;

    if(ctx!=g_wstCtx)
        return;

    if(ctx->gbm)
        gbm_device_destroy(ctx->gbm);

    if(ctx->fd >= 0)
        close(ctx->fd);
    free(ctx);
}


void* WstGLCreateNativeWindow( WstGLCtx *ctx, int x, int y, int width, int height ) {

    void *result;
    result = (void*) gbm_surface_create(ctx->gbm, width, height,
                 GBM_FORMAT_XRGB8888,
                 GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
    return result;
}

void WstGLDestroyNativeWindow( WstGLCtx *ctx, void *nativeWindow ) {

    if(ctx == NULL || ctx->gbm == NULL)
        return;
    if(nativeWindow == NULL)
        return;
    struct gbm_surface *gs = nativeWindow;
    gbm_surface_destroy(gs);
}


bool WstGLGetNativePixmap( WstGLCtx *ctx, void *nativeBuffer, void **nativePixmap ) {
    bool result = false;

    struct gbm_surface *gs = (struct gbm_surface*) nativeBuffer;

    struct gbm_bo *bo = gbm_bo_create(ctx->gbm, ctx->modeInfo->hdisplay, ctx->modeInfo->vdisplay,
        GBM_FORMAT_XRGB8888, GBM_BO_USE_RENDERING | GBM_BO_USE_SCANOUT);

    *nativePixmap = bo;
}

void WstGLGetNativePixmapDimensions( WstGLCtx *ctx, void *nativePixmap, int *width, int *height )
{
    struct gbm_bo *bo = (struct gbm_bo*) nativePixmap;
    *width = gbm_bo_get_width(bo);
    *height = gbm_bo_get_height(bo);
}

void WstGLReleaseNativePixmap( WstGLCtx *ctx, void *nativePixmap )
{
    struct gbm_bo *bo = (struct gbm_bo*) nativePixmap;
    gbm_bo_destroy(bo);
}

void* WstGLGetEGLNativePixmap( WstGLCtx *ctx, void *nativePixmap ) {
    return nativePixmap;
}

void WstGLSwapBuffers( WstGLCtx *ctx, void* nativeBuffer) {
    struct gbm_bo *bo;
	struct drm_fb *fb;
    int ret;
	bo = gbm_surface_lock_front_buffer(disp_kmsc->gbm.surface);
	fb = drm_fb_get_from_bo(disp_kmsc, next_bo);
    ret = drmModePageFlip(ctx.fd, disp_kmsc->drm.crtc_id, fb->fb_id,
			DRM_MODE_PAGE_FLIP_EVENT, &waiting_for_flip);
}

void