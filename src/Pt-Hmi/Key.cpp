#include <Pt/Hmi/Key.h>

namespace Pt{
namespace Hmi{

Key::Key()
: _unicode(0)
, _alt(false)
, _shift(false)
, _ctrl(false)
{
}


Key::Key( Pt::Char ch, bool shift, bool ctrl, bool alt)
: _unicode(ch)
, _alt(alt)
, _shift(shift)
, _ctrl(ctrl)
{
}


Key::~Key()
{
}
  
}}