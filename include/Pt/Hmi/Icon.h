/* Copyright (C) 2019 Marc Boris Duerner
 
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

#ifndef Pt_Hmi_Icon_h
#define Pt_Hmi_Icon_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Image.h>
#include <Pt/System/Path.h>

#include <vector>

namespace Pt {

namespace Hmi {

class IconImpl;

class PT_HMI_API IconProvider
{
    friend class IconImpl;

    public:
        IconProvider();

        virtual ~IconProvider();

        virtual bool empty() const = 0;

        virtual void clear() = 0;

        virtual void addImage(const Gfx::SizeF& size, const Gfx::Image& image) = 0;

        virtual void addImage(const Gfx::SizeF& size, const System::Path& path) = 0;

        virtual Gfx::SizeF minimumSize() const = 0;
        
        virtual Gfx::SizeF maximumSize() const = 0;

        virtual const Gfx::Image& getImage(const Gfx::SizeF& area) = 0;

    private:
        void attachIcon(IconImpl* parent);

        void detachIcon(IconImpl* parent);

    private:
        std::vector<IconImpl*> _icons;
};


class PT_HMI_API Icon
{
    public:
        Icon();

        Icon(const Icon& icon);

        Icon(IconProvider& provider);

        ~Icon();

        Icon& operator=(const Icon& icon);

        bool empty() const;

        void clear();

        void addImage(const Gfx::Image& image);

        void addImage(const Gfx::SizeF& size, const Gfx::Image& image);

        void addImage(const Gfx::SizeF& size, const System::Path& path);

        void addImage(double width, double height, const System::Path& path);

        const Gfx::Image& getImage(const Gfx::SizeF& area) const;

        Gfx::SizeF minimumSize() const;

        Gfx::SizeF maximumSize() const;

    private:
        void detach();

    private:
        mutable class IconImpl* _impl;
};

} // namespace

} // namespace

#endif
