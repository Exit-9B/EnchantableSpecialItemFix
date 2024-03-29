cmake_minimum_required(VERSION 3.22)

project(
	EnchantableSpecialItemFix
	VERSION 1.0.2
	LANGUAGES CXX
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(
		FATAL_ERROR
			"In-source builds are not allowed."
	)
endif()

get_filename_component(
	Skyrim64Path
	"[HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Bethesda Softworks\\Skyrim Special Edition;installed path]"
	ABSOLUTE CACHE
)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${Skyrim64Path}/Data" CACHE PATH
		"Install path prefix (e.g. Skyrim Data directory or Mod Organizer virtual directory)."
		FORCE
	)
endif()

if(CMAKE_GENERATOR MATCHES "Visual Studio")
	option(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD "Include INSTALL target to default build." OFF)
endif()

option(BUILD_SKYRIMVR "Build the mod for Skyrim VR" OFF)

if (BUILD_SKYRIMVR)
	add_compile_definitions(SKYRIMVR)
endif()

list(APPEND CMAKE_IGNORE_PATH "${PROJECT_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}/cmake"
	"${PROJECT_SOURCE_DIR}/tools/BethesdaCMakeModules/Modules"
)

include(AddCXXFiles)
include(Papyrus)

add_library("${PROJECT_NAME}" SHARED)

target_compile_features(
	"${PROJECT_NAME}"
	PRIVATE
		cxx_std_20
)

add_cxx_files("${PROJECT_NAME}")

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/src/Plugin.h.in
	${CMAKE_CURRENT_BINARY_DIR}/src/Plugin.h
	@ONLY
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in
	${CMAKE_CURRENT_BINARY_DIR}/version.rc
	@ONLY
)

target_sources(
	"${PROJECT_NAME}"
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/src/Plugin.h
		${CMAKE_CURRENT_BINARY_DIR}/version.rc
		.clang-format
		.editorconfig
		README.md
		vcpkg.json
)

target_precompile_headers(
	${PROJECT_NAME}
	PRIVATE
		src/PCH/PCH.h
)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/src
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(
		${PROJECT_NAME}
		PRIVATE
			"/sdl"             # Enable Additional Security Checks
			"/utf-8"           # Set Source and Executable character sets to UTF-8
			"/Zi"              # Debug Information Format

			"/permissive-"     # Standards conformance
			"/Zc:preprocessor" # Enable preprocessor conformance mode

			"/wd4200"          # nonstandard extension used : zero-sized array in struct/union

			"$<$<CONFIG:DEBUG>:>"
			"$<$<CONFIG:RELEASE>:/Zc:inline;/JMC-;/Ob3>"
	)

	target_link_options(
		${PROJECT_NAME}
		PRIVATE
			"$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
			"$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>"
	)
endif()

add_subdirectory("external/CommonLibSSE" CommonLibSSE EXCLUDE_FROM_ALL)

find_package(spdlog CONFIG REQUIRED)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
		CommonLibSSE::CommonLibSSE
		spdlog::spdlog
)

file(GLOB Papyrus_SOURCES
	LIST_DIRECTORIES false
	CONFIGURE_DEPENDS
	"scripts/*.psc"
)

add_papyrus(
	"Papyrus"
	GAME ${Skyrim64Path}
	IMPORTS ${CMAKE_CURRENT_SOURCE_DIR}/scripts
	SOURCES ${Papyrus_SOURCES}
	OPTIMIZE
	ANONYMIZE
)

source_group("Scripts" FILES ${Papyrus_SOURCES})

install(
	FILES
		"$<TARGET_FILE:${PROJECT_NAME}>"
		"$<TARGET_PDB_FILE:${PROJECT_NAME}>"
	DESTINATION "SKSE/Plugins"
	COMPONENT SKSEPlugin
)

install(
	FILES ${Papyrus_SOURCES}
	DESTINATION "Source/Scripts"
	COMPONENT Data
)

install(
	FILES ${Papyrus_OUTPUT}
	DESTINATION "Scripts"
	COMPONENT Data
)

install(
	FILES
		${CMAKE_CURRENT_SOURCE_DIR}/data/Patches/EnchantableSpecialItemFix_Gaunt.esp
		${CMAKE_CURRENT_SOURCE_DIR}/data/Patches/EnchantableSpecialItemFix_USSEP.esp
	DESTINATION "."
	COMPONENT Patches
)

install(
	FILES
		${CMAKE_CURRENT_SOURCE_DIR}/data/fomod/info.xml
		${CMAKE_CURRENT_SOURCE_DIR}/data/fomod/ModuleConfig.xml
	DESTINATION "fomod"
	COMPONENT Fomod
	EXCLUDE_FROM_ALL
)
