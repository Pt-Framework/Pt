# Proposal: Umstellung Pt-Forms auf SizeConstraint und LayoutParams

Status: Entwurf  
Scope: nur Measure/Layout-Vertrag in Pt-Forms  
Nicht in diesem Schritt: `setAutoSize`, `applyDock`, Alignment, Grid-`*`

## Ziel

`SizePolicy` und den Merge „strengere Parent-Policy überschreibt das Kind“ ersetzen durch das Android/WPF-Modell:

- Parent reicht einen **Slot-Constraint** (`SizeConstraint`).
- Kind hat **eigene** Längen (`LayoutParams`: `Exact` / `Auto` / `Fill`) plus gespeicherte Größe.
- Default bleibt designer-tauglich: `Exact` + aktuelle `size()` (nicht intrinsisch 0).

Wildcat kann später `AutoSize` und `DockPosition` auf `Length` abbilden. Pt kennt diese Flags in dieser Stufe nicht.

## Warum

Heutiges `Control::measure(const SizePolicy&)`:

- merged Parent- und Kind-Policy (`horizontal() > …` oder Parent `Fixed`);
- Default-`onMeasure` liefert `(0, 0)`;
- `Fixed` des Parents überschreibt Kind-`Fixed` (Designer-Größe geht verloren);
- `maximumSize()` wird in `measure` nicht angewendet.

Das ist weder WYSIWYG-fähig noch kompatibel mit Match/Wrap/Exact.

## Neue Typen

Neue Header, z. B. `include/Pt/Forms/SizeConstraint.h` und `include/Pt/Forms/LayoutParams.h` (oder beide in einer Datei).

```cpp
namespace Pt { namespace Forms {

class SizeConstraint
{
    public:
        enum Mode { Unspecified, AtMost, Exactly };

        SizeConstraint();
        SizeConstraint(Mode h, Mode v, const Gfx::SizeF& size);

        static SizeConstraint unspecified();
        static SizeConstraint atMost(const Gfx::SizeF& size);
        static SizeConstraint exactly(const Gfx::SizeF& size);

        Mode horizontal() const;
        Mode vertical() const;
        const Gfx::SizeF& size() const;

        bool operator==(const SizeConstraint&) const;
        bool operator!=(const SizeConstraint&) const;

    private:
        Mode _h;
        Mode _v;
        Gfx::SizeF _size;
};

enum class Length { Exact, Auto, Fill };

class LayoutParams
{
    public:
        LayoutParams(); // width=Exact, height=Exact, size=(0,0)

        Length width() const;
        Length height() const;
        void setWidth(Length);
        void setHeight(Length);

        const Gfx::SizeF& size() const;
        void setSize(const Gfx::SizeF&);

        bool operator==(const LayoutParams&) const;
        bool operator!=(const LayoutParams&) const;

    private:
        Length _width;
        Length _height;
        Gfx::SizeF _size;
};

} }
```

`SizeConstraint` beschreibt nur das Angebot des Parents.  
`LayoutParams` beschreibt nur den Wunsch des Kindes. Kein Merge zweier Policy-Enums.

### Auflösung einer Achse

| Kind `Length` | Parent `Mode` | gemessene Länge |
|---|---|---|
| `Exact` | * | `LayoutParams.size` (Min/Max) |
| `Auto` | `Unspecified` | intrinsisch (`onMeasure`) |
| `Auto` | `AtMost` / `Exactly` | intrinsisch, gedeckelt auf Slot |
| `Fill` | `Unspecified` | intrinsisch |
| `Fill` | `AtMost` / `Exactly` | Slotgröße |

Danach: `max(minSize)`, `min(maxSize)` wenn Max gesetzt, Pixel-Align über `scaling()`.

## Änderungen an `Control`

Dateien: `include/Pt/Forms/Control.h`, `src/Pt-Forms/Control.cpp`

### Entfernen / ersetzen

| Alt | Neu |
|---|---|
| `SizePolicy _sizePolicy` | `LayoutParams _layoutParams` |
| `SizePolicy _lastPolicy` | `SizeConstraint _lastConstraint` |
| `sizePolicy()` / `setSizePolicy()` | `layoutParams()` / `setLayoutParams()` |
| `measure(const SizePolicy&)` | `measure(const SizeConstraint&)` |
| `onMeasure(const SizePolicy&)` | `onMeasure(const SizeConstraint&)` |

### Semantik

- `setLayoutParams`: Align der Größe, Member setzen, `relayout()`.
- `onRequestResize` / `setSize`: `_layoutParams.setSize(alignedSize)`. Die `Length`-Modes bleiben unverändert.
- `onRescale`: `_layoutParams.setSize(scaling.align(...))` statt `_sizePolicy.setSize`.
- `relayout`: unverändert `_isMeasureInvalid` / `_isLayoutInvalid`.
- Default-`onLayout`: unverändert leer (Geometrie nur durch Layouts/`resize`).
- Default-`onMeasure`: `return _layoutParams.size();` — **nicht** `(0, 0)`.

