/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_TEXT_API_H
#define PT_TEXT_API_H

#include <Pt/Api.h>

#if defined(PT_TEXT_API_EXPORT)
#    define PT_TEXT_API PT_EXPORT
#  else
#    define PT_TEXT_API PT_IMPORT
#  endif

#endif
