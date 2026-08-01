/*
 * Copyright (C) 2020-2026 by Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef PT_MCP_CONTENTTYPE_H
#define PT_MCP_CONTENTTYPE_H

#include <Pt/Mcp/Api.h>
#include <Pt/Formatter.h>
#include <string>

namespace Pt {

namespace Mcp {

/** @brief Writes the wrapper bytes of an MCP content block around a Formatter.

    ContentFormatter adds the begin/finish protocol for the enclosing
    content block (e.g. {"type":"text","text":"...")) around an existing
    Pt::Formatter: beginFormat() writes the opening bytes and returns the
    Pt::Formatter the decomposed result is drained into via
    Decomposer::beginFormat() and Decomposer::advanceFormat();
    finishFormat() flushes any buffered output and writes the closing
    bytes once the result is fully drained. A ContentFormatter does not
    need to be a Formatter itself - it may wrap an existing one. Instances
    are created by ContentType::getFormatter() and must be released via
    ContentType::releaseFormatter().
*/
class PT_MCP_API ContentFormatter
{
  public:
    virtual ~ContentFormatter();

    /** @brief Writes the opening bytes of the content block to @a os and
        returns the Formatter the result is to be decomposed into.
    */
    Pt::Formatter& beginContent(std::ostream& os);

    /** @brief Flushes buffered output and writes the closing bytes of the
        content array to @a os.
    */
    void finishContent(std::ostream& os);

  protected:
    ContentFormatter();

    /** @brief Writes the opening bytes of the concrete content block.

        Implementations write the type-specific opening JSON, e.g.
        {"type":"text","text":".
    */
    virtual void onBeginContent() = 0;

    /** @brief Returns the Formatter the decomposed result is drained into.

        Called by beginContent() after onBeginContent() has written the opening bytes.
    */
    virtual Pt::Formatter& onBeginFormat() = 0;

    /** @brief Writes the closing bytes of the concrete content block.
    */
    virtual void onFinishContent() = 0;

    /** @brief Writes raw bytes to the output stream.
    */
    void write(const char* s, std::size_t n);

    /** @brief Returns the output stream passed to beginContent().
    */
    std::ostream& output() const
    { return *_os; }

  private:
    std::ostream* _os;
};

/** @brief Formats a tool result as MCP content.

    ContentType determines how a Decomposer result is serialized into an
    MCP content block (text, image, etc.). getFormatter() creates the
    ContentFormatter used to write the block; the caller drives it
    directly (beginFormat(), the Decomposer begin/advance protocol,
    finishFormat()) and releases it via releaseFormatter().

    @par Thread-safety
    Implementations are shared, process-wide singletons (see textContent(),
    imageContent()) that may be used by many Responders concurrently.
    getFormatter() must therefore be stateless: it may only read immutable
    members of the ContentType (set once at construction) and must never
    cache a Formatter, stream or scratch buffer as a mutable member of the
    ContentType itself. All per-request state must live exclusively in the
    ContentFormatter instance returned by getFormatter().
*/
class PT_MCP_API ContentType
{
  public:
    virtual ~ContentType();

    /** @brief Creates a new ContentFormatter for a result.

        The caller owns the returned ContentFormatter and must release it
        via releaseFormatter() once done with it, whether the format
        completed normally or was abandoned. The output stream is not
        known yet; it is passed to ContentFormatter::beginFormat().
    */
    virtual ContentFormatter* getFormatter() const = 0;

    /** @brief Releases a ContentFormatter previously obtained from getFormatter().

        Deletes @a formatter; the caller must not use it afterward. Does
        not write anything - call formatter->finishFormat() first to
        complete the content block normally.
    */
    virtual void releaseFormatter(ContentFormatter* formatter) const = 0;
};


/** @brief Formats tool results as MCP text content.

    Serializes the result using TextFormatter (compact YAML-like)
    and embeds it in a text content block.
*/
class PT_MCP_API TextContent : public ContentType
{
  public:
    TextContent();

    ContentFormatter* getFormatter() const override;

    void releaseFormatter(ContentFormatter* formatter) const override;
};

PT_MCP_API const TextContent& textContent();


/** @brief Formats tool results as MCP image content.

    Serializes a decomposed binary result (the tool's return value must
    decompose to a single Binary node) as a base64-encoded MCP "image"
    content block. The binary bytes are base64-encoded incrementally as
    they are decomposed, without buffering the whole image in memory.
*/
class PT_MCP_API ImageContent : public ContentType
{
  public:
    explicit ImageContent(const std::string& mimeType = "image/png");

    ContentFormatter* getFormatter() const override;

    void releaseFormatter(ContentFormatter* formatter) const override;

  private:
    std::string _mimeType;
};

PT_MCP_API const ImageContent& imageContent();

} // namespace Mcp

} // namespace Pt

#endif // PT_MCP_CONTENTTYPE_H
