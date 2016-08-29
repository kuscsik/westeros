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
#include <stdio.h>
#include "westeros-gl.h"

#define RESULT_TEST_PASS 0
#define RESULT_TEST_FAIL 1


int TEST_WstGLInit() {
    printf("Test: %s\n", __FUNCTION__);
    WstGLCtx* ctx = WstGLInit();

    if(ctx!=NULL);
      return RESULT_TEST_FAIL;

    return RESULT_TEST_PASS;
}

int TEST_WstGLTerm() {
    printf("Test: %s\n", __FUNCTION__);
    int result = 0;
    return result;
}

int TEST_WstGLCreateNativeWindow() {
    printf("Test: %s\n", __FUNCTION__);
    int result = 0;
    return result;
}


int TEST_WstGLDestroyNativeWindow() {
    printf("Test: %s\n", __FUNCTION__);
    int result = 0;
    return result;
}


int TEST_WstGLGetNativePixmap() {
    printf("Test: %s\n", __FUNCTION__);
    int result = 0;
    return result;
}


int TEST_WstGLGetNativePixmapDimensions() {
    printf("Test: %s\n", __FUNCTION__);
    int result = 0;
    return result;
}


int TEST_WstGLReleaseNativePixmap() {
    printf("Test: %s\n", __FUNCTION__);
    int result = 0;
    return result;
}


int TEST_WstGLGetEGLNativePixmap() {
    printf("Test: %s\n", __FUNCTION__);
    int result = 0;
    return result;
}

typedef int (*testfunction)();
int (*testFunctions[])() = {
    TEST_WstGLInit,
    TEST_WstGLTerm,
    TEST_WstGLCreateNativeWindow,
    TEST_WstGLDestroyNativeWindow,
    TEST_WstGLGetNativePixmap,
    TEST_WstGLGetNativePixmapDimensions,
    TEST_WstGLReleaseNativePixmap,
    TEST_WstGLGetEGLNativePixmap,
};

int main() {
    testfunction t;
    for(int i = 0 ; i < sizeof(testFunctions)/sizeof(testfunction); i++) {
      t = testFunctions[i];
      printf("Result: %s\n", t()==0 ? "OK" : "FAIL" );
    }
  return 0;
}
