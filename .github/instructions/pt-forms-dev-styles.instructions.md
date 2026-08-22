---
description: "Guidelines and current architecture for Pt::Forms styles, renderers, shared style reset, extracted ButtonStyler/PanelStyle slices, and widget integration in PushButton, Panel, and Label."
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
- **Drawing Attributes in Widget**: One `%StyleOptions` class is used for both the complete global options and a sparse widget overlay. The default constructor is an empty bag. `%StyleOptions::defaults()` inserts the built-in contract tokens. Presence is bag membership via `%find<T>()` / `%set` / `%reset<T>()`. Each token is a concrete option type (`%ForegroundOption`, `%FontOption`, ...). `%Application` owns the live global instance constructed from `%defaults()`. Widgets or their styler hold a second instance as the overlay. On the button slice, `%ButtonStyler` owns that overlay. On slices that still use the template `%Styler`, the widget owns it and passes it into the styler. The renderer resolves global options plus overlay during `prepare(...)` or shared-facet `onReset(...)`. Overlay callers must use `%find<T>()`; it may be 0. Do not call `%value()` on a missing overlay option. Global callers use `%options.get<T>().value()` because `%defaults()` guarantees contract tokens. Do not push ad-hoc local overrides into shared renderers during every invalidate pass. Direct renderer mutation remains a legacy-only pattern or a private/custom-renderer-only pattern.
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
- `%PushButton` uses the non-template `%StylerBase` controller with `%ButtonStyler::setRenderer(...)` and a single `%bind(style, options)` path. Other extracted slices still use the template `%Styler<RendererT, OptionsT>` with `%bind` / `%rebind` until they are migrated. Instantiations use `%StyleOptions` as `OptionsT`.
- `%Style` is the registry of shared renderer facets for the active style. `%StyleOptions` is both the global token store and the widget overlay type. `%Application` owns the live global instance from `%StyleOptions::defaults()` and the live `%Style`.
- Global theme changes must run through `%Application::setStyle(...)` or `%Application::setStyleOptions(...)`. That path calls `%Style::reset(const StyleOptions&)`, which fans the global options out to each shared facet through `%Style::Facet::reset(...)` and protected `%onReset(...)`.
- Shared renderers fetched directly from `%Style` must treat `%onReset(...)` as their synchronization point for global defaults. Implement `%onReset(...)` by calling `%prepare(...)` with the current global `%StyleOptions` and a default-constructed overlay (`StyleOptions empty;`).
- Each extracted slice separates responsibilities explicitly:
  - Overlay: a second `%StyleOptions` instance. Default-constructed means no overrides (`%hasOverrides()` is false). Presence is `%find<T>()`. Font overrides are a `%FontOption` stored in the bag.
  - `XState`: transient render state only.
  - `XRenderer`: prepared drawing state, measure/layout/render primitives, and optional icon preparation. Button renderers derive from `%Renderer`.
  - `XStyler`: renderer-binding controller only.
  - Widget: owns control flow, geometry, text/icon caches, and the helper that produces `XState`. On `%StylerBase` slices the styler owns the overlay. On template `%Styler` slices the widget still owns the overlay.
