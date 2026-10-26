# Platinum (Pt) - Agent Instructions

You are designing and developing Platinum: a general-purpose, easy to use,
high-performance, multi-platform C++ framework to buld applications from one
codebase. Develop durable framework concepts, APIs, and implementations that
must serve many applications, platforms, and user requirements.

## General Instructions

- Treat every change as framework design, not application-specific code. Prefer
  coherent, composable abstractions that fit existing framework concepts and
  serve a broad range of applications, platforms, and requirements.
- Do not default to the smallest local solution when it would create a narrow
  API, encode one application's policy, or prevent valid future uses.
- Evaluate designs beyond the current example: consider API clarity,
  extensibility, cross-platform behavior, performance, ownership, and failure
  handling. Avoid speculative generalization; broader abstractions must remain
  justified, understandable, and proportionate to their boundary.
- Reuse and extend established framework patterns when they express the model
  well. Do not add special cases or convenience shortcuts that weaken
  conceptual consistency.
- Prefer coherent improvements over compatibility-preserving workarounds when
  the broader design benefit is clear.
- Before you add a local abstraction, check whether the framework already
  provides the required behavior.
- Reuse or extend the framework abstraction that owns the behavior. Do not
  implement general behavior only in local code.
- Use repo-root-relative paths in repository content and user-facing references.
  Use absolute paths for tool calls when required or when a multi-root workspace
  could make a relative path ambiguous.
- In multi-root workspaces, identify the workspace folder explicitly before
  resolving relative paths.
- Do not search `build/`, `tmp/`, `prebuilt/`, `doc/website/`, or large
  vendored trees (`src/skia`, `src/openssl`, …) unless the task requires it.
- Use ASCII characters in authored source files, source comments, documentation
  source, and file names unless the task requires non-ASCII text. This does not
  restrict generated, rendered, or runtime output.

## Applicable Instruction Files

- Use the Instructions Index to identify instruction files whose topics apply to
  the requested work.
- Read each applicable instruction file in full, even if it was automatically
  attached to the context.
- Treat instruction files as authoritative; do not rely on remembered
  conventions.

## Instructions Index

- For Coding guidelines & style, directory layout, writing and reviewing code read:
  `.github/instructions/coding.instructions.md`

- For Doxygen comments, public headers, API docs read:
  `.github/instructions/documenting.instructions.md`

- For design proposals, implementation tasks in `doc/proposals/`, and concepts in `doc/concepts/` read:
  `.github/instructions/proposals.instructions.md`

- For creating or updating thematic instruction indexes that map topics to public headers read:
  `.github/instructions/api-index.instructions.md`

- For Build system & setup, building, build configuration, adding source files read:
  `.github/instructions/building.instructions.md`

- For Unit tests, test suites, assertions, test registration, test failures read:
  `.github/instructions/testing.instructions.md`
  `.github/instructions/pt-unit-api.instructions.md`

- For Jam scripts, `Jamfile`, `*.jam` (not jam C sources) read:
  `.github/instructions/jam.instructions.md`

- For Jam executable internals, `jam/src`, Jambase boot, toolset `.jam` read:
  `.github/instructions/jam-internals.instructions.md`

- For Signals, slots, delegates, events, connect, emit, observable read:
  `.github/instructions/pt-api-signals.instructions.md`

- For Basic types, program options, application settings, events, Any, dates,
  times and atomic integers read:
  `.github/instructions/pt-api-basics.instructions.md`

- For Unicode strings, text codecs, regex, Base64, parsing and formatting read:
  `.github/instructions/pt-api-text.instructions.md`

- For Memory management, smart pointers and allocators read:
  `.github/instructions/pt-api-memory.instructions.md`

- For Coroutines, awaitables and generators read:
  `.github/instructions/pt-api-coroutines.instructions.md`

- For Serialization, composers and formatters read:
  `.github/instructions/pt-api-serialization.instructions.md`

- For JSON documents, streaming parser and writer, serialization formatter read:
  `.github/instructions/pt-json-api.instructions.md`

- For I/O streams, buffers and zlib streams read:
  `.github/instructions/pt-api-streams.instructions.md`

- For Type traits, type information, source info and conversion utilities read:
  `.github/instructions/pt-api-utilities.instructions.md`

- For database connections, queries, result sets, transactions, and database backends read:
  `.github/instructions/pt-db-api.instructions.md`

- For threads, mutexes, semaphores, condition variables, queues, subprocesses, clocks, system time and monotonic ticks read:
  `.github/instructions/pt-system-api-concurrency.instructions.md`

- For file and directory operations, paths and FileDevice read:
  `.github/instructions/pt-system-api-filesystem.instructions.md`

- For logging, log levels, targets and channels read:
  `.github/instructions/pt-system-api-logging.instructions.md`

