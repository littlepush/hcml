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
int __break_sibling_and_generate( hcml_node_t *h, struct hcml_tag_t *root_tag, int index, const char* suf ) {
    int _c;
    struct hcml_tag_t * __tag, * __sibling;
    _c = 0;
    __tag = root_tag;
    while ( _c != index ) {
        __tag = __tag->n_tag;
        ++_c;
    }
    __sibling = __tag->n_tag;
    __tag->n_tag = NULL;
    hcml_generate_cxx_lang(h, __tag, suf);
    __tag->n_tag = __sibling;
    return h->errcode;
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
    if ( HCML_ERR_OK != __break_sibling_and_generate(h, op_tag->c_tag, 0, NULL) ) return h->errcode;
    if ( !hcml_append_code_format(h, " %s ", op) ) return h->errcode;
    if ( HCML_ERR_OK != __break_sibling_and_generate(h, op_tag->c_tag, 1, NULL) ) return h->errcode;
    return HCML_ERR_OK;
}

/* Generate Keyword tag */
int __generate_cxx_keyword( hcml_node_t *h, const char *keyword ) {
    hcml_append_code_format(h, "%s ", keyword);
    return h->errcode;
}

/* Generate Code Wrapper */
int __generate_cxx_wrapper( 
    hcml_node_t *h, struct hcml_tag_t *root_tag,
    const char *begin, const char* end, const char* suf 
) {
    do {
        if ( !hcml_append_code_format(h, begin ) ) break;
        if ( root_tag->c_tag == NULL ) {
            hcml_set_error(h, HCML_ERR_ESYNTAX, 
                "Syntax Error, empty wrapper %s%s", begin, end);
            break;
        }
        if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, suf) ) break;
        if ( !hcml_append_code_format(h, end) ) break;
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
    int _pure_string_size = 0;
    do {
        if ( root_tag->is_tag == 0 ) {
            if ( !hcml_append_code_format(h, "%s(\"", h->print_method) ) break;
            _pure_string_size = hcml_append_pure_string(h, root_tag->data_string, root_tag->dl);
            if ( _pure_string_size == 0 ) break;
            if ( !hcml_append_code_format(h, "\", %d);", _pure_string_size) ) break;
        } else {
            __peol = __cxx_get_prop(root_tag, "eol");
            __pgetval = __cxx_get_prop(root_tag, "val");
            __pgetaddr = __cxx_get_prop(root_tag, "addr");
            __pgetref = __cxx_get_prop(root_tag, "ref");

            if ( __cxx_is_tag(root_tag, "string") ) {
                if ( !hcml_append_code_format(h, "\"") ) break;
                if ( root_tag->c_tag != NULL ) {
                    // String Tag is an atomic tag, all data insider will be wraped
                    // by quote
                    if ( !hcml_append_code_format(h, "%.*s", 
                        root_tag->c_tag->dl, root_tag->c_tag->data_string)
                    ) {
                        break;                    
                    }
                }
                if ( !hcml_append_code_format(h, "\"") ) break;
            } else if ( __cxx_is_tag(root_tag, "const") ) {
                if ( root_tag->c_tag == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error, invalid const tag");
                    break;
                }
                if ( !hcml_append_code_format(h, "%.*s", 
                    root_tag->c_tag->dl, root_tag->c_tag->data_string)
                ) {
                    break;
                }
            } else if ( __cxx_is_tag(root_tag, "empty") ) {
                /* Do nothing for empty tag */
            } else if ( __cxx_is_tag(root_tag, "invoke") ) {
                __prop = __cxx_get_prop(root_tag, "ptr");
                if ( __prop != NULL ) {
                    if ( !hcml_append_code_format(h, "->") ) break;
                } else {
                    if ( !hcml_append_code_format(h, ".") ) break;
                }
                __prop = __cxx_get_prop(root_tag, "name");
                if ( __prop == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error, missing function name for invoke.");
                    break;
                }

                if ( !hcml_append_code_format(h, "%.*s(", __prop->vl, __prop->value) ) break;
                if ( root_tag->c_tag != NULL ) {
                    if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, ", ") ) break;
                    h->rsize -= 2;
                }
                if ( !hcml_append_code_format(h, ")" ) ) break;
            } else if ( __cxx_is_tag(root_tag, "call") ) {
                __prop = __cxx_get_prop(root_tag, "name");
                if ( __prop == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error, missing function name for call.");
                    break;
                }

                if ( !hcml_append_code_format(h, "%.*s(", __prop->vl, __prop->value) ) break;
                if ( root_tag->c_tag != NULL ) {
                    if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, ", ") ) break;
                    h->rsize -= 2;
                }
                if ( !hcml_append_code_format(h, ")" ) ) break;
            } else if ( __cxx_is_tag(root_tag, "list") ) {
                if ( !hcml_append_code_format(h, "{") ) break;
                if ( root_tag->c_tag != NULL ) {
                    if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, ", ") ) break;
                    h->rsize -= 2;
                }
                if ( !hcml_append_code_format(h, "}" ) ) break;
            } else if ( __cxx_is_tag(root_tag, "var") ) {
                __prop = __cxx_get_prop(root_tag, "type");
                if ( __prop != NULL ) {
                    if ( !hcml_append_code_format(h, "%.*s ", __prop->vl, __prop->value) ) break;
                }
                __prop = __cxx_get_prop(root_tag, "name");
                if ( __prop == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error, missing variable name for var.");
                    break;
                }
                if ( __pgetval != NULL ) {
                    if ( !hcml_append_code_format(h, "(*%.*s)", __prop->vl, __prop->value) ) break;
                } else if ( __pgetaddr != NULL || __pgetref != NULL ) {
                    if ( !hcml_append_code_format(h, "(&%.*s)", __prop->vl, __prop->value) ) break;
                } else {
                    if ( !hcml_append_code_format(h, "%.*s", __prop->vl, __prop->value) ) break;
                }
                if ( root_tag->c_tag != NULL ) {
                    if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, NULL) ) break;
                }
            } else if ( __cxx_is_tag(root_tag, "code") ) {
                if ( root_tag->c_tag == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error, invalid code tag");
                    break;
                }
                if ( !hcml_append_code_format(h, "%.*s", 
                    root_tag->c_tag->dl, root_tag->c_tag->data_string)
                ) {
                    break;
                }
            } else if ( __cxx_is_tag(root_tag, "line") ) {
                if ( root_tag->c_tag != NULL ) {
                    if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, NULL) ) break;
                }
                if ( !hcml_append_code_format(h, ";") ) break;
            } else if ( __cxx_is_tag(root_tag, "print")) {
                if ( !hcml_append_code_format(h, "%s(", h->print_method) ) break;
                if ( root_tag->c_tag == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: Empty print is not allowed");
                    break;
                }
                if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, NULL) ) break;
                if ( !hcml_append_code_format(h, ");") ) break;
            } else if ( __cxx_is_tag(root_tag, "subscript") ) {
                if ( HCML_ERR_OK != __generate_cxx_wrapper(h, root_tag, "[", "]", NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "block") ) {
                if ( HCML_ERR_OK != __generate_cxx_wrapper(h, root_tag, "{\n", "}", "\n") ) break;
            } else if ( __cxx_is_tag(root_tag, "parentheses") ) {
                if ( HCML_ERR_OK != __generate_cxx_wrapper(h, root_tag, "(", ")", NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "post_increase") ) {
                if ( HCML_ERR_OK != __generate_cxx_wrapper(h, root_tag, "", "++", NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "pre_increase") ) {
                if ( HCML_ERR_OK != __generate_cxx_wrapper(h, root_tag, "++", "", NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "post_decrease") ) {
                if ( HCML_ERR_OK != __generate_cxx_wrapper(h, root_tag, "", "--", NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "pre_decrease") ) {
                if ( HCML_ERR_OK != __generate_cxx_wrapper(h, root_tag, "--", "", NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "set") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "=") ) break;
            } else if ( __cxx_is_tag(root_tag, "great") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, ">") ) break;
            } else if ( __cxx_is_tag(root_tag, "greatequal") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, ">=") ) break;
            } else if ( __cxx_is_tag(root_tag, "less") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "<") ) break;
            } else if ( __cxx_is_tag(root_tag, "lessequan") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "<=") ) break;
            } else if ( __cxx_is_tag(root_tag, "equal") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "==") ) break;
            } else if ( __cxx_is_tag(root_tag, "notequal") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "!=") ) break;
            } else if ( __cxx_is_tag(root_tag, "plus") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "+") ) break;
            } else if ( __cxx_is_tag(root_tag, "minus") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "-") ) break;
            } else if ( __cxx_is_tag(root_tag, "times") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "*") ) break;
            } else if ( __cxx_is_tag(root_tag, "divid") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "/") ) break;
            } else if ( __cxx_is_tag(root_tag, "mod") ) {
                if ( HCML_ERR_OK != __generate_cxx_binary_operator(h, root_tag, "%") ) break;
            } else if ( __cxx_is_tag(root_tag, "return") ) {
                if ( HCML_ERR_OK != __generate_cxx_keyword(h, "return") ) break;
            } else if ( __cxx_is_tag(root_tag, "break") ) {
                if ( HCML_ERR_OK != __generate_cxx_keyword(h, "break") ) break;
            } else if ( __cxx_is_tag(root_tag, "continue") ) {
                if ( HCML_ERR_OK != __generate_cxx_keyword(h, "continue") ) break;
            } else if ( __cxx_is_tag(root_tag, "true") ) {
                if ( HCML_ERR_OK != __generate_cxx_keyword(h, "true") ) break;
            } else if ( __cxx_is_tag(root_tag, "false") ) {
                if ( HCML_ERR_OK != __generate_cxx_keyword(h, "false") ) break;
            } else if ( __cxx_is_tag(root_tag, "new") ) {
                if ( HCML_ERR_OK != __generate_cxx_keyword(h, "new") ) break;
            } else if ( __cxx_is_tag(root_tag, "delete") ) {
                if ( HCML_ERR_OK != __generate_cxx_keyword(h, "delete ") ) break;
            } else if ( __cxx_is_tag(root_tag, "typeinit") ) {
                __prop = __cxx_get_prop(root_tag, "type");
                if ( __prop == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error, missing type name for type init.");
                    break;
                }

                if ( !hcml_append_code_format(h, "%.*s(", __prop->vl, __prop->value) ) break;
                if ( root_tag->c_tag != NULL ) {
                    if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, ", ") ) break;
                    h->rsize -= 2;
                }
                if ( !hcml_append_code_format(h, ")" ) ) break;
            } else if ( __cxx_is_tag(root_tag, "condition") ) {
                if ( root_tag->c_tag == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: invalidate condition, missing case");
                    break;
                }
                if ( !__cxx_is_tag(root_tag->c_tag, "case") ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: invalidate condition, first child must be case");
                    break;
                }
                if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "case") ) {
                if ( root_tag->f_tag == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: cannot use case individual");
                    break;
                }
                /* First Case */
                if ( root_tag->f_tag->c_tag == root_tag ) {
                    if ( !hcml_append_code_format(h, "if ( ") ) break;
                } else {
                    if ( !hcml_append_code_format(h, "else if ( ") ) break;
                }
                if ( __tag_child_count(root_tag) < 2 ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: case at least need have two child node");
                    break;
                }
                /* Temperate break the relation */
                if ( HCML_ERR_OK != __break_sibling_and_generate(h, root_tag->c_tag, 0, NULL) )
                    break;
                if ( !hcml_append_code_format(h, " ) ") ) break;
                if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag->n_tag, NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "else") ) {
                if ( root_tag->f_tag == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: cannot use else individual");
                    break;
                }
                if ( !hcml_append_code_format(h, "else ") ) break;
                if ( root_tag->c_tag == NULL ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: empty else is not allowed");
                    break;
                }
                if ( HCML_ERR_OK != hcml_generate_cxx_lang(h, root_tag->c_tag, "\n") ) break;
            } else if ( __cxx_is_tag(root_tag, "each") ) {
                if ( __tag_child_count(root_tag) < 3 ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: each must have at least 3 child node");
                    break;
                }

                if ( !hcml_append_code_format(h, "for (") ) break;
                if ( HCML_ERR_OK != __break_sibling_and_generate(h, root_tag->c_tag, 0, NULL) ) break;
                if ( !hcml_append_code_format(h, " : ") ) break;
                if ( HCML_ERR_OK != __break_sibling_and_generate(h, root_tag->c_tag, 1, NULL) ) break;
                if ( !hcml_append_code_format(h, ")") ) break;

                /* All node from 3rd will be formateed as the loop body */
                if ( HCML_ERR_OK != hcml_generate_cxx_lang(
                    h, __child_tag_at_index(root_tag, 2), NULL) ) break;
            } else if ( __cxx_is_tag(root_tag, "for") ) {
                if ( __tag_child_count(root_tag) < 4 ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: for must have at least 4 child node");
                    break;
                }

                if ( !hcml_append_code_format(h, "for (") ) break;
                if ( HCML_ERR_OK != __break_sibling_and_generate(h, root_tag->c_tag, 0, NULL) ) break;
                if ( !hcml_append_code_format(h, "; ") ) break;
                if ( HCML_ERR_OK != __break_sibling_and_generate(h, root_tag->c_tag, 1, NULL) ) break;
                if ( !hcml_append_code_format(h, "; ") ) break;
                if ( HCML_ERR_OK != __break_sibling_and_generate(h, root_tag->c_tag, 2, NULL) ) break;
                if ( !hcml_append_code_format(h, ") ") ) break;
                if ( HCML_ERR_OK != hcml_generate_cxx_lang(
                    h, __child_tag_at_index(root_tag, 3), NULL) ) 
                    break;
            } else if ( __cxx_is_tag(root_tag, "while") ) {
                if ( __tag_child_count(root_tag) < 2 ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: while must have at least 2 child node");
                    break;
                }
                if ( !hcml_append_code_format(h, "while (") ) break;
                if ( HCML_ERR_OK != __break_sibling_and_generate(h, root_tag->c_tag, 0, NULL) ) break;
                if ( !hcml_append_code_format(h, ") ") ) break;
                if ( HCML_ERR_OK != hcml_generate_cxx_lang(
                    h, __child_tag_at_index(root_tag, 1), NULL) )
                    break;
            } else if ( __cxx_is_tag(root_tag, "do") ) {
                if ( __tag_child_count(root_tag) < 2 ) {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, 
                        "Syntax Error: while must have at least 2 child node");
                    break;
                }
                if ( !hcml_append_code_format(h, "do ") ) break;
                if ( HCML_ERR_OK != hcml_generate_cxx_lang(
                    h, __child_tag_at_index(root_tag, 1), NULL) )
                    break;
                if ( !hcml_append_code_format(h, " while (") ) break;
                if ( HCML_ERR_OK != __break_sibling_and_generate(h, root_tag->c_tag, 0, NULL) ) break;
                if ( !hcml_append_code_format(h, ");") ) break;
            } else {
                if ( h->exlangfp != NULL ) {
                    if ( HCML_ERR_OK != (*(hcml_lang_generator)h->exlangfp)(h, root_tag, suf) )
                        break;
                } else {
                    hcml_set_error(h, HCML_ERR_ESYNTAX, "Syntax Error: Unsupported Tag: %*.s", 
                        root_tag->dl, root_tag->data_string);
                    break;
                }
            }
        }
        if ( __peol != NULL ) {
            if ( !hcml_append_code_format(h, ";") ) break;
        }
        if ( suf != NULL ) {
            if ( !hcml_append_code_format(h, "%s", suf) ) 
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