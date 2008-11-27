#ifndef PT_CMA_API_H
#define PT_CMA_API_H

#include <Pt/Api.h>

#if defined(PT_CMA_API_EXPORT)
#    define PT_CMA_API PT_EXPORT
#  else
#    define PT_CMA_API PT_IMPORT
#  endif

#endif 
