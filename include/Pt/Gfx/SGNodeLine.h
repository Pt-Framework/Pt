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

#ifndef PT_GFX_SGNODELINE_H
#define PT_GFX_SGNODELINE_H

#include <Pt/Gfx/SGNode.h>


namespace Pt{
namespace Gfx{


/** @brief A scene-graph node class that specifies a line.
  */
class PT_GFX_API SGNodeLine : public SGNode {
    public:
        inline SGNodeLine(RenderMode rm = RenderInherit)
        : SGNode( rm )
        {}

        inline SGNodeLine(RenderMode rm, const PointT& from, const PointT& to)
        : SGNode( rm )
        , _from ( from )
        , _to   ( to )
        {}

        inline SGNodeLine(RenderMode rm, const PointT& from, const PointT& to, const TransformT& transform)
        : SGNode( rm, transform )
        , _from ( from )
        , _to   ( to )
        {}

        inline SGNodeLine(RenderMode rm, const PointT& from, const PointT& to, const TransformT& transform, const Children& children)
        : SGNode( rm, transform, children )
        , _from ( from )
        , _to   ( to )
        {}

        virtual ~SGNodeLine();

        //
        // Management functions
        //

        virtual void clear();

        //
        // Direct access to the line object
        //

        inline void set(const PointT& from, const PointT& to)
        {
            _from = from;
            _to   = to;
        }

        inline const PointT& from() const
        { return _from; }

        inline const PointT& to() const
        { return _to; }

    protected:
        virtual void drawImpl(ImagePainter2& painter, const TransformT& transform) const;

    private:
        PointT _from;
        PointT _to;
};


} // namespace
} // namespace

#endif
