---
description: "Using Pt signals, slots, delegates, and events for type-safe callbacks."
---

# Signals, Slots and Delegates

The Pt signal/slot system provides type-safe, lifetime-managed callbacks.

For detailed documentation with examples, read:
- `doc/groups/sigslot.dox` — Conceptual overview and introduction
- `doc/include/Pt/Signal.h` — Signal usage, connecting, emitting, disconnecting
- `doc/include/Pt/Delegate.h` — Delegate usage, call() vs invoke()
- `doc/include/Pt/Slot.h` — Slot concept and creation helpers
- `doc/include/Pt/Method.h` — Method slots (member function wrappers)
- `doc/include/Pt/Function.h` — Function slots (free function wrappers)
- `doc/include/Pt/ConstMethod.h` — Const method slots

## Key Classes

| Class | Header | Purpose |
|-------|--------|---------|
| `Signal<A1, ..., A10>` | `<Pt/Signal.h>` | Multi-cast emitter — calls all connected slots |
| `Delegate<R, A1, ..., A10>` | `<Pt/Delegate.h>` | Single-target callable with return value |
| `Connection` | `<Pt/Connection.h>` | Represents a signal-to-slot link |
| `Connectable` | `<Pt/Connectable.h>` | Base class providing lifetime management |
| `Signal<const Pt::Event&>` | `<Pt/Signal.h>` | Event dispatch routed by type |
| `Event` / `BasicEvent<T>` | `<Pt/Event.h>` | Base for custom event types |

## Creating Slots

Use `Pt::slot()` to create slot objects:

| Target | Call |
|--------|------|
| Member function | `Pt::slot(object, &Class::method)` |
| Const member function | `Pt::slot(constObject, &Class::constMethod)` |
| Free function | `Pt::slot(&freeFunction)` |
| Another signal (chaining) | `Pt::slot(otherSignal)` |
| A delegate | `Pt::slot(delegate)` |
| Bound parameter | `Pt::slot(Pt::slot(obj, &Class::method), boundValue)` |

## Implementation Notes for Agents

- `Signal.tpp` contains 10 partial specializations (0–10 parameters) with
  identical structure — only the parameter count varies.
- `<Pt/Signal.h>` includes `<Pt/Signal.tpp>` inside `namespace Pt`.
- All slot-creation headers (`Method.h`, `Function.h`, `ConstMethod.h`) are
  included transitively by `<Pt/Signal.h>` and `<Pt/Delegate.h>`.
