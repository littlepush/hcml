AM_CFLAGS = -Wall
AM_CXXFLAGS = \
	-std=c++11 \
	-Wall \
	-fPIC

lib_LTLIBRARIES = libhcml.la
libhcml_la_SOURCES = \
	hcml.c \
	hcml_cxx.c

include_HEADERS = \
	hcml.h \
	hcml_def.h \
	hcml_cxx.h \
	hcml.hpp

bin_PROGRAMS = hcmlt
hcmlt_LDADD = libhcml.la
hcmlt_SOURCES = hcml-test.cpp
