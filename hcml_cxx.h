/*
    hcml_cxx.h
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

#ifndef HCML_HCML_CXX_H_DEF_
#define HCML_HCML_CXX_H_DEF_

#include "hcml_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Genearte C++ Code according to the parsed tag */
int hcml_generate_cxx_lang( hcml_node_t *h, struct hcml_tag_t *root_tag, const char*suf );

#ifdef __cplusplus
}
#endif

#endif /*
    __hcml_def.h__
    Push Chen
*/