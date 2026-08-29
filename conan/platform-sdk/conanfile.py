import os
import sys

from conan import ConanFile


class PtPlatformSdkConan(ConanFile):
    name = "pt-platform-sdk"
    description = "Precompiled third-party libraries for Platinum (CI zip only)"
    license = "Proprietary"
    url = "https://github.com/Pt-Framework/Pt"

    settings = "os", "compiler", "build_type", "arch"

    default_options = {
        "*:shared": True,
    }

    def requirements(self):
        self.requires("zlib/[>=1.2.11 <2]")
        self.requires("libpng/[>=1.6 <2]")
        self.requires("libjpeg-turbo/[>=3.0]")
        self.requires("freetype/[>=2.10 <3]")
        self.requires("openssl/[>=3.0 <4]")

    def generate(self):
        conan_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        sys.path.insert(0, conan_dir)
        from JamDepsGenerator import JamDepsGenerator

        is_debug = str(self.settings.build_type) == "Debug"
        filename = "platform-sdk-debug.jam" if is_debug else "platform-sdk-release.jam"
        JamDepsGenerator(self,
                         root_var="PLATFORM_SDK_ROOT",
                         output_filename=filename).generate()
