---
description: "Guidelines and current architecture for Pt::Forms styles, renderers, shared style reset, extracted ButtonStyler/PanelStyler slices, and widget integration in PushButton, Panel, and Label."
---

# Forms Style Architecture

## Zero Abstraction Leakage
- Avoid passing complex temporary layout state objects or "magic" data structures to renderers.
- Flatten API parameters: pass primitives and explicitly calculated bounds (e.g., `const Gfx::RectF& textRect`) directly into the rendering functions.
- Renderer APIs must not mutate widget-owned geometry, collections, or model objects. Layout methods return sizes/rectangles; the widget applies them to its own storage.

## Target Architecture vs. Legacy
- New or refactored renderer APIs must accept only primitives, pre-calculated bounds, scalars, enums, and typed style flags.
- Legacy widget-bound renderer APIs may remain for compatibility, but do not copy that pattern into new facets.
- Legacy `prepare(...)` / `prepareLayout(...)` patterns that fill mutable `Brush`, `Pen`, or `Font` out-parameters are transitional only. Do not introduce that shape in new APIs.
- When touching a legacy renderer that still takes the widget itself, prefer moving the touched slice to the primitive-only model instead of extending the widget-bound contract with new methods, new out-parameters, or new widget leaks.

## Parameter Shape
- Prefer flat primitive parameters as the default API shape.
- Introduce a small POD parameter struct only when the same pre-calculated group of rects, scalars, and flags is reused together across multiple methods.
- Never use such structs to smuggle widgets, text layout engines, mutable helper objects, or temporary layout state into the renderer.

## Measure, Layout, and Render Cycle
- Renderers strictly separate the measuring, layouting, and rendering phases.
- **Naming Convention:**
    - **No naked generic methods:** We strictly avoid unqualified composite methods like `measure()`, `layout()`, or `render()`.
    - **Always use explicit layer names:** Every method MUST explicitly name the layer it operates on (e.g., `measureFrame()`, `layoutContent()`, `renderIndicator()`). This removes ambiguity and "god-functions".
- **Measure:** Calculate intrinsic sizes using symmetrical inside-out layers. First measure inner content (e.g., `measureContent`), then pass it to outer decoration layers (e.g., `measureFrame`).
- **Layout:** Layouts work outside-in. Outer layers subtract padding/borders and return an inner `contentRect` (e.g., `layoutFrame`). Inner layers then partition that rect (e.g., `layoutContent`).
- **Render:** Drawing functions must take the precise, pre-calculated sub-rectangles (like `textRect` or `frameRect`). Only methods that draw the full widget background or an integrated full-control layer may take the full control bounds.

## Layered Rendering and Feature Extraction (Look Borrowing)
- **Render Layer Design**: Widgets paint themselves in explicit sequential layers (e.g., background/chrome, then text, then icons/mnemonics) rather than using one monolithic `render()` call. Each layer uses precisely partitioned `RectF` sub-rectangles from the `layout()` phase.
- **Extracted Primitives over Composites**: When designing renderers, we enforce extracted primitives over monolithic composite functions:
    - *No "God Methods"*: Do **not** provide naked `render()` methods, huge generic `layout(...)` methods, or catch-all names like `renderControl()` on new facets. The widget is responsible for orchestrating the measure/layout/render sequence by explicitly calling the individual renderer layers (e.g., `layoutFrame` then `layoutContent`) in sequence.
    - *Extracted Primitives*: Individual visual elements (like `renderEntry`, `renderIndicator`, or `renderFrame`) are made available as mandatory virtual hooks. Style implementers override these fine-grained primitives.
