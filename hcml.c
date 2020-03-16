/*
    hcml.c
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

#include "hcml.h"
#include "hcml_def.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *__value_true = "true";
const char *__value_false = "false";

/* Append new tag To current tag list and return the new tag */
struct hcml_tag_t * __append_tag( 
    hcml_node_t * h, struct hcml_tag_t * current_tag, struct hcml_tag_t * new_tag 
) {
    if ( current_tag != NULL ) {
        if ( current_tag->is_tag == 1 && current_tag->is_ended == 0 ) {
            if ( current_tag->c_tag != NULL ) {
                /* Which will never happen */
                __set_error__(h, HCML_ERR_EPARSE, 
                    "Syntax Error, UnFormated tag at line: %d", h->line);
                __free_tag(new_tag);
                return NULL;
            }
            current_tag->c_tag = new_tag;
            new_tag->f_tag = current_tag;
        } else {
            current_tag->n_tag = new_tag;
            new_tag->f_tag = current_tag->f_tag;
        }
    }
    return new_tag;
}

#define __CHK_LEFT_NOT_ZERO__(...)                                      \
    if ( __rleft == 0 ) { __set_error__(h, HCML_ERR_EPARSE,             \
        "Syntax Error: invalidate tag at line: %d", h->line);           \
        __VA_ARGS__; break; }


