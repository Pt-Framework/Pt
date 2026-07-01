/* Copyright (C) 2008 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#include "TarHeader.h"

#include <Pt/System/TarWriter.h>
#include <Pt/IOError.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>

namespace Pt {

///////////////////////////////////////////////////////////////////////
// TarWriterImpl
///////////////////////////////////////////////////////////////////////

class TarWriterImpl
{
  public:
    TarWriterImpl()
    : _target(0)
    { }

    explicit TarWriterImpl(std::ostream& os)
    : _target(&os)
    { }

    void attach(std::ostream& os)
    {
        _target = &os;
    }

    void detach()
    {
        _target = 0;
    }

    void reset()
    {
        _target = 0;
    }

    void addFile(const Pt::System::Path& path,
                 const char* data,
                 std::size_t size,
                 Pt::System::FileInfo::Perms permissions)
    {
        std::string p = path.toLocal();
        writePaxHeaderIfNeeded(p, "");
        writeUStarHeader(p, "", '0', size, permissions, std::time(0));
        if(data && size > 0)
        {
            writeRaw(data, size);
            writePadding((512u - (size % 512u)) % 512u);
        }
    }

    void addDirectory(const Pt::System::Path& path,
                      Pt::System::FileInfo::Perms permissions)
    {
        std::string p = path.toLocal();
        if( ! p.empty() && p.back() != '/')
            p += '/';
        writePaxHeaderIfNeeded(p, "");
        writeUStarHeader(p, "", '5', 0, permissions, std::time(0));
    }

    void addSymlink(const Pt::System::Path& path,
                    const Pt::System::Path& target)
    {
        std::string p = path.toLocal();
        std::string t = target.toLocal();
        writePaxHeaderIfNeeded(p, t);
        writeUStarHeader(p, t, '2', 0, Pt::System::FileInfo::NoPerms,
                         std::time(0));
    }

    void addHardlink(const Pt::System::Path& path,
                     const Pt::System::Path& target)
    {
        std::string p = path.toLocal();
        std::string t = target.toLocal();
        writePaxHeaderIfNeeded(p, t);
        writeUStarHeader(p, t, '1', 0, Pt::System::FileInfo::NoPerms,
                         std::time(0));
    }

    void finish()
    {
        char block[512] = {};
        writeRaw(block, 512);
        writeRaw(block, 512);
    }

  private:
    std::ostream* _target;

    void requireTarget()
    {
        if( ! _target )
            throw Pt::IOError("tar writer: no output stream");
    }

    void writeRaw(const char* data, std::size_t size)
    {
        requireTarget();
        _target->write(data, static_cast<std::streamsize>(size));
        if(_target->fail())
            throw Pt::IOError("tar writer: write failed");
    }

    void writePadding(std::size_t n)
    {
        static const char zeros[512] = {};
        while(n >= sizeof(zeros))
        {
            writeRaw(zeros, sizeof(zeros));
            n -= sizeof(zeros);
        }
        if(n > 0)
            writeRaw(zeros, n);
    }

    static void writeOctalField(char* field, std::size_t len,
                                unsigned long long value)
    {
        char fmt[16];
        std::snprintf(fmt, sizeof(fmt), "%%0%zullo",
                      static_cast<std::size_t>(len - 1));
        char tmp[64];
        std::snprintf(tmp, sizeof(tmp), fmt, value);
        std::memcpy(field, tmp, len - 1);
        field[len - 1] = '\0';
    }

    static unsigned computeChecksum(const char* block)
    {
        unsigned sum = 0;
        for(int i = 0; i < 512; ++i)
        {
            if(i >= 148 && i < 156)
                sum += 32u;
            else
                sum += static_cast<unsigned>(
                           static_cast<unsigned char>(block[i]));
        }
        return sum;
    }

    // Returns a single Pax record: "<len> <key>=<value>\n"
    static std::string makePaxRecord(const std::string& key,
                                     const std::string& value)
    {
        // len includes all bytes of the record (including the len digits)
        std::size_t baseLen = 1u + key.size() + 1u + value.size() + 1u;
        std::size_t len = baseLen;
        for(;;)
        {
            std::string lenStr = std::to_string(len);
            if(lenStr.size() + baseLen == len)
                break;
            len = lenStr.size() + baseLen;
        }
        return std::to_string(len) + " " + key + "=" + value + "\n";
    }

    static bool hasNonAscii(const std::string& s)
    {
        for(unsigned char c : s)
        {
            if(c > 127u)
                return true;
        }
        return false;
    }

    static bool needsPax(const std::string& path, const std::string& linkname)
    {
        return path.size() > 99u
            || ( ! linkname.empty() && linkname.size() > 99u)
            || hasNonAscii(path)
            || ( ! linkname.empty() && hasNonAscii(linkname));
    }

    void writePaxHeaderIfNeeded(const std::string& path,
                                const std::string& linkname)
    {
        if( ! needsPax(path, linkname) )
            return;

        // Build pax record content
        std::string content;
        if(path.size() > 99u || hasNonAscii(path))
            content += makePaxRecord("path", path);
        if( ! linkname.empty() && (linkname.size() > 99u || hasNonAscii(linkname)))
            content += makePaxRecord("linkpath", linkname);

        if(content.empty())
            return;

        // Derive a short name for the pax header block itself
        std::string paxName = "PaxHeaders/";
        std::size_t slash   = path.rfind('/');
        paxName += (slash == std::string::npos) ? path : path.substr(slash + 1);
        if(paxName.size() > 99u)
            paxName.resize(99u);

        writeUStarHeader(paxName, "", 'x', content.size(),
                         Pt::System::FileInfo::NoPerms, std::time(0));
        writeRaw(content.c_str(), content.size());
        writePadding((512u - (content.size() % 512u)) % 512u);
    }

    void writeUStarHeader(const std::string& path,
                          const std::string& linkname,
                          char typeflag,
                          std::size_t size,
                          Pt::System::FileInfo::Perms permissions,
                          time_t mtime)
    {
        char block[512] = {};
        auto* h = reinterpret_cast<UStarHeader*>(block);

        // Split path into UStar prefix + name if needed
        std::string name   = path;
        std::string prefix;
        if(name.size() > 99u)
        {
            // Find rightmost '/' within prefix length limit
            for(std::size_t i = std::min(path.size() - 1u, std::size_t(154u));
                i > 0u; --i)
            {
                if(path[i] == '/')
                {
                    std::string candidate = path.substr(i + 1);
                    if(candidate.size() <= 99u)
                    {
                        prefix = path.substr(0, i);
                        name   = candidate;
                        break;
                    }
                }
            }
            if(name.size() > 99u)
                name.resize(99u); // truncate as last resort
        }

        // name
        std::memcpy(h->name, name.c_str(),
                    std::min(name.size(), sizeof(h->name) - 1u));

        // prefix
        if( ! prefix.empty())
            std::memcpy(h->prefix, prefix.c_str(),
                        std::min(prefix.size(), sizeof(h->prefix) - 1u));

        // numeric fields
        unsigned long long permbits =
            static_cast<unsigned long long>(permissions) & 0777ull;
        writeOctalField(h->mode,  sizeof(h->mode),  permbits);
        writeOctalField(h->uid,   sizeof(h->uid),   0ull);
        writeOctalField(h->gid,   sizeof(h->gid),   0ull);
        writeOctalField(h->size,  sizeof(h->size),
                        static_cast<unsigned long long>(size));
        writeOctalField(h->mtime, sizeof(h->mtime),
                        static_cast<unsigned long long>(mtime < 0 ? 0 : mtime));

        // typeflag
        h->typeflag = typeflag;

        // linkname
        if( ! linkname.empty())
            std::memcpy(h->linkname, linkname.c_str(),
                        std::min(linkname.size(), sizeof(h->linkname) - 1u));

        // magic + version
        std::memcpy(h->magic, "ustar", 5);
        h->magic[5]   = '\0';
        h->version[0] = '0';
        h->version[1] = '0';

        // uname + gname
        std::memcpy(h->uname, "root", 4);
        std::memcpy(h->gname, "root", 4);

        // checksum (must be computed last)
        unsigned sum = computeChecksum(block);
        std::snprintf(h->checksum, 8, "%06o", sum);
        h->checksum[6] = '\0';
        h->checksum[7] = ' ';

        writeRaw(block, 512);
    }
};

///////////////////////////////////////////////////////////////////////
// TarWriter
///////////////////////////////////////////////////////////////////////

TarWriter::TarWriter()
: _impl(new TarWriterImpl())
{ }


TarWriter::TarWriter(std::ostream& os)
: _impl(new TarWriterImpl(os))
{ }


TarWriter::~TarWriter()
{
    delete _impl;
}


void TarWriter::attach(std::ostream& os)
{
    _impl->attach(os);
}


void TarWriter::detach()
{
    _impl->detach();
}


void TarWriter::reset()
{
    _impl->reset();
}


void TarWriter::addFile(const Pt::System::Path& path,
                        const char* data,
                        std::size_t size,
                        Pt::System::FileInfo::Perms permissions)
{
    _impl->addFile(path, data, size, permissions);
}


void TarWriter::addDirectory(const Pt::System::Path& path,
                             Pt::System::FileInfo::Perms permissions)
{
    _impl->addDirectory(path, permissions);
}


void TarWriter::addSymlink(const Pt::System::Path& path,
                           const Pt::System::Path& target)
{
    _impl->addSymlink(path, target);
}


void TarWriter::addHardlink(const Pt::System::Path& path,
                            const Pt::System::Path& target)
{
    _impl->addHardlink(path, target);
}


void TarWriter::finish()
{
    _impl->finish();
}

} // namespace
