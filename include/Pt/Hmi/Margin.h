#ifndef Pt_Hmi_Margine_H
#define Pt_Hmi_Margine_H

#include <Pt/Hmi/Api.h>

namespace Pt{
namespace Hmi{

class Margin
{
  public:
    Margin(const double& all)
    {
      setAll(all);
    }

    Margin(const double& left, const double& top, const double& right, const double& bottom)
    {
      assign( left, top, right, bottom );
    }

    Margin()    
    {
      assign( 0, 0, 0, 0);
    }

    void assign( const double& left, const double& top, const double& right, const double& bottom )
    {
      _left = left;
      _top = top;
      _right = right;
      _bottom = bottom;
    }

    const double& left() const
    {
      return _left;
    }
    
    void setLeft(const double& left)
    {
      _left = left;
    }

    const double& top() const
    {
      return _top;
    }
    
    void setTop(const double& top)
    {
      _top = top;
    } 

    const double& right() const
    {
      return _right;
    }
    
    void setRight(const double& right)
    {
      _right = right;
    } 

    const double& bottom() const
    {
      return _bottom;
    }
    
    void setBottom(const double& bottom)
    {
      _bottom = bottom;
    } 

    void setAll(const double& value)
    {
      assign( value, value, value, value );
    }

  private:
    double _left;
    double _top;
    double _right;
    double _bottom;        
};

}}

#endif
