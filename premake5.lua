workspace "AuraEngine"
	architecture "x64"
	configurations {
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Aura/vendor/GLFW/include"
IncludeDir["GLAD"] = "Aura/vendor/GLAD/include"
IncludeDir["ImGui"] = "Aura/vendor/imgui"
IncludeDir["glm"] = "Aura/vendor/glm"
IncludeDir["stb_image"] = "Aura/vendor/stb_image"

include "Aura/vendor/GLFW"
include "Aura/vendor/GLAD"
include "Aura/vendor/imgui"

project "Aura"
	location "Aura"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"


	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "arpch.h"
	pchsource "Aura/src/arpch.cpp"

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.h",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}
	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}

	links {
		"GLFW",
		"ImGui",
		"GLAD",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"
		buildoptions { "/utf-8" }
		defines {
			"AR_PLATFORM_WINDOWS",
			"AR_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}


	filter "configurations:Debug"
		defines "AR_DEBUG"
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines "AR_RELEASE"
		runtime "Release"
		optimize "on"
	filter "configurations:Dist"
		defines "AR_DIST"
		runtime "Release"
		optimize "on"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{
		"Aura/vendor/spdlog/include",
		"Aura/src",
		"Aura/vendor",
		"%{IncludeDir.glm}"
	}
	
	links {
		"Aura"
	}

	filter "system:windows"
		systemversion "latest"
		buildoptions { "/utf-8" }

		defines {
			"AR_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "AR_DEBUG"
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines "AR_RELEASE"
		runtime "Release"
		optimize "on"
	filter "configurations:Dist"
		defines "AR_DIST"
		runtime "Release"
		optimize "on"

		



