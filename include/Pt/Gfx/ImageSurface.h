/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef PT_GFX_ImageSurface_H
#define PT_GFX_ImageSurface_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/PaintLayer.h>
#include <Pt/Gfx/Paint.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/System/Path.h>

#include <string>
#include <vector>
#include <cstddef>

namespace Pt {

namespace Gfx {

class RasterCanvas;

/** @brief Image drawing surface.
*/
class PT_GFX_API ImageSurface : public PaintSurface
{
  public:
    ImageSurface();

    ImageSurface(Pt::ssize_t width, Pt::ssize_t height, 
                 std::size_t stride = 0);

    virtual ~ImageSurface();

    void reset(const Gfx::Image& image);

    void reset(Pt::ssize_t width, Pt::ssize_t height, 
               std::size_t stride = 0);

    const Gfx::Image& image() const;

    const Gfx::Size& size() const;

    void setScaleFactor(double scaleFactor);

  public:
    static void setFontDir(const System::Path& path);

    static const std::string& defaultFont();

    static void setDefaultFont(const std::string& name);

    static std::vector<std::string> fontNames();

  private:
    RasterCanvas* _canvas;
};

/** @brief Image drawing layer.
*/
class PT_GFX_API ImageLayer : public PaintLayer
{
    public:
        ImageLayer();

        ImageLayer(Pt::ssize_t width, Pt::ssize_t height, 
                   std::size_t stride = 0);

        virtual ~ImageLayer();

        void reset(const Gfx::Image& image);

        void reset(Pt::ssize_t width, Pt::ssize_t height, 
                   std::size_t stride = 0);

        const Gfx::Image& image() const;

        const Gfx::Size& size() const;

        void setScaleFactor(double scaleFactor);

    protected:
        virtual void onDraw(PaintSurface& surface,
                            const Paint& paint,
                            const Gfx::PointF& to,
                            const Gfx::RectF* rect) const override;
    private:
        ImageSurface _surface;
};

} // namespace

} // namespace

#endif