- The overlay must not absorb transient interaction flags, and `XState` must not absorb style tokens. Keep local override data and transient render state separate. Do not reintroduce per-class `hasOverrides()`, `generation()`, `_generation`, or `_overrides` on a slice-specific options type.
- `XRenderer::prepare(const StyleOptions& options, const StyleOptions& overlay)` is the explicit synchronization point for extracted slices. Resolve global defaults plus overlay tokens during `%onPrepare(...)` and cache all data needed later by painters, measure/layout hooks, render hooks, and optional icon preparation. Global font access is `%options.get<FontOption>().value()`. Overlay font access is `%overlay.find<FontOption>() ? overlay.find<FontOption>()->getFont(baseFont) : baseFont`.
- Render hooks must consume prepared renderer state plus `XState` only. If a renderer needs information from `%StyleOptions`, resolve and cache it during `%onPrepare(...)` or shared-facet `%onReset(...)` instead of re-fetching it during render.
- `%StylerBase` owns the currently bound renderer, the custom renderer source, and the style, global-options, and local-options generation fields used for change detection, including override-mode flips from `%StyleOptions::hasOverrides()`. It does not expose an untyped `%renderer()` accessor. The overlay lives in the derived styler.
- Measure/layout/render code should use the typed `XStyler::renderer()` accessor. Do not add an untyped `%renderer()` on `%StylerBase`.
- `%StylerBase::bind(const Style&, const StyleOptions&)` is the only public bind path. Local options come from `%onLocalOptions()`. It calls `%onStyleRenderer(...)` when `%isStyleChanged(...)` is true and `%onBindOptions(...)` when a renderer is bound, the renderer is custom or an override clone, and `%isOptionsChanged(...)` is true.
- `%isStyleChanged(...)` is true when no renderer is bound, the `%Style` generation changed, local override mode flipped, or the custom renderer source is dirty. `%isOptionsChanged(...)` is true when the global `%StyleOptions` generation or the overlay generation changed. There is no `%onIsStyleChanged()` or `%onIsOptionsChanged()` hook.
- After `%onStyleRenderer(...)`, `%StylerBase` invalidates the cached options generations so a newly acquired renderer always goes through `%onBindOptions(...)` when it is custom or an override clone.
- On `%StylerBase` slices, `%onStyleRenderer(const Style&)` returns the renderer to bind, or 0 if none is available. It must not prepare that renderer. It must already include the override clone when local overrides exist and the custom renderer when one is assigned. `%onBindOptions(const StyleOptions&)` applies tokens to the already bound renderer. The base already skipped shared prototypes (`_custom || _hasOverrides`).
- `%ButtonStyler` caches a typed `%ButtonRenderer*` at each creation site and returns it from `%renderer()` without casting. No downcast from `%StylerBase`.
- `%ButtonStyler::setRenderer(ButtonRenderer*)` stores a typed custom pointer and marks the custom source dirty through `%setCustomRenderer(...)`. The next `%bind(...)` acquires it through `%onStyleRenderer(...)`. A null pointer falls back to the current style. Do not compare stored renderer pointers for identity; addresses can be reused.
- `%ButtonStyler` does not track `_hasOverrides` or `_overridesGeneration`. Token-only changes with the same override mode are option changes and prepare the current renderer in place. Override-mode flips rebind.
- `%ButtonStyler::onStyleRenderer(...)` returns the custom renderer when one is assigned. Otherwise it returns the shared `%ButtonRenderer` from `%Style`, or a `%create()` clone of that prototype when local overrides exist.
- `%ButtonStyler::onBindOptions(...)` prepares only because the base already skipped shared prototypes. Shared style prototypes stay synchronized through `%Style::reset()`.
- On remaining template `%Styler` slices, `%bind` / `%rebind` still follow the replace-versus-refresh split.
- In the style-path bind, clone and locally prepare a private renderer only when the overlay actually contains overrides. If there are no local overrides, bind the shared renderer from `%Style` directly and do not prepare it locally.
- When a bind or update path cannot obtain a renderer, keep the cached prepare generations invalid. Only store the current prepare generations after a successful `%onBindOptions(...)` or `%XRenderer::prepare(...)` call.
- Widgets that use an extracted slice should keep `setRenderer(T* renderer)` pointer-based when `nullptr` is part of the public API contract for falling back to the current style.
- Widgets that use `%StylerBase` call `%bind(style, options)` during `%onInvalidate()`. Widgets that still use the template `%Styler` call `%rebind(style, options, localOptions)`.
- In `%onInvalidate()`, call the base implementation first, reacquire the renderer through the slice styler, then refresh widget-owned derived caches such as icon pixmaps, and finally request relayout.

### Button Slice

- `%PushButton` uses the extracted slice pattern with a `%StyleOptions` overlay, `%ButtonState`, `%ButtonRenderer`, and `%ButtonStyler`.
- `%ButtonStyler` derives from `%StylerBase` and owns the `%StyleOptions` overlay.
- The overlay stores only widget-local override tokens such as foreground, contour, accent/highlight colors, text color, and font overrides. Unset slots fall back to the theme.
- `%ButtonState` owns only transient interaction flags: `enabled`, `hovered`, `focused`, `pressed`, and `flat`.
- `%ButtonRenderer` derives from `%Renderer` and provides the prepared button-specific primitives `measureContent()`, `measureFrame()`, `layoutFrame()`, `layoutContent()`, `layoutMnemonic()`, `textPainter()`, `prepareIcon()`, `renderBackground()`, `renderChrome()`, `renderText()`, `renderMnemonic()`, and `renderIcon()`.
- `%PushButton::setRenderer(ButtonRenderer*)` keeps `nullptr` as the public style-fallback API. It stores the pointer with `%ButtonStyler::setRenderer(...)` and then calls `%bind(style, options)` immediately because `%invalidate()` only posts an event. `%PushButton::onInvalidate()` calls `%bind(style, options)`, refreshes the widget-owned icon picture through `%ButtonRenderer::prepareIcon(...)`, and then requests relayout.
- Button widgets should use `%ButtonState` as the single source of truth for render-relevant booleans such as `pressed` and `flat`. Keep pure control-flow bookkeeping outside `%ButtonState`.

### Panel Slice

- `%Panel` and `%Label` share the extracted slice pattern with a `%StyleOptions` overlay, `%PanelState`, `%PanelRenderer`, and `%PanelStyle`.
- The overlay owns only widget-local panel override tokens such as background, contour, text color, and font overrides. `%Panel` and `%TabView` keep `_hasBackground` / `_hasFrame` as widget policy flags.
- `%PanelState` currently owns only the transient render flags `enabled` and `focused`.
- `%PanelRenderer` provides the prepared panel-like primitives `measureFrame()`, `layoutFrame()`, `textPainter()`, `renderBackground()`, `renderFrame()`, `renderText()`, and `renderIcon()`.
- `%Panel::setRenderer(PanelRenderer*)` and `%Label::setRenderer(PanelRenderer*)` keep `nullptr` as the public style-fallback API. Their `%onInvalidate()` paths reacquire the active renderer through `%PanelStyle::rebind(...)`, refresh widget-owned icon caches afterwards, and then request relayout.
- Prefer reusing `%PanelRenderer` for generic framed/background/text/icon container chrome before introducing a new dedicated renderer family. Keep a dedicated renderer only when the new widget needs genuinely different primitives, metrics, or native integration.

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