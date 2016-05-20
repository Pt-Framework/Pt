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
#include <ios>
#include <cstdlib>

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

        Image* advance()
        {
            if( ! _init )
            {
                _err.error_exit     = &JpegReaderImpl::onJpegError;
                //_err.emit_message   = ...;
                //_err.output_message = ...;
	              _decomp.err = jpeg_std_error(&_err);

                jpeg_create_decompress(&_decomp);

                //_dcomp.src = ...;
                _init = true;
            }

            if(_state == OnBegin)
            {
                int r = jpeg_read_header(&_decomp, 1);
                if(r == JPEG_SUSPENDED)
                    return 0;

                _state = OnHeader;
            }

            if(_state == OnHeader)
            {
                int r = jpeg_start_decompress(&_decomp);
                if(r == 0)
                    return 0;

                _state = OnData;
            }
            
            if(_state == OnData)
            {
                for(;;)
                {
                    JSAMPARRAY data = 0;
                    JDIMENSION dataLines = 1;
                    JDIMENSION n = jpeg_read_scanlines(&_decomp, data, dataLines);
                    if(n == 0)
                        return 0;
                }

                if(_decomp.output_scanline < _decomp.output_height)
                    return 0;

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

        static void onJpegError(j_common_ptr cinfo)
        {
            std::cerr << "jpeg error: " << cinfo->err->msg_code << std::endl;
            throw Pt::IOError("invalid jpeg format");
        } 
    
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
        jpeg_decompress_struct _decomp;
        bool                   _init;
        Image*                 _image;
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