/* Parse the code */
void __parse_hcml__( hcml_node_t *h, const char *rbuf, int rbufl ) {
    int __rindex, __rleft;
    struct hcml_tag_t *__root_tag, *__current_tag, *__temp_tag;
    struct hcml_prop_t *__temp_prop, *__temp_prop2;
    const char *__saved_begin;
    int __flag;

    __flag = 0;
    __rindex = 0;
    __rleft = rbufl;
    __root_tag = NULL;
    __current_tag = NULL;
    __temp_tag = NULL;
    __temp_prop = __temp_prop2 = NULL;
    __saved_begin = rbuf;
    h->line = 1;

    while ( __rindex < rbufl ) {
        /* All String */
        while ( __rleft != 0 && rbuf[__rindex] != '<' ) {
            if ( rbuf[__rindex] == '\n' ) { ++h->line; }
            ++__rindex; --__rleft;
        }
        if ( __rleft == 0 ) {
            /* End of source */
            if ( (rbuf + __rindex - __saved_begin) == 0 ) break;
            /* All string */
            __temp_tag = __malloc_string(__saved_begin, (rbuf + __rindex - __saved_begin));
            if ( __root_tag == NULL ) __root_tag = __temp_tag;
            __current_tag = __append_tag(h, __current_tag, __temp_tag);
            break;
        }

        /* Try to guess what current '<' should be */
        /* At this time, we at least has one char left in cache */
        if ( rbuf[__rindex + 1] == '/' ) {
            /* 
                May be </cxx:...>, end of tag 
                current tag must not be null, and current is a tag, and not end
            */
            if ( __current_tag != NULL && __current_tag->is_tag == 1 ) {
                if ( __current_tag->is_ended == 0 ) {
                    /* Yes, we are in a tag's content, and it's not ended yet,
                    1 for '<', 1 for current '/', 1 for the '>' after the tag name */
                    if ( __current_tag->dl < (__rleft - 3) ) {
                        /* Yes, we still have enough pending data to read */
                        if ( strncmp(
                            rbuf + __rindex + 2, 
                            __current_tag->data_string, 
                            __current_tag->dl) == 0 
                        ) {
                            /* Check if we have unsaved string before wo close the tag */
                            if ( (rbuf + __rindex - __saved_begin) > 0 ) {
                                /* All string */
                                __temp_tag = __malloc_string(__saved_begin, 
                                    (rbuf + __rindex - __saved_begin));
                                __append_tag(h, __current_tag, __temp_tag);
                            }
                            /* Yes! this is the end of the tag */
                            __current_tag->is_ended = 1;

                            /* Skip the end tag </...> */
                            __rindex += (__current_tag->dl + 3);
                            __rleft -= (__current_tag->dl - 3);
                            /* Skip space after end of tag */
                            while ( __rleft != 0 && isspace(rbuf[__rindex])) {
                                if ( rbuf[__rindex] == '\n' ) ++h->line;
                                ++__rindex; --__rleft;
                            }
                            __CHK_LEFT_NOT_ZERO__()
                            /* Go to check next value */
                            __saved_begin = (rbuf + __rindex);
                            continue;
                        } else {
                            /* This may be not cxx tag, can be an html end tag */
                            /* Skip current </ */
                            __rindex += 2; 
                            __rleft -= 2;
                            continue;
                        }
                    } else {
                        __set_error__(h, HCML_ERR_EPARSE,
                            "Syntax Error: missing end tag of %s, line: %d", 
                            __current_tag->data_string, h->line);
                        break;
                    }
                } else {
                    /* check parent tag */
                    if ( __current_tag->f_tag != NULL && __current_tag->f_tag->is_ended == 0 ) {
                        if ( __current_tag->f_tag->dl < (__rleft - 3) ) {
                            if ( strncmp(
                                rbuf + __rindex + 2,
                                __current_tag->f_tag->data_string,
                                __current_tag->f_tag->dl
                                ) == 0 
                            ) {
                                /* Check if we have unsaved string before wo close the tag */
                                if ( (rbuf + __rindex - __saved_begin) > 0 ) {
                                    /* All string */
                                    __temp_tag = __malloc_string(__saved_begin, 
                                        (rbuf + __rindex - __saved_begin));
                                    __append_tag(h, __current_tag, __temp_tag);
                                }
                                /* Pop current tag, go up level */
                                __current_tag = __current_tag->f_tag;
                                __current_tag->is_ended = 1;

                                /* Skip the end tag </...> */
                                __rindex += (__current_tag->dl + 3);
                                __rleft -= (__current_tag->dl - 3);

                                /* Skip space after end of tag */
                                while ( __rleft != 0 && isspace(rbuf[__rindex])) {
                                    if ( rbuf[__rindex] == '\n' ) ++h->line;
                                    ++__rindex; --__rleft;
                                }
                                __CHK_LEFT_NOT_ZERO__()
                                /* Go back to check next value */
                                __saved_begin = (rbuf + __rindex);

                                continue;
                            } else {
                                // We are still in string tag, not need to end now
                                ++__rindex; --__rleft;
                                continue;
                            }
                        } else {
                            __set_error__(h, HCML_ERR_EPARSE,
                                "Syntax Error: missing end tag of %.*s, line: %d", 
                                __current_tag->dl,
                                __current_tag->data_string, h->line);
                            break;
                        }
                    } else {
                        // We are still in string tag, not need to end now
                        ++__rindex; --__rleft;
                        continue;
                    }
                }
            }
        }

        /* Try to guess if this is a begin tag */
        /* <cxx:...> at least have 7 bytes left */
        if ( __rleft > 7 ) {
            if ( 
                strncmp(rbuf + __rindex + 1, h->lang_prefix, h->lang_prefix_l) == 0 && 
                rbuf[__rindex + 1 + h->lang_prefix_l] == ':'
            ) {
                /* This is a new tag */
                if ( (rbuf + __rindex - __saved_begin) > 0 ) {
                    /* We have string before */
                    __temp_tag = __malloc_string(__saved_begin, (rbuf + __rindex - __saved_begin));
                    if ( __root_tag == NULL ) __root_tag = __temp_tag;
                    __current_tag = __append_tag(h, __current_tag, __temp_tag);
                    /* Error hanppend */
                    if ( __current_tag == NULL ) break;
                }
                /* go on to parse the new tag */
                __rindex += 1;
                __rleft -= 1;
                __saved_begin = rbuf + __rindex;
                while ( 
                    __rleft != 0 && 
                    !isspace(rbuf[__rindex]) && 
                    rbuf[__rindex] != '>' && 
                    rbuf[__rindex] != '/' 
                ) {
                    ++__rindex; --__rleft;
                }
                __CHK_LEFT_NOT_ZERO__()
                __temp_tag = __malloc_tag( __saved_begin, (rbuf + __rindex - __saved_begin) );
                if ( __root_tag == NULL ) __root_tag = __temp_tag;
                __current_tag = __append_tag(h, __current_tag, __temp_tag);
                if ( __current_tag == NULL ) break;

                while ( __flag == 0 ) {
                    /* Skip all whitespace */
                    while ( __rleft != 0 && isspace(rbuf[__rindex]) ) {
                        if ( rbuf[__rindex] == '\n' ) { ++h->line; }
                        ++__rindex; --__rleft;                    
                    }
                    __CHK_LEFT_NOT_ZERO__(__flag = 1)
                    if ( rbuf[__rindex] == '/' ) {
                        __set_error__(h, HCML_ERR_EPARSE, 
                            "Syntax Error: inline tag not supported, at line: %d", h->line);
                        __flag = 1;
                        break;
                    }

                    /* Try to parse the properties */
                    if ( rbuf[__rindex] != '>' ) {
                        /*
                            Property format should be: __KEY__="__VALUE__"
                            The double quote cannot be omited
                            Or:
                            __KEY__
                            Will be consider as __KEY__="true"
                        */
                        __saved_begin = rbuf + __rindex;
                        while ( __rleft != 0 && isalpha(rbuf[__rindex]) ) {
                            if ( rbuf[__rindex] == '\n' ) ++h->line;
                            ++__rindex; --__rleft;;
                        }
                        __CHK_LEFT_NOT_ZERO__(__flag = 1)
                        __temp_prop = __malloc_prop(__saved_begin, rbuf + __rindex - __saved_begin);

                        if ( rbuf[__rindex] != '=' ) {
                            if ( isspace(rbuf[__rindex]) || rbuf[__rindex] == '>' ) {
                                /* Prop = true */
                                __set_prop_value(__temp_prop, __value_true, 4);
                            } else {
                                /* Error */
                                __set_error__(h, HCML_ERR_EPARSE,
                                    "Syntax Error: invalidate property at line: %d", h->line);
                                __flag = 1;
                                break;
                            }
                        } else {
                            if ( rbuf[__rindex + 1] != '\"' ) {
                                __set_error__(h, HCML_ERR_EPARSE,
                                    "Syntax Error: missing \" at line: %d", h->line);
                                __flag = 1;
                                break;
                            }
                            /* skip =, now is '"' */
                            __saved_begin = rbuf + __rindex + 1;
                            /* Skip the first '"' */
                            __rindex += 2; __rleft -= 2;
                            do {
                                while ( __rleft != 0 && rbuf[__rindex] != '"' ) {
                                    ++__rindex; --__rleft;
                                }
                                __CHK_LEFT_NOT_ZERO__(__flag = 1)
                                /* Escape \" */
                                if ( rbuf[__rindex - 1] == '\\' ) {
                                    ++__rindex; --__rleft;
                                    continue;
                                }
                                break;
                            } while ( 1 );
                            if ( __flag != 0 ) break;
                            /* Save last '"' */
                            ++__rindex; --__rleft;
                            __set_prop_value(__temp_prop, __saved_begin, rbuf + __rindex - __saved_begin);
                        }
                        if ( __current_tag->p_root == NULL ) {
                            __current_tag->p_root = __temp_prop;
                        } else {
                            __temp_prop2 = __current_tag->p_root;
                            while ( __temp_prop2->n_prop != NULL ) {
                                __temp_prop2 = __temp_prop2->n_prop;
                            }
                            __temp_prop2->n_prop = __temp_prop;
                        }
                    } else {
                        // Yes, we meet the '>', end of current tag part
                        break;
                    }
                }

                if ( __flag != 0 ) break;
                if ( rbuf[__rindex] == '>' ) {
                    /* Current tag begin part has end */
                    ++__rindex, --__rleft;
                    /* Skip space after end of tag */
                    while ( __rleft != 0 && isspace(rbuf[__rindex])) {
                        if ( rbuf[__rindex] == '\n' ) ++h->line;
                        ++__rindex; --__rleft;
                    }
                    __CHK_LEFT_NOT_ZERO__()
                    __saved_begin = rbuf + __rindex;
                    continue;
                }
            } else {
                ++__rindex; --__rleft;
            }
        }
    }

    __dump_tag( __root_tag, 0 );
    __generate_cxx_lang( h, __root_tag );
    __free_tag( __root_tag );
}

