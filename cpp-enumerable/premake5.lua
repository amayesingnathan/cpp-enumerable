project "cpp-enumerable"
    language "C++"
    cppdialect "C++20"
		
    targetdir 	("%{wks.location}/bin/%{prj.name}/" .. outputDir)
    objdir 		("%{wks.location}/obj/%{prj.name}/" .. outputDir)
	
	pchheader "pch.h"
	pchsource "src/pch.cpp"

    files 
    { 
        "src/**.h", 
        "src/**.cpp",
    }
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

    includedirs
    {
        "%{IncludeDir.cpp_enumerable}",
    }

	links
	{
	}
	
    filter "system:windows"
        kind "StaticLib"
        staticruntime "off"
        systemversion "latest"
		
	filter "system:linux"
        kind "ConsoleApp"
        staticruntime "off"
        pic "On"
        systemversion "latest"

    filter "configurations:Debug"
		runtime "Debug"
        symbols "on"
    filter "configurations:Release"
		runtime "Release"
        optimize "on"