/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_MCP_CONTENTTYPE_H
#define PT_MCP_CONTENTTYPE_H

#include <Pt/Mcp/Api.h>
#include <Pt/Formatter.h>
#include <string>

namespace Pt {

namespace Mcp {

/** @brief Writer for one MCP content block around a Formatter.

    %ContentFormatter is the per-request writer the result-content
    group described. %beginContent() writes the opening bytes of the
    content array and of the concrete block (for example a text
    member), then returns the %Pt::Formatter the decomposed result is
    drained into. %finishContent() flushes buffered output and writes
    the closing bytes. The formatter is not itself a %Pt::Formatter;
    it wraps one.

    Instances come from %ContentType::getFormatter() and must be
    released with %ContentType::releaseFormatter(). Subclass this type
    to implement a custom block: %onBeginContent() writes the
    type-specific opening JSON, %onBeginFormat() returns the inner
    formatter, and %onFinishContent() writes the closing JSON.
    %write() and %output() are for those overrides.

    @ingroup Pt-Mcp-Content
*/
class PT_MCP_API ContentFormatter
{
  public:
    /** @brief Destroys the formatter.
    */
    virtual ~ContentFormatter();

    /** @brief Writes the opening bytes of the content block to @a os.

        Returns the %Pt::Formatter the result is to be decomposed into.
    */
    Pt::Formatter& beginContent(std::ostream& os);

    /** @brief Flushes buffered output and writes the closing bytes to @a os.
    */
    void finishContent(std::ostream& os);

  protected:
    /** @brief Creates an idle content formatter.
    */
    ContentFormatter();

    /** @brief Writes the opening bytes of the concrete content block.

        Implementations write the type-specific opening JSON, for
        example the start of a text or image member.
    */
    virtual void onBeginContent() = 0;

    /** @brief Returns the Formatter the decomposed result is drained into.

        Called by %beginContent() after %onBeginContent() has written
        the opening bytes.
    */
    virtual Pt::Formatter& onBeginFormat() = 0;

    /** @brief Writes the closing bytes of the concrete content block.
    */
    virtual void onFinishContent() = 0;

    /** @brief Writes @a n raw bytes from @a s to the output stream.
    */
    void write(const char* s, std::size_t n);

    /** @brief Returns the output stream passed to %beginContent().
    */
    std::ostream& output() const
    { return *_os; }

  private:
    std::ostream* _os;
};

/** @brief Strategy that formats a tool result as MCP content.

    %ContentType chooses how a decomposer result becomes an MCP
    content block. %getFormatter() creates the %ContentFormatter used
    to write that block. The caller drives %beginContent(), the
    decomposer begin/advance protocol, then %finishContent(), and
    releases the formatter with %releaseFormatter() whether the
    format completed or was abandoned.

    Implementations used as process-wide singletons, including
    %textContent() and %imageContent(), may run on many responders at
    once. %getFormatter() must be stateless on the content type: it
    may read immutable members set at construction and must not cache
    a formatter, stream, or scratch buffer as a mutable member. All
    per-request state belongs on the %ContentFormatter.

    @ingroup Pt-Mcp-Content
*/
class PT_MCP_API ContentType
{
  public:
    /** @brief Destroys the content type.
    */
    virtual ~ContentType();

    /** @brief Creates a new %ContentFormatter for a result.

        The caller owns the returned formatter and must release it
        with %releaseFormatter(). The output stream is passed later to
        %ContentFormatter::beginContent().
    */
    virtual ContentFormatter* getFormatter() const = 0;

    /** @brief Releases a formatter previously obtained from %getFormatter().

        Deletes @a formatter. The caller must not use it afterward.
        Call %ContentFormatter::finishContent() first to complete the
        content block normally.
    */
    virtual void releaseFormatter(ContentFormatter* formatter) const = 0;
};


/** @brief Formats tool results as MCP text content.

    %TextContent serializes the procedure result with a compact
    YAML-like formatter and embeds that text in an MCP text content
    block. %textContent() is the process-wide instance and the default
    for every %Tool.

    @ingroup Pt-Mcp-Content
*/
class PT_MCP_API TextContent : public ContentType
{
  public:
    /** @brief Creates a text content type.
    */
    TextContent();

    /** @brief Creates a new formatter for a text content block.
    */
    ContentFormatter* getFormatter() const override;

    /** @brief Releases a formatter previously obtained from %getFormatter().
    */
    void releaseFormatter(ContentFormatter* formatter) const override;
};

/** @brief Returns the process-wide text content type.

    @ingroup Pt-Mcp-Content
*/
PT_MCP_API const TextContent& textContent();


/** @brief Formats tool results as MCP image content.

    %ImageContent serializes a decomposed binary result as a
    base64-encoded MCP image content block. The tool's return value
    must decompose to a single Binary node. The bytes are encoded as
    they are decomposed, without buffering the whole image. The MIME
    type is set at construction and defaults to image/png.

    %imageContent() is the process-wide instance with that default
    MIME type. A different MIME type needs a caller-owned
    %ImageContent that outlives the tool.

    @ingroup Pt-Mcp-Content
*/
class PT_MCP_API ImageContent : public ContentType
{
  public:
    /** @brief Creates an image content type with MIME type @a mimeType.
    */
    explicit ImageContent(const std::string& mimeType = "image/png");

    /** @brief Creates a new formatter for an image content block.
    */
    ContentFormatter* getFormatter() const override;

    /** @brief Releases a formatter previously obtained from %getFormatter().
    */
    void releaseFormatter(ContentFormatter* formatter) const override;

  private:
    std::string _mimeType;
};

/** @brief Returns the process-wide PNG image content type.

    @ingroup Pt-Mcp-Content
*/
PT_MCP_API const ImageContent& imageContent();

} // namespace Mcp

} // namespace Pt

#endif // PT_MCP_CONTENTTYPE_H
