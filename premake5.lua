solution ( "ringbuf-test" )
  configurations { "Release", "Debug" }
  platforms { "x64" }

  if _ACTION == "clean" then
    os.rmdir(".vs")
    os.rmdir("test")
    os.remove("ringbuf-test.VC.db")
    os.remove("ringbuf-test.sln")
    os.remove("ringbuf-test.vcxproj")
    os.remove("ringbuf-test.vcxproj.filters")
    os.remove("ringbuf-test.vcxproj.user")
    os.remove("ringbuf-test.make")
    os.remove("Makefile")
    return
  end

  -- A project defines one build target
  project ( "ringbuf-test" )
  kind ( "ConsoleApp" )
  language ( "C" )
  targetname ("ringbuf-test")
  files { "./*.h", "./*.c" }
  defines { "_UNICODE" }
  staticruntime "On"

  configuration ( "Release" )
    optimize "On"
    objdir ( "./test/tmp" )
    targetdir ( "./test" )
    defines { "NDEBUG", "_NDEBUG" }

  configuration ( "Debug" )
    symbols "On"
    objdir ( "./test/tmp" )
    targetdir ( "./test" )
    defines { "DEBUG", "_DEBUG" }

  configuration ( "vs*" )
    defines { "WIN32", "_WIN32", "_WINDOWS",
              "_CRT_SECURE_NO_WARNINGS", "_CRT_SECURE_NO_DEPRECATE",
              "_CRT_NONSTDC_NO_DEPRECATE", "_WINSOCK_DEPRECATED_NO_WARNINGS" }

  configuration ( "gmake" )
    warnings  "Default" --"Extra"

  configuration { "gmake", "macosx" }
    defines { "__APPLE__", "__MACH__", "__MRC__", "macintosh" }

  configuration { "gmake", "linux" }
    defines { "__linux__" }

  configuration { "gmake", "bsd" }
    defines { "__BSD__" }
