/*
 * Copyright (C) 2020-2026 by Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include "ImageFormatter.h"

namespace Pt {

namespace Mcp {

ImageFormatter::ImageFormatter(const std::string& mimeType)
: _b64(new Pt::Base64Codec())
, _mimeType(mimeType)
{
}


Pt::Formatter& ImageFormatter::beginContent(std::ostream& os)
{
    os << "{\"type\":\"image\",\"data\":\"";
    _b64.attach(os);
    return *this;
}


void ImageFormatter::finishContent(std::ostream& os)
{
    _b64.flush();
    os << "\",\"mimeType\":\"" << _mimeType << "\"}";
}


void ImageFormatter::onAddBinary(const char* /*name*/, const char* /*type*/,
                                 const char* value, std::size_t length, const char* /*id*/)
{
    _b64.write(value, static_cast<std::streamsize>(length));
}

} // namespace Mcp

} // namespace Pt
