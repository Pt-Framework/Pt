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
// ===== Internal Helper Functions - Drawing Functions ==================================
// ======================================================================================

// ### TODO ###


// ======================================================================================
// ===== ImagePainter2::PathData Class Implementation ===================================
// ======================================================================================

class ImagePainter2::PathData::Data {
    public:
        // Instruction type
        enum InsType {
            IT_Begin, IT_End,
            IT_MoveTo, IT_LineTo, IT_ArcTo, IT_QuadBezierTo
        };

        // Instruction structure
        struct Instruction {
            InsType type;
            float   x1, y1;
            float   x2, y2;

            inline Instruction(InsType type_)
            : type(type_)
            {}

            inline Instruction(InsType type_, float x1_, float y1_)
            : type(type_), x1(x1_), y1(y1_)
            {}

            inline Instruction(InsType type_, float x1_, float y1_, float x2_, float y2_)
            : type(type_), x1(x1_), y1(y1_), x2(x2_), y2(y2_)
            {}
        };

        // Instruction list
        typedef std::vector<Instruction> Instructions;

    public:
        inline Data()
        {}

        inline ~Data()
        { clear(); }

        inline void clear()
        {
            _instructions.clear();
            _transformedPoints.clear();
        }

        inline void setAutoClose(bool autoClose)
        { _autoClose = _autoClose; }

        inline bool autoClose() const
        { return _autoClose; }

        inline void add(InsType type)
        { _instructions.push_back(Instruction(type)); }

        inline void add(InsType type, float x1, float y1)
        { _instructions.push_back(Instruction(type, x1, y1)); }

        inline void add(InsType type, float x1, float y1, float x2, float y2)
        { _instructions.push_back(Instruction(type, x1, y1, x2, y2)); }

        inline std::vector<Instruction>& instructions()
        { return _instructions; }

        inline const std::vector<Instruction>& instructions() const
        { return _instructions; }

        inline std::vector<PointF>& transformedPoints()
        { return _transformedPoints; }

        inline const std::vector<PointF>& transformedPoints() const
        { return _transformedPoints; }

    private:
        bool                _autoClose;
        Instructions        _instructions;

        std::vector<PointF> _transformedPoints;
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
    _pathData->clear();
    _pathData->data().add(PathData::Data::IT_Begin);
}

void ImagePainter2::moveTo(float x, float y)
{ _pathData->data().add(PathData::Data::IT_MoveTo, x, y); }

void ImagePainter2::lineTo(float x, float y)
{ _pathData->data().add(PathData::Data::IT_LineTo, x, y); }

void ImagePainter2::arcTo(float x, float y)
{ _pathData->data().add(PathData::Data::IT_ArcTo, x, y); }

void ImagePainter2::quadraticBezierTo(float cx, float cy, float x, float y)
{ _pathData->data().add(PathData::Data::IT_QuadBezierTo, cx, cy, x, y); }

void ImagePainter2::endPath(bool autoClose)
{
    _pathData->data().add(PathData::Data::IT_End);
    _pathData->data().setAutoClose(autoClose);
}

void ImagePainter2::transformPath()
{
    const std::vector<PathData::Data::Instruction>& insts   = _pathData->data().instructions     ();
          std::vector<PointF>&                      pointsF = _pathData->data().transformedPoints();

    // ### TODO ###
}

void ImagePainter2::pushPath()
{
    PathData* pathData = new PathData(*_pathData);

    _pathDataStack.push_back(pathData);
}

bool ImagePainter2::popPath()
{
    if(_pathDataStack.empty()) return false;

    delete _pathData;
    _pathData = _pathDataStack.back();

    _pathDataStack.pop_back();

    return true;
}

void ImagePainter2::setPathData(AutoPtr<Painter::BasicPathData> pd)
{
    BasicPathData* basicPathData = pd.get();
    PathData*      pathData      = dynamic_cast<PathData*>(basicPathData);

    if(!pathData) return;

    delete _pathData;
    _pathData = pathData;

    pd.release();
}

AutoPtr<Painter::BasicPathData> ImagePainter2::getPathData() const
{
    PathData*      pathData      = new PathData(*_pathData);
    BasicPathData* basicPathData = dynamic_cast<BasicPathData*>(pathData);

    return AutoPtr<BasicPathData>(basicPathData);
}

void ImagePainter2::clearPathDataBuffer()
{
    for(size_t i = 0; i < _pathDataStack.size(); ++i) delete _pathDataStack[i];
    _pathDataStack.clear();

    _pathData->clear();
}

void ImagePainter2::strokePath()
{
    const std::vector<PointF>& pointsF = _pathData->data().transformedPoints();

    if(pointsF.empty()) {
        generatePointsFromPath();
        if(pointsF.empty()) return;
    }

    drawPolyline(pointsF.data(), pointsF.size(), _pathData->data().autoClose());
}

void ImagePainter2::fillPath()
{
    const std::vector<PointF>& pointsF = _pathData->data().transformedPoints();

    if(pointsF.empty()) {
        generatePointsFromPath();
        if(pointsF.empty()) return;
    }

    fillPolygon(pointsF.data(), pointsF.size());
}

// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

void ImagePainter2::generatePointsFromPath()
{
    const std::vector<PathData::Data::Instruction>& insts   = _pathData->data().instructions     ();
          std::vector<PointF>&                      pointsF = _pathData->data().transformedPoints();

    // ### TODO ###
}


} // namespace
} // namespace
