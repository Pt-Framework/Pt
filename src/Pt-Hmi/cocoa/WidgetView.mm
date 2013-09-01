/***************************************************************************
 *   Copyright (C) 2008 Marc Boris Duerner                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#import "WidgetView.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PointingEvent.h>
#import <AppKit/NSApplication.h>
#import <AppKit/NSEvent.h>
#import <AppKit/NSTrackingArea.h>


@implementation WidgetView

- (WidgetView*) init : (Pt::Hmi::GfxOutputImpl*) controll
{
    self = [super init];
    _outControll = controll;
    
    int opts = (NSTrackingActiveAlways | NSTrackingInVisibleRect | NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved);
    
    NSTrackingArea *area = [[NSTrackingArea alloc] initWithRect:[self bounds] options:opts owner:self userInfo:nil];
    [self addTrackingArea:area];
    
    return self;
}

- (BOOL) acceptsFirstResponder
{
    return TRUE;
}


- (void) flagsChanged:(NSEvent*)ev
{
    unsigned int mod = [ev modifierFlags];
    _outControll->onSpezialKeyEvent(mod);
}

- (void)keyDown:(NSEvent *)ev
{
    NSString* chars = [ev characters];
    unichar character = [chars characterAtIndex: 0];

    _outControll->onKeyDown(character);
}

- (void)keyUp:(NSEvent *)ev
{
    NSString* chars = [ev characters];
    unichar character = [chars characterAtIndex: 0];
    
    _outControll->onKeyUp(character);
}


- (void) drawRect:(NSRect)rect
{
    if(_outControll->model() == nil)
        return;
    
    //Create a raw buffer to hold pixel data which we will fill algorithmically
    NSInteger width =  _outControll->model()->PaintBuffer.width();
    NSInteger height = _outControll->model()->PaintBuffer.height();

    NSInteger dataLength = width * height * 4;
    UInt8 *data = (UInt8*)malloc(dataLength * sizeof(UInt8));
    
    //Fill pixel buffer with color data
    for (int j=0; j<height; j++)
    {
        for (int i=0; i<width; i++)
        {
            
            const Pt::Gfx::ARgbColor& pixel = _outControll->model()->PaintBuffer.pixel(i,j);
                                       
            int index = 4*(i+j*width);
            
            data[index]  = pixel.red();
                                       
            data[++index]=pixel.green();
            data[++index]=pixel.blue();
            data[++index]=255;
            
        }
    }
    
    // Create a CGImage with the pixel data
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, data, dataLength, NULL);
    CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
    CGImageRef image = CGImageCreate(width, height, 8, 32, width * 4, colorspace, kCGBitmapByteOrder32Big | kCGImageAlphaPremultipliedLast, provider, NULL, true, kCGRenderingIntentDefault);

    CGContextDrawImage((CGContext*)[[NSGraphicsContext currentContext] graphicsPort],rect,image);
    
    //Clean up
    CGColorSpaceRelease(colorspace);
    CGDataProviderRelease(provider);
    free(data);
}

- (void)setFrameOrigin:(NSPoint)origin
{
    [super setFrameOrigin:origin];
    
    _outControll->onPositionAndSize();
}

- (void)setFrameSize:(NSSize)frameSize
{
    [super setFrameSize:frameSize];
    
    _outControll->onPositionAndSize();
}

- (void) mouseDown:(NSEvent*)ev
{
    NSPoint mp = [ev locationInWindow];
    _outControll->onLMouseDown(mp.x,mp.y);
}

- (void) mouseUp:(NSEvent*)ev
{
    NSPoint mp = [ev locationInWindow];
    _outControll->onLMouseUp(mp.x,mp.y);
}

- (void) mouseDragged:(NSEvent*)ev
{
    
    NSPoint mp = [ev locationInWindow];
    _outControll->onMouseMove(mp.x,mp.y);
}

- (void) mouseMoved:(NSEvent *)ev
{
    NSPoint mp = [ev locationInWindow];
    _outControll->onMouseMove(mp.x,mp.y);
}

- (BOOL) windowShouldClose:(id)window
{
    return _outControll->onCanClose();
}

-(void) dealloc
{

}

@end
