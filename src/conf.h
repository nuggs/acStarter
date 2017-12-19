/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef CONF_H
#define CONF_H

// Figure out which platform we're using
#if defined(WIN32) || defined(_WIN32)
#   define ACS_WIN32 1
#   define ESCAPE_CODE "^<ESC^>"
#   ifdef _WIN64
        // if I ever have 64bit builds
#   else
        // Only for 32bit only stuff
#   endif
#elif __APPLE__
    // This is only for desktops or servers, no iPhone or iPad support.
#   include "TargetConditionals.h"
#   if TARGET_OS_MAC
#       define ACS_OSX 1
#   else
#       error "Unknown Apple platform"
#   endif
#elif __linux__
#   define ACS_LINUX 1
#   define ESCAPE_CODE "\033"
#elif __unix__
    // UNIX
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown compiler"
#endif

// Decides whether symbols are imported from a DLL or exported to
// a DLL.  the ACS_SOURCE symbol is defined by all source files of
// the library, so you don't have to worry about it.
#if defined(_MSC_VER)
#   if defined(ACS_STATICLIB)
#       define ACS_API
#   else
#       if defined(ASC_SOURCE)
            // If building the DLL, export the symbols tagged like this
#           define ACS_API __declspec(dllexport)
#       else
            // If consuming the DLL, import the symbols tagged like this
#           define ACS_API __declspec(dllimport)
#       endif
#   endif
#elif defined(__GNUC__)
#   if defined(ACS_STATICLIB)
#       define ACS_API
#   else
#       if defined(ACS_SOURCE)
#           define ACS_API __attribute__ ((visibility ("default")))
#       else
            // If you use -fvisibility=hidden in GCC, exception handling and
            // RTTI would break if visibility wasn't set during export and
            // import because GCC would immediately forget all types of
            // information encountered.
            // See http://gcc.gnu.org/wiki/Visibility
#           define ACS_API __attribute__ ((visibility ("default")))
#       endif
#   endif
#else
#   error "Unknown compiler, please implement shared library macros"
#endif

#endif // CONF_H
