---
description: "Using Pt signals, slots, delegates, and events for type-safe callbacks."
---

# Doxygen Group and Headers

- Group definition: `include/Pt/Api-sigslot.h` — `@defgroup sigslot`
- Public class documentation lives beside each declaration: `Invokable.h`,
  `Callable.h`, `Slot.h`, `Function.h`, `Method.h`, `ConstMethod.h`,
  `Lambda.h`, `Delegate.h`, and `Signal.h`.

# Signals, Slots and Delegates

The Pt signal/slot system provides type-safe, lifetime-managed callbacks.

For detailed documentation with examples, read:
- `include/Pt/Api-sigslot.h` — Conceptual overview and introduction
- `include/Pt/Signal.h` — Signal usage, connecting, emitting, disconnecting
- `include/Pt/Delegate.h` — Delegate usage, call() vs invoke()
- `include/Pt/Slot.h` — Slot concept and creation helpers
- `include/Pt/Method.h` — Method slots (member function wrappers)
- `include/Pt/Function.h` — Function slots (free function wrappers)
- `include/Pt/Lambda.h` — Lambda and function object slots
- `include/Pt/ConstMethod.h` — Const method slots

## Key Classes

| Class | Header | Purpose |
|-------|--------|---------|
| `Signal<As...>` | `<Pt/Signal.h>` | Multi-cast emitter — calls all connected slots |
| `Delegate<R, As...>` | `<Pt/Delegate.h>` | Single-target callable with return value |
| `Connection` | `<Pt/Connection.h>` | Represents a signal-to-slot link |
| `Connectable` | `<Pt/Connectable.h>` | Base class providing lifetime management |
| `Lambda<F, R, A...>` | `<Pt/Lambda.h>` | Lambda or function object adapter |
| `LambdaSlot<F, R, A...>` | `<Pt/Lambda.h>` | Lambda adapter usable as a slot |
| `Signal<const Pt::Event&>` | `<Pt/Signal.h>` | Event dispatch routed by type |
| `Event` / `BasicEvent<T>` | `<Pt/Event.h>` | Base for custom event types |

## Creating Slots

Use `Pt::slot()` to create slot objects:

| Target | Call |
|--------|------|
| Member function | `Pt::slot(object, &Class::method)` |
| Const member function | `Pt::slot(constObject, &Class::constMethod)` |
| Free function | `Pt::slot(&freeFunction)` |
| Non-generic lambda/function object | `Pt::slot(lambda)` |
| Generic lambda/function object | `Pt::slot<R, A...>(lambda)` |
| Context-bound non-generic lambda | `Pt::slot(context, lambda)` |
| Context-bound generic lambda | `Pt::slot<R, A...>(context, lambda)` |
| Another signal (chaining) | `Pt::slot(otherSignal)` |
| A delegate | `Pt::slot(delegate)` |
| Bound parameter | `Pt::slot(Pt::slot(obj, &Class::method), boundValue)` |

Binding copies the final argument and may be repeated until a zero-argument
slot remains. Signatures are unbounded parameter packs; explicitly
`Pt::Void`-padded SigSlot forms are unsupported.

## Implementation Notes for Agents

- `Signal<const Pt::Event&>` remains a specialized routed-event implementation
  in `Signal.h` and `src/Pt/Signal.cpp`.
- All slot-creation headers (`Method.h`, `Function.h`, `ConstMethod.h`) are
  included transitively by `<Pt/Signal.h>` and `<Pt/Delegate.h>`.
