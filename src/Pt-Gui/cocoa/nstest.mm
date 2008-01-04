#import <AppKit/NSApplication.h>
#import <AppKit/NSWindow.h>
#import <iostream>

int main( int argc, const char* argv[])
{
    std::cerr << "Started" << std::endl;
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSApplication *app = [NSApplication sharedApplication];

    NSWindow* window; 
    window = [[NSWindow alloc] initWithContentRect:NSMakeRect(100.0, 350.0, 200.0, 40.0)      
                                                   styleMask:NSTitledWindowMask | 
                                                             NSClosableWindowMask | 
                                                             NSMiniaturizableWindowMask
                                                   backing:NSBackingStoreBuffered
                                                   defer:YES];

    [window setTitle:@"Hello, World!"];
    [window makeKeyAndOrderFront:nil];
    [app run];
    
    [window release];
    [app release];
    [pool release];
    return 0;	
}
