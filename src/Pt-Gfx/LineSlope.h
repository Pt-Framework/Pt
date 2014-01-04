#ifndef PT_GFX_LINESLOPE_H
#define PT_GFX_LINESLOPE_H

namespace Pt{
namespace Gfx{


class LineSlope
{
public:
    LineSlope()
    { }
    
    ~LineSlope()
    { }
    
    inline void setDX( int dx )
    { _dx = dx; }
    
    inline int dx() const 
    { return _dx;}
    
    inline void setDY( int dy )
    { _dy = dy; }
    
    inline int dy() const 
    { return _dy; }
    
    inline void setK( double k )
    { _k = k; }
    
    inline double k() const
    { return _k; }
    
private:
    int _dx, _dy; /* dy/dx is (rational) slope */
    double _k;	  /* x0 * dy - y0 * dx */
};

}//namespace Gfx
}//namespace Pt 

#endif
