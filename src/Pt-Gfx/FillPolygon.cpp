#include "FillPolygon.h"
#include "Pt/Math/Rect.h"
#include <iostream>


namespace Pt{
namespace Gfx{


FillPolygon::FillPolygon()
: _colorBuffer( 3000 )
{ }

void FillPolygon::draw( ARgbImage& image, const Brush& brush, std::vector<Math::Point>& points )
{
    if( points.end() != points.begin() )
        points.push_back( points[0] );

    
     _clipper(points, Pt::Math::Rect( Pt::Math::Point(0,0), Pt::Math::Size( image.width() - 1, image.height() - 1 )) );
        
    if( points.empty())
        return;        
        
    setupGlobalEdgeTable( points );

    EdgeSet::iterator   it       = _globalEdgeTable.begin();
    size_t              scanLine = it->ymin;

    addEdgeToActiveTable( it, scanLine );

    do
    {
        output( image, scanLine );
        scanLine++;
        _activeEdgeTable.updateEdges( scanLine );
        addEdgeToActiveTable( it, scanLine );
        _activeEdgeTable.sort();
    }
    while( !_activeEdgeTable.empty() );
}

void FillPolygon::setupGlobalEdgeTable( const std::vector<Math::Point>& points )
{
    Edge        edge;

    _globalEdgeTable.clear();

    for( size_t i = 1; i < points.size(); ++i )
    {
        edge.dy = points[i].y() - points[i-1].y();
        
        if( edge.dy == 0 )
            continue;

        edge.dx     = points[i].x() - points[i-1].x();
        edge.xaccu  = edge.dx;
        
        if( points[i-1].y() < points[i].y() )
        {
            edge.ymin = points[i-1].y();                            
            edge.ymax = points[i].y() ;
            edge.x    = points[i-1].x();            
        }
        else
        {
            edge.ymin = points[i].y();                
            edge.ymin = points[i].y();
            edge.ymax = points[i-1].y() ;
            edge.x    = points[i].x();
        }

        _globalEdgeTable.insert( edge );
    }
}

}//namespace Pt
}//namespace Gfx


