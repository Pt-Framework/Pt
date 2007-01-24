/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef Ptv_Gui_ImageButton_h
#define Ptv_Gui_ImageButton_h

#include <Pt/Text/String.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/ARgbInterleavedImage.h>
#include <Pt/Gui/Pixmap.h>
#include <Pt/Gui/Widget.h>

#include <memory>


// TODO It would probably be more reasonable to derive ImageButton (and Button) from a super-class
// AbstractButton, that has all common code in it.



namespace Pt {

namespace Gui {

	class Pixmap;

	/**
	 * @brief The implementation for a Button widget.
	 *
	 * A button (sometimes known as a command button or push button) is a widget that provides
	 * the user a simple way to trigger an event by clicking said button. When a user clicks
	 * the button this usually triggers some event dispatch mechanism which will call handle
	 * code which reacts to the button click like confirming a dialog or executing a search
	 * request.
	 *
	 * This Button class provides the 'clicked'-Signal for that matter. A Slot can be connected
	 * to this Signal and will be executed whenever the button was clicked.
	 *
	 * A typical button's presentation is a rectangle, wider than it is tall, with a descriptive
	 * Text in its center. This Text can be set using the constructor of this class or by calling
	 * setTex().
	 *
	 * A button usually has two visual states: pressed and not pressed.
	 *
	 * A foreground and a background color can be set for a button. The foreground color is
	 * used as color for the label's Text. The background color is used for the button's interior
	 * except for the border drawing. The default background color is a gray shade.
	 *
	 * \image Button-default_cancel_button.png A default cancel button
	 */
	class PT_GUI_API ImageButton : public Widget
	{
		public:
			/**
			 * @brief Constructor for the Button widget.
			 *
			 * A button widget is created. The given parent is set as parent of this button and
			 * the button is added to the parent's children list. The button is positioned at the
			 * given location using the given size. An optional Text can be passed as an argument.
			 * This Text will be shown as button Text. If no Text is given, no Text is shown.
			 *
			 * @param parent The parent widget for this button. The button will become the child of
			 * this parent and be shown inside of it. To create a top-level widget 0 can be passed
			 * as an argument.
			 * @param at The position of this button inside its parent relative to the parent's top-left corner.
			 * @param size The size of this button. The size must be >0 for width and height.
			 * @param Text The (optional) Text of this button.
			 * @see setText(const std::string)
			 */
			ImageButton(Widget& parent,
			       const Pt::Math::Point& at,
				   const Pt::Math::Size& size,
				   const Pt::Gfx::ARgbImage& image);

			//! @brief Emptry destructor for the button widget.
			~ImageButton();

			/**
			 * @brief Updates the presentation of this button.
			 *
			 * It does a complete repaint including the background, the border and the Text of the button.
			 */
			virtual void update();

			// Inherit doc
			virtual  Pt::Math::Size minimumSize();

			// Inherit doc
			virtual  Pt::Math::Size preferredSize();

		public:
			/**
			 * @brief A signal that notifies the registered slots when this button was clicked by the user.
			 */
			Signal<> clicked;

		protected:
			//! Internal resize event handle method.
			virtual void _resizeEvent(const ResizeEvent& event);

			//! Internal mouse event handle method.
			virtual void _mouseEvent(const MouseEvent& event);

			//! Internal repaint event handle method.
			virtual void _paintEvent(const PaintEvent& event);

			//! Internal move event handle method.
			virtual void _mouseMoveEvent(const MouseMoveEvent& event);

		protected:
			bool _pressed;

		private:
			/**
			 * @brief Draws the button in pressed state using the given painter.
			 */
			void drawPressed(Painter& painter);

			/**
			 * @brief Draws the button in pressed state using the given painter.
			 */
			void drawNormal(Painter& painter, bool focused);

		private:
			std::auto_ptr<Pixmap> _backbuffer;
			Pt::Gfx::ARgbImage   _image;
	};

} // namespace Gui

} // namespace Pt

#endif
