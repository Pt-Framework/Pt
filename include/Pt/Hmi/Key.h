#ifndef PT_HMI_KEY_H
#define PT_HMI_KEY_H

#include <Pt/Hmi/Api.h>
#include <Pt/String.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API Key
{
  public:
    Key();

    Key( Pt::Char ch, bool shift = false, bool ctrl = false, bool alt = false);

    virtual ~Key();
  
   Pt::Char unicode() const
   {
    return _unicode;
   }

   void setUnicode( Pt::Char ch )
   {
      _unicode= ch; 
   }

   bool alt() const
   {
      return _alt;
   }

   void setAlt( bool alt )
   {
      _alt = alt;
   }

   bool shift() const
   {
      return _shift;
   }

   void setShift( bool b )
   {
      _shift = b;
   }

   bool ctrl() const
   {
      return _ctrl;
   }

   void setCtrl( bool ctrl )
   {
      _ctrl = ctrl;  
   }


   bool operator==(const Key& k ) const
   {
      if(_alt != k._alt )
        return false;
        
       if( _ctrl != k._ctrl )
        return false;

       if( _shift != k._shift )
        return false;

        if( _unicode != k._unicode)
          return false;

        return true;
   }

   bool operator!=(const Key& k ) const
   {
      return !( *this == k );
   }

    bool operator<(const Key& k ) const
    {
      return toString() < k.toString();
    }


   bool empty() const
   {
      return _unicode == 0;
   }

   void clear()
   {
      _unicode = 0;
	    _alt = false;
	    _shift = false;
	    _ctrl = false;
   }


   const Pt::String toString()  const
    {
      Pt::String str;
        str += _alt ? "A+" : "";
        str += _shift ? "S+" : "";
        str += _ctrl ? "C+" : "";
        str += _unicode;
        return str;

    }
  private:
    Pt::Char _unicode;
	  bool _alt;
	  bool _shift;
	  bool _ctrl;
};

}}
#endif