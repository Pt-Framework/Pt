/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PTV_GUI_LAYOUTDATA_H
#define PTV_GUI_LAYOUTDATA_H

#include <Pt/Api.h>
#include <Pt/Gui/Margin.h>

namespace Pt {

namespace Gui {

	/**
	 * @brief The base class for all LayoutManager-specific LayoutData objects.
	 *
	 * A single LayoutData object describes the layout constraints for a single widget
	 * which is going to be layouted using a specific LayoutManager. It contains specific
	 * information about the layout data for a widget. In the case of this base class, only
	 * a margin can be specified. For complex LayoutManager other information, like position,
	 * stretching, orientation of widgets to each others and others hints may be adjusted
	 * with the layout data object.
	 * 
	 * Usually every LayoutManager does provide its specific LayoutData object, which contains
	 * all necessary information to layout a widget in its container context. Only very basic
	 * LayoutManagers may use this base class.
	 *
	 * @see Layout
	 */
	class PT_API LayoutData
	{
		public:
			/**
			 * @brief Constructs a new LayoutData object using the optional margins.
			 *
			 * Constructs a new LayoutData object using the optional margins. If no margins are
			 * specified, a 0-margin (0, 0, 0, 0) is used.
			 *
			 * @param margin The margin to use for this LayoutData object.
			 */
			LayoutData(const Margin& margin = Margin(0, 0, 0, 0))
			: _margin(margin)
			{
			}

			/**
			 * @brief Sets the margin for this LayoutData object.
			 *
			 * !Information
			 * Setting the margin does not automatically trigger a re-layout of the
			 * container this LayoutData's widget is contained in. Use Widget::updateLayout()
			 * to manually start a re-layout.
			 *
			 * @param margin The new margin for this LayoutData object.
			 */
			void setMargin(const Margin& margin)
			{
				_margin = margin;
			}

			/**
			 * @brief Returns the margin of this LayoutData object.
			 * @return The margin of this LayoutData object.
			 */
			const Margin& margin() const
			{
				return _margin;
			}

		private:
			Margin _margin;
	};

} // namespace Gui

} // namespace Pt

#endif
