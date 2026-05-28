---
applyTo: "**/*Style.h,**/*Style.cpp,**/StyleOptions.h,**/StyleOptions.cpp,**/Application.cpp,**/Application.h,**/Platinum*.cpp,**/Platinum*.h,**/Button.cpp,**/Button.h,**/PushButton.cpp,**/PushButton.h,**/Panel.cpp,**/Panel.h,**/Label.cpp,**/Label.h,**/CheckBox.cpp,**/CheckBox.h,**/SpinBox.cpp,**/SpinBox.h,**/ProgressBar.cpp,**/ProgressBar.h,**/LineEdit.cpp,**/LineEdit.h,**/Slider.cpp,**/Slider.h,**/ScrollBar.cpp,**/ScrollBar.h,**/ComboBox.cpp,**/ComboBox.h,**/ListBox.cpp,**/ListBox.h,**/TabView.cpp,**/TabView.h"
description: "Guidelines and current architecture for Pt::Forms styles, renderers, shared style reset, extracted ButtonStyle/PanelStyle slices, and widget integration in PushButton, Panel, and Label."
---

# Forms Style Architecture

## Zero Abstraction Leakage
- Avoid passing complex temporary layout state objects or "magic" data structures to renderers.
- Flatten API parameters: pass primitives and explicitly calculated bounds (e.g., `const Gfx::RectF& textRect`) directly into the rendering functions.
- Renderer APIs must not mutate widget-owned geometry, collections, or model objects. Layout methods return sizes/rectangles; the widget applies them to its own storage.

## Renderer API Shape
- All renderer APIs accept only primitives, pre-calculated bounds, scalars, enums, and typed style flags. No renderer takes widget references or pointers.
- Legacy `prepare(...)` / `prepareLayout(...)` patterns that fill mutable `Brush`, `Pen`, or `Font` out-parameters are transitional only. Do not introduce that shape in new APIs.

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

## State Ownership by Layer
- Typed style flags must match the layer that is being rendered.
- Container flags carry container semantics such as enabled, disabled, focused, and only container-level highlighted states.
- Repeated-element semantics such as selected, current, checked, pressed, or row-highlighted belong to the corresponding item/tab/cell flags, not to the outer container flags.
- Do not encode visual state indirectly through mutable drawing-attribute out-parameters. State must be explicit in flags or in immutable primitive inputs.

## Painter and Attribute Override Management
- **Painters in Renderer**: Widgets do *not* instantiate `Painter` objects internally for generic drawing (like text). They must request it through the Renderer (e.g., `renderer->textPainter(surface)`). This guarantees that the styling engine's active fonts, text colors, and antialiasing states are applied correctly.
- **Drawing Attributes in Widget**: In extracted style slices, the widget stores local override tokens in a dedicated local options object such as `ButtonStyleOptions` or `PanelStyleOptions`. The widget passes that object into the slice binder, and the renderer resolves global `StyleOptions` plus local tokens during `prepare(...)` or shared-facet `onReset(...)`. Do not push ad-hoc local overrides into shared renderers during every invalidate pass. Direct renderer mutation remains a legacy-only pattern or a private/custom-renderer-only pattern.
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
- **Explicit Background Layer**: Provide a `renderBackground(PaintContext& ctx, const Gfx::RectF& rect, State)` step for transparent or panel-like widgets. The state parameter must match the touched slice: use typed slice state such as `ButtonState` or `PanelState` on migrated facets, and use `StyleFlags` only on legacy, still-flag-based facets.
- **Often Empty by Default**: In native themes (like Windows/Platinum), the implementation (`onRenderBackground`) is often intentionally empty, leaving the widget transparent to the parent container.
- **Special Cases Only**: These backgrounds are primarily filled only in special cases, such as when a custom background override is explicitly set on the widget via attributes (checked via `opts.background()`).

## Widget Attribute Lifecycle and Renderer Management

Forms currently uses two renderer-management patterns. Preserve the established pattern of the touched widget and do not mix them.

### Direct Override Pattern

- Some widgets still keep renderer-local overrides directly on the widget and lazily clone a private renderer on the first override.
- In that pattern, public getters proxy through the active renderer first and fall back to global `StyleOptions` when no renderer-local override is available.
- Setters may lazily create a private renderer clone, push the override into that renderer immediately, and then call `invalidate()`.
- `onInvalidate()` may either acquire a private clone when local overrides exist or reuse the shared style prototype when no overrides exist.
- Never call individual `renderer->setXxx()` unconditionally during every invalidate pass. Either push a changed local override from the setter or perform one explicit bulk transfer when a private renderer is first acquired.

