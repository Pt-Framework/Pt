/* Copyright (C) 2005-2007 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_MFC_API_H
#define PT_MFC_API_H

#include <Pt/Api.h>

#define PT_MFC_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_MFC_VERSION_MINOR PT_VERSION_MINOR
#define PT_MFC_VERSION_REVISION PT_VERSION_REVISION
#define PT_MFC_VERSION_PRERELEASE PT_VERSION_PRERELEASE
 
#if defined(PT_MFC_API_EXPORT)
#    define PT_MFC_API PT_EXPORT
#  else
#    define PT_MFC_API PT_IMPORT
#  endif

namespace Pt {

/** @namespace Pt::Mfc
    @brief MFC bridge.
*/
namespace Mfc {

}
}
 
#endif
