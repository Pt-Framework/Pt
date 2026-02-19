/* Copyright (C) 2017 Marc Duerner 
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

#include <Pt/Gfx/PngWriter.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Size.h>
#include <Pt/IOError.h>

#include <vector>
#include <iostream>
#include <ios>
#include <cstdlib>

#include <png.h>

namespace Pt {

namespace Gfx {

class PngWriterImpl
{
    public:
        PngWriterImpl()
        : _target(0)
        , _pngWrite(0)
        , _pngInfo(0)
        , _image(0)
        , _row(0)
        , _rowBuffer(0)
        { }

        PngWriterImpl(std::ostream& os)
        : _target(&os)
        , _pngWrite(0)
        , _pngInfo(0)
        , _image(0)
        , _row(0)
        , _rowBuffer(0)
        { }

        ~PngWriterImpl()
        {
            if(_pngWrite)
            {
                png_destroy_write_struct(&_pngWrite, &_pngInfo);
            }
        }

        void attach(std::ostream& os)
        {
            _target = &os;
        }

        void detach()
        {
            _target = 0;
        }

        void reset()
        {
            if(_pngWrite)
            {
                png_destroy_write_struct(&_pngWrite, &_pngInfo);
            }

            _pngWrite = 0;
            _pngInfo = 0;
            _image = 0;
            _row = 0;

            detach();
        }
        
        void beginWrite(const Image& image)
        {
            if( ! _target || ! _target->rdbuf() )
                return;

            if( ! _pngWrite )
            {
                _pngWrite = png_create_write_struct(PNG_LIBPNG_VER_STRING, 
                                                    NULL, &onPngError, &onPngWarning);
                if( ! _pngWrite )
                    throw IOError("png error");

                _pngInfo = png_create_info_struct(_pngWrite);
                if( ! _pngInfo )
                {
                    png_destroy_write_struct(&_pngWrite, &_pngInfo);
                    throw IOError("png error");
                }
 
                png_set_write_fn(_pngWrite, this, &onPngWrite, &onPngFlush);

                png_set_IHDR(_pngWrite, _pngInfo, 
                             image.width(), image.height(), 8, 
                             PNG_COLOR_TYPE_RGB, 
                             PNG_INTERLACE_NONE, 
                             PNG_COMPRESSION_TYPE_DEFAULT, 
                             PNG_FILTER_TYPE_DEFAULT);
            }

            _image = &image;
            _row = 0;

            png_write_info(_pngWrite, _pngInfo);
        }

        bool advance()
        {
            if( ! _target || ! _target->rdbuf() ||
                ! _image || _row >= _image->height() )
                return true;

            //TODO: use ImageFormat API to convert to required output format
            if( _image->format() != ImageFormat::argb32() )
                throw IOError("invalid image format");

            // allocate memory for one row (3 bytes per pixel - RGB)
            _rowBuffer.resize( 3 * _image->width() );

            // convert row from ARGB32 to RGB24
            std::size_t imageWidth = _image->pixelStride() * _image->width();
            const Pt::uint8_t* from = _image->data() + _row * imageWidth;
            Pt::uint8_t* to = &_rowBuffer[0];
            
            for(int x = 0; x < _image->width(); ++x)
            {
                *to++ = from[2];
                *to++ = from[1];
                *to++ = from[0];

                from += _image->pixelStride();
            }
            
            png_write_row(_pngWrite, &_rowBuffer[0]);
            ++_row;

            if( _row < _image->height() )
                return false;

            png_write_end(_pngWrite, _pngInfo);
            _image  = 0;
            _row = 0;
            return true;
        }

        void write(const Image& image)
        {
            beginWrite(image);
            
            while( ! advance() )
                ;
        }

        static void onPngWrite(png_structp png, png_bytep data, png_size_t length)
        {
            png_voidp p = png_get_io_ptr(png);
            Pt::Gfx::PngWriterImpl* writer = static_cast<Pt::Gfx::PngWriterImpl*>(p);
            writer->onWrite(png, data, length);
        }

        void onWrite(png_structp png, png_bytep data, png_size_t length)
        {
            const char* buffer = reinterpret_cast<const char*>(data);
            std::streamsize n = static_cast<std::streamsize>(length);

            //std::clog << "writing " << length << " bytes." << std::endl;
            _target->rdbuf()->sputn(buffer, n);
        }

        static void onPngFlush(png_structp png)
        {
            png_voidp p = png_get_io_ptr(png);
            Pt::Gfx::PngWriterImpl* writer = static_cast<Pt::Gfx::PngWriterImpl*>(p);
            writer->onFlush(png);
        }

        static void onFlush(png_structp png)
        {
            //std::clog << "flushing buffer." << std::endl;
            //_target->rdbuf()->sync();
        }

        static void onPngError(png_structp png, png_const_charp msg)
        {
            std::cerr << msg << std::endl;
            throw Pt::IOError("png failure");
        }

        static void onPngWarning(png_structp png, png_const_charp msg)
        {
            std::clog << msg << std::endl;
        }

    private:
        enum State 
        {
            OnBegin = 0
        };

    private:
        std::ios*             _target;
        png_structp           _pngWrite;
        png_infop             _pngInfo;
        const Image*          _image;
        std::size_t           _row;
        std::vector<png_byte> _rowBuffer;
};


PngWriter::PngWriter()
: _impl( new PngWriterImpl() )
{
}


PngWriter::PngWriter(std::ostream& os)
: _impl( new PngWriterImpl(os) )
{
}


PngWriter::~PngWriter()
{
  delete _impl;
}


void PngWriter::attach(std::ostream& os)
{
    _impl->attach(os);
}


void PngWriter::detach()
{
    _impl->detach();
}


void PngWriter::reset()
{
    _impl->reset();
}


void PngWriter::beginWrite(const Image& image)
{
    _impl->beginWrite(image);
}


bool PngWriter::advance()
{
    return _impl->advance();
}


void PngWriter::write(const Image& image)
{
    _impl->write(image);
}

} // namespace

} // namespace