### Extracted Style Slice Pattern

- `%PushButton`, `%Panel`, and `%Label` do **not** use the direct override pattern above. Their styling flow is centered on a split between `%Style`, `%StyleOptions`, local slice options, slice state, a slice renderer, and a slice binder.
- `%Style` is the registry of shared renderer facets for the active theme. `%StyleOptions` is the global theme token store. `%Application` owns the live instances of both.
- Global theme changes must run through `%Application::setStyle(...)` or `%Application::setStyleOptions(...)`. That path calls `%Style::reset(const StyleOptions&)`, which fans the global options out to each shared facet through `%Style::Facet::reset(...)` and protected `%onReset(...)`.
- Shared renderers fetched directly from `%Style` must treat `%onReset(...)` as their synchronization point for global defaults. Implement `%onReset(...)` by calling `%prepare(...)` with the current global `%StyleOptions` and an empty local slice-options object.
- Each extracted slice separates responsibilities explicitly:
  - `XStyleOptions`: widget-local override tokens plus a generation counter only.
  - `XState`: transient render state only.
  - `XRenderer`: prepared drawing state, measure/layout/render primitives, and optional icon preparation.
  - `XStyle`: renderer-binding controller — a thin subclass of `StyleBinder<XRenderer, XStyleOptions>`.
  - Widget: owns control flow, geometry, text/icon caches, local `XStyleOptions`, and the helper that produces `XState`.
- `XStyleOptions` must not absorb transient interaction flags, and `XState` must not absorb style tokens. Keep local override data and transient render state separate.
- `XRenderer::prepare(const StyleOptions&, const XStyleOptions&)` is the explicit synchronization point for extracted slices. Resolve global defaults plus local override tokens during `%onPrepare(...)` and cache all data needed later by painters, measure/layout hooks, render hooks, and optional icon preparation.
- Render hooks must consume prepared renderer state plus `XState` only. If a renderer needs information from `%StyleOptions`, resolve and cache it during `%onPrepare(...)` or shared-facet `%onReset(...)` instead of re-fetching it during render.

#### StyleBinder Template

- `StyleBinder<RendererT, LocalOptionsT>` is the common base class for all extracted-slice binders. It lives fully inline in `Style.h`, immediately after the `Style` class definition.
- Concrete binders such as `ButtonStyle` and `PanelStyle` publicly inherit `StyleBinder<XRenderer, XStyleOptions>` and add only a default constructor. Do not duplicate bind/rebind/renderer logic in subclasses.
- `StyleBinder` owns the currently bound renderer (`FacetPtr<RendererT>`), the active binding mode, and three generation fields (`_boundStyleGeneration`, `_styleOptionsGeneration`, `_localOptionsGeneration`) for local prepare bookkeeping.
- The binding enum uses the names `SharedRenderer`, `CustomOverrides`, `CustomRenderer` to avoid shadowing the outer `Pt::Forms::Style` type. Old code that referenced `Style`/`Override`/`Custom` must be updated.
- Measure/layout/render code should call `XStyle::renderer()` directly.
- `StyleBinder::bind(const Pt::Forms::Style&, const StyleOptions&, const LocalOptionsT&)` is the style-path bind. It always leaves custom mode and switches to `SharedRenderer` or `CustomOverrides`.
- In the style-path bind, clone and locally prepare a private renderer only when `localOptions.hasOverrides()` is true. If there are no local overrides, bind the shared renderer from `%Style` directly and do not prepare it locally.
- `StyleBinder::bind(RendererT&, const StyleOptions&, const LocalOptionsT&)` is only the explicit custom-renderer assignment path, typically from a widget `setRenderer(XRenderer*)` API.
- `StyleBinder::rebind(const Pt::Forms::Style&, const StyleOptions&, const LocalOptionsT&)` re-prepares only the currently assigned custom renderer when the local prepare generations changed. For `SharedRenderer` and `CustomOverrides`, it delegates back through `bind(style, ...)` so the active renderer source is reacquired centrally.
- When a bind or rebind path cannot obtain a renderer, keep the cached prepare generations invalid. Only store the current prepare generations after a successful `prepare(...)` call.
- When introducing a new style slice, create a minimal `XStyle` class that publicly inherits `StyleBinder<XRenderer, XStyleOptions>` with only a default constructor.
- Widgets that use an extracted slice should keep `setRenderer(T* renderer)` pointer-based when `nullptr` is part of the public API contract for falling back to the current style. Non-null pointers map to `bind(*renderer, ...)`; null maps to `bind(Application::instance().style(), ...)`.
- Widgets that use an extracted slice may call `rebind(style, options, localOptions)` uniformly during `%onInvalidate()`. The binder keeps the custom path local and routes style and override paths back through `bind(style, ...)`.
- In `%onInvalidate()`, call the base implementation first, reacquire the renderer through the slice binder, then refresh widget-owned derived caches such as icon pixmaps, and finally request relayout.

