from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, cmake_layout

windows = "Windows"
linux = "Linux"


class CubeConnectClientRecipe(ConanFile):

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"
    requires = ["enet/1.3.18", "glfw/3.4", "glad/0.1.36", "stb/cci.20230920",
                "glm/cci.20230113", "assimp/5.4.1",
                "imgui/cci.20230105+1.89.2.docking"]

    def build_requirements(self):
        if self.settings.os == windows:
            print("using mingw")
            self.tool_requires("mingw-w64/8.0.2")

    def generate(self):
        tc = CMakeToolchain(self)
        if self.settings.os == windows:
            tc.presets_prefix = "conan-windows"
        elif self.settings.os == linux:
            tc.presets_prefix = "conan-linux"
        tc.generate()

    def layout(self):
        build_folder = ""
        if self.settings.os == windows:
            build_folder = "build_windows"
        elif self.settings.os == linux:
            build_folder = "build_linux"
        cmake_layout(self, build_folder=build_folder)
