/*
    hcml_def.c
    Project: HCML
    Author: Push Chen(littlepush)
    Github: https://github.com/littlepush/hcml
    Date: 2020-03-16

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

#include "hcml_def.h"
#include "hcml.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Create and init a new prop node */
struct hcml_prop_t * __malloc_prop( const char * key, int kl ) {
    struct hcml_prop_t *_p = (struct hcml_prop_t *)malloc(sizeof(struct hcml_prop_t));
    _p->key = key;
    _p->kl = kl;
    _p->value = NULL;
    _p->vl = 0;
    _p->n_prop = NULL;
    return _p;
}

/* Set the prop's value */
void __set_prop_value( struct hcml_prop_t * p, const char * value, int vl ) {
    p->value = value;
    p->vl = vl;
}

/* Free a property node and all it's siblings */
void __free_prop( struct hcml_prop_t * p ) {
    if ( p == NULL ) return;
    if ( p->n_prop != NULL ) {
        __free_prop(p->n_prop);
        p->n_prop = NULL;
    }
    free(p);
}

/* Create and init a new tag node */
struct hcml_tag_t * __malloc_tag( const char * key, int kl ) {
    struct hcml_tag_t *_t = (struct hcml_tag_t *)malloc(sizeof(struct hcml_tag_t));
    _t->data_string = key;
    _t->dl = kl;
    _t->is_tag = 1;
    _t->is_ended = 0;
    _t->p_root = NULL;
    _t->c_tag = NULL;
    _t->n_tag = NULL;
    _t->f_tag = NULL;
    return _t;
}

/* Create and init a string tag node */
struct hcml_tag_t * __malloc_string( const char * value, int vl ) {
    struct hcml_tag_t *_t = (struct hcml_tag_t *)malloc(sizeof(struct hcml_tag_t));
    _t->data_string = value;
    _t->dl = vl;
    _t->is_tag = 0;
    _t->is_ended = 1;
    _t->p_root = NULL;
    _t->c_tag = NULL;
    _t->n_tag = NULL;
    _t->f_tag = NULL;
    return _t;
}

/* Free a tag node, and all it's properies and children and siblings */
void __free_tag( struct hcml_tag_t * t ) {
    if ( t == NULL ) return;
    if ( t->p_root != NULL ) {
        __free_prop(t->p_root);
        t->p_root = NULL;
    }
    if ( t->c_tag != NULL ) {
        __free_tag(t->c_tag);
        t->c_tag = NULL;
    }
    if ( t->n_tag != NULL ) {
        __free_tag(t->n_tag);
        t->n_tag = NULL;
    }
    t->f_tag = NULL;
    free(t);
}

/* Set the error message */
void __set_error__( hcml_node_t *h, int eno, const char * fmt, ... ) {
    int _ml;
    va_list _arglist;
    va_start( _arglist, fmt );
#ifdef __IS_WINDOWS__
    _ml = vsprintf_s( h->errmsg, 255, fmt, _arglist );
#else
    _ml = vsnprintf( h->errmsg, 255, fmt, _arglist );
#endif
    va_end( _arglist );
    h->errmsg[_ml] = 0;
    h->errcode = eno;
}

void __print_check_escape_char( char c ) {
    if ( !isspace(c) ) {
        printf("%c", c);
    } else {
        if ( c == ' ' ) printf(" ");
        else if ( c == '\n' ) printf("\\n");
        else if ( c == '\t' ) printf("\\t");
        else if ( c == '\v' ) printf("\\v");
        else if ( c == '\f' ) printf("\\f");
        else if ( c == '\r' ) printf("\\r");
    }
}

void __print_num_string( const char * s, int n ) {
    int i;
    for ( i = 0; i < n; ++i ) {
        __print_check_escape_char( s[i] );
    }
}

/* Dump debug structure info */
void __dump_tag( struct hcml_tag_t * root, int lv ) {
    struct hcml_prop_t * _p;
    if ( root->is_tag == 1 ) {
        printf("%*s<Tag>: %.*s", lv * 2, "", root->dl, root->data_string);
        _p = root->p_root;
        if ( root->p_root != NULL ) printf("(");
        while ( _p != NULL ) {
            printf("%.*s: %.*s", _p->kl, _p->key, _p->vl, _p->value);
            if ( _p->n_prop != NULL ) {
                printf(", ");
            }
            _p = _p->n_prop;
        }
        if ( root->p_root != NULL ) printf(")");
        printf("\n");
    } else {
        if ( root->dl > 10 ) {
            printf("%*s<String>: ", lv * 2, "");
            __print_num_string( root->data_string, 6 );
            printf("...");
            __print_num_string( root->data_string + root->dl - 4, 4 );
        } else {
            printf("%*s<String>: ", lv * 2, "");
            __print_num_string( root->data_string, root->dl );
        }
        printf("(%d)\n", root->dl);
    }
    if ( root->c_tag != NULL ) {
        __dump_tag(root->c_tag, lv + 1);
    }
    if ( root->n_tag != NULL ) {
        __dump_tag(root->n_tag, lv);
    }
}

