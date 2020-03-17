/*
    hcml.h
    Project: HCML
    Author: Push Chen(littlepush)
    Github: https://github.com/littlepush/hcml
    Date: 2020-03-15

    MIT License

    Copyright (c) 2020 Push Chen

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once

#ifndef HCML_HCML_H_DEF_
#define HCML_HCML_H_DEF_

#include <stdio.h>

/* Version 0.1 */
#define HCML_VERSION                0x00000003
#ifndef PACKAGE_VERSION
#define HCML_VERSION_STRING         "0.3"
#else
#define HCML_VERSION_STRING         PACKAGE_VERSION
#endif

#ifndef PACKAGE_BUGREPORT
#define HCML_AUTHOR_EMAIL           "littlepush@gmail.com"
#else
#define HCML_AUTHOR_EMAIL           PACKAGE_BUGREPORT
#endif

#if ( defined WIN32 | defined _WIN32 | defined WIN64 | defined _WIN64 )
#define __IS_WINDOWS__  1
#endif

#ifndef __IS_WINDOWS__
#if defined(HAVE_STDLIB_H) && HAVE_STDLIB_H == 1
#include <stdlib.h>
#else
#error "missing stdlib.h in system, please check sysroot"
#endif

#if defined(HAVE_STRING_H) && HAVE_STRING_H == 1
#include <string.h>
#else
#error "missing string.h in system, please check sysroot"
#endif

#if defined(HAVE_MEMORY_H) && HAVE_MEMORY_H == 1
#include <memory.h>
#else
#error "missing memory.h in system, please check sysroot"
#endif

#if defined(HAVE_SYS_TYPES_H) && HAVE_SYS_TYPES_H == 1
#include <sys/types.h>
#endif

#else

#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#endif

#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdarg.h>

#include <hcml_def.h>
#include <hcml_cxx.h>

#define HCML_ERR_INVALIDATE_SRCPATH     -2  /* Invalidate Input source path */
#define HCML_ERR_INVALIDATE_HANDLER     -1  /* Invalidate hcml handler */
#define HCML_ERR_OK                     0   /* No Error */
#define HCML_ERR_ERRSRC                 1   /* Cannot open source file for reading */
#define HCML_ERR_ESTAT                  2   /* Stat source file error */
#define HCML_ERR_ESBUFALLOC             3   /* Malloc Error for Reading Buffer */
#define HCML_ERR_ERBUF                  4   /* Invalidate Result Buffer, Not NULL */
#define HCML_ERR_ERBUFSIZE              5   /* Invalidate Result Buffer, Size not 0 */
#define HCML_ERR_ERBUFALLOC             6   /* Malloc Error for Result Buffer */
#define HCML_ERR_EPRINT                 7   /* Invalidate Print Method */
#define HCML_ERR_EPARSE                 8   /* Pasre error, see message for deatil */
#define HCML_ERR_ESYNTAX                9   /* Syntax Error, see message for deatil */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HCML_ORIGIN_SOURCE_STRING
#define __MIN_PURE_STRING__         1
#endif

/* 
    The HCML Object Handler 
 */
typedef void *      hcml_t;

/* 
    Create an hcml handler 
 */
hcml_t hcml_create();

/* 
    Destroy an hcml handler and release all dynamic data 
 */
void hcml_destroy( hcml_t h );

/* 
    Get the output buffer 
 */
const char * hcml_get_output( hcml_t h );

/*
    Get the last error message 
 */
const char * hcml_get_errstr( hcml_t h );

/*
    Get the last error code
 */
int hcml_get_errcode( hcml_t h );

/*
    Set error code and message
 */
void hcml_set_error( hcml_t h, int code, const char * msgfmt, ... );

/* 
    Get the output size 
 */
int hcml_get_output_size( hcml_t h );

/*
    Set the static string print method
 */
void hcml_set_print_method( hcml_t h, const char* method );

/*
    Get the static string print method
*/
const char * hcml_get_print_method( hcml_t h );

/*
    Set the language prefix, default is "cxx" and return the old
 */
void hcml_set_lang_prefix( hcml_t h, const char * prefix );

/*
    Get the language prefix
 */
const char * hcml_get_lang_prefix( hcml_t h );

/*
    Set and get the old language generator function point.
    fp can be NULL
*/
hcml_lang_generator hcml_set_lang_generator( hcml_t h, hcml_lang_generator fp );

/*
    Set and get the old extend language generator function point.
    fp can be NULL
*/
hcml_lang_generator hcml_set_exlang_generator( hcml_t h, hcml_lang_generator fp );

/* Dump debug structure info */
void hcml_dump_tag( struct hcml_tag_t * root, int lv );

/* Append Data to node result */
int hcml_append_code_data( hcml_node_t *h, const char *s, int l );

/* Append Code Format */
int hcml_append_code_format( hcml_node_t *h, const char *fmt, ... );

/* Append Pure string, will automatically change the escape char */
int hcml_append_pure_string( hcml_node_t *h, const char *s, int l );

/*
    Parse the input file and output to a dynamically allocated memory
 */
int hcml_parse( hcml_t h, const char * src_path );

#ifdef __cplusplus
}
#endif

#endif /*
    __hcml.h__
    Push Chen
*/