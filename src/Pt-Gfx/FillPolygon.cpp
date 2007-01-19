#include "FillPolygon.h"
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

    setupGlobalEdgeTable( points );
    //6e-006

    EdgeSet::iterator   it       = _globalEdgeTable.begin();
    size_t              scanLine = it->ymin;

    addEdgeToActiveTable( it, scanLine );
    //1e-006

    do
    {
        output( image, scanLine );
        scanLine++;
        _clock.start();
        _activeEdgeTable.updateEdges( scanLine );
        Pt::System::TimeValue time = _clock.stop();
        std::cerr<<"Update Edges time : "<< (double) time.seconds() + time.microSeconds() / 1000000.0<<std:: endl;

        addEdgeToActiveTable( it, scanLine );

        _activeEdgeTable.sort();
    }
    while( !_activeEdgeTable.empty() );
}

void FillPolygon::setupGlobalEdgeTable( const std::vector<Math::Point>& points )
{
    Edge        edge;
    Pt::ssize_t dy;

    _globalEdgeTable.clear();

    for( size_t i = 1; i < points.size(); ++i )
    {
        dy = points[i].y() - points[i-1].y();

        if( dy == 0 )
            continue;

        edge.rslope = ( points[i].x() - points[i-1].x() ) / ( (double)(dy) );

        if( points[i-1].y() < points[i].y() )
        {
            edge.ymin = points[i-1].y();
            edge.ymax = points[i].y() ;
            edge.x    = points[i-1].x();
        }
        else
        {
            edge.ymin = points[i].y();
            edge.ymax = points[i-1].y() ;
            edge.x    = points[i].x();
        }

        _globalEdgeTable.insert( edge );
    }
}

}//namespace Pt
}//namespace Gfx


