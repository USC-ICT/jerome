cmake_minimum_required(VERSION 3.6)
project(jerome)

# customize
# includes for 3rd party dependecies (boost, eigen, nlopt)
set(jerome_includes "${PROJECT_SOURCE_DIR}/../build/osx/include")

# custom tweaks for eigen library
set(EIGEN_SPARSEVECTOR_PLUGIN "${PROJECT_SOURCE_DIR}/jerome/type/matrix_eigen_sparse_vector_plugin.hpp")

# regular variables

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/")
set(jerome_GENERATED_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/jerome.generated")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DEIGEN_SPARSEVECTOR_PLUGIN=\\\"${EIGEN_SPARSEVECTOR_PLUGIN}\\\"")

# -------------------------------------------------
# kstem

set(kstem_dir "${PROJECT_SOURCE_DIR}/3rdParty/kstem/source")
file(GLOB_RECURSE kstem_source_files "${kstem_dir}/*.cpp" "${kstem_dir}/hash.c")
add_library(kstem SHARED ${kstem_source_files})
target_include_directories(kstem
        PUBLIC "${kstem_dir}"
        PUBLIC "${kstem_dir}/data"
        )

# -------------------------------------------------
# jerome library

add_custom_command(
        COMMENT "generating includes"
        OUTPUT
            "${jerome_GENERATED_OUTPUT_DIR}/startup.js.inc"
            "${jerome_GENERATED_OUTPUT_DIR}/utteranceCompiler.js.inc"
            "${jerome_GENERATED_OUTPUT_DIR}/scion.js.inc"
            "${jerome_GENERATED_OUTPUT_DIR}/HTMLReporter.xslt.inc"
        COMMAND bash "${PROJECT_SOURCE_DIR}/scripts/make_includes.sh" "${PROJECT_SOURCE_DIR}" "${jerome_GENERATED_OUTPUT_DIR}"
        DEPENDS
            "${PROJECT_SOURCE_DIR}/jerome/scripting/startup.js"
            "${PROJECT_SOURCE_DIR}/jerome/scripting/utteranceCompiler.js"
            "${PROJECT_SOURCE_DIR}/3rdParty/SCION/dist/scion.js"
            "${PROJECT_SOURCE_DIR}/jerome/ir/report/HTMLReporter.xslt"
)

find_package(LibXml2)
find_package(LibXslt)

set(jerome_libraries
        ${LIBXML2_LIBRARIES}
        ${LIBXSLT_LIBRARIES}
        kstem
        )

# find boost 
find_library(boost_lib boost PATHS "${PROJECT_SOURCE_DIR}/../build/osx/Frameworks")
if (NOT boost_lib)

	find_library(boost_lib boost_thread)
		
	if (boost_lib)

		message( "boost_thread found")
		find_library(boost1_lib boost_system)
		if (boost1_lib)
			message( "boost_system found")
			set(boost_lib ${boost_lib} ${boost1_lib})
		endif (boost1_lib)

		find_library(boost1_lib boost_date_time)
		if (boost1_lib)
			message( "boost_date_time found")
			set(boost_lib ${boost_lib} ${boost1_lib})
		endif (boost1_lib)

		find_library(boost1_lib boost_signals)
		if (boost1_lib)
			set(boost_lib ${boost_lib} ${boost1_lib})
		endif (boost1_lib)

		find_library(boost1_lib boost_filesystem)
		if (boost1_lib)
			set(boost_lib ${boost_lib} ${boost1_lib})
		endif (boost1_lib)

		find_library(boost1_lib boost_program_options)
		if (boost1_lib)
			set(boost_lib ${boost_lib} ${boost1_lib})
		endif (boost1_lib)

		find_library(boost1_lib boost_locale)
		if (boost1_lib)
			set(boost_lib ${boost_lib} ${boost1_lib})
		endif (boost1_lib)

	endif (boost_lib)

endif (NOT boost_lib)

if (boost_lib)
	set(jerome_libraries ${jerome_libraries} ${boost_lib})
else (boost_lib)
	message("Cannot find boost")
endif (boost_lib)

# find JavaScriptCore

find_library(JavaScriptCore_lib JavaScriptCore)
if (JavaScriptCore_lib)
	set(jerome_libraries ${jerome_libraries} ${JavaScriptCore_lib})
else (JavaScriptCore_lib)
	message("Cannot find JavaScriptCore")
endif (JavaScriptCore_lib)

# find nlopt

find_library(nlopt_lib nlopt_cxx "${PROJECT_SOURCE_DIR}/../build/osx/lib" NO_DEFAULT_PATH)

if (nlopt_lib)
	set(jerome_libraries ${jerome_libraries} ${nlopt_lib})
else (nlopt_lib)
	message("Cannot find nlopt")
endif (nlopt_lib)

# find CoreFoundation on Mac/iOS

if (APPLE)

	find_library(CoreFoundation_lib CoreFoundation)

	if (CoreFoundation_lib)
		set(jerome_libraries ${jerome_libraries} ${CoreFoundation_lib})
	else (CoreFoundation_lib)
		message("Cannot find CoreFoundation")
	endif (CoreFoundation_lib)

endif (APPLE)

message( "hello ${OBJECT_DIR}")
message(${boost_lib})
message(${JavaScriptCore_lib})
message(${nlopt_lib})
message("hello")


file(GLOB_RECURSE jerome_source_files "${PROJECT_SOURCE_DIR}/jerome/*.cpp")

add_library(jerome SHARED ${jerome_source_files}  "${jerome_GENERATED_OUTPUT_DIR}/startup.js.inc"
        "${jerome_GENERATED_OUTPUT_DIR}/utteranceCompiler.js.inc"
        "${jerome_GENERATED_OUTPUT_DIR}/scion.js.inc"
        "${jerome_GENERATED_OUTPUT_DIR}/HTMLReporter.xslt.inc")
        
target_include_directories(jerome
        PUBLIC ${PROJECT_SOURCE_DIR}
        PRIVATE ${jerome_includes}
        ${jerome_GENERATED_OUTPUT_DIR}
        ${LIBXML2_INCLUDE_DIR}
        ${LIBXSLT_INCLUDE_DIR}
        "/usr/include/eigen3"
        )

target_link_libraries(jerome ${jerome_libraries})

# simple_jerome example

add_executable(simple_jerome
        examples/simple_jerome/main.cpp)

target_link_libraries(simple_jerome LINK_PUBLIC jerome)


