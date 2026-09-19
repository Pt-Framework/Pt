/* Copyright (C) 2015-2023 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_JSON_API_H
#define PT_JSON_API_H

#include <Pt/Api.h>

#define PT_JSON_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_JSON_VERSION_MINOR PT_VERSION_MINOR
#define PT_JSON_VERSION_REVISION PT_VERSION_REVISION
#define PT_JSON_VERSION_PRERELEASE PT_VERSION_PRERELEASE

#if defined(PT_JSON_API_EXPORT)
#    define PT_JSON_API PT_EXPORT
#  else
#    define PT_JSON_API PT_IMPORT
#  endif

namespace Pt {

/** @namespace Pt::Json
    @brief Read and write JSON ducuments.
*/
namespace Json {

    class Node;
    class Enddocument;
    class JsonReader;

} // namespace Json

} // namespace Pt

#endif
