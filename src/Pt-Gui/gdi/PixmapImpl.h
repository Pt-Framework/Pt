/***************************************************************************
 *   Copyright (C) 2006 Marc Boris D�rner                                  *
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

#ifndef Pt_Gui_Gdi_PixmapImpl_h
#define Pt_Gui_Gdi_PixmapImpl_h

#include <Pt/Api.h>
#include <Pt/Math/Size.h>
#include <Pt/Gui/Painter.h>
#include "Drawable.h"
#include "PixmapPainter.h"

#include <windows.h>

namespace Pt {

namespace Gui {

    class PixmapImpl : public Drawable
    {
        public:
            PixmapImpl(size_t width, size_t height);

            PixmapImpl(const PixmapImpl& oldPixmap);

            virtual ~PixmapImpl();

            const Math::Size& size() const
            { return _size; }

            Painter painter();

            HBITMAP bitmapHandle() const;

            virtual HDC beginPaint();

            virtual void endPaint();

            virtual HDC deviceContext() const;

            virtual bool isPainting() const;


        private:
            void setupDeviceContext();

        private:
            Math::Size      _size;
            HDC            _deviceContext;
            HBITMAP        _bitmapHandle;
            PixmapPainter* _painter;

            HPEN   _oldPen;
            HBRUSH _oldBrush;
            HFONT  _oldFont;
    };

} // namespace Gui

} // namespace Pt

#endif
