/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2015 Marc Boris Duerner
   
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
// ===== ImagePainter2::PathData Class Implementation ===================================
// ======================================================================================

struct ImagePainter2::PathData::Data {
    // ### TODO ###

    inline void clear()
    {
        // ### TODO ###
    }
};


ImagePainter2::PathData::PathData()
: _data(new Data())
{}

ImagePainter2::PathData::PathData(const PathData& pd)
: _data(new Data())
{ *_data = *pd._data; }

ImagePainter2::PathData::~PathData()
{ delete _data; }

void ImagePainter2::PathData::clear()
{ _data->clear(); }

const ImagePainter2::PathData& ImagePainter2::PathData::operator=(const ImagePainter2::PathData& pd)
{
    *_data = *pd._data;
    return *this;
}


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

void ImagePainter2::clearMatrixBuffer()
{ _affineMatrix2D.clear(); }

void ImagePainter2::pushMatrix()
{ _affineMatrix2D.push(); }

void ImagePainter2::popMatrix()
{ _affineMatrix2D.pop(); }

void ImagePainter2::getRawMatrix(float m[3][3]) const
{ _affineMatrix2D.getRaw(m); }

void ImagePainter2::updateMatrixUsingRaw(const float m[3][3], MatrixUpdateMode mode)
{ _affineMatrix2D.updateUsingRaw(m, mode); }

void ImagePainter2::loadIdentityMatrix()
{ _affineMatrix2D.identity(); }

void ImagePainter2::translate(float x, float y, MatrixUpdateMode mode)
{ _affineMatrix2D.translate(x, y, mode); }

void ImagePainter2::scaleAboutOrigin(float x, float y, MatrixUpdateMode mode)
{ _affineMatrix2D.scaleAboutOrigin(x, y, mode); }

void ImagePainter2::rotateAboutOrigin(float deg, MatrixUpdateMode mode)
{ _affineMatrix2D.rotateAboutOrigin(deg, mode); }

void ImagePainter2::shearXDirection(float deg, MatrixUpdateMode mode)
{ _affineMatrix2D.shearXDirection(deg, mode); }

void ImagePainter2::shearYDirection(float deg, MatrixUpdateMode mode)
{ _affineMatrix2D.shearYDirection(deg, mode); }

void ImagePainter2::reflectAboutOrigin(MatrixUpdateMode mode)
{ _affineMatrix2D.reflectAboutOrigin(mode); }

void ImagePainter2::reflectAboutXAxis(MatrixUpdateMode mode)
{ _affineMatrix2D.reflectAboutXAxis(mode); }

void ImagePainter2::reflectAboutYAxis(MatrixUpdateMode mode)
{ _affineMatrix2D.reflectAboutYAxis(mode); }

void ImagePainter2::beginPath()
{
    // ### TODO ###
}

void ImagePainter2::moveTo(float x, float y)
{
    // ### TODO ###
}

void ImagePainter2::lineTo(float x, float y)
{
    // ### TODO ###
}

void ImagePainter2::arcTo(float x, float y)
{
    // ### TODO ###
}

void ImagePainter2::quadraticBezierTo(float cx, float cy, float x, float y)
{
    // ### TODO ###
}

void ImagePainter2::endPath(bool autoClose)
{
    // ### TODO ###
}

void ImagePainter2::transformPath()
{
    // ### TODO ###
}

void ImagePainter2::pushPath()
{ _pathDataStack.push_back(_pathData); }

bool ImagePainter2::popPath()
{
    if(_pathDataStack.empty()) return false;

    _pathData = _pathDataStack.back();
    _pathDataStack.pop_back();

    return true;
}

void ImagePainter2::setPathData(const SmartPtr<ImagePainter2::BasicPathData>& pd)
{ _pathData = dynamic_cast<const PathData&>(*pd.get()); }

SmartPtr<Painter::BasicPathData> ImagePainter2::getPathData() const
{
    Painter::BasicPathData* pd = dynamic_cast<Painter::BasicPathData*>(
                                     new ImagePainter2::PathData(_pathData)
                                 );

    return SmartPtr<Painter::BasicPathData>(pd);
}

void ImagePainter2::clearPathDataBuffer()
{
    _pathData.clear();
    _pathDataStack.clear();
}


} // namespace
} // namespace
