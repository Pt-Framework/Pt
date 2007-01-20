/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_GUI_VERTICALLAYOUT_H
#define PT_GUI_VERTICALLAYOUT_H

#include <Pt/Connectable.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gui/Api.h>
#include <Pt/Gui/LayoutManager.h>
#include <Pt/Gui/LayoutData.h>

#include <map>

namespace Pt {

namespace Gui {


	/**
	 * @brief The VerticalLayout Manager positions the widget vertically.
	 *
	 * This LayoutManager positions the widget vertically in the order they
	 * where added to their parent's widget (which this layout manager is
	 * associated with).
	 *
	 * The widgets' horizontal position can be specified when setting the layout
	 * data of a widget. Possibal horizontal positions are top, bottom, center and grab for the
	 * positioning at the top, bottom or center of the available vertical space.
	 * Grab does change the height in of the widget to the full available vertical
	 * space. For all other cases the widget's preferred height is used.
	 *
	 * The behaviour of the widget's width can be controlled by the widthBehaviour-
	 * parameter of the constructor (WidthBehaviour):
	 * - UniformWidth will set the width of all widgets to the preferred width of
	 * the widest widget.
	 * - VaryingWidth will set the width of each widget to its respective preferred
	 * width.
	 *
	 * A gap/spacing can be specified using the constructor. When set to a non-zero value
	 * this set amount of pixels is left as spacing between the horizontally positioned
	 * widgets. A negative value will lead to overlapping widgets.
	 *
	 * This LayoutManager does not wrap the widgets if there is not enough horizontal
	 * space in the parent widget available. The widgets will be positioned beyond
	 * the parent's borders instead.
	 *
	 * !!Attention
	 * LayoutManager are only inofficially supported by now. Use them at your own risk.
	 * The documentation is not completed yet.
	 *
	 * @see HorizontalLayoutData
	 */
	class PT_GUI_API VerticalLayout : public Layout
	{
		public:
			enum Mode {
				UniformHeight,
				VaryingHeight
			};

			public:
				enum Orientation {
					Left,
					Right,
					Center,
					Grab
			};

	/**
	 * @brief The LayoutData class for the VerticalLayout manager.
	 *
	 * An object of this LayoutData class stores layout information of the orientation
	 * and margin for the layouting of one widget. These informations can be accessed
	 * with the methods orientation() and margin().
	 *
	 * The orientation specifies how the widget should be layouted if there is more
	 * horizontal space available than the preferred width of the widget needs:
	 *
	 * - Left - The widget is positioned at the left using the preferred width of the widget.
	 * - Right - The widget is positioned at the right using the preferred width of the widget.
	 * - Center - The widget is positioned at the horizontal center using the preferred
	 * width of the widget.
	 * - Grab - The widget uses the complete horizontal space; its width it set to the
	 * available horizontal space.
	 *
	 * @see VerticalLayout
	 */
	 class PT_GUI_API LayoutData : public Gui::LayoutData
			{
				public:
					LayoutData( Orientation orientation = Left, const Margin& margin = Margin(0, 0, 0, 0) )
					: Gui::LayoutData(margin)
					, _orientation(orientation)
					{}

					//virtual LayoutData* clone() const
					//{ return new LayoutData(*this); }

					void setOrientation(Orientation orientation)
					{ _orientation = orientation; }

					Orientation orientation() const
					{ return _orientation; }

				protected:
					Orientation _orientation;
			};

		public:
			static VerticalLayout& create(Widget& widget, Mode mode = VaryingHeight, size_t gap = 0);

			void set(Widget& widget, Orientation orientation = Left, const Margin& margin = Margin(0, 0, 0, 0));

			void remove(Widget& widget);

			ssize_t maximumHeight() const;

			virtual void update();

			virtual Math::Size minimumSize();

			virtual Math::Size preferredSize();

		protected:
			VerticalLayout(Widget& widget, Mode mode, size_t gap);

			Math::Size calculateSize(Widget& parent, bool forPreferredSize);

		private:
			Mode _mode;
			size_t _gap;

			typedef std::map<Widget*, LayoutData> WidgetMap;
			WidgetMap _widgets;
	};

} // namespace Gui

} // namespace Pt

#endif
