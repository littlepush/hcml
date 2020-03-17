/*
    hcml_util.h
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

#ifndef HCML_HCML_UTIL_H_DEF_
#define HCML_HCML_UTIL_H_DEF_

#include "hcml_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Create and init a new prop node */
struct hcml_prop_t * __malloc_prop( const char * key, int kl );

/* Set the prop's value */
void __set_prop_value( struct hcml_prop_t * p, const char * value, int vl );

/* Free a property node and all it's siblings */
void __free_prop( struct hcml_prop_t * p );

/* Create and init a new tag node */
struct hcml_tag_t * __malloc_tag( const char * key, int kl );

/* Create and init a string tag node */
struct hcml_tag_t * __malloc_string( const char * value, int vl );

/* Free a tag node, and all it's properies and children and siblings */
void __free_tag( struct hcml_tag_t * t );

/* Set the error message */
#define __set_error__(h, args...)   hcml_set_error((hcml_t)h, args)

#ifdef __cplusplus
}
#endif

#endif /*
    __hcml_def.h__
    Push Chen
*/