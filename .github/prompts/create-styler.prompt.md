---
name: "create-styler"
description: "Implement an XStyler slice for a Pt::Forms widget"
argument-hint: "Widget class, e.g. Menu"
agent: "agent"
---

Implement a styler slice for `${input:widgetClass:Enter the widget class, for example Menu}`. Treat the supplied class as `XWidget` and derive `XState`, `XRenderer`, and `XStyler` from that name. Implement the slice; do not write a migration plan.

The following references are authoritative and override older wording:

- `.github/instructions/pt-forms-dev-styles.instructions.md`
- `include/Pt/Forms/Styler.h`, `src/Pt-Forms/Styler.cpp`
- `include/Pt/Forms/ButtonStyler.h`, `src/Pt-Forms/ButtonStyler.cpp`
- `include/Pt/Forms/PanelStyler.h`, `src/Pt-Forms/PanelStyler.cpp`
- `src/Pt-Forms/PushButton.cpp`, `src/Pt-Forms/Panel.cpp`, `src/Pt-Forms/Label.cpp`

Use `PlatinumButtonRenderer` / `PlatinumPanelRenderer` as the `onReset` reference. Do not introduce a compatibility typedef. Document new public declarations with `@brief` only.

## Files

- Create `include/Pt/Forms/XStyler.h` and `src/Pt-Forms/XStyler.cpp`.
- Put `XState`, `XRenderer`, and `XStyler` in those files. Do not split them.
- Register both files in `src/Pt-Forms/Jamfile`, `src/Pt/Pt.vcxproj`, and `src/Pt/Pt.vcxproj.filters`.
- If the widget already has a matching slice, wire that slice instead of duplicating it.

## XStyler

- `XStyler` derives from `Styler`.
- Own a `StyleOptions` overlay and a typed `FacetPtr<XRenderer>`.
- Expose `setRenderer(XRenderer* = 0)`, typed `options()` accessors, option getters/setters, and a typed forwarding method for every `XRenderer` operation the widget uses in invalidate, measure, layout, or paint.
- Do not expose a public `renderer()` accessor. Do not downcast from `Styler`. `bind` is only a change signal.
- `onBindOptions(global)` must bind the overlay with `_options.bind(&global)` and return the overlay. Do not reset it.
- `onStyleRenderer(style)` must return the shared `style.get<XRenderer>()`; do not clone or reset it.
- `onCreateRenderer(style)` must return a `create()` clone, or `0`.
- `setRenderer` stores the typed pointer and calls `init(renderer)`. A null pointer falls back to the current style on the next `bind`.
- Option getters resolve through `_options.get<T>().value()`. Setters store a local token with `_options.set(...)` and must not invalidate or bind.
- Preserve partial font overrides: `setFontSize`, `setFontWeight`, and `setFontSlant` start from `_options.findLocal<FontOption>()` when present, change only the requested attribute, and store the result.
- Forwarding methods delegate to the private typed renderer when available and otherwise provide a defined safe fallback. A forwarding method may return a nullable resource when an unavailable renderer has no meaningful replacement, such as a text painter.

## XRenderer

- Derive from `Renderer` using `Renderer(typeid(XRenderer), refs)`. Do not derive from `Style::Facet` directly.
- Implement `onReset(const StyleOptions&)` as the sole pure-virtual hook, using the combined bag: the overlay with its parent. Do not add `prepare()` / `onPrepare()`.
- Provide typed `create()` / `onCreate()`.
- Use explicit layer names. Do not add naked `measure()`, `layout()`, or `render()`.

## Widget

- `XStyler` owns the overlay. Remove any widget-owned overlay bag.
- Keep the widget's public style-option API as a thin facade: getters call the styler; setters call the styler then `invalidate()`.
- Do not access `styler.options()` from widget style getters or setters and do not add an `Application::styleOptions()` fallback.
- Keep widget policy flags on the widget, not in the overlay.
- `setRenderer` must call `styler.setRenderer(renderer)`, immediately `bind(style, options)`, invalidate widget-owned renderer-dependent caches, then `invalidate()`.
- `onInvalidate` must call the base implementation, then `bind(style, options)`. When `bind` returns true, refresh widget-owned renderer-dependent caches. Always request relayout.
- In invalidate, measure, layout, and paint, call typed `XStyler` forwarding methods. Do not declare an `XRenderer*` local or call `renderer->...` from the widget.
- Preserve existing geometry, icon caches, paint-layer order, and policy flags.

## Platinum

- Add a `PlatinumXRenderer` in `PlatinumStyle` when the widget needs a theme renderer.
- Use `onReset(const StyleOptions&)` and resolve tokens as `options.get<T>().value()`.

Final validation: `jam.bat -q -j4` from the repository root, exit code 0. No unit tests are required.
