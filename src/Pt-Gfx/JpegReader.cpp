/* Copyright (C) 2016 Marc Boris Duerner
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

#include <Pt/Gfx/JpegReader.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Size.h>
#include <Pt/IOError.h>
#include <iostream>
#include <cstdlib>
#include <cassert>

extern "C" {
#include <jpeglib.h>
}

namespace Pt {

namespace Gfx {

class JpegReaderImpl
{
    public:
        JpegReaderImpl()
        : _target(0)
        , _state(OnBegin)
        , _init(false)
        , _image(0)
        { }

        JpegReaderImpl(std::istream& is, Image& image)
        : _target(&is)
        , _state(OnBegin)
        , _init(false)
        , _image(&image)
        { }

        ~JpegReaderImpl()
        {
            if(_init)
            {
                jpeg_destroy_decompress(&_decomp);
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
            if(_init)
            {
                jpeg_destroy_decompress(&_decomp);
                _init = false;
            }

            _state = OnBegin;
            
            detach();
        }

        Image& get()
        {
            if( ! _image )
                throw IOError("jpeg error");

            int eof = std::ios::traits_type::eof();
            
            do
            {
                int c = _target->rdbuf()->sgetc();
                if(c == eof)
                    throw IOError("invalid jpeg format");
            } 
            while( ! advance() );

            return *_image;
        }

        Image* advance()
        {
            if( ! _target || ! _target->rdbuf() || ! _image )
              return 0;

            if( ! _init )
            {
                _source.init_source = &JpegReaderImpl::onInitSource;
                _source.fill_input_buffer = &JpegReaderImpl::onFillBuffer;
                _source.skip_input_data = &JpegReaderImpl::onSkipInput;
                _source.term_source = &JpegReaderImpl::onTermSource;
                _source.resync_to_restart = &jpeg_resync_to_restart;
                _source.next_input_byte = reinterpret_cast<JOCTET*>(_buffer);
                _source.bytes_in_buffer = 0;

                _err.error_exit = &JpegReaderImpl::onErrorExit;
                _err.emit_message = &JpegReaderImpl::onEmitMessage;
                _err.output_message = &JpegReaderImpl::onOutputMessage;
	              
                _decomp.err = jpeg_std_error(&_err);
                _decomp.client_data = this;
                jpeg_create_decompress(&_decomp);
                _decomp.src = &_source;
                
                _init = true;
            }

            std::streamsize n = _bufferSize - _source.bytes_in_buffer;

            // protecte against incorrect marker lengths larger than 2K 
            if( n == 0 )
                throw Pt::IOError("invalid jpeg format");

            std::streamsize avail = _target->rdbuf()->in_avail();
            if(avail < n)
                n = avail;

            if(avail > 0)
            {
                char* egptr = _buffer + _source.bytes_in_buffer;
                n = _target->rdbuf()->sgetn(egptr, n);
                _source.bytes_in_buffer += static_cast<std::size_t>(n);
            }

            if(_state == OnBegin)
            {
                int r = jpeg_read_header(&_decomp, 1);
                if(r == JPEG_SUSPENDED)
                    return 0;

                if(r != JPEG_HEADER_OK)
                    return 0;

                _state = OnHeader;
            }

            if(_state == OnHeader)
            {
                _decomp.out_color_space = JCS_RGB;

                int r = jpeg_start_decompress(&_decomp);
                if(r == 0)
                    return 0;

                 if(_decomp.output_components != 3)
                    throw IOError("jpeg error");

                _image->reset(_decomp.output_width, _decomp.output_height,
                              ImageFormat::rgb32());

                _state = OnData;
            }
            
            if(_state == OnData)
            {
                while(_decomp.output_scanline < _decomp.output_height)
                {
                    Pt::ssize_t bytesPerLine = _decomp.output_components * _image->width();
                    Pt::ssize_t off = bytesPerLine * _decomp.output_scanline;
                    JSAMPROW data = _image->data() + off;
                    JDIMENSION n = jpeg_read_scanlines(&_decomp, &data, 1);
                    if(n == 0)
                        return 0;
                }

                _state = OnFinish;
            }
            
            if(_state == OnFinish)
            {
                int r = jpeg_finish_decompress(&_decomp);
                if(r == 0)
                    return 0;

                _state = OnEnd;
            }

            return _state == OnEnd ? _image : 0;
        }

        static void onErrorExit(j_common_ptr cinfo)
        {
            throw Pt::IOError("invalid jpeg format");
        } 

        static void onEmitMessage(j_common_ptr, int)
        { } 

        static void onOutputMessage(j_common_ptr)
        { } 

        static void onInitSource(j_decompress_ptr)
        { }

        static boolean onFillBuffer(j_decompress_ptr decomp)
        {
            JpegReaderImpl* self = static_cast<JpegReaderImpl*>(decomp->client_data);
            
            if( decomp->src->bytes_in_buffer > 0 )
            {
                std::memmove(self->_buffer, 
                             decomp->src->next_input_byte,
                             decomp->src->bytes_in_buffer);

                decomp->src->next_input_byte = reinterpret_cast<JOCTET*>(self->_buffer);
            }
            
            return FALSE;
        }

        static void onSkipInput(j_decompress_ptr decomp, long n)
        {
            if(n > 0)
            {
                long avail = static_cast<long>(decomp->src->bytes_in_buffer);
                if(n > avail)
                    n = avail;
                
                decomp->src->next_input_byte += n;
                decomp->src->bytes_in_buffer -= n;
            }
        }

        static void onTermSource(j_decompress_ptr)
        { }
    
    private:
        enum State 
        {
            OnBegin = 0,
            OnHeader = 1,
            OnData = 2,
            OnFinish = 3,
            OnEnd = 4
        };
    
    private:
        std::ios*              _target;
        State                  _state;
        jpeg_error_mgr         _err;
        jpeg_source_mgr        _source;
        jpeg_decompress_struct _decomp;
        bool                   _init;
        Image*                 _image;
        static const int       _bufferSize = 2048;
        char                   _buffer[_bufferSize];
};


JpegReader::JpegReader()
: _impl( new JpegReaderImpl() )
{
}


JpegReader::JpegReader(std::istream& is, Image& image)
: _impl( new JpegReaderImpl(is, image) )
{
}


JpegReader::~JpegReader()
{
  delete _impl;
}


void JpegReader::attach(std::istream& is, Image& image)
{
    _impl->attach(is, image);
}


void JpegReader::detach()
{
    _impl->detach();
}


void JpegReader::reset()
{
    _impl->reset();
}


Image* JpegReader::advance()
{
    return _impl->advance();
}

} // namespace

} // namespace
