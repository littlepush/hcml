/*
    hcml_cxx.c
    Project: HCML
    Author: Push Chen(littlepush)
    Github: https://github.com/littlepush/hcml
    Date: 2020-03-17

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

#ifdef __cplusplus
extern "C" {
#endif

/* Get Properties Count  */
int __tag_prop_count( struct hcml_tag_t * tag ) {
    int _c;
    struct hcml_prop_t * __prop;
    _c = 0;
    __prop = tag->p_root;
    while ( __prop != NULL ) {
        ++_c;
        __prop = __prop->n_prop;
    }
    return _c;
}

/* Get child tag count */
int __tag_child_count( struct hcml_tag_t * tag ) {
    int _c;
    struct hcml_tag_t * __tag;
    _c = 0;
    __tag = tag->c_tag;
    while ( __tag != NULL ) {
        ++_c;
        __tag = __tag->n_tag;
    }
    return _c;
}

/* Get xth child */
struct hcml_tag_t* __child_tag_at_index( struct hcml_tag_t * root_tag, int index ) {
    int _c;
    struct hcml_tag_t * __tag;
    _c = 0;
    __tag = root_tag->c_tag;
    while ( _c != index && __tag != NULL ) {
        ++_c;
        __tag = __tag->n_tag;
    }
    return __tag;
}

/* Break sibling relation and generate code */
int __bread_sibling_and_generate( hcml_node_t *h, struct hcml_tag_t *root_tag, int index, const char* suf ) {
    int _c;
    struct hcml_tag_t * __tag, * __sibling;
    _c = 0;
    __tag = root_tag;
    while ( _c != index ) {
        __tag = root_tag->n_tag;
        ++_c;
    }
    __sibling = __tag->n_tag;
    __tag->n_tag = NULL;
    hcml_generate_cxx_lang(h, __tag, suf);
    __tag->n_tag = __sibling;
    return h->errcode;
}

/* Append Data to node result */
int __append_code_data( hcml_node_t *h, const char *s, int l ) {
    if ( l == 0 ) { return 1; }
    while ( (h->bufsize - h->rsize) <= l ) {
        h->presult = (char *)realloc(h->presult, h->bufsize * 2);
        if ( h->presult == NULL ) {
            hcml_set_error(h, HCML_ERR_ERBUFALLOC, 
                "Malloc Error for Result Buffer");
            return 0;
        }
        h->bufsize *= 2;
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
            hcml_set_error(h, HCML_ERR_ERBUFALLOC, 
                "Malloc Error for Result Buffer");
            return 0;
        }
        h->bufsize *= 2;
    }
#ifdef __IS_WINDOWS__
    _append_len = vsprintf_s( h->presult + h->rsize, h->bufsize - h->rsize, fmt, _arglist );
#else
    _append_len = vsnprintf( h->presult + h->rsize, h->bufsize - h->rsize, fmt, _arglist );
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
    return 1;
}

int __cxx_is_tag( struct hcml_tag_t *tag, const char *tagname ) {
    return strncmp( tag->data_string, tagname, tag->dl ) == 0;
}

struct hcml_prop_t *__cxx_get_prop( struct hcml_tag_t *tag, const char* propname ) {
    struct hcml_prop_t * _p = tag->p_root;
    while ( _p != NULL ) {
        if ( strncmp( _p->key, propname, _p->kl ) == 0 ) break;
        _p = _p->n_prop;
    }
    return _p;
}

/* Generate binary operator */
int __generate_cxx_binary_operator( hcml_node_t *h, struct hcml_tag_t *op_tag, const char* op ) {
    if ( __tag_child_count(op_tag) != 2 ) {
        hcml_set_error(h, HCML_ERR_ESYNTAX, 
            "Syntax Error, missing tag arround %s", op);
        return HCML_ERR_ESYNTAX;
    }
    if ( HCML_ERR_OK != __bread_sibling_and_generate(h, op_tag->c_tag, 0, NULL) ) return h->errcode;
    if ( !__append_code_format(h, " %s ", op) ) return h->errcode;
    if ( HCML_ERR_OK != __bread_sibling_and_generate(h, op_tag->c_tag, 1, NULL) ) return h->errcode;
    return HCML_ERR_OK;
}