#### FontOption Helper

- `FontOption` is a composable font-override helper defined in `StyleOptions.h` alongside the global `StyleOptions` class.
- It stores an optional local `AutoPtr<Gfx::Font>` and four internal partial-override bits (`All`, `Size`, `Weight`, `Slant`).
- `FontOption` has no generation counter of its own. The enclosing `XStyleOptions` class bumps its own generation via `setOverride(Font)` after each delegate call.
- `XStyleOptions` classes store a `FontOption _font;` private member and expose the standard public font API (`font()`, `setFont()`, `setFontSize()`, `setFontWeight()`, `setFontSlant()`, `getFont(base)`) by one-line delegation to `_font`.
- The enclosing `enum StyleOverride` only needs a single `Font` bit. The partial merge logic (full vs. size/weight/slant) is handled internally by `FontOption::getFont(base)`.
- When creating a new `XStyleOptions` class, include a `FontOption _font;` member and follow the delegation pattern in `ButtonStyleOptions` or `PanelStyleOptions`.

### Button Slice

- `%PushButton` uses the extracted slice pattern with `%ButtonStyleOptions`, `%ButtonState`, `%ButtonRenderer`, and `%ButtonStyle`.
- `%ButtonStyle` publicly inherits `StyleBinder<ButtonRenderer, ButtonStyleOptions>` and adds only a default constructor. All bind/rebind/renderer logic is inherited.
- `%ButtonStyleOptions` owns only widget-local override tokens such as foreground, contour, accent/highlight colors, text color, and font overrides, plus its own generation counter. Font overrides delegate to a `FontOption _font;` member with a single `Font` bit in `enum StyleOverride`.
- `%ButtonState` owns only transient interaction flags: `enabled`, `hovered`, `focused`, `pressed`, and `flat`.
- `%ButtonRenderer` provides the prepared button-specific primitives `measureContent()`, `measureFrame()`, `layoutFrame()`, `layoutContent()`, `layoutMnemonic()`, `textPainter()`, `prepareIcon()`, `renderBackground()`, `renderChrome()`, `renderText()`, `renderMnemonic()`, and `renderIcon()`.
- `%PushButton::setRenderer(ButtonRenderer*)` keeps `nullptr` as the public style-fallback API. `%PushButton::onInvalidate()` reacquires the active renderer through `%ButtonStyle::rebind(...)`, refreshes the widget-owned icon picture through `%ButtonRenderer::prepareIcon(...)`, and then requests relayout.
- Button widgets should use `%ButtonState` as the single source of truth for render-relevant booleans such as `pressed` and `flat`. Keep pure control-flow bookkeeping outside `%ButtonState`.

### Panel Slice

- `%Panel` and `%Label` share the extracted slice pattern with `%PanelStyleOptions`, `%PanelState`, `%PanelRenderer`, and `%PanelStyle`.
- `%PanelStyle` publicly inherits `StyleBinder<PanelRenderer, PanelStyleOptions>` and adds only a default constructor.
- `%PanelStyleOptions` owns only widget-local panel override tokens such as background, contour, text color, and font overrides, plus its own generation counter. Font overrides delegate to a `FontOption _font;` member with a single `Font` bit in `enum StyleOverride`.
- `%PanelState` currently owns only the transient render flags `enabled` and `focused`.
- `%PanelRenderer` provides the prepared panel-like primitives `measureFrame()`, `layoutFrame()`, `textPainter()`, `renderBackground()`, `renderFrame()`, `renderText()`, and `renderIcon()`.
- `%Panel::setRenderer(PanelRenderer*)` and `%Label::setRenderer(PanelRenderer*)` keep `nullptr` as the public style-fallback API. Their `%onInvalidate()` paths reacquire the active renderer through `%PanelStyle::rebind(...)`, refresh widget-owned icon caches afterwards, and then request relayout.
- Prefer reusing `%PanelRenderer` for generic framed/background/text/icon container chrome before introducing a new dedicated renderer family. Keep a dedicated renderer only when the new widget needs genuinely different primitives, metrics, or native integration.

