/*
    hcml_def.h
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

#ifndef HCML_HCML_DEF_H_DEF_
#define HCML_HCML_DEF_H_DEF_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /* An error message buffer */
    char                        errmsg[256];
    /* Error code */
    int                         errcode;
    /* Output buffer size */
    int                         bufsize;
    /* Output length */
    int                         rsize;
    /* Output content */
    char                        *presult;
    /* Print Method */
    char                        print_method[128];
    /* Print Method Length */
    int                         print_method_l;
    /* Lang Prefix */
    char                        lang_prefix[32];
    /* Lang Prefix Length */
    int                         lang_prefix_l;
    /* Parsing Line */
    int                         line;
    /* Language Generator Function Point */
    void                        *langfp;
} hcml_node_t;

/* Tag Property Node */
struct hcml_prop_t {
    const char              *key;
    int                     kl;
    const char              *value;
    int                     vl;
    struct hcml_prop_t      *n_prop;
};

/* Tag Node */
struct hcml_tag_t {
    /* 0: String, 1: Tag */
    int                     is_tag;
    /* 0: Not End, 1: Ended */
    int                     is_ended;
    /* When is tag, the name after cxx: */
    const char              *data_string;
    int                     dl;
    struct hcml_prop_t      *p_root;
    struct hcml_tag_t       *c_tag;
    struct hcml_tag_t       *n_tag;
    struct hcml_tag_t       *f_tag;
};

/* Function Point to generate final code */
typedef int (*hcml_lang_generator)( hcml_node_t *, struct hcml_tag_t*, const char*);

#ifdef __cplusplus
}
#endif

#endif /*
    __hcml_def.h__
    Push Chen
*/