from conan import ConanFile


class PtPrebuiltConan(ConanFile):
    name = "pt-prebuilt"
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