- For shared libraries, symbols and plugins read:
  `.github/instructions/pt-system-api-plugins.instructions.md`

- For event loops, timers, cross-thread events, async loop operations and console applications read:
  `.github/instructions/pt-system-api-eventloop.instructions.md`

- For asynchronous I/O, native handle/fd monitoring, streams, pipes, serial ports, URI parsing and tar archives read:
  `.github/instructions/pt-system-api-io.instructions.md`

- For TCP and UDP sockets, endpoints, connection setup and multicast read:
  `.github/instructions/pt-net-api.instructions.md`

- For reflection, meta-objects, types, invocation and properties read:
  `.github/instructions/pt-reflex-api.instructions.md`

- For Lua scripting, script execution, reflected bindings and asynchronous Lua calls read:
  `.github/instructions/pt-lua-api.instructions.md`

- For HTTP basics, messages, requests, replies, streams, errors and credentials read:
  `.github/instructions/pt-http-api-messages.instructions.md`

- For HTTP clients, authentication, requests, persistent connections and pipelining read:
  `.github/instructions/pt-http-api-clients.instructions.md`

- For HTTP servers, authorization, responders, services, request routing and HTTPS read:
  `.github/instructions/pt-http-api-servers.instructions.md`

- For HTTP WebSocket upgrade, framed I/O and handshake services read:
  `.github/instructions/pt-http-api-websocket.instructions.md`

- For implementing RPC clients and services:
  `.github/instructions/pt-remoting-api.instructions.md`

- For XML-RPC clients and services:
  `.github/instructions/pt-xmlrpc-api.instructions.md`

- For JSON-RPC clients, services, HTTP transport, serialization read:
  `.github/instructions/pt-jsonrpc-api.instructions.md`

- For SOAP clients and services:
  `.github/instructions/pt-soap-api.instructions.md`

- For implementing an MCP server, MCP tools, HTTP transport, standard-I/O transport read:
  `.github/instructions/pt-mcp-api.instructions.md`

- For SSL/TLS encrypted streams, certificates, contexts, verification, handshakes read:
  `.github/instructions/pt-ssl-api.instructions.md`

- For Create, implement, register, declare, activate or deactivate a Cosmo component,
  implement a component interface, wire component dependencies, use the
  component framework, review component code read:
  `.github/instructions/pt-cosmo-api-components.instructions.md`

- For Cosmo enumerable collections, type-safe iteration, resource lifetime, auto-release pools, smart pointers, and buffers read:
  `.github/instructions/pt-cosmo-api-utilities.instructions.md`

- For Pixel types, image formats, image access, pixel iteration (`Pt::Gfx` images) read:
  `.github/instructions/pt-gfx-api-images.instructions.md`

- For Draw shapes, paths, text, bitmaps, stroke, fill, gradient, paint surface read:
  `.github/instructions/pt-gfx-api-drawing.instructions.md`

- For Forms application lifecycle, runtime services, and visual hierarchy read:
  `.github/instructions/pt-forms-api-application.instructions.md`

- For Forms widgets, views, controls, screens, responders, and form content read:
  `.github/instructions/pt-forms-api-widgets.instructions.md`

- For Forms windows, popups, workspaces, window management, and window events read:
  `.github/instructions/pt-forms-api-windows.instructions.md`

- For Forms invalidation, layout, painting, graphics backends, and pixmaps read:
  `.github/instructions/pt-forms-api-updating.instructions.md`

- For Forms pointer, touch, keyboard, focus, cursor, and text-input events read:
  `.github/instructions/pt-forms-api-input.instructions.md`

- For Forms layout containers, child arrangement, sizing, spacing, and alignment read:
  `.github/instructions/pt-forms-api-layouts.instructions.md`

- For Forms command buttons, push buttons, check boxes, and their renderers read:
  `.github/instructions/pt-forms-api-buttons.instructions.md`

- For Forms labels, panels, progress displays, and their renderers read:
  `.github/instructions/pt-forms-api-displays.instructions.md`

- For Forms text, number, choice, and range editors with their renderers read:
  `.github/instructions/pt-forms-api-editors.instructions.md`

- For Forms lists, tabs, scroll bars, scroll views, and their renderers read:
  `.github/instructions/pt-forms-api-collections.instructions.md`

- For Forms menu bars, popup menus, menu items, and submenus read:
  `.github/instructions/pt-forms-api-menus.instructions.md`

- For Forms icons, icon providers, and reusable formatted text blocks read:
  `.github/instructions/pt-forms-api-icons.instructions.md`

- For Forms styles, style options, stylers, renderers, and the Platinum style read:
  `.github/instructions/pt-forms-api-styling.instructions.md`

- For XML parsing, writing, DTDs, namespaces, and serialization read:
  `.github/instructions/pt-xml-api.instructions.md`
