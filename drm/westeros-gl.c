/**
* Author: Zoltan Kuscsik <zoltan.kuscsik@linaro.org>
*/
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
#include "westeros-gl.h"

#ifndef DEFAULT_DRM_CARD
#define DEFAULT_DRM_CARD "/dev/dri/card0"
#endif

#define CONNECTOR_ID 1;

typedef struct _WstGLCtx
{
    EGLDisplay dpy;
    EGLContext ctx;
    struct gbm_device* gbm;
    struct gbm_bo *bo;
    drmModeRes *res;
    drmModeConnector *connector;
    drmModeModeInfo *modeInfo;
    EGLImageKHR image;

    int fd;
} WstGLCtx;

static int init_egl(WstGLCtx *glctx)
{
  EGLint major, minor;

  glctx->dpy = eglGetDisplay((NativeDisplayType) glctx->gbm);
  if (!glctx->dpy)
    return -EINVAL;

  eglInitialize(glctx->dpy, &major, &minor);
  eglBindAPI(EGL_OPENGL_API);

  glctx->ctx = eglCreateContext(glctx->dpy, NULL, EGL_NO_CONTEXT, NULL);

  if (!glctx->ctx)
    return -EINVAL;

  eglMakeCurrent(glctx->dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, glctx->ctx);
  return 0;
}

WstGLCtx* WstGLInit() {
    int i, fd;
    drmModeConnector *connector;

    fd = open(DEFAULT_DRM_CARD, O_RDWR);

    if(fd < 0)
        return NULL;

    WstGLCtx* result = (WstGLCtx*) calloc(1, sizeof(WstGLCtx));

    if(result == NULL)
        return NULL;

    result->fd = fd;


    result->res = drmModeGetResources(fd);

    if(!result->res)
        goto failGetResources;

  for (i = 0; i < result->res->count_connectors; i++) {
    connector = drmModeGetConnector(fd, result->res->connectors[i]);

    if (!connector)
      continue;
      if(connector->connection == DRM_MODE_CONNECTED
          && connector->count_modes > 0)
        break;
      drmModeFreeConnector(connector);
    }        

    if(connector!=NULL) {
        result->connector = connector;
        result->modeInfo = &(connector->modes[0]);
    }
    else
        {
         goto failGetResources;
        }

    result->gbm = gbm_create_device( fd );

    if(!result->gbm) {
        goto failOpenGbm;
    }

    result->bo = gbm_bo_create(result->gbm, result->modeInfo->hdisplay, result->modeInfo->vdisplay,
        GBM_BO_FORMAT_XRGB8888,
        GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);

    if (!result->bo)
        goto failOpenGbm;

    if(init_egl(result))
        goto failInitEGL;

    return result;

failInitEGL:
failOpenGbm:
     gbm_device_destroy(result->gbm);
failGetResources:
     if(result->connector)
         drmModeFreeConnector(result->connector);
     close(fd);
     free(result);
     return NULL;
}

void WstGLTerm( WstGLCtx *ctx ) {
    if(ctx == NULL)
        return;

    if(ctx->gbm)
        gbm_device_destroy(ctx->gbm);

    if(ctx->fd >= 0)
        close(ctx->fd);
    free(ctx);
}


void* WstGLCreateNativeWindow( WstGLCtx *ctx, int x, int y, int width, int height ) {
    void* result = NULL;
    return result;
}
void WstGLDestroyNativeWindow( WstGLCtx *ctx, void *nativeWindow ) {
    return;
}
bool WstGLGetNativePixmap( WstGLCtx *ctx, void *nativeBuffer, void **nativePixmap ) {
    bool result = false;

    return result;
}
void WstGLGetNativePixmapDimensions( WstGLCtx *ctx, void *nativePixmap, int *width, int *height ) {

}
void WstGLReleaseNativePixmap( WstGLCtx *ctx, void *nativePixmap ) {
}
void* WstGLGetEGLNativePixmap( WstGLCtx *ctx, void *nativePixmap ) {
    void* result = NULL;

    return result;
}
