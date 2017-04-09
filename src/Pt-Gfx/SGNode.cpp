/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2015 Marc Boris Duerner
 * Copyright (C) 2010 Aloysius Indrayanto
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/

#include <stdexcept>

#include <Pt/Gfx/SGNode.h>
#include <Pt/Gfx/ImagePainter2.h>


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== SGNode Class ===================================================================
// ======================================================================================

SGNode::~SGNode()
{}

void SGNode::clear()
{
    for(Children::iterator it = _children.begin(); it != _children.end(); ++it) {
        delete *it;
    }

    _pen   = Pen  ();
    _brush = Brush();

    _transform.identity();
    _children.clear();
}


// ======================================================================================
// ===== SGNodePath Class ===============================================================
// ======================================================================================

SGNodePath::~SGNodePath()
{ clear(); }

void SGNodePath::clear()
{
    _path.clear();

    SGNode::clear();
}

void SGNodePath::draw(ImagePainter2& painter, TransformStack& tstack, const Transform& transform)
{
    //
    Transform t = transform;

    //
    for(Children::iterator it = _children.begin(); it != _children.end(); ++it) {
        tstack.push(t);
        (*it)->draw(painter, tstack, t);
        t = tstack.pop();
    }

    //
    if(_path.isNull()) return;

    //
    std::vector<PointF> pointsF;
    _path.generatePoints(pointsF, 1);

    t.transformPoints(pointsF.data(), pointsF.size());

    painter.setPen(_pen);
    painter.setBrush(_brush);

    painter.fillPolygon(pointsF.data(), pointsF.size());
}


} // namespace
} // namespace
