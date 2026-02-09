/* Copyright (C) 2015 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef PT_GFX_BASIC_VIEW_H
#define PT_GFX_BASIC_VIEW_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ViewBase.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

template <typename FormatT>
class BasicImage;

template <typename FormatT>
class BasicConstImage;


template <typename FormatT>
class BasicView : public ViewBase
{
    public:
        typedef FormatT Format;

    public:
        explicit BasicView(const Format& format);

        explicit BasicView(BasicImage<FormatT>& image);

        template <typename OtherT>
        explicit BasicView(BasicImage<OtherT>& image);

        virtual ~BasicView()
        { }

        void reset();

        void reset(BasicImage<FormatT>& image);

        Pt::uint8_t* data()
        { return _data; }

        const Pt::uint8_t* data() const
        { return _data; }

        const Format& format() const
        { return *_format; }

    protected:
        using ViewBase::reset;

    private:
        Pt::uint8_t*   _data;
        const Format*  _format;
};


template <typename FormatT>
class BasicConstView : public ViewBase
{
    public:
        typedef FormatT Format;

    public:
        explicit BasicConstView(const Format& format);

        explicit BasicConstView(const BasicImage<FormatT>& image);

        explicit BasicConstView(const BasicConstImage<FormatT>& image);

        virtual ~BasicConstView()
        { }

        void reset()
        {
            _data = 0;
            ViewBase::reset(0, 0, 0, 0);
        }

        void reset(const BasicImage<FormatT>& image);

        void reset(const BasicConstImage<FormatT>& image);

        const Pt::uint8_t* data() const
        { return _data; }

        const Format& format() const
        { return *_format; }
    
    protected:
        using ViewBase::reset;

    private:
        const Pt::uint8_t*  _data;
        const Format*       _format;
};

} // namespace

} // namespace

#include <Pt/Gfx/BasicView.hpp>

#endif