- **Exception: The Composite Fallback Pattern (Layer Template Method)**: While custom-drawn themes must strictly implement fine-grained primitives, native OS rendering APIs (e.g., Windows UxTheme, macOS CoreUI) often treat complex controls as indivisible, opaque visual units. To bridge this gap:
  - **Base Facets** provide a top-level `onRender<Layer>` virtual method (e.g., `onRenderFrame`).
  - **Default Implementation**: The base class orchestrates and explicitly calls the pure virtual sub-primitives (`onRenderEntry`, `onRenderButton`).
  - **Custom Themes**: Only override the purely visual primitives, relying on the base layer method to piece them together.
  - **Native OS Themes**: Override the top-level `onRender<Layer>` to draw the entire control as one monolithic block via OS APIs. The primitives can be left empty if conceptually dead.
  - **Rule of Thumb**: Only introduce a composite `onRender<Layer>` fallback if the target OS API strictly prevents primitive decomposition or handles layout inseparable from rendering.
    - **Important**: A composite `render<Layer>()` or `onRender<Layer>()` entry point is compatible with the "no naked generic methods" rule because it still names a specific layer.
- **Text & Info**: Isolate `renderText`, `renderIcon`, and `renderMnemonic`.
  - `renderText` takes the exact `textRect`, ensuring pixel-perfect clipping.
  - Keep `renderText` and `renderMnemonic` completely decoupled. Do not pass combined states or artificial empty rectangles to them.

### Borrowable Primitives vs. Integrated Subparts
- Expose a visual primitive as a public renderer method only when custom widgets can plausibly reuse it by itself without inheriting hidden widget semantics.
- Keep tightly integrated subparts protected behind a composite layer when their layout, borders, hit areas, or visual state are inseparable from the surrounding control.
- Use `SpinBox` as the integrated-subpart model: up/down buttons are not public primitives because styles may merge them with the entry frame or place them inside the entry.
- Use `Slider` as the borrowable-primitive model: the handle and track can be public primitives because custom widgets such as scrubbers, timelines, mixers, range editors, and color controls may reasonably reuse them.
- Preserve a public composite `render<Layer>` entry point whenever native frameworks or OS theme APIs usually draw the control as one high-fidelity unit. Public primitives support custom drawing reuse; the composite supports native fidelity.

## Collection and View Controls
- For collection/view widgets with repeated visuals (rows, cells, tabs, items), separate container chrome from repeated-element rendering whenever the repeated element visuals are plausibly borrowable on their own.
- Prefer reusing `PanelRenderer` or another existing generic container renderer for pane/background/frame chrome. Keep a dedicated container renderer only when container-specific chrome, metrics, or native integration genuinely differ.
- Repeated-element renderer APIs may accept only flat content inputs such as rects, text, icons, scalars, typed flags, or a small POD snapshot composed from those values. Do not pass the repeated-element widget, model object, or mutable vectors/collections into the renderer.
- Layout ownership stays with the widget. A renderer may compute a rect for one row/tab/cell, but it must not mutate a widget-owned array, collection, or element object to store the result.
- `TabView` is expected to own the tab collection, current-selection state, tab geometry, tab measurement/layout, and tab input handling itself.
- Keep the tab-strip logic private to `TabView`; do not reintroduce a separate `TabBar` helper or widget unless a new requirement proves that the logic cannot stay local.
- If repeated tab visuals ever need to become independently borrowable later, extract only renderer-level primitives first; do not start by reintroducing a child control or duplicated style state.

## State Ownership by Layer
- Typed style flags must match the layer that is being rendered.
- Container flags carry container semantics such as enabled, disabled, focused, and only container-level highlighted states.
- Repeated-element semantics such as selected, current, checked, pressed, or row-highlighted belong to the corresponding item/tab/cell flags, not to the outer container flags.
- Do not encode visual state indirectly through mutable drawing-attribute out-parameters. State must be explicit in flags or in immutable primitive inputs.

