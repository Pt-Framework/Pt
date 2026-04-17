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

#include <Pt/Gfx/PngReader.h>
#include <Pt/Gfx/Argb32.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Size.h>
#include <Pt/IOError.h>
#include <iostream>
#include <ios>
#include <cstdlib>
#include <png.h>

namespace Pt {

namespace Gfx {

class PngReaderImpl
{
    public:
        PngReaderImpl()
        : _target(0)
        , _state(OnBegin)
        , _pngRead(0)
        , _pngInfo(0)
        , _bufferSize(0)
        , _image(0)
        , _width(0)
        , _height(0)
        , _depth(0)
        , _channels(0)
        { }

        PngReaderImpl(std::istream& is, Image& image)
        : _target(&is)
        , _state(OnBegin)
        , _pngRead(0)
        , _pngInfo(0)
        , _bufferSize(0)
        , _image(&image)
        , _width(0)
        , _height(0)
        , _depth(0)
        , _channels(0)
        { }

        ~PngReaderImpl()
        {
            if(_pngRead)
            {
                png_destroy_read_struct(&_pngRead, &_pngInfo, (png_infopp)0);
            }
        }

        void attach(std::istream& is, Image& image)
        {
            _target = &is;
            _image = &image;
        }

        void detach()
        {
            _target = 0;
            _image = 0;
        }

        void reset()
        {
            if(_pngRead)
            {
                png_destroy_read_struct(&_pngRead, &_pngInfo, (png_infopp)0);
            }

            _state = OnBegin;
            _pngRead = 0;
            _pngInfo = 0;

            _bufferSize = 0;

            _width = 0;
            _height = 0;
            _depth = 0;
            _channels = 0;

            detach();
        }

        Image& get()
        {
            if( ! _image )
                throw IOError("png error");

            int eof = std::ios::traits_type::eof();
            
            do
            {
                int c = _target->rdbuf()->sgetc();
                if(c == eof)
                    throw IOError("invalid png format");
            } 
            while( ! advance() );

            return *_image;
        }

        Image* advance()
        {
            if( ! _target || ! _target->rdbuf() || ! _image )
                return 0;

            if( ! _pngRead )
            {
                _pngRead = png_create_read_struct(PNG_LIBPNG_VER_STRING, 
                                                  NULL, &onPngError, &onPngWarning);
                if( ! _pngRead )
                    throw IOError("png error");

                _pngInfo = png_create_info_struct(_pngRead);
                if( ! _pngInfo )
                {
                    png_destroy_read_struct(&_pngRead, &_pngInfo, (png_infopp)0);
                    throw IOError("png error");
                }

                png_set_read_fn(_pngRead, this, onPngRead);
                png_set_progressive_read_fn(_pngRead, this, &onPngInfo, &onPngRow, &onPngEnd);
            }

            std::streamsize avail = _target->rdbuf()->in_avail();

            if(_state == OnBegin)
            {
                if(avail <= 0)
                  return 0;

                std::streamsize signatureSize = 7;
                
                std::streamsize s = std::min(signatureSize - _bufferSize, avail);

                std::streamsize n = _target->rdbuf()->sgetn(_buffer + _bufferSize, s);
                if(n > 0)
                {
                    avail -= n;
                    _bufferSize += n;
                }

                if(_bufferSize < signatureSize)
                    return 0;

                int isPng = png_sig_cmp((png_byte*)_buffer, 0, png_size_t(_bufferSize));
                
                if(isPng != 0)
                  throw IOError("invalid png format");

                png_set_sig_bytes( _pngRead, static_cast<int>(n) );

                _bufferSize = 0;
                _state = OnData;
            }

            while(avail > 0 && _state != OnEnd)
            {
                std::streamsize n = avail > sizeof(_buffer) ? sizeof(_buffer)
                                                            : avail;

                avail -= _target->rdbuf()->sgetn(_buffer, n);
                png_process_data(_pngRead, _pngInfo, (png_byte*)_buffer, (png_size_t)n);
            }

            return _state == OnEnd ? _image : 0;
        }
        
        static void onPngRead(png_structp png, png_bytep data, png_size_t length)
        {
            png_voidp p = png_get_io_ptr(png);
            Pt::Gfx::PngReaderImpl* reader = static_cast<Pt::Gfx::PngReaderImpl*>(p);
            reader->onRead(png, data, length);
        }

        void onRead(png_structp png, png_bytep data, png_size_t length)
        {
            char* buffer = reinterpret_cast<char*>(data);
            std::streamsize n = static_cast<std::streamsize>(length);
            _target->rdbuf()->sgetn(buffer, n);
        }
        
        static void onPngInfo(png_structp png, png_infop info)
        {
            png_voidp p = png_get_progressive_ptr(png);
            Pt::Gfx::PngReaderImpl* reader = static_cast<Pt::Gfx::PngReaderImpl*>(p);
            reader->onInfo(png, info);

            // update info for libpng
            png_read_update_info(png, info);
        }

