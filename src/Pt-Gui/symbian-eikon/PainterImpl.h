/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
 *   Copyright (C) 2008 Peter Barth                                        *
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

#ifndef PT_GUI_SYMBIAN_PAINTERIMPL_H
#define PT_GUI_SYMBIAN_PAINTERIMPL_H

#include <vector>
#include <list>

#include <Pt/Gui/Api.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Region.h>
#include <Pt/Gfx/Algorithm.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/Gfx/Rgb565Image.h>
#include <Pt/Gfx/Rgb555Image.h>
#include <Pt/String.h>

// symbian includes
#include <e32std.h>

// Symbian class forwards
class CGraphicsContext;
class CGraphicsDevice;
class CFont;
class CFbsBitmap;
class CEikonEnv;

namespace Pt {

namespace Gui {

    class Pixmap;

    /**
     * @brief Painter implementation for symbian CGraphicsContext.
     * 
     * This is the base class for two seperate painter implementations:
     * PixmapPainter and WidgetPainter.
     * 
     * Both will use a CGraphicsContext to draw, but different implementations
     * of begin/end to activate the context.
     * 
     * This painter will only draw when a context is provided.
     * If the context can not be activated we're assuming that there is no
     * window/bitmap server connection and simply do nothing at all.
     */
    class PainterImpl 
    {
        public:
            PainterImpl();

            virtual ~PainterImpl();

            /**
             * @brief will be called from Widget/Pixmap when the connection
             * to the window/font server is about to be destroyed
             */
            void destructResources();
            
            virtual void begin();

            virtual void end();

            /**
             * @brief will be called from Painter::~Painter() 
             */
            virtual void cleanUp();
            
            void setPen(const Gfx::Pen& pen);

            const Gfx::Pen& pen() const;

            void setBrush(const Gfx::Brush& brush);

            const Gfx::Brush& brush() const;

            const Gfx::Font& font() const;

            void setFont(const Gfx::Font& font);

            Gfx::FontMetrics fontMetrics() const;

            Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

            const std::list<std::string>& fontFamilyNames();

            virtual void drawPixel(const Math::Point& to);

            virtual void drawLine(const Math::Point& from, const Math::Point& to);

            virtual void drawText(const Math::Point& to, const Pt::String& text);

            virtual void drawRect(const Gfx::Rect& rect);

            virtual void drawEllipse(const Math::Point& topLeft, const Math::Size& size);

            virtual void drawPolyline(const Math::Point* points, const size_t pointCount);

            virtual void fillRect(const Gfx::Rect& rect);

            virtual void fillEllipse(const Math::Point& topLeft, const Math::Size& size);

            virtual void fillPolygon(const Math::Point* points, const size_t pointCount);

            virtual void drawPixmap(const Math::Point& to, Pixmap& pm);

            virtual void drawPixmap(const Math::Point& to, Pixmap& pm, const Gfx::Region& pmRegion);

            void drawImage(const Pt::Math::Point& to, const Gfx::ARgbImage& image);
            
            void drawImage(const Pt::Math::Point& to, const Gfx::ARgb8888Image& image);

            void drawImage(const Pt::Math::Point& to, const Gfx::ARgbImage& image, const Pt::Gfx::Region& imageRegion);
            
            void drawImage(const Pt::Math::Point& to, const Gfx::ARgb8888Image& image, const Pt::Gfx::Region& imageRegion);

            /**
             * @brief Helper struct to hold the necessary context information
             * used for painting.
             */
            struct ContextInfo
            {
                CGraphicsContext* _gc;
                CGraphicsDevice* _device;
                const CFont* _nativeFont;
                CEikonEnv* _coeEnv;
                TPoint _offset;
                TRect _clipRect;
                
                ContextInfo() 
                : _gc(0)
                , _device(0)
                , _nativeFont(0)
                , _coeEnv(0)
                , _offset(0, 0)
                , _clipRect(0, 0, 0, 0)
                {                    
                }
            };
                        
        protected:
            /**
             * @brief Apply context information to the attributes of this class.
             */            
            void applyContextInfo(const ContextInfo& contextInfo);
            
            template <typename Iterator>
            void drawImage(ssize_t x, ssize_t y, Iterator begin, Iterator end, size_t width, size_t height)
            {
                Gfx::Rgb888Image rgb32Image(width, height);
                assign(begin, end, rgb32Image.begin());
                drawCompatibleImage(x, y, (char*)rgb32Image.data(), rgb32Image.width(), rgb32Image.height());
            }

            void drawCompatibleImage(size_t x, size_t y, const char* data, size_t width, size_t height);

            /**
             * @brief Ensure active context and activate pen.
             * 
             * @see activatePen
             */
            void updatePen();
            
            /**
             * @brief Activate pen but only if it has changed.
             * <br/><b>Graphic context must be active</b>
             */
            void activatePen();

            /**
             * @brief Ensure active context and activate font.
             * 
             * @see activateFont
             */
            void updateFont();
            
            /**
             * @brief Activate font but only if it has changed.
             * <br/><b>Graphic context must be active</b>
             */
            void activateFont();

            /**
             * @brief Ensure active context and activate brush.
             * 
             * @see activateBrush
             */
            void updateBrush();

            /**
             * @brief Activate brush but only if it has changed.
             * <br/><b>Graphic context must be active</b>
             */            
            void activateBrush();

            /**
             * @brief This will ensure that the graphic context is active.
             */ 
            bool ensureActiveContext();
            
            /**
             * @brief Release font resources.
             */
            void freeFont();
            
        protected:
            // current pen
            Gfx::Pen _pen;
            // current brush
            Gfx::Brush _brush;
            // current font
            Gfx::Font  _font;

            // last used pen
            Gfx::Pen _oldPen;
            // last used brush
            Gfx::Brush _oldBrush;
            // last used font
            Gfx::Font  _oldFont;

            // if a pen was used before this pointer will be valid, otherwise it's 0
            // it will be used to detect whether an old pen is available
            Gfx::Pen* _oldPenRef;
            // see above
            Gfx::Brush* _oldBrushRef;
            // see above
            Gfx::Font*  _oldFontRef;
            
            // symbian context information
            CGraphicsContext* _gc;
            CGraphicsDevice* _device;
            const CFont* _nativeFont;
            bool _fontOwner;
            CEikonEnv* _coeEnv;
            // this is used as a painting offset
            // always add this offset to your coordinates
            TPoint _offset;
            // holds the current clipping rectangle
            TRect _clipRect;
            
            // TODO: Use auto_ptr? Feasible with symbian classes?
            CFbsBitmap* _brushBitmap;
            CFbsBitmap* _drawBitmap;  
            
        private:
            // These are used to unlink the 
            static const Gfx::Pen _defaultPen;
            static const Gfx::Brush _defaultBrush;
            static const Gfx::Font _defaultFont;
                
    };
    
    class Drawable;
    
    /**
     * @brief Concrete Painter implementation used when drawing into Drawables.
     */
    class ConcretePainter : public PainterImpl
    {
        public:
            /**
             * @brief Construct Painter.
             * 
             * @param drawable Drawable to draw to.
             */
            ConcretePainter(Drawable& drawable);

            /**
             * @brief Destruct Painter.
             */
            virtual ~ConcretePainter();

            /**
             * @brief Activate painter by applying graphic context information.
             */
            virtual void begin();

            /**
             * @brief Deactivate painter.
             */
            virtual void end();
            
        private:
            Drawable& _drawable;
            
            // flag to indicate if context is active.
            bool _active;            
    };

} // namespace Gui

} // namespace Pt
#endif
