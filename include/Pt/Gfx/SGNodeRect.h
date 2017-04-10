/* Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2017-2017 Aloysius Indrayanto

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

#ifndef PT_GFX_SGNODERECT_H
#define PT_GFX_SGNODERECT_H

#include <Pt/Gfx/SGNodePath.h>


namespace Pt{
namespace Gfx{


/** @brief A scene-graph node class that specifies a rectangle or a rounded rectangle.
  */
class PT_GFX_API SGNodeRect : public SGNodePath {
    public:
        inline SGNodeRect(RenderMode rm = RenderInherit)
        : SGNodePath( rm )
        {}

        inline SGNodeRect(RenderMode rm, const RectT& rect, float radius = 0.0f)
        : SGNodePath( rm )
        { set(rect, radius); }

        inline SGNodeRect(RenderMode rm, const RectT& rect, float radius, const TransformT& transform)
        : SGNodePath( rm, Path(), transform )
        { set(rect, radius); }

        inline SGNodeRect(RenderMode rm, const RectT& rect, float radius, const TransformT& transform, const Children& children)
        : SGNodePath( rm, Path(), transform, children )
        { set(rect, radius); }

        virtual ~SGNodeRect();

        //
        // Management functions
        //

        virtual void clear();

        //
        // Direct access to the rectangle object
        //

        void set(const RectT& rect, float radius = 0.0f);

        inline const RectT& rect() const
        { return _rect; }

        inline float radius() const
        { return _radius; }


    protected:
        RectT _rect;
        float _radius;
};


} // namespace
} // namespace

#endif
