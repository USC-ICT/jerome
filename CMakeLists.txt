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
add_library(kstem STATIC ${kstem_source_files})
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

	find_library(boost_lib boost_thread HINTS "/usr/local/lib" "/usr/lib")
		
	if (boost_lib)

		macro (find_boost arg)
			find_library(boost_${arg}_lib ${arg} HINTS "/usr/local/lib" "/usr/lib")
			if (boost_${arg}_lib)
				message( "${arg} found")
				set(boost_lib ${boost_lib} ${boost_${arg}_lib})
			else (boost_${arg}_lib)
				message( "${arg} not found")			
			endif (boost_${arg}_lib)
		endmacro()

		message( "boost_thread found")
		
		find_boost(boost_system)
		find_boost(boost_date_time)
		find_boost(boost_signals)
		find_boost(boost_filesystem)
		find_boost(boost_program_options)
		find_boost(boost_locale)
		
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

find_library(nlopt_lib NAMES nlopt nlopt_cxx HINTS "${PROJECT_SOURCE_DIR}/../build/osx/lib"  "/usr/local/lib" "/usr/lib/x86_64-linux-gnu" "/usr/lib")

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

add_library(jerome STATIC ${jerome_source_files}  "${jerome_GENERATED_OUTPUT_DIR}/startup.js.inc"
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

set(exec_libs jerome ${boost_lib})

if (NOT APPLE)
	find_library(pthread_lib pthread)
	set(exec_libs ${exec_libs} ${pthread_lib})
	
endif (NOT APPLE)

add_executable(simple_jerome
        examples/simple_jerome/main.cpp)

target_link_libraries(simple_jerome ${exec_libs})


