/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_MCP_API_CONTENT_H
#define PT_MCP_API_CONTENT_H

/** @addtogroup Pt-Mcp-Content

    @brief Format a tool result as an MCP content block.

    A successful tools/call result is not raw JSON of the return
    value. It is an MCP content array, and each block has a type such
    as text or image. %ContentType chooses that block. The responder
    decomposes the procedure result and drains it through a
    %ContentFormatter the content type creates.

    %getFormatter() returns a new %ContentFormatter. The caller owns
    it until %releaseFormatter(). %beginContent() writes the opening
    bytes of the content array and of the concrete block, then
    returns the %Pt::Formatter the decomposer writes into.
    %finishContent() writes the closing bytes. The output stream is
    passed to those two calls, not to %getFormatter().

    %textContent() is the default. It serializes the result as compact
    YAML-like text and wraps it in a text block. %imageContent()
    expects a result that decomposes to a single Binary node, encodes
    those bytes as base64, and wraps them in an image block with MIME
    type image/png. Construct an %ImageContent with another MIME type
    when the image is not PNG.

    %textContent() and %imageContent() return process-wide singletons.
    They may be used by many responders at once. %getFormatter() must
    therefore be stateless on the %ContentType: per-request state
    lives only in the %ContentFormatter. A custom %ContentType follows
    the same rule. Subclass %ContentType and %ContentFormatter, keep
    immutable configuration on the content type, and put buffers and
    the inner formatter on the formatter instance.

    %Tool::setContent() stores a pointer. The %ContentType must
    outlive the tool. The builtin singletons do.
*/

#endif
