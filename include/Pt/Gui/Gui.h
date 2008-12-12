/*
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
namespace Pt {
/** @namespace Pt::Gui
    @brief Graphic User Interfaces

    This module offers classes and concepts for programming of graphical
    applications. It provides an Application class containing the application-wide
    event queue, functionality for delivery of high-level events, creation of
    widgets, windows and pixmaps, layouting and painting on widget and pixmaps.
*/
namespace Gui {

}

}

/** \page "Simple applications"
!!!A simple application

This tutorial demonstrates how to create a basic graphical application using PPR.

The application which is created during this tutorial will consist of a window
which contains a button. Pressing this button will draw a line, an ellipse and a
filles rectangle at random positions of the window. Clicking anywhere on the
window's content area will paint a dot of random size at the position the user
clicked. The application will also react to the 't'-key. When this key is pressed,
a text is printed at a random position of the window's content area.


!!Basic application

The most basic GUI application which can be created using PPR looks as follows:

\code
#include "ptv/gui/Application.h"
#include <ptv/Main.h>

int main(int argc, char* argv[])
{
    ptv::gui::Application app;
    return app.run();
    
    return 0;
}
\endcode

This code creates an Application-object which contains the event loop and
all event processing for the GUI application. To start the event loop and event
handling run() must be called. This method only returns when the application is
about to exit. This will happen when an unhandled exception is thrown or when
Application::exit() was called.

The Application object queues all events from the underlying window system and
dispatches them to the widgets for which the events where generated. The
platform-specific events are converted into PPR-events before they are dispatched,
so no platform-specific code has to be written when PPR is used.

As the code above does not call Application::exit() and contains no other code
which would do anything useful, this application will never exit.

Note that the only platform-independent way of defining the main-function is as
shown above. The header "ptv/Main.h" defines a generic main-function for all
platforms. This method should be used as main-function for the application to
avoid platform-specific code. The main-function takes the number of arguments
(argc) and the argument themselves (argv) as it is common for the main-function.


!!Basic exception handling

To catch any exception which may be thrown by the application the following code
can be used.

\code
int main(int argc, char* argv[])
{
    try
    {
        ptv::gui::Application app;

        return app.run();
    }
    catch(const ptv::Exception& e)
    {
        cerr << "Exception: " << e.what() << "(" << e.sourceInfo().line() << " in " << e.sourceInfo().func() << ")" << endl;
    }
    catch(const std::exception& e)
    {
        cerr << "Exception: " << e.what() << "(" << e.sourceInfo().line() << " in " << e.sourceInfo().func() << ")" << endl;
        return 1;
    }

    return 0;
}
\endcode

The super-class of all exception which may be thrown by PPR is ptv::Exception.
So catching this exception will catch all exception that are thrown inside the
PPR-API.

Catching more specific exceptions may be reasonable.


!!Adding the application window

The first step to a real GUI application is by adding a window to the application.
We first create a class which derives from Widget and will act as top-level window
for our application. The class Widget is the base-class for all widgets.

To use the Widget class we first have to include "ptv/gui/Widget.h".

\code
#include <ptv/gui/Widget.h>
\endcode

The basic implementation of our window class is as follows:

\code
class TutorialWindow : public ptv::gui::Widget
{
    public:
        TutorialWindow()
        : Widget( )
        {
            Widget::setTitle("Tutorial");
        }
};
\endcode

The class is named "TutorialWindow". The constructor does nothing except setting the title of
the widget to "Tutorial". This title is shown in the title bar of top-level widgets (=windows)
only.

To actually create and show the window we have to expand our main-function:

\code
ptv::gui::Application app;

TutorialWindow tut;
connect(tut.closed, app, &gui::Application::exit);

tut.show();

return app.run();
\endcode

The first and last lines are the same as before. Three things are happening in the
three new lines in the middle:
- An object of our tutorial window class (TutorialWindow) is created. At this moment the
window is still hidden and not shown on screen.
- We connect the widget's Signal 'closed' to the method Application::exit().
Whenever the 'closed'-signal of the TutorialWindow object is sent, Application::exit() will
in turn be called. This will make Application::run() to exit and return, which will exit the
application, as no more code is left in main(). The Signal 'closed' is sent when the widget
is about to be destroyed by the underlying platform. In case of a top-level widget this
happens when the user closes the window or when the widget is closed by the program.
As we want to exit the application when the window is closed, we connect the Signal to
Application::exit(). The concepts of the Signal-Slot-mechanism will be discussed later in
this document. For now it is sufficient to understand that a sent 'closed'-Signal will lead
to a call to Application::exit().
- The last step is to show our window by calling 'tut.show()' (-> Widget::show()).

When this application is executed an empty window will be shown:

\image ppr-gui-tutorial1-empty_window.png "Empty window"


As we do not paint our window's client area the underlying desktop or windows which are
dragged over the window can be seen.

Note the window title ("Tutorial") and the correct termination of the application when
the window is closed.


!!Drawing the client area
The underlying platform will request a repaint for the widget when all or parts of it
became dirty. A widget becomes dirty for example when another widget has covered the
widget and then was moved away. The part which was covered is now dirty and has to be
repainted.

All of the widget has to be repainted for example when the window was minimized and then
restored again. All widgets of this window are dirty then.

For our tutorial we only want to paint the top-level widget's content completely white.
This will avoid any artefacts of the desktop or other widgets which were previously
covering part of our TutorialWindow.

The underlying platform will trigger an event (PaintEvent) when a repaint has to be done.
This PaintEvent is delivered to the method Widget::_paintEvent() of the widget object
which is supposed to be repainted by the application event processing unit. As our
TutorialWindow class does derive from Widget we can override _paintEvent() to do our
own PaintEvent-handling:

\code
#include <ptv/gfx/Point.h>
#include <ptv/gfx/Rect.h>

#include <ptv/gui/Painter.h>
#include <ptv/gui/PaintEvent.h>
#include <ptv/gfx/Brush.h>

...

virtual void _paintEvent(const PaintEvent& event)
{
    gui::Painter widgetPainter = this->painter();
    
    widgetPainter.setBrush(Brush(ARgbColor(65535, 65535, 65535)));
    widgetPainter.fillRect(event.rect());
}
\endcode

In the above code the Painter concept of PPR's gui-module is used for the first time.
Before we can paint to the widget (or a pixmap) a Painter object has to be requested
from the widget (or pixmap) on which we want to paint.

The Painter object is stored in a local variable. It is ok to copy the Painter as often
as needed, this does have no major resource penalty. It's essential, though, that the
Painter object is freed after it was used. In this case the Painter object is freed when
the method is returning as the object is only in local scope and so freed automatically.

The Painter object is not supposed to be stored for a longer period of time. It should
only be used to finish the current drawing and then be freed again. If the object is
stored for a longer time, the correct behaviour of the painter can not be guaranteed.
Malfunction may only occur on certain platforms!

After the painter was optained, we set its brush color and style to a solid Brush with
white color (65535 for all three color components.) The color range for each color
component of a ARGB-color goes from 0 to 65535 (=2<sup>16</sup>). A color value of
65535, 65535, 65535 (=0xffff, 0xffff, 0xffff) will result in a bright white.

At last we paint a filled rectangle. As dimensions for the rectangle we use the
information which was delivered with the PaintEvent. A PaintEvent contains an information
about which part of the widget is dirty and should be repainted. The dirty area can be
obtained by calling PaintEvent::rect(). It can save a lot of processing time to only
(re)paint the area of the widget which is actually dirty.

!!Painting
As seen in the code above a Painter object is used to draw on a widget surface. The Painter
class provides methods for drawing outlined and filled shapes like rectangles, circles,
ellipses, polygons, but also simpler graphical primitives like pixels, lines and polylines.

It's necessary to distinguish between outlined shapes and filled shapes. Outlined shapes
for example are points, lines, outlined rectangle, outlined circles/ellipses, polylines
and text. Filled shapes for example are filled rectangles, filled circles/ellipses and
polygons.

The painter provides two drawing tools: Pen and Brush

The Pen is used to draw outlined shapes, the Brush is used to draw filled shapes.
- The pen can have a color and a size. A blue pen with a pen-size of 5 will draw a line
  in blue color with a line size of 5 pixels, for example. This is valid for all outlined
  shapes, except text-drawing. For texts only the pen color, but not the pen-size is used.
  (The font size can be specified when setting the current font of the painter.)
- The brush can be a solid color or a texture. When the brush is set to a solid green, the
  filling of the rectangle is completely drawn with this color. When a texture (ARgbImage)
  is specified as brush texture, the interior of the rectangle is painted using this
  texture.

To draw a rectangle with both filling and outline, first a filled rectangle and afterwards
an outlined rectangle with the same position and size can be drawn.

To set the Pen of the painter use the method Painter::setPen(). To set the Brush of the
painter use the method Painter::setBrush(). Both properties are separate and don't interfere
with each other.

Besides pen and brush the font for a Painter can be specified. A Font object consists of
the font family name, a font style (italic, bold), rotation angle and font size. To set the
font of the Painter the method Painter::setFont() can be used.

After the pen, brush or font was set these settings are valid until they are changed. Even
when the Painter object was freed between drawing operations and then obtained again by calling
Widget::painter() the pen, brush and font settings are still the same as before.


!!Adding a Button to the window

The next thing to add to our tutorial application will be a Button which is positioned in the
left-upper corner of the window. Clicking the button will produce a 'clicked'-Signal which will
be processed and draw a line, a rectangle and an ellipse at a random position and size.

As the Button object needs to be available during the execution time of the application we
first add a member variable to our TutorialWindow class:

\code
#include <ptv/gui/Button.h>

...

private:
    auto_ptr<Button> _actionButton;
\endcode

We use an Auto-Pointer (auto_ptr) here so we don't have to manually free the Button object when
the window object is destroyed.

We construct and initialize the Button widget in the TutorialWindow's constructor:

\code
TutorialWindow()
: Widget( )
{
    Widget::setTitle("Tutorial");

    // Create the Action-Button
    _actionButton.reset(new Button(*this, Point(10, 10),  Size(50, 30), "Paint!"));

    // Connect clicked-event of buttons with handle methods.
    connect(_actionButton->clicked, *this, &TutorialWindow::onActionButton);
}
\endcode

The first parameter of the Button's constructor specifies the parent widget for the Button
in which the Button will be positioned. As we want to add the Button to our window, we
pass $*this$ because we are in the constructor of our window widget.

The second parameter specifies the position of the Button relative to its parent's top-left
corner. The Button is positioned at position (10, 10) inside the Tutorial window.

The third parameter specifies the size of the Button. To fit our text ("Paint!") into it
a size of 50 (width) and 30 (height) pixels is used.

The fourth parameter specifies the Button's text.

The last line of the constructor will be explained in the next chapter of this tutorial.

Running the application will produce this result:

\image ppr-gui-tutorial1-window_with_button.png



!!High-level event handling (Signal-Slot) and painting

The last line of the above constructor demonstrates the Signal-Slot mechanism of PPR.

\code
    connect(_actionButton->clicked, *this, &TutorialWindow::onActionButton);
\endcode

With the given code we connect the Signal 'clicked' of the Button-class to the method
$onActionButton()$ of our TutorialWindow-widget. The Signal 'clicked' will send high-level
clicked-events provided by the Button-class whenver the Button is clicked. By connecting
the 'clicked'-Signal to the $onActionButton()$-method this method is called whenever
the 'clicked'-Signal is sent.

Every class can provide Signals. They act as a starting point (drain) to sent events in the form
of Event objects. The Signals of a class can be connected to other Signals, to methods,
to static methods and to free functions during runtime. The general term for these connection-targets
is 'Slots'.

When sending an event to a Signal the event is sent to all connected Slots. If the Slot is a
Signal itself, the event will be sent as if the event was sent directly by this Signal. Any Slots
connected to this Signal will also receive the event. If the Slot is a method or function, the
method or function will be called and the event is passed as parameter.

Thus only methods which have the exact same parameter list as the Signal can be connected to it.
This, for example, means that if the Signal sends no event object, the method or function has to
have an empty parameter list. If the Signal sent a single const-reference to MouseEvent, the
Slot-method would have to have a parameter of type const-reference MouseEvent. (This applies to
Signal-Signal connection as well.)

When a Slot is destroyed the connection is released automatically. It is not necessary to
close the connection manually.


For our example we define the $onActionButton()$-method (which we connect to the 'clicked'-Signal
of the Button-widget) as follows:

\code
void onActionButton()
{
}
\endcode

The parameter list for the method is empty as the clicked-Signal does not send any event
objects. The parameter list of the handling-method always has to have a parameter list
that corresponds to the parameter list of the Signal it is connected to.

The $onActionButton()$-method above is still empty. To demonstrate how a more complex
paint process may look like, a click on the Button will paint a line, a rectangle and
an ellipse, each at a random position and with random size:

\code
#include <ptv/gfx/Pen.h>

...

void onActionButton()
{
    // Get the painter of the widget.
    gui::Painter widgetPainter = this->painter();

    // Set pen size to 3 and color to black.
    widgetPainter.setPen(Pen(3, ARgbColor(0, 0, 0)));

    // Draw a line (using the just set pen) at a random position.
    int x1 = random(0, this->size().width());
    int y1 = random(0, this->size().height());
    int x2 = random(0, this->size().width());
    int y2 = random(0, this->size().height());
    widgetPainter.drawLine(Point(x1, y1), Point (x2, y2));

    // Set a green solid brush.
    widgetPainter.setBrush(Brush(ARgbColor(0, 65535, 0)));

    // Draw a filled rectangle at a random position with a random size using the just set brush.
    x1         = random(0, this->size().width() - 100);
    y1         = random(0, this->size().height() - 100);
    int width  = random(30, 100);
    int height = random(30, 100);
    widgetPainter.fillRect(Rect(Point(x1, y1), Size(width, height)));

    // Set a red solid brush.
    widgetPainter.setBrush(Brush(ARgbColor(65535, 0, 0)));

    // Draw a filled ellipse at a random position with a random size still using the just set brush.
    x1     = random(0, this->size().width()  - 100);
    y1     = random(0, this->size().height() - 100);
    width  = random(30, 100);
    height = random(30, 100);
    widgetPainter.fillEllipse(Point(x1, y1), Size(width, height));
}
\endcode

This code should be quite self-explaining. First we again obtain a Painter ob ject to draw on the window
widget. Then we set a pen of size 3 and black color (0, 0, 0). Using this pen we draw a line at a random
position. The two points are calculated before the actual drawLine()-call is executed.

The line is followed by a filled rectangle. As this is a filled shape a brush is used to paint it. We set
the painter's brush to a bright green (0, 65535, 0) color. The fillRect()-method takes a Rect-object (rectangle)
as parameter, which internally consists of a point and a size spanning the rectangle.

The drawing of the ellipse is similar to the drawing of the rectangle. We first set the brush color to
a solid red (65535, 0, 0) and then draw the ellipse at the specified (random) position using the random
width and height. Note that the specified point does not describes the center of the ellipse but the top-left
corner of a virtual bounding box around the ellipse. The width and height also references to this virtual
bounding box.

The result looks like this:

| \image ppr-gui-tutorial1-basic_shapes.png "" | \image ppr-gui-tutorial1-basic_shapes_with_ellipse_bounding_box.png |

The right screengrab additionally shows the virtual bounding box around the ellipse. The dot in the top-left
corner is the position of point which is passed as first parameter to $fillEllipse()$.


!!Handling low-level events - MouseEvent

This segment will demonstrate how to react to more basic user input. We already "listened" to a Button-click
by connecting a method to a Button's 'clicked'-Signal. This Signal is a high-level event. Processing
low-level events, which may be more flexible, can be done by overriding event handling-methods of the
Widget class or one of its sub-classes. The code of the overriding method will contain specific event
handling-code.

In this tutorial we want to listen to mouse-presses of the left mouse-button on the window and draw a dot
at the position where the mouse was pressed.

MouseEvents do not send "mouse click"-events, because a click actually consists of a "mouse was pressed"
directly followed by a "mouse was released" and not just a single activity. Because a click is a complex
event it is sent as high-level event. MouseEvents, on the other hand, only inform about basic input, like
"the left mouse button was pressed", "the right mouse button was released while the keyboard's Shift key
was down" etc.

Mouse-actions are dispatched to the method Widget::_mouseEvent(const MouseEvent& event) of the widget for which
they occured. This method receives a MouseEvent object which stores a reference to the widget, specifies
what action occured, which button was pressed, the x- and y-position where the mouse event occured and
a bit-field specifying which modifiers were active while the mouse event occured.

As stated above, we just have to override $_mouseEvent(const MouseEvent& event)$. That's done as follows:

\code
#include <ptv/gui/MouseEvent.h>

...

virtual void _mouseEvent(const MouseEvent& event)
{
    // Draw a filled blue dot if the left mouse was pressed at the position of the mouse.
    if (event.action() == MouseEvent::Press && event.button() == MouseEvent::LeftButton)
    {
        // Get the painter of the widget.
        gui::Painter widgetPainter = painter();

        // Set the painter's brush to blue color.
        widgetPainter.setBrush(ARgbColor(0, 0, 65535));

        // Draw a dot (circle) of random size at the mouse event's position.
        int pointSize = random(5, 50);
        widgetPainter.fillCircle(Point(event.x() - pointSize / 2, event.y() - pointSize / 2), pointSize);
    }
}
\endcode

The first thing is to check the precise nature of the event. We checked if the event is for
a pressed mouse button (as opposed to a released mouse button or a double-click) and for which
of the mouse buttons the event occured. If the left mouse button is pressed, we will proceed to
draw the dot.

For this we get the Painter object, set the brush to a solid blue (0, 0,0 65535) and draw a
circle of random width.

If the MouseEvent's action is not for a mouse-button click or is not for the left mouse button,
we just ignore the event.

Note that we didn't connect any signals as the _mouseEvent()-method is a internal event-handling
method of the Widget-class which is informed about (Mouse)Events in any case.

The result of some clicking in the window will look like this:

\image ppr-gui-tutorial1-dots.png


!!Handling key events

Handling key events is similar to handling mouse events, except a KeyEvent object stores different
informations: The key-code or the key-character is stored, depending on the key type which was pressed.

A key-code specifies a key which has no real character-representation, like for example the shift
key, the enter key or the 'Page up' key.

Character keys, on the other hand, have a character representation. Those keys are the ones which
usually will create some output on the screen when typed, like a, b, c, 1, 2, 3 or !, ", .

To handle key events we have to override the method Widget::_keyEvent(const KeyEvent& event) in our
TutorialWindow-class and add the event-handling code.

For our example we want to react to the key 't' and draw a text inside the window's content area
when the key is pressed.

The code to do this is as follows:

\code
#include <ptv/gui/KeyEvent.h>
#include <ptv/gfx/Font.h>

...

virtual void _keyEvent(const KeyEvent& event)
{
    if(event.text() == 't' && event.type() == KeyEvent::Press) {
        gui::Painter widgetPainter = this->painter();

        widgetPainter.setPen(Pen(1, ARgbColor(random(0, 40000), random(0, 40000), random(0, 40000))));
        widgetPainter.setFont(Font("Arial", 17, Font::NormalStyle));

        int x = random(0, this->size().width());
        int y = random(0, this->size().height());
        widgetPainter.drawText(Point(x, y), "Test-Text!");
    }
}
\endcode

First we check if the text of the KeyEvent is a lower-case 't' and the key was pressed (and not
released). If this is not the case, nothing is done.

If 't' is pressed, we first obtain the painter to draw on the widget, set the Pen to a random
color (the Pen size is of no use for text output) and then set the font for the painter to
"Arial" with a font size of 17 and a normal font style (other values would be 'italic' and 'bold').

When there is no font-family named "Arial" is available on the current platform, the platform will
choose another font which may or may not match the requested font the best.

Finally we draw the text "Test-Text!" at a random position on the screen. The previously set pen
color is used as text color.

Holding the 't'-key for some time will have the following result:

\image ppr-gui-tutorial1-text.png ""

If we wanted to react to a non-character-key, for example the Shift-key, we would have to
use the method KeyEvent::code() and check against an enum value of KeyEvent::KeyCode.


!!!Putting everything together
After putting all of the above code segments together we get the following code:

\code
#include "ptv/gui/Application.h"

#include <ptv/Main.h>

#include <ptv/gfx/Point.h>
#include <ptv/gfx/Rect.h>

#include <ptv/gui/Widget.h>
#include <ptv/gui/Button.h>
#include <ptv/gui/MouseEvent.h>
#include <ptv/gui/Painter.h>
#include <ptv/gui/PaintEvent.h>
#include <ptv/gui/KeyEvent.h>
#include <ptv/gfx/Pen.h>
#include <ptv/gfx/Brush.h>
#include <ptv/gfx/Font.h>

using namespace std;
using namespace ptv;
using namespace ptv::gfx;
using namespace ptv::gui;


class TutorialWindow : public ptv::gui::Widget
{
    public:
        TutorialWindow()
        : Widget( )
        {
            Widget::setTitle("Tutorial");

            // Create the Action-Button
            _actionButton.reset(new Button(*this, Point(10, 10),  Size(50, 30), "Paint!"));

            // Connect clicked-event of buttons with handle methods.
            connect(_actionButton->clicked, *this, &TutorialWindow::onActionButton);
        }

        void onActionButton()
        {
            // Get the painter of the widget.
            gui::Painter widgetPainter = this->painter();

            // Set pen size to 3 and color to black.
            widgetPainter.setPen(Pen(3, ARgbColor(0, 0, 0)));

            // Draw a line (using the just set pen) at a random position.
            int x1 = random(0, this->size().width());
            int y1 = random(0, this->size().height());
            int x2 = random(0, this->size().width());
            int y2 = random(0, this->size().height());
            widgetPainter.drawLine(Point(x1, y1), Point (x2, y2));

            // Set a green solid brush.
            widgetPainter.setBrush(Brush(ARgbColor(0, 65535, 0)));

            // Draw a filled rectangle at a random position with a random size using the just set brush.
            x1         = random(0, this->size().width() - 100);
            y1         = random(0, this->size().height() - 100);
            int width  = random(30, 100);
            int height = random(30, 100);
            widgetPainter.fillRect(Rect(Point(x1, y1), Size(width, height)));

            // Set a red solid brush.
            widgetPainter.setBrush(Brush(ARgbColor(65535, 0, 0)));

            // Draw a filled ellipse at a random position with a random size still using the just set brush.
            x1     = random(0, this->size().width()  - 100);
            y1     = random(0, this->size().height() - 100);
            width  = random(30, 100);
            height = random(30, 100);
            widgetPainter.fillEllipse(Point(x1, y1), Size(width, height));
        }


        int random(int min, int max)
        {
            return (int)(((double)rand() / RAND_MAX) * max + min);
        }


    protected:
        virtual void _mouseEvent(const MouseEvent& event)
        {
            // Draw a filled blue dot if the left mouse was pressed at the position of the mouse.
            if (event.action() == MouseEvent::Press && event.button() == MouseEvent::LeftButton)
            {
                // Get the painter of the widget.
                gui::Painter widgetPainter = painter();

                // Set the painter's brush to blue color.
                widgetPainter.setBrush(ARgbColor(0, 0, 65535));

                // Draw a dot (circle) of random size at the mouse event's position.
                int pointSize = random(5, 50);
                widgetPainter.fillCircle(Point(event.x() - pointSize / 2, event.y() - pointSize / 2), pointSize);
            }
        }

        virtual void _paintEvent(const PaintEvent& event)
        {
            gui::Painter widgetPainter = this->painter();

            widgetPainter.setBrush(Brush(ARgbColor(65535, 65535, 65535)));
            widgetPainter.fillRect(event.rect());
        }

        virtual void _keyEvent(const KeyEvent& event)
        {
            if(event.text() == 't' && event.type() == KeyEvent::Press) {
                gui::Painter widgetPainter = this->painter();

                widgetPainter.setPen(Pen(1, ARgbColor(random(0, 40000), random(0, 40000), random(0, 40000))));
                widgetPainter.setFont(Font("Arial", 17, Font::NormalStyle));

                int x = random(0, this->size().width());
                int y = random(0, this->size().height());
                widgetPainter.drawText(Point(x, y), "Test-Text!");
            }
        }

    private:
        auto_ptr<Button> _actionButton;
};


int main(int argc, char* argv[])
{
    try
    {
        ptv::gui::Application app;

        TutorialWindow tut;
        connect(tut.closed, app, &gui::Application::exit);

        tut.show();

        return app.run();
    }
    catch(const ptv::Exception& e)
    {
        cerr << "Exception: " << e.what() << "(" << e.sourceInfo().line() << " in " << e.sourceInfo().func() << ")" << endl;
    }
    catch(const std::exception& e)
    {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}
\endcode

Using all the implemented functionality will produce this nice output:

\image ppr-gui-tutorial1-every_shape.png


/// DOXYS_OFF
TODO
!!!Advanced widget handling
!!Double buffering (introduce pixmap)
!!Real Widget event handling (repaint using backbuffer)
/// DOXYS_ON


*/


/** \page "Glossary"
|| || Word         || Description ||
| 1 |Widget         |Widget is contraction of the words "window" and "gadget". In graphical user
                     interface programming, a widget (or control) is an interface element that
                     a computer user interacts with, such as a window, a button or a text box.  |
| 2 |Pixmap         |A pixmap is a hardware accelerated device-dependent rasterized image. A
                     pixmap may be used instead of an image for performance reason. Its internal
                     format is device-specific and depends primarily on the color depth of the
                     device this pixmap was created for. As no conversion is necessary and the
                     pixmap can be bit-blitted directly to the graphics card's memory (and may
                     even be stored inside the graphics card's memory) the usage of pixmap is
                     the fastest way to draw complex graphics on a graphical device.            |
*/
