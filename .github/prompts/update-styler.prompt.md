---
name: "update-styler"
description: "Plan a migration to the StylerBase API"
argument-hint: "Styler class to migrate, e.g. CheckboxStyler"
agent: "Planner"
---

Prepare an implementation plan to migrate `${input:stylerClass:Enter the XStyler class to migrate, for example CheckboxStyler}` from `Styler<XRenderer, StyleOptions>` to `StylerBase`, following `ButtonStyler`/`PanelStyler` and `PushButton`/`Panel`/`Label`. Treat the supplied class as `XStyler` below and derive `XRenderer`, `XStyle`, and related widget names accordingly. Preserve the existing `XStyle.h` / `XStyle.cpp` file names, do not introduce a compatibility typedef, and require `@brief` documentation only for declarations changed by this migration.

The following references are authoritative and override older wording in instruction files:
include/Pt/Forms/StylerBase.h, src/Pt-Forms/StylerBase.cpp
include/Pt/Forms/ButtonStyle.h, src/Pt-Forms/ButtonStyle.cpp
include/Pt/Forms/PanelStyle.h, src/Pt-Forms/PanelStyle.cpp
src/Pt-Forms/PushButton.cpp, src/Pt-Forms/Panel.cpp, src/Pt-Forms/Label.cpp
Use `PlatinumButtonRenderer` / `PlatinumPanelRenderer` as the `onPrepare` reference.

`StylerBase` API contract:
- The only bind path is `bind(style, options)`.
- `onBindOptions(global)` must bind the overlay to the global options with `_options.bind(&global)` and return the overlay. Do not prepare it.
- `onStyleRenderer(style)` must return the shared `style.get<XRenderer>()`; do not clone or prepare it.
- `onCreateRenderer(style)` must return a `create()` clone, or `0`.
- `setRenderer(XRenderer* = 0)` must store the typed pointer and call `init(renderer)`. A null pointer falls back to the style.
- Do not expose an `XStyler::renderer()` accessor or return a renderer pointer from any other public `XStyler` API. Provide the typed `options()` accessors only. Do not downcast from `StylerBase`; the return value of `bind` is only a change signal and never exposes a renderer.
- Cache the shared, cloned, or specific renderer in a private typed pointer. Only `XStyler` may access that pointer directly.
- For every `XRenderer` operation used by the widget's invalidation, measurement, layout, or painting phases, provide a matching, typed `XStyler` forwarding method. Preserve the `XRenderer` operation's parameter and return shapes; do not introduce unqualified composite wrappers. The forwarding methods delegate to the private typed renderer pointer when available and otherwise provide a defined safe fallback. The migrated widget must call the typed styler methods unconditionally and must not observe the styler binding state. Exception: a forwarding method may return a nullable resource when an unavailable renderer has no meaningful replacement, such as a renderer-configured text painter. The widget may handle that documented nullable result but must not query `isBound()` or otherwise inspect the binding state.
- `StylerBase` determines whether the local overlay requires an override clone by comparing it with the global options: `localOptions.isDefault(styleOptions)` sets `_isDefaultOptions`. When `_isDefaultOptions` is true, the shared style renderer is used; otherwise an override clone is created.
- `StylerBase` prepares only a specific renderer or override clone (`_isRenderer || ! _isDefaultOptions`) through `Renderer::prepare(localOptions)`. Shared style prototypes are not prepared during bind; they are synchronized through `Style::reset` calling `Renderer::onReset(options)`.
- `StylerBase::bind(style, options)` returns `true` when the effective renderer binding or effective options changed and renderer-dependent widget caches may need refreshing. It may return `true` even when no renderer can be obtained; typed styler forwarding methods encapsulate that state and remain safe to call, except for documented nullable resource results.
- Move the widget's typed style-option accessors and mutators to `XStyler`. The styler getters resolve effective values through `_options.get<T>().value()`; its setters create and store the matching local option token through `_options.set(...)` without invalidating or binding.
- Preserve partial font overrides in `XStyler`: `setFontSize`, `setFontWeight`, and `setFontSlant` begin with `_options.findLocal<FontOption>()` when present, change only the requested attribute, and store the result.

Renderer:
- `XRenderer` derives directly from `Renderer` using `Renderer(typeid(XRenderer), refs)`; do not derive directly from `Style::Facet`.
- Use `prepare(const StyleOptions&)` / `onPrepare(const StyleOptions&)` and one combined bag: the overlay with its parent.
- Implement `onReset(options)` as `prepare(options)`. Do not use an empty overlay bag.
- Provide typed `create()` / `onCreate()`.

Widget:
- Remove the overlay from the widget. `XStyler` owns its `StyleOptions`.
- Keep the widget's existing public style-option API as a thin facade: getters call the corresponding typed `XStyler` getter; setters call the typed `XStyler` setter followed by `invalidate()`.
- Do not access `styler.options()` from widget style getters or setters and do not add a second `Application::styleOptions()` fallback.
- Implement `font()` by returning `styler.font()`; the overlay's `FontOption` is materialized against the inherited global font by `StyleOptions::bind()`.
- Keep widget policy flags, such as `_hasBackground`, on the widget rather than converting them into tokens.
- `setRenderer` must call `styler.setRenderer(renderer)`, immediately `bind(style, options)`, invalidate every widget-owned renderer-dependent cache, then `invalidate()`.
- `onInvalidate` must call the base implementation, then `bind(style, options)`. When `bind` returns `true`, invalidate every widget-owned renderer-dependent cache before refreshing it. The widget must not check `styler.isBound()`; a styler forwarding method must safely clear, preserve, or return the appropriate fallback for unavailable renderer data. A documented nullable forwarding result may be handled as an unavailable resource. Always request relayout. Do not use `rebind`.
- In all widget invalidation, measure, layout, and paint paths, call the corresponding typed `XStyler` forwarding method rather than accessing an `XRenderer` pointer. Do not check `styler.isBound()` or otherwise expose the styler binding state in the widget. A widget may only test a documented nullable result from a typed forwarding method; it must not infer or query the binding state by another means. The widget must not declare an `XRenderer*` local, call `styler.renderer()`, or invoke `renderer->...` directly.
- Preserve the existing lifecycle phase order, widget-owned geometry, icon caches, paint-layer order, and widget policy flags while moving only the renderer boundary into `XStyler`.

Platinum and concrete theme renderers:
- Use `onPrepare(const StyleOptions&)` and remove two-bag resolvers.
- Resolve tokens as `options.get<T>().value()`.
- Resolve fonts as `options.get<FontOption>().value()`. The `FontOption` materializes partial overrides against the inherited global font when the overlay is bound, so renderers do not need a separate base-font lookup or `getFont()` call.

Do not change:
- `Jamfile`, `.vcxproj`, or file names.
- Other `Styler<>` slices that have not yet been migrated.
- Geometry, icon caches, paint order, or policy flags.

The plan must:
- Identify every implementation file and symbol that needs to change for this slice, with the intended change for each.
- Include searches within the migrated slice and its concrete theme renderers for the old `XStyle` type, widget-owned overlay, `rebind(`, two-argument `prepare` / `onPrepare` calls, `styler.renderer()`, `XRenderer*` locals in the widget, and direct `renderer->` calls.
- Include `jam.bat -q -j4` from the repository root as the final validation step and require exit code 0.