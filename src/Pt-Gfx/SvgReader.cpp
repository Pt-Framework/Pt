/* Copyright (C) 2016-2017 Marc Boris Duerner
 * Copyright (C) 2017-2017 Aloysius Indrayanto
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
 * MA  02110-1301  USA
 */

#include <Pt/Gfx/SvgReader.h>
#include "SvgRasterizer.h"


namespace Pt {
namespace Gfx {


// svn propset svn:mime-type text/plain etc/images/*.svg


class SvgReaderImpl {
    public:
        inline SvgReaderImpl();
        inline SvgReaderImpl(std::istream& is, Image& image);
        inline SvgReaderImpl(std::istream& is, Image& image, const AffineTransform& worldTransform);

        inline ~SvgReaderImpl();

        inline void attach(std::istream& is, Image& image);
        inline void attach(std::istream& is, Image& image, const AffineTransform& worldTransform);
        inline void detach();

        inline void reset();

        inline Image* advance();
        inline Image& get();

    private:
        SvgRasterizer* _svgRasterizer;
};

SvgReaderImpl::SvgReaderImpl()
: _svgRasterizer(0)
{}

SvgReaderImpl::SvgReaderImpl(std::istream& is, Image& image)
: _svgRasterizer(0)
{ attach(is, image); }

SvgReaderImpl::SvgReaderImpl(std::istream& is, Image& image, const AffineTransform& worldTransform)
: _svgRasterizer(0)
{ attach(is, image, worldTransform); }

SvgReaderImpl::~SvgReaderImpl()
{ reset(); }

void SvgReaderImpl::attach(std::istream& is, Image& image)
{ attach(is, image, AffineTransform()); }

void SvgReaderImpl::attach(std::istream& is, Image& image, const AffineTransform& worldTransform)
{
    delete _svgRasterizer;

    _svgRasterizer = new SvgRasterizer(is, image, worldTransform);
}

void SvgReaderImpl::SvgReaderImpl::detach()
{
    delete _svgRasterizer;

    _svgRasterizer = 0;
}

void SvgReaderImpl::reset()
{ detach(); }

Image* SvgReaderImpl::advance()
{
    if(!_svgRasterizer) return 0;

    return _svgRasterizer->process() ? &_svgRasterizer->image() : 0;
}

Image& SvgReaderImpl::get()
{
    if(!_svgRasterizer) throw IOError("svg error");

    while(!advance());

    return _svgRasterizer->image();
}

// ================================================================================

SvgReader::SvgReader()
: _impl( new SvgReaderImpl() )
{}

SvgReader::SvgReader(std::istream& is, Image& image)
: _impl( new SvgReaderImpl(is, image) )
{}

SvgReader::SvgReader(std::istream& is, Image& image, const AffineTransform& worldTransform)
: _impl( new SvgReaderImpl(is, image, worldTransform) )
{}

SvgReader::~SvgReader()
{ delete _impl; }

void SvgReader::attach(std::istream& is, Image& image)
{ _impl->attach(is, image); }

void SvgReader::attach(std::istream& is, Image& image, const AffineTransform& worldTransform)
{ _impl->attach(is, image, worldTransform); };

void SvgReader::detach()
{ _impl->detach(); }

void SvgReader::reset()
{ _impl->reset(); }

Image* SvgReader::advance()
{ return _impl->advance(); }

Image& SvgReader::get()
{ return _impl->get(); }


} // namespace
} // namespace
