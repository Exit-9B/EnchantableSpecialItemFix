cmake_minimum_required(VERSION 3.20)

project(
	EnchantableSpecialItemFix
	VERSION 1.0.0
	LANGUAGES CXX
)

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
		FORCE)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	option(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD "Include INSTALL target to default build." OFF)
endif()

list(APPEND CMAKE_IGNORE_PATH "${PROJECT_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

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
		"${PROJECT_NAME}"
		PRIVATE
			"/sdl"	# Enable Additional Security Checks
			"/utf-8"	# Set Source and Executable character sets to UTF-8
			"/Zi"	# Debug Information Format

			"/permissive-"	# Standards conformance
			"/Zc:preprocessor"	# Enable preprocessor conformance mode

			"/wd4200" # nonstandard extension used : zero-sized array in struct/union

			"$<$<CONFIG:DEBUG>:>"
			"$<$<CONFIG:RELEASE>:/Zc:inline;/JMC-;/Ob3>"
	)

	target_link_options(
		"${PROJECT_NAME}"
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

file(GLOB PAPYRUS_SOURCES
	LIST_DIRECTORIES false
	CONFIGURE_DEPENDS
	"scripts/*.psc"
)

add_papyrus(
	"Papyrus"
	GAME ${Skyrim64Path}
	IMPORTS ${CMAKE_CURRENT_SOURCE_DIR}/scripts
	SOURCES ${PAPYRUS_SOURCES}
	OPTIMIZE ANONYMIZE
)

source_group("Scripts" FILES ${PAPYRUS_SOURCES})

install(
	FILES "$<TARGET_FILE:${PROJECT_NAME}>"
	DESTINATION "SKSE/Plugins"
)

install(
	FILES ${PAPYRUS_SOURCES}
	DESTINATION "Source/Scripts"
)

install(
	FILES ${PAPYRUS_OUTPUT}
	DESTINATION "Scripts"
)