### `measure` (Kern, kein Policy-Merge)

1. Cache: wenn `c == _lastConstraint` und nicht `_isMeasureInvalid` ? `_preferredSize`.
2. Intrinsik nur berechnen, wenn mindestens eine Achse nicht `Exact` ist **oder** Kinder existieren; sonst `intrinsic = layoutParams.size()`.
3. Sonst `intrinsic = onMeasure(c)`.
4. Pro Achse `resolveAxis(...)`.
5. `_preferredSize` setzen, Measure-Flag löschen, Layout invalidieren.

`Exactly` des Parents ändert **nicht** ein Kind mit `Length::Exact`. Fill ist ausschließlich `Length::Fill` am Kind.

`minimumSize()` / `maximumSize()` in `resolveAxis` anwenden (`onSetSizeLimits` löst bereits `relayout` aus).

## Form / Top-Level

Datei: Form (bzw. wo das Content-Control gemessen wird)

- Content: `measure(SizeConstraint::exactly(clientSize()))`.
- `resize` auf Slot-Größe nur, wenn das Content-Control auf der Achse `Fill` ist; sonst `preferredSize` und optionale spätere Alignment-Stufe.
- Fenster-Wrap-Content: später `SizeConstraint::unspecified()` — nicht in dieser Stufe zwingend.

## Layout-Klassen (Flow, Dock, Grid, Stack, …)

Alle `child->measure(SizePolicy(...))` umstellen.

Hilfsregel für den Slot `slot`:

- Achse Kind `Fill` ? Parent-Mode `Exactly` mit `slot` auf dieser Achse.
- sonst ? `AtMost` mit `slot`.

Nicht: Kind pauschal mit `exactly(parentSize)` messen.

`onLayout` weist `setPosition` / `resize` zu. Undocked/`Exact`-Kinder ohne Layout-Beteiligung nicht verschieben.

## Widgets

Jedes Override `onMeasure(const SizePolicy&)` ? `onMeasure(const SizeConstraint&)`.

Vertrag: nur **intrinsische** Größe. Klemmen und Exact/Fill macht `Control::measure`.

- Text (Label o. ä.): bei `horizontal() != Unspecified` Breite als Wrap-Maximum nutzen.
- Blätter ohne Override: Base (`layoutParams.size()`).

Compiler bricht alle alten Signaturen.

## `SizePolicy` entsorgen

- Header und alle Includes entfernen, sobald die Call-Sites umgestellt sind.
- Keine Kompatibilitätsschicht in dieser Stufe (vermeidet zwei Modelle).
- Externe Nutzer (Wildcat, Demos, Tests) parallel umstellen.

## Nicht in dieser Stufe

- `Control::setAutoSize`
- `applyDock` / `DockPosition`
- `Alignment` / Anchor
- Star-Grid / Gewicht
- DesignMode-Sonderpfad

Diese Features setzen `Length` von außen; Pt bleibt das Constraint-Backend.

## Cache / Events

Unverändert lassen:

- `relayout()` ? Parent `onRelayoutRequest`
- `LayoutEvent`-Pipeline, Child-Recursion
- Padding/Margin-Align bei Rescale (Größe der Params analog SizePolicy)

Nur `_lastPolicy` durch `_lastConstraint` ersetzen (Vergleich inkl. Mode + Size).

## Migrationsreihenfolge

1. Header `SizeConstraint` / `LayoutParams` anlegen, in `Control.h` einbinden.
2. `Control`-Member und `measure`/`onMeasure` umstellen, Default-`onMeasure` = Params-Größe.
3. `setSize` / Rescale an `LayoutParams.size` koppeln.
4. Form-Content-Measure auf `SizeConstraint::exactly(clientSize())`.
5. Layouts auf Slot-`AtMost`/`Exactly` je Kind-`Length`.
6. Widget-`onMeasure`-Signaturen.
7. `SizePolicy` löschen, Build von Pt-Forms + Nutzer grün.

## Akzeptanzkriterien

- Control mit Default-Params und `size() = (80, 24)` misst 80×24, auch in einem größeren Parent mit `exactly(parent)`.
- Kind `Length::Fill` auf beiden Achsen füllt den Parent-Slot.
- Kind `Length::Auto` + `AtMost(100, ?)` darf kleiner als 100 sein, nicht größer (Wrap).
- Parent-`Exactly` ändert kein `Exact`-Kind.
- `onMeasure` ohne Override liefert nicht mehr `(0, 0)`, sondern die gesetzte Größe.
- Keine `SizePolicy`-Symbole mehr in Pt-Forms.
