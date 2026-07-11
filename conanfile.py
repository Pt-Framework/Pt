import os
import sys

from conan import ConanFile
from conan.tools.files import copy
from conan.tools.env import VirtualBuildEnv


class PtConan(ConanFile):
    name = "pt"
    description = "Platinum C++ framework"
    license = "Proprietary"
    url = "https://github.com/Pt-Framework/Pt"

    settings = "os", "compiler", "build_type", "arch"

    options = {
        "shared":       [True, False],
        "with_ssl":     [True, False],
        "with_gfx":     [True, False],
        "with_forms":   [True, False],
        "with_blend2d": [True, False],
    }
    default_options = {
        "shared":       False,
        "with_ssl":     False,
        "with_gfx":     False,
        "with_forms":   False,
        "with_blend2d": False,
    }

    # ---------------------------------------------------------------------------
    # Dependencies
    # ---------------------------------------------------------------------------

    def requirements(self):
        if self.options.with_gfx or self.options.with_forms:
            self.requires("libpng/[>=1.6 <2]")
            self.requires("zlib/[>=1.2.11 <2]")
            self.requires("libjpeg/[>=9e]")
            self.requires("freetype/[>=2.10 <3]")
        if self.options.with_ssl:
            self.requires("openssl/[>=3.0 <4]")

    # ---------------------------------------------------------------------------
    # Generate build helper files
    # ---------------------------------------------------------------------------

    def generate(self):
        # Activate the build environment (sets CC, CXX, etc. for cross-builds)
        VirtualBuildEnv(self).generate()

        # Write conan-deps.jam so Jamconfigure can find the Conan-provided deps
        sys.path.insert(0, os.path.join(self.source_folder, "conan"))
        from JamDepsGenerator import JamDepsGenerator
        JamDepsGenerator(self).generate()

    # ---------------------------------------------------------------------------
    # Build
    # ---------------------------------------------------------------------------

    def build(self):
        is_debug = str(self.settings.build_type) == "Debug"
        config   = "debug" if is_debug else "release"

        configure_args = [
            "configure",
            "-sCONFIG={}".format(config),
            "--with-conan",
            "-sCONAN_DEPS={}".format(
                os.path.join(self.generators_folder, "conan-deps.jam")
                    .replace("\\", "/")),
        ]

        if is_debug:
            configure_args.append("--debug")
        else:
            configure_args.append("--optimize")

        if self.options.with_ssl:
            configure_args.append("--with-ssl")
        if self.options.with_blend2d:
            configure_args.append("--with-blend2d")

        jam = "jam.bat" if self.settings.os == "Windows" else "./jam.sh"

        self.run("{} {}".format(jam, " ".join(configure_args)),
                 cwd=self.source_folder)
        self.run("{} -j8".format(jam),
                 cwd=self.source_folder)

    # ---------------------------------------------------------------------------
    # Package
    # ---------------------------------------------------------------------------

    def package(self):
        is_debug = str(self.settings.build_type) == "Debug"
        config   = "debug" if is_debug else "release"
        build_dir = os.path.join(self.source_folder, "build", config)

        # Public headers
        copy(self, "*.h",
             src=os.path.join(self.source_folder, "include"),
             dst=os.path.join(self.package_folder, "include"))

        # Static libraries
        copy(self, "*.lib",
             src=build_dir,
             dst=os.path.join(self.package_folder, "lib"),
             keep_path=False)
        copy(self, "*.a",
             src=build_dir,
             dst=os.path.join(self.package_folder, "lib"),
             keep_path=False)

        # Shared libraries
        copy(self, "*.dll",
             src=build_dir,
             dst=os.path.join(self.package_folder, "bin"),
             keep_path=False)
        copy(self, "*.so*",
             src=build_dir,
             dst=os.path.join(self.package_folder, "lib"),
             keep_path=False)
        copy(self, "*.dylib",
             src=build_dir,
             dst=os.path.join(self.package_folder, "lib"),
             keep_path=False)

    # ---------------------------------------------------------------------------
    # Package info (consumed by downstream projects)
    # ---------------------------------------------------------------------------

    def package_info(self):
        # Link order: most dependent first
        self.cpp_info.libs = [
            "PtForms",
            "PtGfx",
            "PtSoap",
            "PtXmlRpc",
            "PtJsonRpc",
            "PtMcp",
            "PtHttp",
            "PtSsl",
            "PtRemoting",
            "PtCosmo",
            "PtDb",
            "PtLua",
            "PtReflex",
            "PtXml",
            "PtJson",
            "PtNet",
            "PtSystem",
            "PtUnit",
            "Pt",
        ]
        self.cpp_info.includedirs = ["include"]

        if self.settings.os == "Windows":
            self.cpp_info.system_libs = [
                "ws2_32", "Iphlpapi", "Psapi",
            ]
        elif self.settings.os == "Linux":
            self.cpp_info.system_libs = ["pthread", "dl", "rt"]
        elif self.settings.os == "Macos":
            self.cpp_info.frameworks = ["AppKit", "Foundation", "Security"]
