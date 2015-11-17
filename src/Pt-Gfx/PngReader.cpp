/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan 
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
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Size.h>
#include <Pt/IOError.h>
#include <iostream>
#include <ios>
#include <cstdlib>
#include <png.h>

namespace {

void onPngRead(png_structp png, png_bytep data, png_size_t length)
{
    png_voidp p = png_get_io_ptr(png);
    std::ios* is = static_cast<std::ios*>(p);

    char* buffer = reinterpret_cast<char*>(data);
    std::streamsize n = static_cast<std::streamsize>(length);
    is->rdbuf()->sgetn(buffer, n);
}


void onPngInfo(png_structp png, png_infop info)
{
    png_voidp p = png_get_progressive_ptr(png);
    Pt::Gfx::PngReader* reader = static_cast<Pt::Gfx::PngReader*>(p);
    Pt::Gfx::PngReader::onInfo(*reader, png, info);
 }


void onPngRow(png_structp png, png_bytep data, png_uint_32 row, int pass)
{
    png_voidp p = png_get_progressive_ptr(png);
    Pt::Gfx::PngReader* reader = static_cast<Pt::Gfx::PngReader*>(p);
    Pt::Gfx::PngReader::onRow(*reader, (unsigned char*)data, (std::size_t)row, pass);
}


void onPngEnd(png_structp png, png_infop info)
{
    png_voidp p = png_get_progressive_ptr(png);
    Pt::Gfx::PngReader* reader = static_cast<Pt::Gfx::PngReader*>(p);
    Pt::Gfx::PngReader::onEnd(*reader, png, info);
}


void onPngError(png_structp png, png_const_charp msg)
{
    std::cerr << msg << std::endl;
    throw Pt::IOError("invalid png format");
}


void onPngWarning(png_structp png, png_const_charp msg)
{
    std::clog << msg << std::endl;
}

} // namespace

namespace Pt {

namespace Gfx {

PngReader::PngReader()
: _target(0)
, _state(OnBegin)
, _pngRead(0)
, _pngInfo(0)
, _image(0)
, _width(0)
, _height(0)
, _depth(0)
, _channels(0)
{
}


PngReader::PngReader(std::istream& is)
: _target(&is)
, _state(OnBegin)
, _pngRead(0)
, _pngInfo(0)
, _image(0)
, _width(0)
, _height(0)
, _depth(0)
, _channels(0)
{
}


PngReader::~PngReader()
{
  if(_pngRead)
  {
      png_destroy_read_struct(&_pngRead, &_pngInfo, (png_infopp)0);
  }
}


void PngReader::attach(std::istream& is)
{
    _target = &is;
}


void PngReader::detach()
{
    _target = 0;
}


void PngReader::reset()
{
    if(_pngRead)
    {
        png_destroy_read_struct(&_pngRead, &_pngInfo, (png_infopp)0);
    }

    _state = OnBegin;
    _pngRead = 0;
    _pngInfo = 0;

    _image = 0;
    _width = 0;
    _height = 0;
    _depth = 0;
    _channels = 0;

    detach();
}


bool PngReader::advance()
{
    if( ! _target || ! _target->rdbuf() || ! _image )
      return false;

    if( ! _pngRead )
    {
        _pngRead = png_create_read_struct(PNG_LIBPNG_VER_STRING, 
                                          NULL, &onPngError, &onPngWarning);
        png_set_read_fn(_pngRead, _target, onPngRead);
        png_set_progressive_read_fn(_pngRead, this, &onPngInfo, &onPngRow, &onPngEnd);

        _pngInfo = png_create_info_struct(_pngRead);

        if( ! _pngRead || ! _pngInfo)
            throw IOError("internal png error");
    }

    std::streamsize avail = _target->rdbuf()->in_avail();

    if(_state == OnBegin)
    {
        if(avail < 8)
          return false;

        char signature[8];
        std::streamsize n = _target->rdbuf()->sgetn(signature, sizeof(signature));
        if(n > 0)
            avail -= n;

        int isPng = png_sig_cmp((png_byte*)signature, 0, static_cast<png_size_t>(n));
        if(isPng != 0)
          throw IOError("invalid png format");

        png_set_sig_bytes( _pngRead, static_cast<int>(n) );
        _state = OnSignature;
    }

    while(avail > 0 && _state != OnEnd)
    {
        std::streamsize n = avail > sizeof(_buffer) ? sizeof(_buffer)
                                                    : avail;

        avail -= _target->rdbuf()->sgetn(_buffer, n);
        png_process_data(_pngRead, _pngInfo, (png_byte*)_buffer, (png_size_t)n);
    }

    return _state == OnEnd;
}


void PngReader::onInfo(PngReader& reader, png_structp png, png_infop info)
{
    // image width in pixel
    reader._width = png_get_image_width(png, info);

    // image height in pixel
    reader._height = png_get_image_height(png, info);
   
    // bits per CHANNEL
    reader._depth = png_get_bit_depth(png, info);
    
    // number of channels
    reader._channels = png_get_channels(png, info);
    
    // color type. (RGB, RGBA, Luminance, luminance alpha... palette... etc)
    png_uint_32 color_type = png_get_color_type(png, info);

    // transformations to image format
    switch (color_type)
    {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(png);
            
            // channel info
            reader._channels = 3;
            break;
        
        case PNG_COLOR_TYPE_GRAY:
            if (reader._depth < 8)
                png_set_expand_gray_1_2_4_to_8(png);
            
            // bitdepth info
            reader._depth = 8;
            break;
    }
    
    // alpha channel
    if (png_get_valid(png, info, PNG_INFO_tRNS))
    {
        png_set_tRNS_to_alpha(png);
        reader._channels += 1;
    }
    
    // round precision down to 8
    if (reader._depth == 16)
    {
        png_set_strip_16(png);
        reader._depth = 8;
    }

    // update info for libpng
    png_read_update_info(png, info);
}


void PngReader::onRow(PngReader& reader, unsigned char* data, std::size_t row, int pass)
{
    // image width in pixel
    png_uint_32 width =  reader._width;

    // image height in pixel
    png_uint_32 height = reader._height;
    
    // bits per CHANNEL
    png_uint_32 bitdepth = reader._depth;
    
    // number of channels
    png_uint_32 channels = reader._channels;

    // resize target image
    Pt::Gfx::Size imageSize(reader._width, reader._height);
    if( imageSize != reader._image->size() )
        reader._image->resize( imageSize, Pt::Gfx::ImageFormat::argb8888() );

    // TODO: png_progressive_combine_row(png_ptr, old_row, data);
    
    std::size_t n = 0;
		for( size_t x = 0; x < width; ++x)
		{
			if( bitdepth == 8 && channels == 3)
			{
        unsigned char red = data[n++];
        unsigned char green = data[n++];
        unsigned char blue = data[n++];

        Pt::Gfx::Color pixel(0, red/255.0f, green/255.0f, blue/255.0f);
				reader._image->setColor(x, row, pixel);
			}

			if( bitdepth == 8 && channels == 4)
			{
        unsigned char red = data[n++];
        unsigned char green = data[n++];
        unsigned char blue = data[n++];
        unsigned char alpha = data[n++];
        
        Pt::Gfx::Color pixel(alpha/255.0f, red/255.0f, green/255.0f, blue/255.0f);
				reader._image->setColor(x, row, pixel);
			}
		}
}


void PngReader::onEnd(PngReader& reader, png_structp, png_infop)
{
    reader._state = OnEnd;
    reader._width = 0;
    reader._height = 0;
    reader._depth = 0;
    reader._channels = 0;
}

} // namespace

} // namespace