/* 
    Create an hcml handler 
 */
hcml_t hcml_create() {
    hcml_node_t * _h;
    _h = (hcml_node_t *)calloc(1, sizeof(hcml_node_t));
    if ( _h == NULL ) return NULL;
    /* Deefault is CXX */
    strcpy(_h->lang_prefix, "cxx");
    _h->lang_prefix_l = 3;
    return (hcml_t)_h;
}

/* 
    Destroy an hcml handler and release all dynamic data 
 */
void hcml_destroy( hcml_t h ) {
    hcml_node_t * _h = (hcml_node_t *)h;
    if ( h == NULL ) return;
    if ( _h->presult != NULL ) {
        free(_h->presult);
        _h->presult = NULL;
    }
    _h->bufsize = 0;
    _h->rsize = 0;
    free(_h);
}

/* 
    Get the output buffer 
 */
const char * hcml_get_output( hcml_t h ) {
    return ((hcml_node_t *)h)->presult;
}

/*
    Get the last error message 
 */
const char * hcml_get_errstr( hcml_t h ) {
    return ((hcml_node_t *)h)->errmsg;
}

/*
    Get the last error code
 */
int hcml_get_errcode( hcml_t h ) {
    return ((hcml_node_t *)h)->errcode;
}

/* 
    Get the output size 
 */
int hcml_get_output_size( hcml_t h ) {
    return ((hcml_node_t *)h)->rsize;
}

/*
    Set the static string print method
 */
