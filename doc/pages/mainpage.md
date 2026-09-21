\mainpage

\htmlonly
</p>
<div class="pt-page-marker pt-page-home">Home</div>

<div class="pt-hero">
  <h1 class="pt-hero-title">The Power in <span class="pt-hero-break">your Hands</span></h1>
  <p class="pt-meta">
    Portable &middot; Modular &middot; Asynchronous &middot; Standard C++ &middot;
    Flexible &middot; Open and proprietary
  </p>
  <p class="pt-hero-lead">
    Platinum (Pt) is a comprehensive C++ framework, which allows developers
    to write high-performance applications for many platforms with only one
    codebase. It provides a large amount of features and is still very easy
    to use. It integrates well into existing toolkits and frameworks.
  </p>
  <div class="pt-actions">
    <div class="pt-actions-start">
      <a class="pt-btn pt-btn-ghost" href="docs.html#getting-started">Getting Started</a>
      <a class="pt-btn pt-btn-ghost" href="docs.html">Documentation</a>
      <a class="pt-btn pt-btn-ghost" href="https://github.com/Pt-Framework/Pt">GitHub</a>
    </div>
    <a class="pt-btn pt-btn-primary" href="downloads.html">Download</a>
  </div>
</div>

<div class="pt-section">
  <h2>Modules</h2>
  <div class="pt-grid">
    <a class="pt-card" href="Pt-Core-Page.html">
      <h3>Core</h3>
      <p>Basic types, memory, text, callbacks, utilities, and serialization.</p>
    </a>
    <a class="pt-card" href="Pt-System-Page.html">
      <h3>System</h3>
      <p>File system, concurrency, event loops, I/O, plugins, and logging.</p>
    </a>
    <a class="pt-card" href="Pt-Net-Page.html">
      <h3>Net</h3>
      <p>TCP and UDP sockets on IPv4 and IPv6.</p>
    </a>
    <a class="pt-card" href="Pt-Http-Page.html">
      <h3>HTTP</h3>
      <p>HTTP messages, clients, servers, and WebSocket.</p>
    </a>
    <a class="pt-card" href="Pt-Ssl-Page.html">
      <h3>SSL</h3>
      <p>SSL/TLS streams, certificates, and contexts.</p>
    </a>
    <a class="pt-card" href="Pt-Gfx-Page.html">
      <h3>Gfx</h3>
      <p>Images, 2D drawing, and text.</p>
    </a>
    <a class="pt-card" href="Pt-Forms-Page.html">
      <h3>Forms</h3>
      <p>Native windows, widgets, layouts, and menus.</p>
    </a>
    <a class="pt-card" href="xmlrpc.html">
      <h3>XmlRpc</h3>
      <p>XML-RPC clients and services over HTTP.</p>
    </a>
    <a class="pt-card" href="Pt-Unit-Page.html">
      <h3>Unit</h3>
      <p>Protocol and data-driven unit testing.</p>
    </a>
    <a class="pt-card" href="Pt-Db-Page.html">
      <h3>Database</h3>
      <p>SQL connections, statements, results, and transactions.</p>
    </a>
    <a class="pt-card" href="Pt-Lua-Page.html">
      <h3>Lua</h3>
      <p>Lua runtime, reflected bindings, and script execution.</p>
    </a>
    <a class="pt-card" href="Pt-Mcp-Page.html">
      <h3>MCP</h3>
      <p>MCP servers that expose C++ procedures as tools.</p>
    </a>
    <div class="pt-card">
      <h3>Reflex</h3>
      <p>Object introspection, reflection, and meta-objects.</p>
    </div>
    <a class="pt-card" href="namespacePt_1_1Cosmo.html">
      <h3>Cosmo</h3>
      <p>Component model with interfaces, plugins, and dependency resolution.</p>
    </a>
  </div>
</div>

<div class="pt-section">
  <h2>Design</h2>
  <div class="pt-build">
    <div class="pt-build-item">
      <h3>Standard C++</h3>
      <p>
        Platinum is written in Standard C++. Templates deliver zero-cost
        abstractions, exceptions keep error handling off the success path,
        and inheritance gives the framework its structure. Together they
        produce an API that is fast, precise, and familiar to C++ developers.
      </p>
      <p>
        The C++ standard library is the foundation. I/O uses iostreams.
        Strings and containers are the ones developers already know, tuned
        and understood.
      </p>
    </div>
    <div class="pt-build-item">
      <h3>Asynchronous stack</h3>
      <p>
        Anything that might block can run asynchronously: sockets, files,
        and higher-level types such as the XML parser. They consume what is
        available and continue later. The same async stack keeps programs
        responsive on a single event loop.
      </p>
    </div>
    <div class="pt-build-item">
      <h3>Modular and non-intrusive</h3>
      <p>
        Each module builds as its own library. Depend only on the parts an
        application needs. The core stays small; the rest of the stack is
        optional. Pt provides solutions without being intrusive and
        integrates with existing code bases, toolkits, and frameworks.
      </p>
    </div>
    <div class="pt-build-item">
      <h3>One codebase</h3>
      <p>
        The same public API from embedded devices and desktops to servers
        and Wasm. Efficient abstractions such as iterators and streams keep
        application code compact, precise, and fast.
      </p>
    </div>
  </div>
</div>
<p>
\endhtmlonly