## Painter and Attribute Override Management
- **Painters in Renderer**: Widgets do *not* instantiate `Painter` objects internally for generic drawing (like text). They must request it through the Renderer (e.g., `renderer->textPainter(surface)`). This guarantees that the styling engine's active fonts, text colors, and antialiasing states are applied correctly.
- **Drawing Attributes in Widget**: One `%StyleOptions` class is used for both the complete global options and a sparse widget overlay. The default constructor is an empty bag. `%StyleOptions::defaults()` inserts the built-in contract tokens. Each token is a concrete option type (`%ForegroundOption`, `%FontOption`, ...). `%Application` owns the live global instance constructed from `%defaults()`. On `%StylerBase` slices the derived styler owns the overlay. On template `%Styler` slices the widget owns it and passes it into the styler. The renderer resolves tokens during `prepare(...)` or shared-facet `onReset(...)`. Overlay-only presence is `%findLocal<T>()` / `%set` / `%reset<T>()`; `%findLocal<T>()` may be 0. Do not call `%value()` on a missing local option. After the parent is set, migrated getters use `%options.get<T>().value()` because the parent supplies the contract tokens.
- **`%StyleOptions` Parent Lookup**: `%StyleOptions` supports a parent chain via `%setParent(const StyleOptions*)` and `%parent()`. There is no `%hasParent()`. When a parent is set, `%get<T>()` and `%find<T>()` search the local bag first and recurse to the parent when `T` is absent. `%findLocal<T>()` and `%hasOverrides()` remain local-only. `%generation()` adds the local generation to the parent generation so change detection sees theme updates. Use this on migrated slices to pass a single resolved options object to renderers instead of separate theme and overlay bags.
- On migrated `%StylerBase` slices, effective tokens are resolved through the parent chain: `%styler.options().get<T>()` returns the overlay token when present and otherwise the theme token. Font merge for partial overrides is still performed explicitly with `%FontOption::getFont(baseFont)` because `%FontOption` carries its own internal override bits. Detect a local font overlay with `%findLocal<FontOption>()`, not `%find<FontOption>()`. Other slices still resolve with a separate overlay bag plus `%get<T>()` until migrated. Do not push ad-hoc local overrides into shared renderers during every invalidate pass. Direct renderer mutation remains a legacy-only pattern or a private/custom-renderer-only pattern.
- **Persistent Painter State**: Cached painters keep their brush, pen, font, clip, and composition state across `begin()` calls. `onPrepare()` must establish the steady-state defaults for every cached painter, and temporary mutations inside `onRender*()` must either be restored before returning or be fully re-established on every later code path that uses that painter.

## Ownership and Cloning
- `onCreate()` must return a freshly allocated facet with `refs = 0`.
- Never pre-increment the reference count or return clones with `refs = 1`; `FacetPtr` and `Style::set()` take their own references.
- Treat `create()` as a prototype clone operation, not as a shared singleton accessor.

## Renderer Defaults and Tokens
- On legacy renderers that still expose getters such as `background()`, `foreground()`, `contour()`, `font()`, and `textColor()`, those values must resolve to either a local override or a meaningful default derived from `StyleOptions`.
- In extracted slices such as `ButtonRenderer` and `PanelRenderer`, resolve the equivalent defaults during `onPrepare(...)` and shared-facet `onReset(...)` instead of reintroducing public prepared-state getters only for theme access.
- Never return static empty placeholder objects for fonts, colors, pens, or brushes when a themed default exists.
- Shared semantic colors and metrics that repeat across a style family belong in `StyleOptions` or in a small centralized derivation layer, not as unrelated magic numbers in each renderer.

## Background Layers
- **Widget Control Background vs. Element Fill**: The term `renderBackground` is strictly reserved for the background of the *entire widget control* (its full bounding rect). Do not use `renderBackground` to refer to the inner fill of a specific visual element (e.g., use `renderEntryBackground` to fill the text entry area, not `renderBackground`).
- **Explicit Background Layer**: Provide a `renderBackground(PaintContext& ctx, const Gfx::RectF& rect, State)` step for transparent or panel-like widgets. The state parameter must match the touched slice: use dedicated slice state such as `ButtonState` or `PanelState` on migrated facets, and prefer widget-specific typed state over any shared cross-widget flag container on remaining legacy APIs.
- **Often Empty by Default**: In native themes (like Windows/Platinum), the implementation (`onRenderBackground`) is often intentionally empty, leaving the widget transparent to the parent container.
- **Special Cases Only**: These backgrounds are primarily filled only in special cases, such as when a custom background override is explicitly set on the widget via attributes (checked via `opts.find<BackgroundOption>()`).

