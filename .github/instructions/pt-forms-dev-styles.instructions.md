---
applyTo: "**/*Style.h,**/*Style.cpp,**/Platinum*.cpp,**/Platinum*.h,**/Button.cpp,**/PushButton.cpp,**/PushButton.h,**/CheckBox.cpp,**/CheckBox.h,**/SpinBox.cpp,**/SpinBox.h,**/ProgressBar.cpp,**/ProgressBar.h,**/LineEdit.cpp,**/LineEdit.h,**/Slider.cpp,**/Slider.h,**/ScrollBar.cpp,**/ScrollBar.h,**/ComboBox.cpp,**/ComboBox.h,**/ListBox.cpp,**/ListBox.h,**/TabView.cpp,**/TabView.h"
description: "Guidelines and architecture for Forms Style and Renderer implementations."
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

## State Ownership by Layer
- Typed style flags must match the layer that is being rendered.
- Container flags carry container semantics such as enabled, disabled, focused, and only container-level highlighted states.
- Repeated-element semantics such as selected, current, checked, pressed, or row-highlighted belong to the corresponding item/tab/cell flags, not to the outer container flags.
- Do not encode visual state indirectly through mutable drawing-attribute out-parameters. State must be explicit in flags or in immutable primitive inputs.

## Painter and Attribute Override Management
- **Painters in Renderer**: Widgets do *not* instantiate `Painter` objects internally for generic drawing (like text). They must request it through the Renderer (e.g., `renderer->textPainter(surface)`). This guarantees that the styling engine's active fonts, text colors, and antialiasing states are applied correctly.
- **Drawing Attributes in Widget**: The widget class natively holds specific drawing overrides (e.g., if a user explicitly sets a custom background color or font on the control). Before calling the measure/layout/render cycle, the widget applies these local overrides directly to the renderer facet (e.g., via `renderer->setBackground(...)`, `renderer->setFont(...)`), ensuring the Renderer draws with the correct overrides without cluttering the layer API.
- **Persistent Painter State**: Cached painters keep their brush, pen, font, clip, and composition state across `begin()` calls. `onPrepare()` must establish the steady-state defaults for every cached painter, and temporary mutations inside `onRender*()` must either be restored before returning or be fully re-established on every later code path that uses that painter.

## Ownership and Cloning
- `onCreate()` must return a freshly allocated facet with `refs = 0`.
- Never pre-increment the reference count or return clones with `refs = 1`; `FacetPtr` and `Style::set()` take their own references.
- Treat `create()` as a prototype clone operation, not as a shared singleton accessor.

## Renderer Defaults and Tokens
- Renderer getters such as `background()`, `foreground()`, `contour()`, `font()`, and `textColor()` must resolve to either a local override or a meaningful default derived from `StyleOptions`.
- Never return static empty placeholder objects for fonts, colors, pens, or brushes when a themed default exists.
- Shared semantic colors and metrics that repeat across a style family belong in `StyleOptions` or in a small centralized derivation layer, not as unrelated magic numbers in each renderer.

## Background Layers
- **Widget Control Background vs. Element Fill**: The term `renderBackground` is strictly reserved for the background of the *entire widget control* (its full bounding rect). Do not use `renderBackground` to refer to the inner fill of a specific visual element (e.g., use `renderEntryBackground` to fill the text entry area, not `renderBackground`).
- **Explicit Background Layer**: Provide a `renderBackground(PaintContext& ctx, const Gfx::RectF& rect, StyleFlags state)` step for transparent or panel-like widgets (e.g., `CheckBox`, `RadioButton`, `Label`).
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

### Button Slice Binding Model

