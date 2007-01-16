/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_TEXT_API_H
#define PT_TEXT_API_H

#include <Pt/Api.h>

#ifdef PT_TEXT_API_IMPORT
#    define PT_TEXT_API PT_IMPORT
#  else
#    define PT_TEXT_API PT_EXPORT
#  endif

#endif
