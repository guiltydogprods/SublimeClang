-- premake5.lua

workspace "SublimeClang"
   configurations { "Debug", "Release" }
   platforms { "x64" }   
   startproject "SublimeClang"
   includedirs { "." }
   defines { "_CRT_SECURE_NO_WARNINGS" }
   
filter "platforms:x64"
	includedirs { "external/glfw-3.3/include", "external/glew-2.1.0/include", "external/glm-0.9.9.5" }
	libdirs { "external/glfw-3.3/lib-vc2017" }	-- there isn't a vs2017 build of glfw yet, but the 2015 one seems to work
   toolset "clang"
   libdirs { "external/glew-2.1.0/lib/Release/x64" }  
   links { "glfw3dll", "opengl32", "glew32" }

   architecture "x86_64"  
   vectorextensions "SSE4.1"

project "SublimeClang"
   location "."
   kind "ConsoleApp"
   language "C"
   buildoptions { "-std=c11 -gno-column-info -ffast-math -Xclang -flto-visibility-public-std -march=native" }
   linkoptions { "-g -gno-column-info" } 
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
