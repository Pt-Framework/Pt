/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#include <Pt/Gfx/ImagePainter2.h>


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== PathData Class Implementation ==================================================
// ======================================================================================

struct ImagePainter2::PathData::Data {
    // ### TODO ###
};

ImagePainter2::PathData::PathData()
: _data(new Data())
{}

ImagePainter2::PathData::PathData(const PathData& pd)
: _data(new Data())
{ *_data = *pd._data; }

ImagePainter2::PathData::~PathData()
{ delete _data; }

const ImagePainter2::PathData& ImagePainter2::PathData::operator=(const ImagePainter2::PathData& pd)
{
    *_data = *pd._data;
    return *this;
}


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

void ImagePainter2::clearMatrixBuffer()
{
}

void ImagePainter2::pushMatrix()
{
}

void ImagePainter2::popMatrix()
{
}

void ImagePainter2::getRawMatrix(float m[3][3]) const
{
}

void ImagePainter2::updateMatrixUsingRaw(const float m[3][3], MatrixUpdateMode mode)
{
}

void ImagePainter2::loadIdentityMatrix()
{
}

void ImagePainter2::translate(float x, float y, MatrixUpdateMode mode)
{
}

void ImagePainter2::scaleAboutOrigin(float x, float y, MatrixUpdateMode mode)
{
}

void ImagePainter2::rotateAboutOrigin(float deg, MatrixUpdateMode mode)
{
}

void ImagePainter2::shearXDirection(float deg, MatrixUpdateMode mode)
{
}

void ImagePainter2::shearYDirection(float deg, MatrixUpdateMode mode)
{
}

void ImagePainter2::reflectAboutOrigin(MatrixUpdateMode mode)
{
}

void ImagePainter2::reflectAboutXAxis(MatrixUpdateMode mode)
{
}

void ImagePainter2::reflectAboutYAxis(MatrixUpdateMode mode)
{
}

void ImagePainter2::beginPath()
{
}

void ImagePainter2::moveTo(float x, float y)
{
}

void ImagePainter2::lineTo(float x, float y)
{
}

void ImagePainter2::arcTo(float x, float y)
{
}

void ImagePainter2::quadraticBezierTo(float cx, float cy, float x, float y)
{
}

void ImagePainter2::endPath(bool autoClose)
{
}

void ImagePainter2::transformPath()
{
}

void ImagePainter2::pushPath()
{
}

void ImagePainter2::popPath()
{
}

void ImagePainter2::setPathData(const BasicPathData& pd)
{ _pathData = dynamic_cast<const PathData&>(pd); }

const ImagePainter2::BasicPathData ImagePainter2::getPathData() const
{ return dynamic_cast<const BasicPathData&>(_pathData); }

void ImagePainter2::clearPathDataBuffer()
{
}


} // namespace
} // namespace
