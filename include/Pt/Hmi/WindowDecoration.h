#ifndef Pt_Hmi_WindowDecoration_h
#define Pt_Hmi_WindowDecoration_h


namespace Pt{
namespace Hmi{

namespace WindowDecoration
{
 enum Flags
 {
    ShowBorder = 1,
    ShowTitleBar = 2,
    ShowMinimizeButton = 4,
    ShowMaximizeButton = 8,
    ShowCloseButton = 16,
    ShowIcon = 32
 };

};

}}

#endif