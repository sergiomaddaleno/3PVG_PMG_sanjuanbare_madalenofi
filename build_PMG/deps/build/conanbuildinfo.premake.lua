#!lua
conan_build_type = "Release"
conan_arch = "x86_64"

conan_includedirs = {"C:/Users/madalenofi/.conan/data/glfw/3.3.8/_/_/package/d3cffeefc8c8bcb6be8abdd182cb1b56fc81bacc/include",
"C:/Users/madalenofi/.conan/data/glew/2.2.0/_/_/package/f923874e83d95e94b1d71b4181e264b3716ade72/include"}
conan_libdirs = {"C:/Users/madalenofi/.conan/data/glfw/3.3.8/_/_/package/d3cffeefc8c8bcb6be8abdd182cb1b56fc81bacc/lib",
"C:/Users/madalenofi/.conan/data/glew/2.2.0/_/_/package/f923874e83d95e94b1d71b4181e264b3716ade72/lib"}
conan_bindirs = {}
conan_libs = {"glfw3", "libglew32"}
conan_system_libs = {"gdi32", "glu32", "opengl32"}
conan_defines = {"GLEW_STATIC"}
conan_cxxflags = {}
conan_cflags = {}
conan_sharedlinkflags = {}
conan_exelinkflags = {}
conan_frameworks = {}

conan_includedirs_glfw = {"C:/Users/madalenofi/.conan/data/glfw/3.3.8/_/_/package/d3cffeefc8c8bcb6be8abdd182cb1b56fc81bacc/include"}
conan_libdirs_glfw = {"C:/Users/madalenofi/.conan/data/glfw/3.3.8/_/_/package/d3cffeefc8c8bcb6be8abdd182cb1b56fc81bacc/lib"}
conan_bindirs_glfw = {}
conan_libs_glfw = {"glfw3"}
conan_system_libs_glfw = {"gdi32"}
conan_defines_glfw = {}
conan_cxxflags_glfw = {}
conan_cflags_glfw = {}
conan_sharedlinkflags_glfw = {}
conan_exelinkflags_glfw = {}
conan_frameworks_glfw = {}
conan_rootpath_glfw = "C:/Users/madalenofi/.conan/data/glfw/3.3.8/_/_/package/d3cffeefc8c8bcb6be8abdd182cb1b56fc81bacc"

conan_includedirs_glew = {"C:/Users/madalenofi/.conan/data/glew/2.2.0/_/_/package/f923874e83d95e94b1d71b4181e264b3716ade72/include"}
conan_libdirs_glew = {"C:/Users/madalenofi/.conan/data/glew/2.2.0/_/_/package/f923874e83d95e94b1d71b4181e264b3716ade72/lib"}
conan_bindirs_glew = {}
conan_libs_glew = {"libglew32"}
conan_system_libs_glew = {}
conan_defines_glew = {"GLEW_STATIC"}
conan_cxxflags_glew = {}
conan_cflags_glew = {}
conan_sharedlinkflags_glew = {}
conan_exelinkflags_glew = {}
conan_frameworks_glew = {}
conan_rootpath_glew = "C:/Users/madalenofi/.conan/data/glew/2.2.0/_/_/package/f923874e83d95e94b1d71b4181e264b3716ade72"

conan_includedirs_glu = {}
conan_libdirs_glu = {}
conan_bindirs_glu = {}
conan_libs_glu = {}
conan_system_libs_glu = {"glu32"}
conan_defines_glu = {}
conan_cxxflags_glu = {}
conan_cflags_glu = {}
conan_sharedlinkflags_glu = {}
conan_exelinkflags_glu = {}
conan_frameworks_glu = {}
conan_rootpath_glu = "C:/Users/madalenofi/.conan/data/glu/system/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9"

conan_includedirs_opengl = {}
conan_libdirs_opengl = {}
conan_bindirs_opengl = {}
conan_libs_opengl = {}
conan_system_libs_opengl = {"opengl32"}
conan_defines_opengl = {}
conan_cxxflags_opengl = {}
conan_cflags_opengl = {}
conan_sharedlinkflags_opengl = {}
conan_exelinkflags_opengl = {}
conan_frameworks_opengl = {}
conan_rootpath_opengl = "C:/Users/madalenofi/.conan/data/opengl/system/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9"

function conan_basic_setup()
    configurations{conan_build_type}
    architecture(conan_arch)
    includedirs{conan_includedirs}
    libdirs{conan_libdirs}
    links{conan_libs}
    links{conan_system_libs}
    links{conan_frameworks}
    defines{conan_defines}
    bindirs{conan_bindirs}
end