### onPaint Hook Pattern

- Widgets that use the extracted slice pattern decompose `onPaint` into explicit per-layer virtual hooks (e.g., `onPaintBackground`, `onPaintFrame`, `onPaintText`, `onPaintIcon`, `onPaintMnemonic`, `onPaintChrome`, `onPaintContent`).
- **Signature rule:** Each hook's parameter list mirrors the corresponding renderer render method 1:1. If `renderer->renderFrame(context, rect, state)` is the target call, then `onPaintFrame(PaintContext& context, const Gfx::RectF& rect, const XState& state)` is the hook signature.
- **State computed once:** The top-level `onPaint` computes `widgetRect` (from `size()`), `contentRect` (from layout cache), and the typed state object once. It passes these to every hook — hooks never call `panelState()` or `buttonState()` themselves.
- **Null-check in hooks:** Each hook fetches the renderer via `_xStyle.renderer()`, null-checks, and then delegates to the corresponding render method.
- **Feature guards stay in hooks:** Early returns for disabled features (e.g., `! _hasBackground`, `_picture.empty()`, `! _icon.empty()`) remain inside the hook body, not in `onPaint`.
- **Complex internal logic stays in hooks:** Image alignment switches, TextBlock line iteration, and similar widget-internal calculations stay inside the hook body. The hook receives the content rect and state; it computes positions internally before calling the renderer.
- **Subclass use cases:** Subclasses can skip a layer (empty override), decorate (call Base then add custom drawing), or completely replace a layer. They receive all data needed without accessing private members or the renderer directly.
- **No renderer access for subclasses:** Hooks do not expose the renderer pointer to subclasses. The base hook fetches the renderer privately and delegates; subclasses that override skip calling `Base::onPaintX(...)` if they want full replacement.

#### Current Widget Hook Signatures

| Widget | Hook | Parameters (after `PaintContext&`) |
|--------|------|------------------------------------|
| `PushButton` | `onPaintBackground` | `rect, state` |
| `PushButton` | `onPaintFrame` | `rect, state` |
| `PushButton` | `onPaintIcon` | `rect, picture, pos, state` |
| `PushButton` | `onPaintText` | `rect, text, pos, state` |
| `PushButton` | `onPaintMnemonic` | `rect, mnemonic, state` |
| `Panel` | `onPaintBackground` | `rect, state` |
| `Panel` | `onPaintContent` | `contentRect, state` |
| `Panel` | `onPaintFrame` | `rect, state` |
| `Label` | `onPaintBackground` | `rect, state` |
| `Label` | `onPaintFrame` | `rect, state` |
| `Label` | `onPaintIcon` | `contentRect, state` |
| `Label` | `onPaintText` | `contentRect, state` |
| `CheckBox` | `onPaintChrome` | `rect, boxRect, state` |
| `CheckBox` | `onPaintText` | `textRect, text, pos, state` |
| `CheckBox` | `onPaintMnemonic` | `rect, mnemonic, state` |

### State Collection

- Each widget should expose one helper that packages only the render-time state expected by its renderer API.
- Use typed flag classes where the renderer API is flag-based.
- Use a dedicated state object where the renderer API is object-based, as in `%ButtonState` and `%PanelState`.
- Keep widget control-flow bookkeeping and non-visual transient internals out of the renderer state helper.

## Global Theme Contract

- `%Application` is the only official mutator for global theme data (`%Style` and `%StyleOptions`).
- Global theme changes must go through `%Application::setStyle(...)` or `%Application::setStyleOptions(...)`.
- Those mutation paths must call `%Style::reset(const StyleOptions&)` before invalidation so that shared renderer facets are synchronized with the new global defaults.
- Do not expose or reintroduce a public mutable `%StyleOptions&` path.
- `%Style` and `%StyleOptions` stay passive data/cache objects with generation counters for cheap pull checks.
- Global theme changes propagate by `%Application::invalidate()` and the existing `%onInvalidate()` flow after the shared-facet reset. Do not add a second update path that mutates renderers directly from a theme observer or setter.
- If a future theme-changed hook is introduced, it may only hang off the same `%Application` mutation path and must not become a separate propagation mechanism.