#include "FillPolygon.h"


namespace Pt{
namespace Gfx{


FillPolygon::FillPolygon()
: _colorBuffer( 10000 ) 
{
}

void FillPolygon::draw( ARgbImage& image, const Brush& brush, std::vector<Math::Point>& points )
{
  
    if( points.end() != points.begin() )
        points.push_back( points[0] );
        
    setupGlobalEdgeTable( points );
    
    EdgeSet::iterator it = _edgeSet.begin();
    
    _currentPos = it;
    addEdgeToEAT( it->ymin);
    
    size_t scanLine = it->ymin;
   
    do
    {        
        for( size_t i = 1; i < _activeEdgeTable.size(); i += 2 )
        {
            memcpy( &image.pixel( _activeEdgeTable[i-1].x, scanLine ), &_colorBuffer[0], (_activeEdgeTable[i].x - _activeEdgeTable[i-1].x)* sizeof( ARgbColor ));

/*            for( size_t xpos = _activeEdgeTable[i-1].x; xpos < _activeEdgeTable[i].x; ++xpos)
                image.pixel( xpos, scanLine ) =  ARgbColor(0,0,0); */
        }
            
        
        scanLine++;   
        removeEdgeAET( scanLine );          
        recalcAETxValue();        
        addEdgeToEAT( scanLine );
        sortEAT();
        
        
    } 
    while( !_activeEdgeTable.empty() );   
}

void FillPolygon::setupGlobalEdgeTable( const std::vector<Math::Point>& points )
{
    Edge        edge;
    Pt::ssize_t dy;
    
    _edgeSet.clear();
    
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

        _edgeSet.insert( edge );    
                                            
    }   
}

}//namespace Pt
}//namespace Gfx


