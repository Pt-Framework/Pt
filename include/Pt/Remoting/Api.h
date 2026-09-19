/* Copyright (C) 2009-2014 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_REMOTING_API_H
#define PT_REMOTING_API_H

#include <Pt/Api.h>

#define PT_REMOTING_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_REMOTING_VERSION_MINOR PT_VERSION_MINOR
#define PT_REMOTING_VERSION_REVISION PT_VERSION_REVISION
#define PT_REMOTING_VERSION_PRERELEASE PT_VERSION_PRERELEASE

#if defined(PT_REMOTING_API_EXPORT)
#    define PT_REMOTING_API PT_EXPORT
#  else
#    define PT_REMOTING_API PT_IMPORT
#  endif

namespace Pt {

/** @namespace Pt::Remoting
    @brief Remote services and clients.
*/
namespace Remoting {

} // namespace Remoting

} // namespace Pt

#endif
