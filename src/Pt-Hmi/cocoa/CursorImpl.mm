#include "CursorImpl.h"
#include <Pt/Hmi/Controller.h>

#include "GfxOutputImpl.h"
#include <Pt/Hmi/GfxOutput.h>
#include <Pt/Hmi/WindowController.h>
#import <AppKit/NSView.h>
#import <AppKit/NSCursor.h>
#import <AppKit/NSLayoutConstraint.h>

namespace Pt{
namespace Hmi{
        
CursorImpl::CursorImpl()
{
    
}
        
CursorImpl::~CursorImpl()
{
    
}
        
void CursorImpl::setCursor(Cursors::Type type, Controller* parent)
{
    if(_type == type)
        return;
            
    _type = type;

    NSCursor* aCursor =  [NSCursor arrowCursor];
            
    switch(_type)
    {
        default:             
        break;
             
        case Pt::Hmi::Cursors::Cross:
            aCursor = [ NSCursor crosshairCursor];
        break;
             
        case Pt::Hmi::Cursors::Hand:
            aCursor = [ NSCursor openHandCursor];
        break;
             
        case Pt::Hmi::Cursors::IBeam:
            aCursor = [ NSCursor IBeamCursor];
        break;
            
        case Pt::Hmi::Cursors::SizeNS:
            aCursor = [ NSCursor resizeUpDownCursor];
        break;
             
        case Pt::Hmi::Cursors::SizeWE:
            aCursor = [ NSCursor resizeLeftRightCursor];
        break;
    }
    
    //Set the cursor as current cursor.
    [ aCursor set];
            
}
        
Cursors::Type CursorImpl::getCursor() const
{
    return _type;
}
                
}}