void hcml_set_print_method( hcml_t h, const char* method ) {
    ((hcml_node_t *)h)->print_method_l = strlen(method);
    if ( ((hcml_node_t *)h)->print_method_l > 127 ) {
        ((hcml_node_t *)h)->print_method_l = 127;
    }
    strncpy(((hcml_node_t *)h)->print_method, method, 
        ((hcml_node_t *)h)->print_method_l);
    ((hcml_node_t *)h)->print_method[((hcml_node_t *)h)->print_method_l] = 0;
}

/*
    Get the static string print method
*/
const char* hcml_get_print_methoc( hcml_t h ) {
    return ((hcml_node_t *)h)->print_method;
}

/*
    Set the language prefix, default is "cxx"
 */

void hcml_set_lang_prefix( hcml_t h, const char * prefix ) {
    ((hcml_node_t *)h)->lang_prefix_l = strlen(prefix);
    if ( ((hcml_node_t *)h)->lang_prefix_l > 127 ) {
        ((hcml_node_t *)h)->lang_prefix_l = 127;
    }
    strncpy(((hcml_node_t *)h)->lang_prefix, prefix, 
        ((hcml_node_t *)h)->lang_prefix_l);
    ((hcml_node_t *)h)->lang_prefix[((hcml_node_t *)h)->lang_prefix_l] = 0;
}
/*
    Get the language prefix
 */
const char * hcml_get_lang_prefix( hcml_t h ) {
    return ((hcml_node_t *)h)->lang_prefix;
}

/*
    Parse the input file and output to a dynamically allocated memory
 */
int hcml_parse( hcml_t h, const char * src_path ) {
    hcml_node_t *_h; /* Handler */
    char *__sbuf; /* Source Code Reading Buffer */
    int __fdsrc; /* source file handler */
    const char *__src_path; /* Temp source path if we need to guess the input language */
    struct stat __fstat;

    /* Init */
    _h = (hcml_node_t *)h;
    __sbuf = NULL;
    __fdsrc = -1;
    __src_path = NULL;

    /* The following error may caused by memory leak, we should not try to 
        touch the memory address in the handler */
    /* Invalidate HCML Handler */
    if ( _h == NULL ) return HCML_ERR_INVALIDATE_HANDLER;

    /* Invalidate Input source path */
    if ( src_path == NULL ) return HCML_ERR_INVALIDATE_SRCPATH;

    /* Reset the errcode */
    _h->errcode = 0;
    _h->errmsg[0] = '\0';

    do {
        __fdsrc = open(src_path, O_RDONLY);
        if ( __fdsrc == -1 ) {
            __set_error__(_h, HCML_ERR_ERRSRC, "Cannot open source file for reading");
            break;
        }
        if ( fstat(__fdsrc, &__fstat) != 0 ) {
            __set_error__(_h, HCML_ERR_ESTAT, "Stat source file error");
            break;
        }
        /* Alloc 1KB for reading buffer */
        __sbuf = (char *)malloc(__fstat.st_size);
        if ( __sbuf == NULL ) {
            __set_error__(_h, HCML_ERR_ESBUFALLOC, "Malloc Error for Reading Buffer");
            break;
        }
        /* Read the whole source code into memory */
        read( __fdsrc, __sbuf, __fstat.st_size );

        /* Init Output buffer 1KB */
        if ( _h->bufsize == 0 && _h->presult != NULL ) {
            __set_error__(_h, HCML_ERR_ERBUF, "Invalidate Result Buffer, Not NULL");
            break;
        }
        if ( _h->bufsize != 0 && _h->presult == NULL ) {
            __set_error__(_h, HCML_ERR_ERBUFSIZE, "Invalidate Result Buffer, Size not 0");
            break;
        }
        if ( _h->bufsize == 0 && _h->presult == NULL ) {
            _h->bufsize = 1024;
            _h->presult = (char *)malloc(_h->bufsize);
            if ( _h->presult == NULL ) {
                __set_error__(_h, HCML_ERR_ERBUFALLOC, "Malloc Error for Result Buffer");
                break;
            }
            _h->presult[0] = '\0';
        }
        if ( _h->print_method_l == 0 ) {
            __set_error__(_h, HCML_ERR_EPRINT, "Invalidate Print Method");
            break;
        }
        /* Internal Paser Call */
        __parse_hcml__(_h, __sbuf, __fstat.st_size);
    } while ( 0 );

    /* Close source file handler */
    if ( __fdsrc != -1 ) {
        close(__fdsrc);
    }
    /* Release source buffer */
    if ( __sbuf != NULL ) {
        free(__sbuf);
    }

    /* Return the last errcode as the fainal result */
    return _h->errcode;
}

#ifdef __cplusplus
}
#endif

/*
    __hcml.c__
    Push Chen
*/