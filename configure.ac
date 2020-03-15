#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(
    [hcml], 
    [1.0.1], 
    [littlepush@gmail.com], 
    [], 
    [https://github.com/littlepush/hcml])
AC_CONFIG_AUX_DIR([build])
AM_MAINTAINER_MODE([disable])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AM_PROG_AR
AC_PROG_CC
AC_PROG_CXX
LT_INIT

# Check if this is the source dir
AC_CONFIG_SRCDIR([hcml.h])

AS_IF([test "x${prefix}" != "xNONE"], [
    CPPFLAGS="-I${prefix}/include ${CPPFLAGS}"
    LDFLAGS="-L${prefix}/lib ${LDFLAGS}"
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
