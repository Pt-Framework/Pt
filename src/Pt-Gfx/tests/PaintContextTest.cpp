/* Copyright (C) 2026 Marc Boris Duerner

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

#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/PaintContext.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/PaintSurface.h>

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

namespace Pt {

namespace Gfx {

namespace {

class TestCanvas : public Canvas
{
    public:
        TestCanvas()
        : _hasClip(false)
        , _clip()
        {
        }

        bool hasClip() const
        {
            return _hasClip;
        }

        const RectF& clip() const
        {
            return _clip;
        }

    protected:
        virtual void onBeginPaint(const Gfx::Paint& /*paint*/) override
        {
        }

        virtual void onFinishPaint() override
        {
        }

        virtual void onSetTransform(const Gfx::Transform& /*tx*/) override
        {
        }

        virtual void onApplyTransform() override
        {
        }

        virtual void onSetCompositionMode(const Gfx::CompositionMode& /*mode*/) override
        {
        }

        virtual void onApplyCompositionMode() override
        {
        }

        virtual void onSetPen(const Pen& /*pen*/) override
        {
        }

        virtual void onApplyPen() override
        {
        }

        virtual void onSetBrush(const Brush& /*pen*/) override
        {
        }

        virtual void onApplyBrush() override
        {
        }

        virtual void onSetFont(const Gfx::Font& /*font*/) override
        {
        }

        virtual void onApplyFont() override
        {
        }

        virtual void onSetClip(const Gfx::RectF* clip) override
        {
            _hasClip = clip != 0;

            if( clip )
                _clip = *clip;
            else
                _clip.clear();
        }

        virtual void onApplyClip() override
        {
        }

        virtual void onDrawLine(const PointF& /*from*/, const PointF& /*to*/) override
        {
        }

        virtual void onDrawPolyline(const Gfx::PointF* /*pts*/, const size_t /*n*/) override
        {
        }

        virtual void onFillPolygon(const Gfx::PointF* /*ps*/, const size_t /*n*/) override
        {
        }

        virtual void onDrawRect(const Gfx::RectF& /*rectangle*/) override
        {
        }

        virtual void onFillRect(const Gfx::RectF& /*rectangle*/) override
        {
        }

        virtual void onDrawEllipse(const Gfx::PointF& /*topLeft*/, const Gfx::SizeF& /*size*/) override
        {
        }

        virtual void onFillEllipse(const Gfx::PointF& /*topLeft*/, const Gfx::SizeF& /*size*/) override
        {
        }

        virtual void onSetPath(const Path& /*path*/) override
        {
        }

        virtual void onDrawPath() override
        {
        }

        virtual void onFillPath() override
        {
        }

        virtual void onDrawPath(const Path& /*path*/) override
        {
        }

        virtual void onFillPath(const Path& /*path*/) override
        {
        }

        virtual const Gfx::FontMetrics& onGetFontMetrics() const override
        {
            static const FontMetrics empty;
            return empty;
        }

        virtual Gfx::TextMetrics onGetTextMetrics(const Pt::String& /*text*/) const override
        {
            return TextMetrics();
        }

        virtual void onDrawText(const Gfx::PointF& /*to*/,
                                const Pt::String& /*text*/,
                                const Gfx::Transform* /*transform*/) override
        {
        }

        virtual void onDrawImage(const Gfx::PointF& /*to*/,
                                 const Gfx::Image& /*image*/,
                                 const Gfx::RectF* /*rect*/ = 0) override
        {
        }

    private:
        bool  _hasClip;
        RectF _clip;
};


class TestSurface : public PaintSurface
{
    public:
        TestSurface()
        : _canvas(0)
        , _size(100.0, 100.0)
        {
        }

        TestCanvas& canvas() const
        {
            return *_canvas;
        }

    protected:
        virtual const Gfx::ImageFormat& onGetFormat() const override
        {
            return ImageFormat::argb32();
        }

        virtual const Gfx::SizeF& onGetSize() const override
        {
            return _size;
        }

        virtual const Scaling& onGetScaling() const override
        {
            return _scaling;
        }

        virtual Gfx::Canvas* onCreateCanvas(Gfx::Canvas* reuse) override
        {
            if(reuse)
            {
                _canvas = static_cast<TestCanvas*>(reuse);
                return reuse;
            }

            _canvas = new TestCanvas();
            return _canvas;
        }