/* Append Data to node result */
int __append_code_data( hcml_node_t *h, const char *s, int l ) {
    if ( l == 0 ) { return 1; }
    while ( (h->bufsize - h->rsize) <= l ) {
        h->presult = (char *)realloc(h->presult, h->bufsize * 2);
        if ( h->presult == NULL ) {
            __set_error__(h, HCML_ERR_ERBUFALLOC, "Malloc Error for Result Buffer");
            return 0;
        }
        h->bufsize *= 2;
        printf("realloced result buffer\n");
    }
    strncpy( h->presult + h->rsize, s, l );
    h->rsize += l;
    h->presult[h->rsize] = '\0';
    return 1;
}

/* Append Code Format */
int __append_code_format( hcml_node_t *h, const char *fmt, ... ) {
    int _append_len;
    va_list _arglist;
    va_start( _arglist, fmt );
#ifdef __IS_WINDOWS__
    _append_len = vsprintf_s( NULL, 0, fmt, _arglist );
#else
    _append_len = vsnprintf( NULL, 0, fmt, _arglist );
#endif
    va_end( _arglist );
    if ( _append_len == 0 ) return 1;
    va_start( _arglist, fmt );
    while ( (h->bufsize - h->rsize) <= _append_len ) {
        h->presult = (char *)realloc(h->presult, h->bufsize * 2);
        if ( h->presult == NULL ) {
            __set_error__(h, HCML_ERR_ERBUFALLOC, "Malloc Error for Result Buffer");
            return 0;
        }
        h->bufsize *= 2;
    }
#ifdef __IS_WINDOWS__
    _append_len = vsprintf_s( h->presult + h->rsize, _append_len + 1, fmt, _arglist );
#else
    _append_len = vsnprintf( h->presult + h->rsize, _append_len + 1, fmt, _arglist );
#endif
    h->rsize += _append_len;
    h->presult[h->rsize] = '\0';
    return 1;
}

int __append_pure_string( hcml_node_t *h, const char *s, int l ) {
    int _w, _i;
    _w = _i = 0;
    while ( _w < l ) {
        while ( _i < l && (!isspace(s[_i])) && s[_i] != '\\' && s[_i] != '\"' ) ++_i;
        if ( (_i - _w) > 0 ) {
            if ( !__append_code_data(h, s + _w, _i - _w) ) return 0;
        }
        if ( _i == l ) break;
        if ( s[_i] == '"' ) {
            if ( !__append_code_data(h, "\\\"", 2) ) return 0;
        } else if ( s[_i] == '\\' ) {
            if ( !__append_code_data(h, "\\\\", 2) ) return 0;
        } else if ( s[_i] == '\r' ) {
            if ( !__append_code_data(h, "\\r", 2) ) return 0;
        } else if ( s[_i] == '\n' ) {
            if ( !__append_code_data(h, "\\n", 2) ) return 0;
        } else if ( s[_i] == '\t' ) {
            if ( !__append_code_data(h, "\\t", 2) ) return 0;
        } else if ( s[_i] == '\v' ) {
            if ( !__append_code_data(h, "\\v", 2) ) return 0;
        } else if ( s[_i] == '\f' ) {
            if ( !__append_code_data(h, "\\f", 2) ) return 0;
        } else if ( s[_i] == ' ' ) {
            if ( !__append_code_data(h, " ", 1) ) return 0;
        }
        ++_i;
        _w = _i;
    }
    return l;
}

/* Genearte C++ Code according to the parsed tag */
void __generate_cxx_lang( hcml_node_t *h, struct hcml_tag_t *root_tag ) {
    if ( root_tag->is_tag == 0 ) {
        /* this is string, make a print */
        if ( !__append_code_format(h, "%s(\"", h->print_method) ) return;
        if ( !__append_pure_string(h, root_tag->data_string, root_tag->dl) ) return;
        if ( !__append_code_format(h, "\", %d);", root_tag->dl) ) return;

        if ( root_tag->n_tag != NULL ) {
            __generate_cxx_lang(h, root_tag->n_tag);
        }
    } else {

    }
}

#ifdef __cplusplus
}
#endif

/*
    __hcml_def.h__
    Push Chen
*/