## Widget Attribute Lifecycle and Renderer Management

Forms currently uses two renderer-management patterns. Preserve the established pattern of the touched widget and do not mix them.

### Direct Override Pattern

- Some widgets still keep renderer-local overrides directly on the widget and lazily clone a private renderer on the first override.
- In that pattern, public getters proxy through the active renderer first and fall back to global `StyleOptions` when no renderer-local override is available.
- Setters may lazily create a private renderer clone, push the override into that renderer immediately, and then call `invalidate()`.
- `onInvalidate()` may either acquire a private clone when local overrides exist or reuse the shared style prototype when no overrides exist.
- Never call individual `renderer->setXxx()` unconditionally during every invalidate pass. Either push a changed local override from the setter or perform one explicit bulk transfer when a private renderer is first acquired.

### Extracted Style Slice Pattern

- `%PushButton`, `%Panel`, and `%Label` do **not** use the direct override pattern above. Their styling flow is centered on a split between `%Style`, `%StyleOptions` (global options plus overlay), slice state, a slice renderer, and a slice styler.
- `%PushButton`, `%Panel`, and `%Label` use the non-template `%StylerBase` controller with `%ButtonStyler` / `%PanelStyler` `%setRenderer(...)` and a single `%bind(style, options)` path. Other extracted slices still use the template `%Styler<RendererT, OptionsT>` with `%bind` / `%rebind` until they are migrated. Instantiations use `%StyleOptions` as `OptionsT`.
- `%Style` is the registry of shared renderer facets for the active style. `%StyleOptions` is both the global token store and the widget overlay type. `%Application` owns the live global instance from `%StyleOptions::defaults()` and the live `%Style`.
- Global theme changes must run through `%Application::setStyle(...)` or `%Application::setStyleOptions(...)`. That path calls `%Style::reset(const StyleOptions&)`, which fans the global options out to each shared facet through `%Style::Facet::reset(...)` and protected `%onReset(...)`.
- Shared renderers fetched directly from `%Style` must treat `%onReset(...)` as their synchronization point for global defaults. Migrated `%Renderer` slices implement `%onReset(...)` by calling the single-argument `%prepare(...)` with the current global `%StyleOptions`. Legacy two-argument slices still call `%prepare(options, emptyOverlay)`.
- Each extracted slice separates responsibilities explicitly:
  - Overlay: a second `%StyleOptions` instance. Default-constructed means no overrides (`%hasOverrides()` is false). Local presence is `%findLocal<T>()`. Font overrides are a `%FontOption` stored in the bag.
  - `XState`: transient render state only.
  - `XRenderer`: prepared drawing state, measure/layout/render primitives, and optional icon preparation. Button and panel renderers derive from `%Renderer`.
  - `XStyler`: renderer-binding controller only.
  - Widget: owns control flow, geometry, text/icon caches, and the helper that produces `XState`. On `%StylerBase` slices the styler owns the overlay. On template `%Styler` slices the widget still owns the overlay.
