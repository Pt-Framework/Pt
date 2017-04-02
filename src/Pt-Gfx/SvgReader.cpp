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

#include <iostream>

#include <Pt/IOError.h>

#include <Pt/Gfx/SvgReader.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Size.h>

#include <Pt/Xml/InputSource.h>
#include <Pt/Xml/XmlReader.h>


namespace Pt {
namespace Gfx {


class SvgReaderImpl {
    public:
        inline SvgReaderImpl();
        inline SvgReaderImpl(std::istream& is, Image& image);

        inline ~SvgReaderImpl();

        inline void attach(std::istream& is, Image& image);
        inline void detach();

        inline void reset();

        inline Image* advance();
        inline Image& get();

    private:
        enum State {
            NotStarted
        };

    private:
        State   _state;
        Image*  _image;

        Xml::BinaryInputSource _bis;
        Xml::XmlReader         _xmlReader;
};

SvgReaderImpl::SvgReaderImpl()
: _state(NotStarted)
, _image(0)
{}

SvgReaderImpl::SvgReaderImpl(std::istream& is, Image& image)
: _state(NotStarted)
, _image(0)
{ attach(is, image); }

SvgReaderImpl::~SvgReaderImpl()
{ reset(); }

void SvgReaderImpl::attach(std::istream& is, Image& image)
{
    _bis.reset(is);
    _xmlReader.reset(_bis);

    _image = &image;
}

void SvgReaderImpl::SvgReaderImpl::detach()
{
    _bis.reset();
    _xmlReader.reset();

    _image = 0;
}

void SvgReaderImpl::reset()
{
    detach();

    _state = NotStarted;
}

Image* SvgReaderImpl::advance()
{
    if(!_image) return 0;

    if(_state == NotStarted) {
    }

    return _image;
}

Image& SvgReaderImpl::get()
{
    if(!_image) throw IOError("svg error");

    while(!advance());
    return *_image;
}

// ================================================================================

SvgReader::SvgReader()
: _impl( new SvgReaderImpl() )
{}

SvgReader::SvgReader(std::istream& is, Image& image)
: _impl( new SvgReaderImpl(is, image) )
{}

SvgReader::~SvgReader()
{ delete _impl; }

void SvgReader::attach(std::istream& is, Image& image)
{ _impl->attach(is, image); }

void SvgReader::detach()
{ _impl->detach(); }

void SvgReader::reset()
{ _impl->reset(); }

Image* SvgReader::advance()
{ return _impl->advance(); }


} // namespace
} // namespace
