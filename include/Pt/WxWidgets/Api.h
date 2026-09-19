/* Copyright (C) 2005-2007 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_WXWIDGETS_API_H
#define PT_WXWIDGETS_API_H

#include <Pt/Api.h>

#define PT_WXWIDGETS_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_WXWIDGETS_VERSION_MINOR PT_VERSION_MINOR
#define PT_WXWIDGETS_VERSION_REVISION PT_VERSION_REVISION
#define PT_WXWIDGETS_VERSION_PRERELEASE PT_VERSION_PRERELEASE
 
#if defined(PT_WXWIDGETS_API_EXPORT)
#    define PT_WXWIDGETS_API PT_EXPORT
#  else
#    define PT_WXWIDGETS_API PT_IMPORT
#  endif

namespace Pt {

/** @namespace Pt::WxWidgets
    @brief WxWidgets bridge.
*/
namespace WxWidgets {
}

}
 
#endif // PT_WXWIDGETS_API_H
