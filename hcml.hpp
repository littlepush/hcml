/*
    hcml.hpp
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

#pragma once

#ifndef HCML_HCML_HPP_
#define HCML_HCML_HPP_

#include <hcml.h>
#include <iostream>
#include <string>

/* 
    This is a C++ HCML handler wrapper
 */
class hcml {

    hcml_t              handler__;

public: 
    // Constructure: Automaticall create a new handler
    hcml( ) : handler__( hcml_create() ) { }
    ~hcml( ) { if ( handler__ != 0 ) hcml_destroy(handler__); }

    // Copy & Move
    hcml( const hcml& rhs ) : handler__( hcml_create() ) {
        if ( rhs.handler__ == 0 ) return;
        hcml_set_print_method(handler__, hcml_get_print_method(rhs.handler__));
        hcml_set_lang_prefix(handler__, hcml_get_lang_prefix(rhs.handler__));
        hcml_set_lang_generator(handler__, hcml_set_lang_generator(rhs.handler__, NULL));
    }
    hcml( hcml&& rhs ) : handler__(rhs.handler__) {
        rhs.handler__ = 0;
    }
    hcml & operator = ( const hcml& rhs ) {
        if ( this == &rhs ) return *this;
        hcml_set_print_method(handler__, hcml_get_print_method(rhs.handler__));
        hcml_set_lang_prefix(handler__, hcml_get_lang_prefix(rhs.handler__));
        hcml_set_lang_generator(handler__, hcml_set_lang_generator(rhs.handler__, NULL));
        return *this;
    }
    hcml & operator = ( hcml&& rhs ) {
        if ( this == &rhs ) return *this;
        handler__ = rhs.handler__;
        rhs.handler__ = 0;
        return *this;
    }

    // Tag Prefix
    void set_tag_prefix( const std::string& prefix ) {
        if ( handler__ == 0 ) return;
        hcml_set_lang_prefix(handler__, prefix.c_str());
    }
    const char * get_tag_prefix() const {
        if ( handler__ == 0 ) return NULL;
        return hcml_get_lang_prefix(handler__);
    }

    // Error
    int errno() const {
        if ( handler__ == 0 ) return 0;
        return hcml_get_errno(handler__);
    }
    const char * errstr() const {
        if ( handler__ == 0 ) return NULL;
        return hcml_get_errstr(handler__);
    }

    // Output
    const char * result() const {
        if ( handler__ == 0 ) return NULL;
        return hcml_get_output(handler__);
    }

    size_t result_size() const {
        if ( handler__ == 0 ) return 0;
        return (size_t)hcml_get_output_size(handler__);
    }

    // Do Parse
    bool parse( const std::string& source_path ) {
        if ( handler__ == 0 ) return false;
        return ( HCML_ERR_OK == hcml_parse(handler__, source_path.c_str()) );
    }
};

template < typename _TyStream >
_TyStream& opreator << ( _TyStream& os, const hcml& h ) {
    if ( h.errno() == HCML_ERR_OK ) {
        os << h.result();
    } else {
        os << h.errstr();
    }
    return os;
}

/*
    __hcml.hpp__
    Push Chen
*/