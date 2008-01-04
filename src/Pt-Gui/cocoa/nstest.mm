#import <AppKit/NSApplication.h>
#import <iostream>

class Test
{
};

int main( int argc, char* argv[])
{
    std::cerr << "Started" << std::endl;
    NSApplication *app = [NSApplication sharedApplication];
    [app run];
    return 0;	
}
