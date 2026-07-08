/*
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Pt/System/TarReader.h>
#include <Pt/System/TarEntry.h>
#include <Pt/System/FileInfo.h>
#include <Pt/System/Path.h>
#include <Pt/IOError.h>
#include <Pt/Arg.h>
#include <Pt/ZStream.h>

#if __cplusplus >= 201703L
#include <filesystem>
#endif

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <cstdio>

static std::string formatPerms(Pt::System::FileInfo::Perms p)
{
    char buf[8];
    std::snprintf(buf, sizeof(buf), "%04o",
                  static_cast<unsigned>(p) & 07777u);
    return buf;
}


static bool isGzip(const std::string& path)
{
    const std::size_t len = path.size();
    if(len > 7 && path.compare(len - 7, 7, ".tar.gz") == 0)
        return true;
    if(len > 4 && path.compare(len - 4, 4, ".tgz") == 0)
        return true;
    return false;
}


static void extract(Pt::System::TarReader& reader, const Pt::System::Path& outDir)
{
    std::ofstream currentFile;

    while( ! reader.isEnd() )
    {
        const Pt::System::TarEntry* entry = reader.advance(8192);
        if( ! entry )
            break;  // starved — in real code: return, wait for data, come back

        // Continuation of the current file
        if( currentFile.is_open() )
        {
            if(entry->avail() > 0)
                currentFile.write(entry->data(),
                              static_cast<std::streamsize>(entry->avail()));

            if(entry->isEnd())
            {
                currentFile.close();
                Pt::System::FileInfo::permissions(outDir / entry->path(), entry->permissions(),
                                                  Pt::System::FileInfo::PermReplace);
            }
            continue;
        }

        // New entry
        const Pt::System::Path fullPath = outDir / entry->path();

        if(entry->type() == Pt::System::TarEntry::Directory)
        {
            std::cout << "  dir  " << formatPerms(entry->permissions())
                      << "  " << fullPath.toLocal() << "\n";
            Pt::System::FileInfo::createDirectories(fullPath);
            Pt::System::FileInfo::permissions(fullPath, entry->permissions(),
                                              Pt::System::FileInfo::PermReplace);
        }
        else if(entry->type() == Pt::System::TarEntry::Link)
        {
            std::cout << "  link " << formatPerms(entry->permissions())
                      << "  " << fullPath.toLocal()
                      << " -> " << entry->linkTarget().toLocal() << "\n";
        }
        else if(entry->type() == Pt::System::TarEntry::Hardlink)
        {
            std::cout << "  hard " << formatPerms(entry->permissions())
                      << "  " << fullPath.toLocal()
                      << " -> " << entry->linkTarget().toLocal() << "\n";
        }
        else if(entry->type() == Pt::System::TarEntry::File)
        {
            std::cout << "  file " << formatPerms(entry->permissions())
                      << "  " << fullPath.toLocal() << "\n";

            Pt::System::Path parentDir( fullPath.dirName() );
            Pt::System::FileInfo::createDirectories(parentDir);

#if __cplusplus >= 201703L
            Pt::String p = fullPath.toString();
            const char32_t* p32 = reinterpret_cast<const char32_t*>(p.c_str());
            const std::filesystem::path fspath(p32);
            currentFile.open(fspath, std::ios::binary | std::ios::trunc);
#else
            currentFile.open(fullPath.toLocal(), std::ios::binary | std::ios::trunc);
#endif
            if( ! currentFile.is_open() )
                throw Pt::AccessFailed( fullPath.toLocal() );

            if(entry->avail() > 0)
                currentFile.write(entry->data(),
                                  static_cast<std::streamsize>(entry->avail()));

            if(entry->isEnd())
            {
                currentFile.close();
                Pt::System::FileInfo::permissions(fullPath, entry->permissions(),
                                                  Pt::System::FileInfo::PermReplace);
            }
        }
        else
        {
            std::cout << "  ???  " << formatPerms(entry->permissions())
                      << "  " << fullPath.toLocal()
                      << " (unknown tar entry type)\n";
        }
    }
}


int main(int argc, char* argv[])
{
    Pt::Arg<bool> gz(argc, argv, 'z', false);
    Pt::Arg<bool> help(argc, argv, "--help", false);
    Pt::Arg<std::string> archive(argc, argv);
    Pt::Arg<std::string> outDirPath(argc, argv);

    if(help || ! archive.isSet() || ! outDirPath.isSet())
    {
        std::cerr << "usage: tar-demo [-z] <archive> <output-dir>\n"
                  << "  -z    force gzip decompression (auto-detected for .tar.gz/.tgz)\n";
        return 1;
    }

    try
    {
        const bool useGzip = gz || isGzip(archive.get());
        const Pt::System::Path outDirP(outDirPath.get().c_str());

        std::ifstream ifs(archive.get(), std::ios::binary);
        if( ! ifs.is_open() )
        {
            std::cerr << "error: cannot open " << archive.get() << "\n";
            return 1;
        }

        if(useGzip)
        {
            Pt::ZIStream zis(ifs, Pt::ZBuffer::Gzip);
            Pt::System::TarReader reader(zis);
            extract(reader, outDirP);
        }
        else
        {
            Pt::System::TarReader reader(ifs);
            extract(reader, outDirP);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "error: " << e.what() << " (" << typeid(e).name() << ")\n";
        return 1;
    }

    return 0;
}