        virtual void onReleaseCanvas() override
        {
        }

        virtual void onSync() override
        {
        }

        virtual void onFinish() override
        {
        }

    private:
        TestCanvas* _canvas;
        SizeF       _size;
        Scaling     _scaling;
};


void assertRectEqual(const RectF& actual, const RectF& expected)
{
    PT_UNIT_ASSERT_NEAR(actual.x(), expected.x());
    PT_UNIT_ASSERT_NEAR(actual.y(), expected.y());
    PT_UNIT_ASSERT_NEAR(actual.width(), expected.width());
    PT_UNIT_ASSERT_NEAR(actual.height(), expected.height());
}

} // anonymous namespace


class PaintContextTest : public Pt::Unit::TestSuite
{
    public:
        PaintContextTest()
        : Pt::Unit::TestSuite("Pt::Gfx::PaintContextTest")
        {
            registerMethod("InitialClip", *this, &PaintContextTest::InitialClip);
            registerMethod("IntersectOnBegin", *this, &PaintContextTest::IntersectOnBegin);
            registerMethod("IntersectAfterBegin", *this, &PaintContextTest::IntersectAfterBegin);
            registerMethod("ResetClipFallsBackToContext", *this,
                           &PaintContextTest::ResetClipFallsBackToContext);
            registerMethod("EmptyIntersectionKeepsClip", *this,
                           &PaintContextTest::EmptyIntersectionKeepsClip);
        }

        void InitialClip()
        {
            TestSurface surface;
            RectF clip(PointF(1.0, 2.0), SizeF(7.0, 5.0));
            PaintContext context(surface, clip);

            Painter painter(context);

            PT_UNIT_ASSERT(surface.canvas().hasClip());
            assertRectEqual(surface.canvas().clip(), clip);
        }

        void IntersectOnBegin()
        {
            TestSurface surface;
            RectF contextClip(PointF(0.0, 0.0), SizeF(10.0, 10.0));
            RectF painterClip(PointF(5.0, 4.0), SizeF(8.0, 7.0));
            RectF expected = painterClip.intersect(contextClip);
            PaintContext context(surface, contextClip);

            Painter painter;
            painter.setClip(painterClip);
            painter.begin(context);

            PT_UNIT_ASSERT(surface.canvas().hasClip());
            assertRectEqual(surface.canvas().clip(), expected);
        }

        void IntersectAfterBegin()
        {
            TestSurface surface;
            RectF contextClip(PointF(0.0, 0.0), SizeF(10.0, 10.0));
            RectF painterClip(PointF(6.0, 3.0), SizeF(8.0, 8.0));
            RectF expected = painterClip.intersect(contextClip);
            PaintContext context(surface, contextClip);

            Painter painter(context);
            painter.setClip(painterClip);

            PT_UNIT_ASSERT(surface.canvas().hasClip());
            assertRectEqual(surface.canvas().clip(), expected);
        }

        void ResetClipFallsBackToContext()
        {
            TestSurface surface;
            RectF contextClip(PointF(2.0, 1.0), SizeF(9.0, 6.0));
            RectF painterClip(PointF(4.0, 3.0), SizeF(2.0, 2.0));
            PaintContext context(surface, contextClip);

            Painter painter(context);
            painter.setClip(painterClip);
            painter.resetClip();

            PT_UNIT_ASSERT(surface.canvas().hasClip());
            assertRectEqual(surface.canvas().clip(), contextClip);
        }

        void EmptyIntersectionKeepsClip()
        {
            TestSurface surface;
            RectF contextClip(PointF(0.0, 0.0), SizeF(4.0, 4.0));
            RectF painterClip(PointF(10.0, 10.0), SizeF(3.0, 3.0));
            PaintContext context(surface, contextClip);

            Painter painter(context);
            painter.setClip(painterClip);

            PT_UNIT_ASSERT(surface.canvas().hasClip());
            PT_UNIT_ASSERT(surface.canvas().clip().isNull());
        }
};


Pt::Unit::RegisterTest<PaintContextTest> _registerPaintContextTest;

} // namespace Gfx

} // namespace Pt