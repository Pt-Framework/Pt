/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2013 Marc Boris Dürner
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#import "WidgetView.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PointingEvent.h>
#import <AppKit/NSApplication.h>
#import <AppKit/NSEvent.h>
#import <AppKit/NSTrackingArea.h>

@implementation WidgetView

- (WidgetView*) init : (Pt::Hmi::GfxOutputDeviceImpl*) device
{
    self = [super init];
    _outDevice = device;
    
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
    _outDevice->onSpezialKeyEvent(mod);
}

- (void)keyDown:(NSEvent *)ev
{
    NSString* chars = [ev characters];
    unichar character = [chars characterAtIndex: 0];
    
    //Emulate tab character on shift pressed.
    if(character == 25)
        character = 9;
    
    _outDevice->onKeyDown(character);
}

- (void)keyUp:(NSEvent *)ev
{
    NSString* chars = [ev characters];
    unichar character = [chars characterAtIndex: 0];
    
    //Emulate tab charecter on shift pressed.
    if(character == 25)
        character = 9;
    
    _outDevice->onKeyUp(character);
}

- (void) drawRect:(NSRect)rect
{
    if(_outDevice->model() == nil)
        return;
    
    //Create a raw buffer to hold pixel data which we will fill algorithmically
    NSInteger width =  _outDevice->model()->PaintSurface.width();
    NSInteger height = _outDevice->model()->PaintSurface.height();

    NSInteger dataLength = width * height * 4;
    UInt8 *data = (UInt8*)malloc(dataLength * sizeof(UInt8));
    
    //Fill pixel buffer with color data
    for (int j=0; j<height; j++)
    {
        for (int i=0; i<width; i++)
        {
            
            const Pt::Gfx::ARgbColor& pixel = _outDevice->model()->PaintSurface.pixel(i,j);
                                       
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
    
    _outDevice->onPositionAndSize();
}

- (void)setFrameSize:(NSSize)frameSize
{
    [super setFrameSize:frameSize];
    
    _outDevice->onPositionAndSize();
}

- (void) mouseDown:(NSEvent*)ev
{
    NSPoint mp = [ev locationInWindow];
    _outDevice->onLMouseDown(mp.x,mp.y);
}

- (void) mouseUp:(NSEvent*)ev
{
    NSPoint mp = [ev locationInWindow];
    _outDevice->onLMouseUp(mp.x,mp.y);
}

- (void) mouseDragged:(NSEvent*)ev
{
    
    NSPoint mp = [ev locationInWindow];
    _outDevice->onMouseMove(mp.x,mp.y);
}

- (void) mouseMoved:(NSEvent *)ev
{
    NSPoint mp = [ev locationInWindow];
    _outDevice->onMouseMove(mp.x,mp.y);
}

- (BOOL) windowShouldClose:(id)window
{
    return _outDevice->onCanClose();
}

@end
