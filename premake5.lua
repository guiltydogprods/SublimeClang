-- premake5.lua

workspace "SublimeClang"
   configurations { "Debug", "Release" }
   platforms { "x64", "arm64" }   
   startproject "SublimeClang"
   includedirs { "." }
   defines { "_CRT_SECURE_NO_WARNINGS" }
   
filter "action:vs*"
   system "windows"
   architecture "x86_64"  
   vectorextensions "SSE4.1"
--   toolset "msc-clangcl"
   toolset "clang"
   defines("WINDOWS")    
   includedirs { "external/glfw-3.3/include", "external/glew-2.1.0/include", "external/glm-0.9.9.5" }
   libdirs { "external/glfw-3.3/lib-vc2017" }   -- there isn't a vs2017 build of glfw yet, but the 2015 one seems to work
   libdirs { "external/glew-2.1.0/lib/Release/x64" }  
   links { "glfw3dll", "opengl32", "glew32" }
   architecture "x86_64"  
   vectorextensions "SSE4.1"
   defines { "X64" }

-- editandcontinue "Off"    
-- includedirs { "external/glfw-3.3/include", "external/glew-2.1.0/include", "external/fmod/api/core/inc", "external/stb" }

filter "action:xcode*"
   system "macosx"
   architecture "ARM64"
   vectorextensions "NEON"
   toolset "clang"
   defines { "MACOS" }    
   buildoptions { "-Xclang -flto-visibility-public-std -fblocks" }
   sysincludedirs { "external/glfw-3.3/include", "external/glew-2.1.0/include", "external/glm-0.9.9.5", "external" }
   libdirs { "external/glfw-3.3/lib-macos" }   -- there isn't a vs2017 build of glfw yet, but the 2015 one seems to work
--   toolset "clang"
--   libdirs { "external/glew-2.1.0/lib/Release/x64" }
   links { "glfw3", "/Library/Frameworks/OpenGL.framework" } --, "opengl32", "glew32" }
   architecture "ARM64"  
   vectorextensions "NEON"
   defines { "ARM64", "RE_PLATFORM_MACOS" }
   warnings "Extra"

   xcodebuildsettings
   {
      ["GCC_C_LANGUAGE_STANDARD"] = "c11";         
      ["CLANG_CXX_LANGUAGE_STANDARD"] = "c++11";
      ["CLANG_CXX_LIBRARY"]  = "libc++";
      ["SDKROOT"] = "macosx";
      ["CLANG_ENABLE_OBJC_WEAK"] = "YES";
      ["CODE_SIGN_IDENTITY"] = "-";                      --iphoneos";  
      ["VALIDATE_WORKSPACE_SKIPPED_SDK_FRAMEWORKS"] = "OpenGL";
      ["CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING"] = "YES";
      ["CLANG_WARN_BOOL_CONVERSION"] = "YES";
      ["CLANG_WARN_COMMA"] = "YES";
      ["CLANG_WARN_CONSTANT_CONVERSION"] = "YES";
      ["CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS"] = "YES";
      ["CLANG_WARN_EMPTY_BODY"] = "YES";
      ["CLANG_WARN_ENUM_CONVERSION"] = "YES";
      ["CLANG_WARN_INFINITE_RECURSION"] = "YES";
      ["CLANG_WARN_INT_CONVERSION"] = "YES";
      ["CLANG_WARN_NON_LITERAL_NULL_CONVERSION"] = "YES";
      ["CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF"] = "YES";
      ["CLANG_WARN_OBJC_LITERAL_CONVERSION"] = "YES";
      ["CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER"] = "YES";
      ["CLANG_WARN_RANGE_LOOP_ANALYSIS"] = "YES";
      ["CLANG_WARN_STRICT_PROTOTYPES"] = "YES";
      ["CLANG_WARN_SUSPICIOUS_MOVE"] = "YES";
      ["CLANG_WARN_UNREACHABLE_CODE"] = "YES";
      ["CLANG_WARN__DUPLICATE_METHOD_MATCH"] = "YES";
      ["GCC_WARN_64_TO_32_BIT_CONVERSION"] = "YES";
      ["GCC_WARN_UNDECLARED_SELECTOR"] = "YES";
      ["GCC_WARN_UNINITIALIZED_AUTOS"] = "YES";
      ["GCC_WARN_UNUSED_FUNCTION"] = "YES";
      ["GCC_NO_COMMON_BLOCKS"] = "YES";
      ["ENABLE_STRICT_OBJC_MSGSEND"] = "YES";
   }   


project "SublimeClang"
   location "."
   kind "ConsoleApp"
   language "C"
   buildoptions { "-std=c11 -gno-column-info -ffast-math -Xclang -flto-visibility-public-std" }
   linkoptions { "-g -gno-column-info -framework OpenGL -framework Cocoa -framework IOKit -framework CoreFoundation" } 
   targetdir "bin/%{cfg.buildcfg}"
--   targetdir "."
 --  symbolspath "."
--   symbolspath 'bin/%{cfg.buildcfg}.pdb'   
--   targetdir "."
   debugdir "../../"
--   dependson { "ion" }
--   pchheader "stdafx.h"
--   pchsource "source/stdafx.cpp"

   files 
   { 
      "source/**.c", 
      "source/**.h", 
   }

   excludes
   {
   }

   filter "configurations:Debug"
      defines { "_DEBUG" }
      optimize "Off"
      symbols  "Full"      
      libdirs { "lib/Debug" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "Full"
      libdirs { "lib/Release" }
