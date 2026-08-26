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
- Provide typed `renderer()` and `options()` accessors. Do not downcast from `StylerBase` or use the return value of `bind`.
- Cache the shared or cloned renderer in the typed pointer used by `renderer()`.
- Use `StyleOptions::hasOptions()` to determine whether the local overlay requires an override clone.
- `StylerBase` prepares only a custom renderer or override clone (`_isRenderer || _isOverride`) through `Renderer::prepare(localOptions)`. Synchronize shared prototypes only through `Style::reset` to `onReset`.
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
- `setRenderer` must call `styler.setRenderer(renderer)`, immediately `bind(style, options)`, then `invalidate()`.
- `onInvalidate` must call the base implementation, then `bind(style, options)`, obtain the typed renderer, update caches, and request relayout. Do not use `rebind`.

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
- Include updating `.github/instructions/pt-forms-dev-styles.instructions.md` for this slice to describe `XStyler : StylerBase`.
- Include searches within the migrated slice and its concrete theme renderers for the old `XStyle` type, widget-owned overlay, `rebind(`, and two-argument `prepare` / `onPrepare` calls.
- Include `jam.bat -q -j4` from the repository root as the final validation step and require exit code 0.