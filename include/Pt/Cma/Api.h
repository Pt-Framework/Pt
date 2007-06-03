#ifndef PTV_CMA_API_H
#define PTV_CMA_API_H

#include <Pt/Api.h>

#if defined(PTV_CMA_API_EXPORT)
#    define PTV_CMA_API PT_EXPORT
#  else
#    define PTV_CMA_API PT_IMPORT
#  endif

#endif 