/* Generate Keyword tag */
int __generate_cxx_keyword( hcml_node_t *h, const char *keyword ) {
    __append_code_format(h, "%s", keyword);
    return h->errcode;
}

/* Generate Code Wrapper */
int __generate_cxx_wrapper( 
    hcml_node_t *h, struct hcml_tag_t *root_tag,
    const char *begin, const char* end, const char* suf 
) {
    do {
        if ( !__append_code_format(h, begin ) ) break;
        if ( root_tag->c_tag == NULL ) {
            hcml_set_error(h, HCML_ERR_ESYNTAX, 
                "Syntax Error, empty wrapper %s%s", begin, end);
            break;
        }
        if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, suf) ) break;
        if ( !__append_code_format(h, end) ) break;
    } while ( 0 );
    return h->errcode;
}

/* Genearte C++ Code according to the parsed tag */
int hcml_generate_cxx_lang( hcml_node_t *h, struct hcml_tag_t *root_tag, const char*suf ) {
    struct hcml_prop_t * __prop = NULL;
    struct hcml_prop_t * __peol = NULL;
    struct hcml_prop_t * __pgetval = NULL;
    struct hcml_prop_t * __pgetaddr = NULL;
    struct hcml_prop_t * __pgetref = NULL;
    do {
        if ( root_tag->is_tag == 0 ) {
            if ( !__append_code_format(h, "%s(\"", h->print_method) ) break;
            if ( !__append_pure_string(h, root_tag->data_string, root_tag->dl) ) break;
            if ( !__append_code_format(h, "\", %d);", root_tag->dl) ) break;
        } else {
            __peol = __cxx_get_prop(root_tag, "eol");
            __pgetval = __cxx_get_prop(root_tag, "val");
            __pgetaddr = __cxx_get_prop(root_tag, "addr");
            __pgetref = __cxx_get_prop(root_tag, "ref");

            if ( __cxx_is_tag(root_tag, "cxx:string") ) {
                if ( !__append_code_format(h, "\"") ) break;
                if ( root_tag->c_tag != NULL ) {
                    // String Tag is an atomic tag, all data insider will be wraped
                    // by quote
                    if ( !__append_code_format(h, "%.*s", 
                        root_tag->c_tag->dl, root_tag->c_tag->data_string)
                    ) {
                        break;                    
                    }
                }
                if ( !__append_code_format(h, "\"") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:const") ) {
                if ( root_tag->c_tag == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error, invalid const tag");
                    break;
                }
                if ( !__append_code_format(h, "%.*s", 
                    root_tag->c_tag->dl, root_tag->c_tag->data_string)
                ) {
                    break;
                }
            } else if ( __cxx_is_tag(root_tag, "cxx:empty") ) {
                /* Do nothing for empty tag */
            } else if ( __cxx_is_tag(root_tag, "cxx:invoke") ) {
                __prop = __cxx_get_prop(root_tag, "ptr");
                if ( __prop != NULL ) {
                    if ( !__append_code_format(h, "->") ) break;
                } else {
                    if ( !__append_code_format(h, ".") ) break;
                }
                __prop = __cxx_get_prop(root_tag, "name");
                if ( __prop == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error, missing function name for invoke.");
                    break;
                }

                if ( !__append_code_format(h, "%.*s(", __prop->vl, __prop->value) ) break;
                if ( root_tag->c_tag != NULL ) {
                    if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, ", ") ) break;
                    h->rsize -= 2;
                }
                if ( !__append_code_format(h, ")" ) ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:call") ) {
                __prop = __cxx_get_prop(root_tag, "name");
                if ( __prop == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error, missing function name for call.");
                    break;
                }

                if ( !__append_code_format(h, "%.*s(", __prop->vl, __prop->value) ) break;
                if ( root_tag->c_tag != NULL ) {
                    if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, ", ") ) break;
                    h->rsize -= 2;
                }
                if ( !__append_code_format(h, ")" ) ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:list") ) {
                if ( !__append_code_format(h, "{") ) break;
                if ( root_tag->c_tag != NULL ) {
                    if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, ", ") ) break;
                    h->rsize -= 2;
                }
                if ( !__append_code_format(h, "}" ) ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:var") ) {
                __prop = __cxx_get_prop(root_tag, "type");
                if ( __prop != NULL ) {
                    if ( !__append_code_format(h, "%.*s ", __prop->vl, __prop->value) ) break;
                }
                __prop = __cxx_get_prop(root_tag, "name");
                if ( __prop == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error, missing variable name for var.");
                    break;
                }
                if ( __pgetval != NULL ) {
                    if ( !__append_code_format(h, "(*%.*s)", __prop->vl, __prop->value) ) break;
                } else if ( __pgetaddr != NULL || __pgetref != NULL ) {
                    if ( !__append_code_format(h, "(&%.*s)", __prop->vl, __prop->value) ) break;
                } else {
                    if ( !__append_code_format(h, "%.*s", __prop->vl, __prop->value) ) break;
                }
                if ( root_tag->c_tag != NULL ) {
                    if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, NULL) ) break;
                }
            } else if ( __cxx_is_tag(root_tag, "cxx:code") ) {
                if ( root_tag->c_tag == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error, invalid code tag");
                    break;
                }
                if ( !__append_code_format(h, "%.*s", 
                    root_tag->c_tag->dl, root_tag->c_tag->data_string)
                ) {
                    break;
                }
            } else if ( __cxx_is_tag(root_tag, "cxx:line") ) {
                if ( root_tag->c_tag != NULL ) {
                    if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, NULL) ) break;
                }
                if ( !__append_code_format(h, ";") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:print")) {
                if ( !__append_code_format(h, "%s(", h->print_method) ) break;
                if ( root_tag->c_tag == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: Empty print is not allowed");
                    break;
                }
                if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, NULL) ) break;
                if ( !__append_code_format(h, ");") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:subscript") ) {
                if ( HCML_ERR_OK != __generate_cxx_wrapper(h, root_tag, "[", "]", NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:block") ) {
                if ( HCML_ERR_OK != __generate_cxx_wrapper(h, root_tag, "{\n", "}", "\n") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:parentheses") ) {
                if ( HCML_ERR_OK != __generate_cxx_wrapper(h, root_tag, "(", ")", NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:post_increase") ) {
                if ( HCML_ERR_OK != __generate_cxx_wrapper(h, root_tag, "(", ")++", NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:pre_increase") ) {
                if ( HCML_ERR_OK != __generate_cxx_wrapper(h, root_tag, "++(", ")", NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:post_decrease") ) {
                if ( HCML_ERR_OK != __generate_cxx_wrapper(h, root_tag, "(", ")--", NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:pre_decrease") ) {
                if ( HCML_ERR_OK != __generate_cxx_wrapper(h, root_tag, "--(", ")", NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:set") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "=") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:great") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, ">") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:greatequal") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, ">=") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:less") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "<") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:lessequan") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "<=") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:equal") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "==") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:notequal") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "!=") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:plus") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "+") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:minus") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "-") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:times") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "*") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:divid") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "/") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:mod") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "%") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:return") ) {
                if ( HCML_ERR_OK != __generate_cxx_keyword(h, "return;") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:break") ) {
                if ( HCML_ERR_OK != __generate_cxx_keyword(h, "break;") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:continue") ) {
                if ( HCML_ERR_OK != __generate_cxx_keyword(h, "continue;") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:true") ) {
                if ( HCML_ERR_OK != __generate_cxx_keyword(h, "true") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:false") ) {
                if ( HCML_ERR_OK != __generate_cxx_keyword(h, "false") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:new") ) {
                if ( HCML_ERR_OK != __generate_cxx_keyword(h, "new") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:delete") ) {
                if ( HCML_ERR_OK != __generate_cxx_keyword(h, "delete ") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:typeinit") ) {
                __prop = __cxx_get_prop(root_tag, "type");
                if ( __prop == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error, missing type name for type init.");
                    break;
                }

                if ( !__append_code_format(h, "%.*s(", __prop->vl, __prop->value) ) break;
                if ( root_tag->c_tag != NULL ) {
                    if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, ", ") ) break;
                    h->rsize -= 2;
                }
                if ( !__append_code_format(h, ")" ) ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:condition") ) {
                if ( root_tag->c_tag == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: invalidate condition, missing case");
                    break;
                }
                if ( !__cxx_is_tag(root_tag->c_tag, "cxx:case") ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: invalidate condition, first child must be case");
                    break;
                }
                if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:case") ) {
                if ( root_tag->f_tag == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: cannot use case individual");
                    break;
                }
                /* First Case */
                if ( root_tag->f_tag->c_tag == root_tag ) {
                    if ( !__append_code_format(h, "if ( ") ) break;
                } else {
                    if ( !__append_code_format(h, "else if ( ") ) break;
                }
                if ( __tag_child_count(root_tag) < 2 ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: case at least need have two child node");
                    break;
                }
                /* Temperate break the relation */
                if ( HCML_ERR_OK != __bread_sibling_and_generate(h, root_tag->c_tag, 0, NULL) )
                    break;
                if ( !__append_code_format(h, " ) ") ) break;
                if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag->n_tag, NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:else") ) {
                if ( root_tag->f_tag == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: cannot use else individual");
                    break;
                }
                if ( !__append_code_format(h, "else ") ) break;
                if ( root_tag->c_tag == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: empty else is not allowed");
                    break;
                }
                if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, "\n") ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:each") ) {
                if ( __tag_child_count(root_tag) < 3 ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: each must have at least 3 child node");
                    break;
                }

                if ( !__append_code_format(h, "for (") ) break;
                if ( HCML_ERR_OK != __bread_sibling_and_generate(h, root_tag->c_tag, 0, NULL) ) break;
                if ( !__append_code_format(h, " : ") ) break;
                if ( HCML_ERR_OK != __bread_sibling_and_generate(h, root_tag->c_tag, 1, NULL) ) break;
                if ( !__append_code_format(h, ")") ) break;

                /* All node from 3rd will be formateed as the loop body */
                if ( HCML_ERR_OK != hcml_generate_cxx_lang(
                    h, __child_tag_at_index(root_tag, 2), NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "cxx:for") ) {
                if ( __tag_child_count(root_tag) < 4 ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: for must have at least 4 child node");
                    break;
                }

                if ( !__append_code_format(h, "for (") ) break;
                if ( HCML_ERR_OK != __bread_sibling_and_generate(h, root_tag->c_tag, 0, NULL) ) break;
                if ( !__append_code_format(h, "; ") ) break;
                if ( HCML_ERR_OK != __bread_sibling_and_generate(h, root_tag->c_tag, 1, NULL) ) break;
                if ( !__append_code_format(h, "; ") ) break;
                if ( HCML_ERR_OK != __bread_sibling_and_generate(h, root_tag->c_tag, 2, NULL) ) break;
                if ( !__append_code_format(h, ") ") ) break;
                if ( HCML_ERR_OK != hcml_generate_cxx_lang(
                    h, __child_tag_at_index(root_tag, 3), NULL) ) 
                    break;
            } else if ( __cxx_is_tag(root_tag, "cxx:while") ) {
                if ( __tag_child_count(root_tag) < 2 ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: while must have at least 2 child node");
                    break;
                }
                if ( !__append_code_format(h, "while (") ) break;
                if ( HCML_ERR_OK != __bread_sibling_and_generate(h, root_tag->c_tag, 0, NULL) ) break;
                if ( !__append_code_format(h, ") ") ) break;
                if ( HCML_ERR_OK != hcml_generate_cxx_lang(
                    h, __child_tag_at_index(root_tag, 1), NULL) )
                    break;
            } else if ( __cxx_is_tag(root_tag, "cxx:do") ) {
                if ( __tag_child_count(root_tag) < 2 ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: while must have at least 2 child node");
                    break;
                }
                if ( !__append_code_format(h, "do ") ) break;
                if ( HCML_ERR_OK != hcml_generate_cxx_lang(
                    h, __child_tag_at_index(root_tag, 1), NULL) )
                    break;
                if ( !__append_code_format(h, " while (") ) break;
                if ( HCML_ERR_OK != __bread_sibling_and_generate(h, root_tag->c_tag, 0, NULL) ) break;
                if ( !__append_code_format(h, ");") ) break;
            }
        }
        if ( __peol != NULL ) {
            if ( !__append_code_format(h, ";") ) break;
        }
        if ( suf != NULL ) {
            if ( !__append_code_format(h, "%s", suf) ) 
                break;
        }
        if ( root_tag->n_tag != NULL ) {
            if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->n_tag, suf) )
                break;
        }
    } while ( 0 );
    return h->errcode;
}

#ifdef __cplusplus
}
#endif

/*
    __hcml_cxx.c__
    Push Chen
*/