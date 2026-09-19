/* Copyright (C) 2008 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_CMA_API_H
#define PT_CMA_API_H

#include <Pt/Api.h>

#define PT_CMA_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_CMA_VERSION_MINOR PT_VERSION_MINOR
#define PT_CMA_VERSION_REVISION PT_VERSION_REVISION
#define PT_CMA_VERSION_PRERELEASE PT_VERSION_PRERELEASE

#if defined(PT_CMA_API_EXPORT)
#    define PT_CMA_API PT_EXPORT
#  else
#    define PT_CMA_API PT_IMPORT
#  endif

#endif 
