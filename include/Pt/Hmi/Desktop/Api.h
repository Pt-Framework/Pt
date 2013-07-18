#ifndef Pt_Hmi_Desktop_Api_H
#define Pt_Hmi_Desktop_Api_H

#include <Pt/Api.h>


#if defined(PT_HMI_DESKTOP_EXPORTS)
#    define PT_HMI_DESKTOP_API PT_EXPORT
#  else
#    define PT_HMI_DESKTOP_API PT_IMPORT
#  endif

#endif