- `%PushButton` does **not** use the direct override pattern above. Its styling flow is centered on `%ButtonStyleOptions`, `%ButtonState`, `%ButtonRenderer`, and `%ButtonStyle`.
- `%ButtonStyleOptions` owns only widget-local override tokens and its own generation. It must not be merged into `%ButtonState`, and `%ButtonStyle` must not keep its own copy. The widget stores the current `%ButtonStyleOptions` object and passes it explicitly into `%ButtonStyle::bind(...)` and `%ButtonStyle::rebind(...)`.
- `%ButtonState` carries only transient interaction state (`enabled`, `hovered`, `focused`, `pressed`, `flat`). Render-time hooks and icon preparation may observe `%ButtonState`, but they must not receive `%ButtonStyleOptions`, `%StyleOptions`, or widget-local override tokens directly.
- `%ButtonRenderer::prepare(const StyleOptions&, const ButtonStyleOptions&)` is the explicit synchronization point for the button slice. Concrete button renderers resolve global defaults plus local override tokens during `%onPrepare(...)` and cache all state needed later by render and icon hooks.
- Button render and icon hooks must work from prepared renderer state plus `%ButtonState` only. If a concrete renderer needs theme data from `%StyleOptions`, resolve and cache it during `%onPrepare(...)` instead of re-fetching it during render.
- `%ButtonStyle` is the renderer-binding controller only. It owns the currently bound `%ButtonRenderer`, tracks the active binding mode (`Style`, `Override`, `Custom`), and tracks the generations needed to decide whether a renderer must be rebound or merely re-prepared. Measure/layout/render/icon code should use the currently bound `%ButtonRenderer` directly instead of forwarding through `%ButtonStyle`.
- `%ButtonStyle::bind(const Pt::Forms::Style&, ...)` is the style-path bind. It must always switch the controller to `%Style` or `%Override`, never keep a previous `%Custom` binding alive, and it must use `%Style::generation()` to detect when the renderer source changed.
- `%ButtonStyle::bind(const Pt::Forms::Style&, ...)` must also use `%StyleOptions::generation()` and `%ButtonStyleOptions::generation()` to decide whether `%ButtonRenderer::prepare(...)` must run even when the bound renderer source stays the same.
- `%ButtonStyle::bind(ButtonRenderer*, ...)` is only the explicit custom-renderer assignment path, typically from `%PushButton::setRenderer(...)`. Do not use pointer identity checks during invalidation to detect whether a custom renderer changed.
- `%ButtonStyle::rebind(...)` only re-prepares the already assigned custom renderer when `%StyleOptions::generation()` or `%ButtonStyleOptions::generation()` changed. `%PushButton::onInvalidate()` should call `%rebind(...)` only when `%isCustom()` is true; otherwise it should stay on the style-path `%bind(style, ...)` call.
- When a bind or rebind path cannot obtain a renderer, keep the cached prepare generations invalid. Only store the current prepare generations after a successful `%ButtonRenderer::prepare(...)` call.
- For button widgets, prefer `%ButtonState` as the single source of truth for render-relevant booleans such as `pressed` and `flat`. Keep pure control-flow bookkeeping outside `%ButtonState`.

### State Collection

- Each widget should expose one helper that packages only the render-time state expected by its renderer API.
- Use typed flag classes where the renderer API is flag-based.
- Use a dedicated state object where the renderer API is object-based, as in `%ButtonState`.
- Keep widget control-flow bookkeeping and non-visual transient internals out of the renderer state helper.

## Global Theme Contract

- `%Application` is the only official mutator for global theme data (`%Style` and `%StyleOptions`).
- Global theme changes must go through `%Application::setStyle(...)` or `%Application::setStyleOptions(...)`.
- Do not expose or reintroduce a public mutable `%StyleOptions&` path.
- `%Style` and `%StyleOptions` stay passive data/cache objects with generation counters for cheap pull checks.
- Global theme changes propagate by `%Application::invalidate()` and the existing `%onInvalidate()` flow. Do not add a second update path that mutates renderers directly from a theme observer or setter.
- If a future theme-changed hook is introduced, it may only hang off the same `%Application` mutation path and must not become a separate propagation mechanism.