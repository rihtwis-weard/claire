from conans import ConanFile, CMake


SETTINGS = (
  "os",
  "compiler",
  "build_type",
  "arch"
)

REQUIRES = (
  "approvaltests.cpp/10.12.0",
  "boost-ext-ut/1.1.8",
  "robin-hood-hashing/3.11.3"
)

GENERATORS = (
  "cmake", "cmake_find_package"
)


class ClaireConan(ConanFile):
  settings = SETTINGS
  requires = REQUIRES
  generators = GENERATORS
  default_options = {}

  def build(self):
    cmake = CMake(self)
    cmake.configure()
    cmake.build()
