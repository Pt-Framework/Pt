#ifndef PT_GFX_EDGETABLE_H
#define PT_GFX_EDGETABLE_H
#include <algorithm>
#include <Edge.h>
#include <set>
#include <iostream>

namespace Pt{
namespace Gfx{

typedef std::multiset<Edge> EdgeSet;

class ActiveEdgeTable : public std::vector<Edge>
{
    public:
        ActiveEdgeTable()
        : std::vector<Edge>( 0 )
        { }

        inline void addEdge( const Edge& edge )
        {  push_back( edge ); }

        inline void updateEdges( size_t ypos )
        {
            size_t trueX;
            
            for( size_t i = 0; i < size(); i++ )
            {
               if( ypos >= (*this)[i].ymax )
               {//remove
                    erase( begin() + i );
                    --i;
               }
               else
               {//recalc the new x value
                    //(*this)[i].x += (*this)[i].rslope;
                    
                    Edge& edge = (*this)[i];
                    trueX = edge.x; 
                               
                    if( edge.dx > 0 && edge.dy > 0)
                    {
                        edge.xaccu += edge.dx;
                        
                        while( edge.xaccu > edge.dy )
                        {
                            trueX++;
                            edge.xaccu -= edge.dy;
                        }
                        edge.x = trueX;
                    }                    
                    else if( edge.dx > 0 && edge.dy < 0)
                    {
                        edge.xaccu += edge.dx;
                        while( edge.xaccu > -edge.dy )
                        {
                            trueX--;
                            edge.xaccu += edge.dy;
                        }
                        
                        if(  edge.xaccu == edge.dy )
                            edge.x = trueX - 1;
                        else
                            edge.x = trueX;
                            
                    }
                    else if( edge.dx < 0 && edge.dy > 0)
                    {
                        edge.xaccu -= edge.dx;
                        while( edge.xaccu > edge.dy )
                        {
                            trueX--;
                            edge.xaccu -= edge.dy;
                        }
                        edge.x = trueX;                    
                    }
                    else if( edge.dx < 0 && edge.dy < 0)
                    {
                        edge.xaccu -= edge.dx;
                        while( edge.xaccu > -edge.dy )
                        {
                            trueX++;
                            edge.xaccu += edge.dy;
                        }
                        if(  edge.xaccu == edge.dy )
                            edge.x = trueX - 1;
                        else
                            edge.x = trueX;      
                    }
               }
            }
        }

        inline void sort()
        { std::sort( begin(), end(), _lessXValue ); }

    private:
        struct LessXValue
        {
            inline bool operator()(const Edge& e1, const Edge& e2) const
            { return e1.x < e2.x; }
        };

        LessXValue _lessXValue;        
};


//typedef std::vector<Edge>      ActiveEdgeTable;

}
}

#endif
