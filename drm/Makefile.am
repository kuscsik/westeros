#
# If not stated otherwise in this file or this component's Licenses.txt file the
# following copyright and licenses apply:
#
# Copyright 2016 Linaro Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

GST_BASE_CFLAGS ?=
GST_BASE_LIBS ?=

AM_CFLAGS = -DUSE_WESTEROS -I${top_srcdir}/include -I/usr/include/drm

lib_LTLIBRARIES = libwesteros_gl.la

libwesteros_gl_la_SOURCES = westeros-gl.c

libwesteros_gl_la_includedir = $(includedir)
libwesteros_gl_la_CXXFLAGS= $(AM_CFLAGS)

libwesteros_gl_la_LDFLAGS= \
   -lwayland-egl

