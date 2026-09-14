/*
  Copyright (C) 2026 Marc Boris Duerner

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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the:
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301 USA
*/

#ifndef PT_FORMS_API_STYLING_H
#define PT_FORMS_API_STYLING_H

/** @addtogroup Pt-Forms-Styling

    @brief Styles, options, and renderers for widget appearance.

    A Forms application has a %Style and %StyleOptions. The style is the
    look of the controls. The options are shared colors, pens, brushes,
    and the font. %Application owns both and starts with %PlatinumStyle.

    Use %Application::setStyle() or %Application::setStyleOptions() to
    change the global theme. Those methods reset shared renderer facets
    and invalidate widgets. Widgets rebuild in %onInvalidate(). A widget
    can overlay local options or assign a custom renderer without
    replacing the style.

    %StyleOptions stores tokens that every style can honor: colors, pens,
    brushes, and the font. Look-specific metrics stay in the derived
    style or renderer. Default construction is an empty overlay.
    %StyleOptions::defaults() fills the built-in tokens. The application
    options are const. Whether a fill or frame is on is a widget
    setting, not a style option. Use %Panel::setBackground(false) to
    turn a fill off.

    A %Renderer implements layouting and painting for one control family.
    Each method names the layer it measures, lays out, or paints, such as
    %ButtonRenderer::measureFrame() or %ButtonRenderer::renderChrome().
    There is no unqualified measure(), layout(), or render(). Renderer
    methods take prepared rectangles, sizes, scalars, enums, typed visual
    state, and text or pixmap values. They do not take the widget, a
    model object, a collection, or a temporary layout object. A small
    parameter struct is appropriate only when the same prepared group is
    reused across methods.

    Named measure methods run inside-out by layer: content, then frame.
    Named layout methods run outside-in: the frame method returns the
    inner content rectangle, then content partitions it. Named render
    methods paint those prepared rectangles. Only the full-control
    background or chrome method takes the widget bounds. The widget owns
    geometry and calls the named methods in order. The renderer does not
    store or mutate widget geometry. @ref Pt-Forms-Page-Updating
    describes the cycle.

    A layer method may paint the layer as a whole. Derived renderers that
    use a platform theme API override that layer method. Derived
    renderers that draw parts override the part methods. The base layer
    method paints the parts in order.

    A part is a public primitive when another control can reuse it
    without the original widget's layout or hit-testing. Parts that a
    style may merge with surrounding chrome are integrated subparts.
    %SliderRenderer track and handle are public primitives.
    %SpinBoxRenderer step controls are integrated subparts.

    Typed visual state matches the layer. Container state carries
    enabled, focused, and container highlight. Item, tab, or cell state
    carries selected, current, checked, or pressed. Visual state is not
    passed as mutable brush, pen, or font out-parameters.

    Family renderers provide a text painter whose font and text color
    already follow the style. A widget does not construct a %Painter for
    that themed text. %Style::Facet::onReset() is the point at which a
    renderer reads %StyleOptions and stores drawing state. Render methods
    use that prepared state plus the visual-state snapshot.

    %renderBackground() paints the whole control. An inner fill uses a
    layer name such as entry background. %PlatinumStyle backgrounds are
    empty unless a local %BackgroundOption is set.

    %Styler binds a control to the current style. Call %Styler::bind()
    from %onInvalidate() after the base implementation. Layouting and
    painting call typed methods on the derived styler. Derive a %Style
    to install a different look. Derive a %Renderer to draw a control
    family. Derive a %Styler only when adding a new control family.

    @code
    Pt::Forms::StyleOptions options = Pt::Forms::StyleOptions::defaults();
    options.set(
        Pt::Forms::AccentColorOption(Pt::Gfx::Color(20, 140, 250))
    );

    Pt::Forms::Application& app = Pt::Forms::Application::instance();
    app.setStyleOptions(options);

    Pt::Forms::PushButton send;
    send.setText("Send");
    send.setForeground(Pt::Gfx::Brush(Pt::Gfx::Color(20, 140, 250)));
    @endcode
*/

#endif
