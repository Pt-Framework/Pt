/*
 * Copyright (C) 2005-2011 Marc Boris Duerner
 * Copyright (C) 2011      Aloysius Indayanto
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef Pt_System_StreamBuffer2_h
#define Pt_System_StreamBuffer2_h

#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <streambuf>
#include <ios>
#include <cstring>

namespace Pt {

namespace System {
    class StreamBuffer2 : public std::streambuf
                        , public Connectable
    {
        private:
            friend class StreamBufferImpl;

            class StreamBufferImpl {
                public:
                    StreamBufferImpl(IODevice& ioDevice, size_t bufferSize, bool extend);

                private:
                    IODevice*    _ioDevice;
                    size_t       _ibufferSize;
                    char*        _ibuffer;
                    std::size_t  _obufferSize;
                    char*        _obuffer;
                    const size_t _pbmax;
                    bool         _oextend;
            };

        public:
            explicit StreamBuffer2(IODevice& ioDevice, size_t bufferSize = 8192, bool extend = false)
            : _impl(ioDevice, bufferSize, extend)
            {
                //StreamBufferInit(*this, bufferSize, extend);
                //StreamBufferAttach(*this, ioDevice);
            }

        private:
            StreamBufferImpl _impl;
    };

} // namespace System

} // namespace Pt

#endif
