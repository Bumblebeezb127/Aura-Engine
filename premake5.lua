workspace "AuraEngine"
	architecture "x64"
	configurations {
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


project "Aura"
	location "Aura"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		-- systemversion "10.0.22621.0"
		systemversion "latest"
		defines {
			"AR_PLATFORM_WINDOWS",
			"AR_BUILD_DLL"
		}
        postbuildcommands {
            -- 使用绝对路径，避免任何相对路径问题
            "{COPY} %{cfg.buildtarget.abspath} ../bin/" .. outputdir .. "/Sandbox/"
        }


	filter "configurations:Debug"
		defines "AR_DEBUG"
		symbols "On"
	filter "configurations:Release"
		defines "AR_RELEASE"
		optimize "On"
	filter "configurations:Dist"
		defines "AR_DIST"
		optimize "On"

	filter {"system:windows","configurations:Release"}
		buildoptions "/utf-8" 
	filter {"system:windows","configurations:Debug"}
		buildoptions "/utf-8"
	filter {"system:windows","configurations:Dist"}
		buildoptions "/utf-8"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{
		"Aura/vendor/spdlog/include",
		"Aura/src"
	}
	
	links {
		"Aura"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		-- systemversion "10.0.22621.0"
		systemversion "latest"
		defines {
			"AR_PLATFORM_WINDOWS"
		}
	filter "configurations:Debug"
		defines "AR_DEBUG"
		symbols "On"
	filter "configurations:Release"
		defines "AR_RELEASE"
		optimize "On"
	filter "configurations:Dist"
		defines "AR_DIST"
		optimize "On"

	filter {"system:windows","configurations:Release"}
		buildoptions "/utf-8" 
	filter {"system:windows","configurations:Debug"}
		buildoptions "/utf-8"
	filter {"system:windows","configurations:Dist"}
		buildoptions "/utf-8"
		