- The overlay must not absorb transient interaction flags, and `XState` must not absorb style tokens. Keep local override data and transient render state separate. Do not reintroduce per-class `hasOverrides()`, `generation()`, `_generation`, or `_overrides` on a slice-specific options type.
- `XRenderer::prepare(const StyleOptions& options)` is the explicit synchronization point for extracted slices that have been migrated to parent lookup. The passed options already include the widget overlay via the parent chain; resolve tokens during `%onPrepare(...)` with `%options.get<T>()` and cache all data needed later by painters, measure/layout hooks, render hooks, and optional icon preparation. For partial font overrides, merge against the parent or theme base font: `%options.findLocal<FontOption>() ? options.get<FontOption>().getFont(base) : options.get<FontOption>().value()`.
- Legacy extracted slices still use `XRenderer::prepare(const StyleOptions& options, const StyleOptions& overlay)`. Resolve global defaults plus overlay tokens during `%onPrepare(...)`: `%options.get<T>().value()` for global tokens and `%overlay.find<T>() ? overlay.get<T>().value() : options.get<T>().value()`. Font access is `%overlay.find<FontOption>() ? overlay.find<FontOption>()->getFont(baseFont) : baseFont`. Migrate such slices to the single-parameter parent-lookup shape when touching them.
- Render hooks must consume prepared renderer state plus `XState` only. If a renderer needs information from `%StyleOptions`, resolve and cache it during `%onPrepare(...)` or shared-facet `%onReset(...)` instead of re-fetching it during render.
- `%StylerBase` owns the bound `%Renderer`, the style generation, the global-options and local-options generation fields, `_isRenderer` (custom renderer assigned through `%init`), and `_isOverride` (local `%StyleOptions::hasOverrides()`). It does not expose an untyped `%renderer()` accessor. The overlay lives in the derived styler.
- Measure/layout/render code should use the typed `XStyler::renderer()` accessor. Do not add an untyped `%renderer()` on `%StylerBase`. `%bind` returns `%Renderer*` for the base implementation only.
- `%StylerBase::bind(const Style&, const StyleOptions&)` is the only public bind path. It always calls `%onBindOptions(global)` first. The derived styler must `%setParent(&global)` on its overlay and return that overlay. When `%isStyleChanged(...)` is true, `%bind` keeps the current renderer if `_isRenderer` is set; otherwise it calls `%onStyleRenderer(style)` and, if that renderer exists and the overlay `%hasOverrides()`, `%onCreateRenderer(style)`. When a renderer is bound, it is custom or an override clone (`_isRenderer || _isOverride`), and `%isOptionsChanged(...)` is true, `%bind` calls `%Renderer::prepare(localOptions)`.
- `%isStyleChanged(...)` is true when no renderer is bound, the `%Style` generation changed, or local override mode flipped (`_isOverride` vs `%hasOverrides()`). `%isOptionsChanged(...)` is true when the global `%StyleOptions` generation or the overlay generation changed. There is no `%onIsStyleChanged()` or `%onIsOptionsChanged()` hook.
- After a style-path rebind, `%StylerBase` invalidates the cached options generations so a newly acquired renderer always goes through `%prepare(...)` when it is custom or an override clone.
- `%onStyleRenderer(const Style&)` returns the shared style renderer, or 0 if none is available. It must not prepare that renderer and must not clone it. `%onCreateRenderer(const Style&)` returns an independent `%create()` clone of that prototype, or 0. When `%init` assigned a custom renderer (`_isRenderer`), `%bind` keeps that renderer and does not call `%onStyleRenderer` / `%onCreateRenderer`.
- `%onBindOptions(const StyleOptions&)` returns the overlay after setting its parent. It does not call `%prepare`. Shared style prototypes stay synchronized through `%Style::reset()` / `%onReset(...)`.
- `%ButtonStyler` and `%PanelStyler` each cache a typed renderer pointer and return it from `%renderer()` without casting. No downcast from `%StylerBase`.
- `%XStyler::setRenderer(XRenderer*)` stores the typed pointer and calls `%init(renderer)`. A null pointer clears `_isRenderer` and falls back to the current style on the next `%bind(...)`. Do not compare stored renderer pointers for identity; addresses can be reused.
- `%ButtonStyler` and `%PanelStyler` do not track `_hasOverrides` or `_overridesGeneration`. Token-only changes with the same override mode are option changes and prepare the current renderer in place. Override-mode flips rebind.
- `%onStyleRenderer(...)` returns `%style.get<XRenderer>()`. `%onCreateRenderer(...)` returns `%create()` of that prototype, or 0.
- On remaining template `%Styler` slices, `%bind` / `%rebind` still follow the replace-versus-refresh split.
- In the style-path bind, clone and locally prepare a private renderer only when the overlay actually contains overrides. If there are no local overrides, bind the shared renderer from `%Style` directly and do not prepare it locally.
- When a bind or update path cannot obtain a renderer, keep the cached prepare generations invalid. Only store the current prepare generations after a successful `%Renderer::prepare(...)` call, or the legacy two-argument `%prepare` on template slices.
- Widgets that use an extracted slice should keep `setRenderer(T* renderer)` pointer-based when `nullptr` is part of the public API contract for falling back to the current style.
- Widgets that use `%StylerBase` call `%bind(style, options)` during `%onInvalidate()`. Widgets that still use the template `%Styler` call `%rebind(style, options, localOptions)`.
- In `%onInvalidate()`, call the base implementation first, reacquire the renderer through the slice styler, then refresh widget-owned derived caches such as icon pixmaps, and finally request relayout.