        void onInfo(png_structp png, png_infop info)
        {
            // image width in pixel
            _width = png_get_image_width(png, info);

            // image height in pixel
            _height = png_get_image_height(png, info);
   
            // bits per CHANNEL
            _depth = png_get_bit_depth(png, info);
    
            // number of channels
            _channels = png_get_channels(png, info);
    
            // color type. (RGB, RGBA, Luminance, luminance alpha... palette... etc)
            png_uint_32 color_type = png_get_color_type(png, info);

            // transformations to image format
            switch (color_type)
            {
                case PNG_COLOR_TYPE_PALETTE:
                    png_set_palette_to_rgb(png);
            
                    // channel info
                    _channels = 3;
                    break;
        
                case PNG_COLOR_TYPE_GRAY:
                    if (_depth < 8)
                        png_set_expand_gray_1_2_4_to_8(png);
            
                    // bitdepth info
                    _depth = 8;
                    break;
            }
    
            // alpha channel
            if (png_get_valid(png, info, PNG_INFO_tRNS))
            {
                png_set_tRNS_to_alpha(png);
                _channels += 1;
            }
    
            // round precision down to 8
            if (_depth == 16)
            {
                png_set_strip_16(png);
                _depth = 8;
            }
        }
        
        static void onPngRow(png_structp png, png_bytep data, png_uint_32 row, int pass)
        {
            png_voidp p = png_get_progressive_ptr(png);
            Pt::Gfx::PngReaderImpl* reader = static_cast<Pt::Gfx::PngReaderImpl*>(p);
            reader->onRow(png, data, row, pass);
        }

        void onRow(png_structp png, png_bytep data, png_uint_32 row, int pass)
        {
            // image width in pixel
            png_uint_32 width =  _width;

            // image height in pixel
            //png_uint_32 height = _height;
    
            // bits per CHANNEL
            png_uint_32 bitdepth = _depth;
    
            // number of channels
            png_uint_32 channels = _channels;

            // resize target image
            if( _image->width() != _width || _image->height() != _height )
            {
                _image->reset( _width, _height, Pt::Gfx::ImageFormat::argb32() );
            }

            // TODO: png_progressive_combine_row(png_ptr, old_row, data);

            std::size_t n = 0;

            if( bitdepth == 8 )
            {
                Argb32LineView lines( _image->data(), _image->width(), _image->height(), 0, Argb32::get() );
                auto line = lines.line(row);
  
                if( channels == 3 )
                {
                    for(auto& pixel : *line)
                    {
                        unsigned char red = data[n++];
                        unsigned char green = data[n++];
                        unsigned char blue = data[n++];

                        pixel = Pt::Gfx::Color(255, red, green, blue);
                    }
                }
                else if( channels == 4 )
                {
                    for(auto& pixel : *line)
                    {
                        unsigned char red = data[n++];
                        unsigned char green = data[n++];
                        unsigned char blue = data[n++];
                        unsigned char alpha = data[n++];

                        pixel = Pt::Gfx::Color(alpha, red, green, blue);
                    }
                }
            }
        }

        static void onPngEnd(png_structp png, png_infop info)
        {
            png_voidp p = png_get_progressive_ptr(png);
            Pt::Gfx::PngReaderImpl* reader = static_cast<Pt::Gfx::PngReaderImpl*>(p);
            reader->onEnd(png, info);
        }

        void onEnd(png_structp png, png_infop info)
        {
            _state = OnEnd;
            _width = 0;
            _height = 0;
            _depth = 0;
            _channels = 0;
        }

        static void onPngError(png_structp png, png_const_charp msg)
        {
            std::cerr << msg << std::endl;
            throw Pt::IOError("invalid png format");
        }

        static void onPngWarning(png_structp png, png_const_charp msg)
        {
            std::clog << msg << std::endl;
        }

    private:
        enum State 
        {
            OnBegin = 0,
            OnData = 1,
            OnEnd = 2
        };

    private:
        std::ios* _target;
        State _state;
        png_structp  _pngRead;
        png_infop    _pngInfo;
        char _buffer[2048];
        std::streamsize _bufferSize;
        Image*      _image;
        std::size_t _width;
        std::size_t _height;
        std::size_t _depth;
        std::size_t _channels;
};


PngReader::PngReader()
: _impl( new PngReaderImpl() )
{
}


PngReader::PngReader(std::istream& is, Image& image)
: _impl( new PngReaderImpl(is, image) )
{
}

PngReader::~PngReader()
{
  delete _impl;
}


void PngReader::attach(std::istream& is, Image& image)
{
    _impl->attach(is, image);
}


void PngReader::detach()
{
    _impl->detach();
}


void PngReader::reset()
{
    _impl->reset();
}


Image* PngReader::advance()
{
    return _impl->advance();
}


Image& PngReader::get()
{
    return _impl->get();
}

} // namespace

} // namespace
