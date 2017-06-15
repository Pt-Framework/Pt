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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#include <Pt/Gfx/Path.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Math.h>
#include <Pt/SourceInfo.h>


namespace Pt {
namespace Gfx {

Path::Path()
: _curX(0)
, _curY(0)
{
}



Path::~Path()
{ 

}


bool Path::isEmpty() const
{
   return _elements.empty();
}


void Path::clear()
{
  return _elements.clear();    
}


void Path::addPath(const Path& p)
{
  closeSubpath();

  insertPath(p);
}


void Path::insertPath(const Path& p)
{
  ElementVector::const_iterator it = p._elements.begin();

  for( ; it != p._elements.end(); ++it)
    _elements.push_back( *it);
}


void Path::closeSubpath()
{
  Element elem(Element::IT_Close);
  _elements.push_back(elem);
}


RectF Path::boundingRect() const
{//TODO: 
  RectF result;
  assert(false);
  return result;
}


const PointF& Path::currentPosition() const
{
  return PointF(_curX,_curY);
}


void Path::moveTo(const PointF& p)
{
    Element elem(Element::IT_MoveTo, p.x() , p.y());

    _elements.push_back(elem);

    _curX = p.x();
    _curY = p.y();
}


void Path::lineTo(const PointF& p)
{
    Element elem(Element::IT_LineTo, p.x() , p.y());

    _elements.push_back(elem);

    _curX = p.x();
    _curY = p.y();
}


void Path::arcTo(const PointF& p, double r)
{
    decomposeArcTo(_curX, _curY, p.x(), p.y(), r);

    _curX = p.x();
    _curY = p.y();
}


void Path::decomposeArcTo(double x1, double y1, double x2, double y2, double r)
{
    // Based on How to create circle with Bézier curves?
    //          http://stackoverflow.com/questions/1734745/how-to-create-circle-with-bézier-curves
    //          Answer by Kpym, 2015 (permalink: http://stackoverflow.com/a/27863181)

    // Line equation : 0 = aX + By + c
    // Normal        : n = ai + bj
    const double a = y2 - y1;
    const double b = x1 - x2;
  //const double c = -(x1 * y2 - x2 * y1);

    // Middle point
    const double xm = (x1 + x2) * 0.5;
    const double ym = (y1 + y2) * 0.5;

    // Radius
    const double ab = Gfx::Math::fastSqrt(a * a + b * b);
    const double rx = ab * 0.5f;
    const double ry = r;

    // Normal vector
    const double iz = -1.0 / ab;
    const double nx = a * iz;
    const double ny = b * iz;

    // Circumference vectors
    const double nxrx = nx * rx;
    const double nxry = nx * ry;
    const double nyrx = ny * rx;
    const double nyry = ny * ry;

    // Optimal distance to the control points for circle approximation
    // using N segments of cubic bezier:
    //    dist = (4 / 3) * tan(pi / 2 / N)
    // If N = 4, then:
    //    dist = (4 / 3) * tan(pi / 2 / 4) = 0.0822479912358
    const double od = 0.552284749831;

    // Curve #1
    const double c1x1 = x1;
    const double c1y1 = y1;
    const double c1x4 = xm   + nxrx;
    const double c1y4 = ym   + nyry;
    const double c1x2 = c1x1 + nxrx * od;
    const double c1y2 = c1y1 + nyry * od;
    const double c1x3 = c1x4 - nyrx * od;
    const double c1y3 = c1y4 - nxry * od;
    Element elem1(Element::IT_CubicBezierTo, c1x2, c1y2, c1x3, c1y3, c1x4, c1y4);

    _elements.push_back(elem1);

    // Curve #2
    const double c2x1 = xm   + nxrx;
    const double c2y1 = ym   + nyry;
    const double c2x4 = x2;
    const double c2y4 = y2;
    const double c2x2 = c2x1 + nyrx * od;
    const double c2y2 = c2y1 - nxry * od;
    const double c2x3 = c2x4 - nxrx * od;
    const double c2y3 = c2y4 + nyry * od;

    Element elem2(Element::IT_CubicBezierTo, c2x2, c2y2, c2x3, c2y3, c2x4, c2y4);

     _elements.push_back(elem2);
}


void Path::quadraticBezierTo(const PointF &c, const PointF& to)
{
    Element elem(Element::IT_QuadBezierTo, c.x() , c.y(), to.x(), to.y());

    _elements.push_back(elem);

    _curX = to.x();
    _curY = to.y();
}


void Path::cubicBezierTo(const PointF &c1, const PointF &c2, const PointF& to)
{   
    Element elem(Element::IT_CubicBezierTo, c1.x() , c1.y(), c2.x(), c2.y(), to.x(), to.y());

    _elements.push_back(elem);

    _curX = to.x();
    _curY = to.y();
}


void Path::bezierTo(const PointF* cxy, size_t controlPointCount, const PointF& to)
{    
    std::vector<double> points;

    for(size_t i = 0; i < controlPointCount; ++i)
    {
        points.push_back(cxy[i].x());
           points.push_back(cxy[i].y());
    }
        
    points.push_back(to.x());
    points.push_back(to.y());


    Element elem(Element::IT_GenNBezierTo, points);

    _elements.push_back(elem);

    _curX = to.x();
    _curY = to.y();
}

void Path::addRect(const SizeF& size)
{    
    const double x = _curX;
    const double y = _curY;

    lineTo(Pt::Gfx::PointF(x, y+ size.height()));
    lineTo(Pt::Gfx::PointF(x + size.width(), y+ size.height()));
    lineTo(Pt::Gfx::PointF(x + size.width(), y));
    lineTo(Pt::Gfx::PointF(x, y));   
}


void Path::addRoundRect(const SizeF& size, float radius)
{
    const double x = _curX;
    const double y = _curY;

    moveTo(Pt::Gfx::PointF( x, y +  radius));
    quadraticBezierTo(Pt::Gfx::PointF( x, y), Pt::Gfx::PointF(x + radius, y));     


    lineTo(Pt::Gfx::PointF(x +  size.width() - radius, y));
    quadraticBezierTo(Pt::Gfx::PointF(x + size.width(), y), Pt::Gfx::PointF(x + size.width(), y + radius));

    lineTo(Pt::Gfx::PointF(x +  size.width(), y + size.height() - radius));
    quadraticBezierTo(Pt::Gfx::PointF(x + size.width(), y+ size.height() ), Pt::Gfx::PointF(x + size.width() - radius, y + size.height()));

    lineTo(Pt::Gfx::PointF(x +  radius, y + size.height()));
    quadraticBezierTo(Pt::Gfx::PointF(x, y + size.height()), Pt::Gfx::PointF(x, y + size.height() - radius));

    lineTo(Pt::Gfx::PointF(x, y + radius));
}
 
                
void Path::addPie(const SizeF& size, float degBegin, float degEnd)
{
  //Todo: decompose in 4 quadrants and use Trigonometry sin/ cos to calculate the point coordinates
   
}


void Path::addChord(const SizeF& size,  float degBegin, float degEnd)
{
//Todo: decompose in 4 quadrants and use Trigonometry sin/ cos to calculate the point coordinates
}


void Path::addEllipse(const SizeF& size)
{
  const Pt::Gfx::PointF p1(_curX, _curY+  size.height() / 2);
  const Pt::Gfx::PointF p2(_curX + size.width(), _curY + size.height() / 2);

  moveTo(p1);
  arcTo( p2, size.height()/2 );   

  moveTo(p2);
  arcTo( p1, size.height()/2 );   
}


void Path::generatePoints(std::vector<PointF>& dst, float smoothness) const
{
    // For convenience
    typedef ElementVector::const_iterator PDIIterator;

    // State variables
    double curX = 0.0;
    double curY = 0.0;

    // Walk through the instructions
    for(PDIIterator it = _elements.begin(); it != _elements.end(); ++it) 
    {
        // Get the instruction
        const Element& ins = *it;

        // Act based on the type of the instruction
        switch(ins.type) 
        {
            case Element::IT_Close:
                if(!dst.empty()) 
                   dst.push_back(Painter::PolygonSeparatorPointF);
            break;

            case Element::IT_MoveTo:
                curX = ins.pxy[0];
                curY = ins.pxy[1];
                break;

            case Element::IT_LineTo:
                if(dst.empty()) dst.push_back( PointF(curX, curY) );
                curX = ins.pxy[0];
                curY = ins.pxy[1];
                dst.push_back( PointF(curX, curY) );
                break;

            case Element::IT_QuadBezierTo:
                generateQuadraticBezierPoints(dst, curX, curY, ins.pxy[0], ins.pxy[1], ins.pxy[2], ins.pxy[3], smoothness);
                curX = ins.pxy[2];
                curY = ins.pxy[3];
                break;

            case Element::IT_CubicBezierTo:
                generateCubicBezierPoints(dst, curX, curY, ins.pxy[0], ins.pxy[1], ins.pxy[2], ins.pxy[3], ins.pxy[4], ins.pxy[5], smoothness);
                curX = ins.pxy[4];
                curY = ins.pxy[5];
                break;

            case Element::IT_GenNBezierTo:
                generateGenericNBezierPoints(dst, curX, curY, ins.pxy, smoothness);
                curX = ins.pxy[ins.pxy.size() - 2];
                curY = ins.pxy[ins.pxy.size() - 1];
                break;

            default:
                break;
        }
    }

    // Remove dangling separator point as needed
    if(!dst.empty() && dst.back().x() > Painter::MaximumCoordinateF && dst.back().y() > Painter::MaximumCoordinateF)
        dst.pop_back();
}


void Path::transform(const Transform& transform)
{
   // For convenience
    typedef ElementVector::iterator PDIIterator;

    // Walk through the instructions
    for(PDIIterator it = _elements.begin(); it != _elements.end(); ++it) 
    {
        // Get the instruction
        Element& elem = *it;

        // Act based on the type of the instruction
        switch(elem.type) 
        {
            case Element::IT_Close:
            break;

            case Element::IT_MoveTo:
            case Element::IT_LineTo:            
            {
                Pt::Gfx::PointF p( elem.pxy[0], elem.pxy[1] );
                p = transform * p;

                elem.pxy[0] = p.x();
                elem.pxy[1] = p.y();
            }
             break;

            case Element::IT_QuadBezierTo:
            {
                Pt::Gfx::PointF p1( elem.pxy[0], elem.pxy[1] );
                p1 = transform * p1;

                elem.pxy[0] = p1.x();
                elem.pxy[1] = p1.y();

                Pt::Gfx::PointF p2( elem.pxy[2], elem.pxy[3] );
                p2 = transform * p2;

                elem.pxy[2] = p2.x();
                elem.pxy[3] = p2.y();
            }
            break;

            case Element::IT_CubicBezierTo:
            {
                Pt::Gfx::PointF p1( elem.pxy[0], elem.pxy[1] );
                p1 = transform * p1;

                elem.pxy[0] = p1.x();
                elem.pxy[1] = p1.y();

                Pt::Gfx::PointF p2( elem.pxy[2], elem.pxy[3] );
                p2 = transform * p2;

                elem.pxy[2] = p2.x();
                elem.pxy[3] = p2.y();



                Pt::Gfx::PointF p3( elem.pxy[4], elem.pxy[5] );
                p3 = transform * p3;

                elem.pxy[4] = p3.x();
                elem.pxy[5] = p3.y();
              }
              break;

            case Element::IT_GenNBezierTo:
             break;

            default:
                break;
        }
      }
}

} // namespace

} // namespace
