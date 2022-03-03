local packages = json.decode(io.readfile(".cache/packages.json"))
local projects = "" .. _MAIN_SCRIPT_DIR .. "/.build/" .. _ACTION .. "/projects"

workspace "ce"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    startproject "ce-test"
    warnings "extra"
    symbols "On"
    language "C++"
    cppdialect "C++17"
    rtti "Off"
    exceptionhandling "Off"
    staticruntime "off"
    debugdir (_MAIN_SCRIPT_DIR)
    location ("" .. _MAIN_SCRIPT_DIR .. "/.build/" .. _ACTION)
    
    --defines { "CE_USER_INCLUDE=\"ce_config.h\"" }

filter { "configurations:Debug" }
    defines { "DEBUG" }
    flags { "FatalWarnings" }

filter { "configurations:Release" }
    defines { "NDEBUG" }
    optimize "speed"
    
filter {}

project ".build"
    kind "Makefile"
    location (projects)

    files { "*" }
    buildcommands { _PREMAKE_COMMAND .. " " .. _ACTION .. " --file=" .. _MAIN_SCRIPT }
    rebuildcommands { "cd " .. _MAIN_SCRIPT_DIR, "call premake5.bat" }
    cleancommands { "cd " .. _MAIN_SCRIPT_DIR }

project "ce"
    kind "StaticLib"
    location (projects)

    includedirs { "lib/h" }
    vpaths { ["/ce"] = "lib/h/**", ["*"] = "lib/*" }
    files { "lib/**" }

project "ce-test"
    kind "ConsoleApp"
    location (projects)

    links { "ce", "googletest" }
    includedirs { "lib/h", packages.googletest.h }
    files { "test/**" }

project "googletest"
    kind "StaticLib"
    location (projects)

    local self = packages.googletest
    includedirs
    { 
        self.h,
        self.src,
        self.src .. "../",
    }
    files
    { 
        self.h .. "**.h",
        self.src .. "**.h",
        self.src .. "gtest.cc",
        self.src .. "gtest-death-test.cc",
        self.src .. "gtest-filepath.cc",
        self.src .. "gtest-matchers.cc",
        self.src .. "gtest-port.cc",
        self.src .. "gtest-printers.cc",
        self.src .. "gtest-test-part.cc",
        self.src .. "gtest-typed-test.cc",
    }