### Widget Slice

- A widget using the extracted slice pattern consists of a `%StyleOptions` overlay, a widget-specific `%XState`, `%XRenderer`, and `%XStyler`.
- `%XStyler` derives from `%StylerBase`, owns the `%StyleOptions` overlay, and exposes typed `%renderer()` and `%options()` accessors. The widget retains geometry, widget-owned caches, and render-time state.
- The overlay stores only widget-local override tokens appropriate to the widget, such as backgrounds, foregrounds, contours, accents, text colors, and font overrides. Unset tokens fall back to the global theme through the parent set in `%XStyler::onBindOptions(...)`. Keep widget policy flags, such as whether a background or frame is enabled, on the widget rather than in the overlay.
- `%XState` owns only transient, render-relevant interaction flags. It is the single source of truth for those flags; keep pure control-flow bookkeeping outside it.
- `%XRenderer` derives from `%Renderer` and provides only the prepared measure, layout, painter, cache-preparation, and render primitives that the widget needs. Follow the explicit layer naming and parameter-shape rules above. Reuse an existing generic renderer family when its primitives, metrics, and native integration fit; introduce a dedicated renderer family only when they genuinely differ.
- `%XRenderer::prepare(const StyleOptions& options)` receives the widget overlay with the global options as its parent. Resolve tokens with `%options.get<T>()`, merge partial font overrides explicitly against the base font, and have `%onReset(...)` call `%prepare(...)` so shared renderers are refreshed only through `%Style::reset(...)`.
- `%Widget::setRenderer(XRenderer*)` keeps `nullptr` as the public style-fallback API. It stores the pointer through `%XStyler::setRenderer(...)`, immediately calls `%bind(style, options)` because `%invalidate()` only posts an event, and then invalidates.
- `%Widget::onInvalidate()` calls the base implementation, binds through the same path, reacquires the typed renderer, refreshes any widget-owned derived caches, and requests relayout.
- `%StylerBase` retains its existing `%isDefault(global)` decision for selecting the shared renderer or a private override clone.

### State Collection

- Each widget should expose one helper that packages only the render-time state expected by its renderer API.
- Use typed flag classes where the renderer API is flag-based.
- Use a dedicated state object where the renderer API is object-based, as in `%ButtonState` and `%PanelState`.
- Keep widget control-flow bookkeeping and non-visual transient internals out of the renderer state helper.

## Global Theme Contract

- `%Application` is the only official mutator for global theme data (`%Style` and `%StyleOptions`).
- Global theme changes must go through `%Application::setStyle(...)` or `%Application::setStyleOptions(...)`.
- Those mutation paths must call `%Style::reset(const StyleOptions&)` before invalidation so that shared renderer facets are synchronized with the new global defaults.
- `%StyleOptions` stores only cross-theme tokens that multiple style families can reasonably honor as part of a shared contract.
- Theme-specific geometry or look details that do not have such a shared contract belong in the concrete derived style or renderer implementation, not in `%StyleOptions` and not in `%Style` itself.
- Do not expose or reintroduce a public mutable `%StyleOptions&` path.
- `%Style` and `%StyleOptions` stay passive data/cache objects with generation counters for cheap pull checks.
- Global theme changes propagate by `%Application::invalidate()` and the existing `%onInvalidate()` flow after the shared-facet reset. Do not add a second update path that mutates renderers directly from a theme observer or setter.
- If a future theme-changed hook is introduced, it may only hang off the same `%Application` mutation path and must not become a separate propagation mechanism.