cmake_minimum_required(VERSION 3.6)
project(jerome)

# customize
set(jerome_3rdPartyRoot "${PROJECT_SOURCE_DIR}/3rdParty/build/macosx")

# includes for 3rd party dependecies (boost, eigen, nlopt)
set(jerome_includes "${PROJECT_SOURCE_DIR}/../build/osx/include")

# custom tweaks for eigen library
set(EIGEN_SPARSEVECTOR_PLUGIN "${PROJECT_SOURCE_DIR}/jerome/type/matrix_eigen_sparse_vector_plugin.hpp")

# regular variables

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/")
set(jerome_GENERATED_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/jerome.generated")

message("compiler: ${CMAKE_CXX_COMPILER_ID}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "[cC][lL][aA][nN][gG]")
# hide inlines == smaller executable and no warnings
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DGCC_SYMBOLS_PRIVATE_EXTERN -DGCC_INLINES_ARE_PRIVATE_EXTERN -fvisibility-inlines-hidden -fPIC -DEIGEN_SPARSEVECTOR_PLUGIN=\\\"${EIGEN_SPARSEVECTOR_PLUGIN}\\\"")
else ()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DGCC_SYMBOLS_PRIVATE_EXTERN -DGCC_INLINES_ARE_PRIVATE_EXTERN -fPIC -DEIGEN_SPARSEVECTOR_PLUGIN=\\\"${EIGEN_SPARSEVECTOR_PLUGIN}\\\"")
endif ()

# -------------------------------------------------
# kstem

set(kstem_dir "${PROJECT_SOURCE_DIR}/3rdParty/kstem/source")
file(GLOB_RECURSE kstem_source_files 
  "${kstem_dir}/*.cpp" 
  "${kstem_dir}/hash.c"
)

# shared, because LGPL
add_library(kstem STATIC ${kstem_source_files})

target_include_directories(kstem
  PUBLIC "${kstem_dir}"
  PUBLIC "${kstem_dir}/data"
  )

# -------------------------------------------------
# jerome library

# generate includes
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

# ----
# find libraries

set(jerome_libraries kstem)

# find libxml
find_package(LibXml2)
set(jerome_libraries ${jerome_libraries} ${LIBXML2_LIBRARIES})

# find libxslt
find_package(LibXslt)
set(jerome_libraries ${jerome_libraries} ${LIBXSLT_LIBRARIES})

# find boost 
find_library(boost_lib boost PATHS "${jerome_3rdPartyRoot}/Frameworks")
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
  message("Found boost: ${boost_lib}")
  set(jerome_libraries ${jerome_libraries} ${boost_lib} boost_system boost_date_time boost_signals boost_filesystem boost_program_options boost_locale javascriptcoregtk-4.0)
else (boost_lib)
	message("Cannot find boost")
endif (boost_lib)

# find JavaScriptCore
#find_library(JavaScriptCore_lib JavaScriptCoregtk-4.0 HINTS 
find_library(JavaScriptCore_lib javascriptrcoregtk-4.0 HINTS 
  "/usr/lib/x86_64-linux-gnu" 
  )
if (JavaScriptCore_lib)
  message("Found JavaScriptCore: ${JavaScriptCore_lib}")
  set(jerome_libraries ${jerome_libraries} ${JavaScriptCore_lib})
else (JavaScriptCore_lib)
	message("Cannot find JavaScriptCore")
endif (JavaScriptCore_lib)

# find nlopt
find_library(nlopt_lib NAMES nlopt nlopt_cxx HINTS 
  "${jerome_3rdPartyRoot}/lib"  
  "/usr/local/lib" 
  "/usr/lib/x86_64-linux-gnu" 
  "/usr/lib"
)

if (nlopt_lib)
  message("Found nlopt: ${nlopt_lib}")
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

# jerome library source files
file(GLOB_RECURSE jerome_source_files 
		"${PROJECT_SOURCE_DIR}/jerome/*.cpp")

add_library(jerome STATIC ${jerome_source_files}  
  "${jerome_GENERATED_OUTPUT_DIR}/startup.js.inc"
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
  "/usr/include/webkitgtk-4.0"

)

target_link_libraries(jerome ${jerome_libraries} )

# -------------------------------------------------
# jerome_tool

set(exec_libs jerome ${boost_lib})

if (NOT APPLE)
	find_library(pthread_lib pthread)
	set(exec_libs ${exec_libs} ${pthread_lib})
endif (NOT APPLE)

# jerome_tool sources
file(GLOB_RECURSE jerome_tool_source_files 
  "${PROJECT_SOURCE_DIR}/jerome_tool/*.cpp")

add_executable(jerome_tool
  ${jerome_tool_source_files})
target_include_directories(jerome_tool PRIVATE 
  ${PROJECT_SOURCE_DIR}
  ${jerome_includes}
  ${jerome_GENERATED_OUTPUT_DIR}
  ${LIBXML2_INCLUDE_DIR}
  ${LIBXSLT_INCLUDE_DIR}
  "/usr/include/eigen3"
)

# rename jerome_too to jerome
set_target_properties(jerome_tool PROPERTIES OUTPUT_NAME "jerome")

target_link_libraries(jerome_tool ${exec_libs})

# -------------------------------------------------
# Jerome4Java

find_package(Java REQUIRED)
find_package(JNI REQUIRED)


# jerome library source files
file(GLOB_RECURSE Jerome4Java_source_files 
		"${PROJECT_SOURCE_DIR}/Jerome4Java/cpp/*.cpp")

add_library(Jerome4Java SHARED ${Jerome4Java_source_files})
        
target_include_directories(Jerome4Java
  PUBLIC ${PROJECT_SOURCE_DIR}
  PRIVATE ${jerome_includes}
  ${jerome_GENERATED_OUTPUT_DIR}
  ${LIBXML2_INCLUDE_DIR}
  ${LIBXSLT_INCLUDE_DIR}
  "/usr/include/eigen3"
  "/usr/include/webkitgtk-4.0"
  ${JNI_INCLUDE_DIRS}
)

set(Jerome4Java_libraries ${jerome_libraries} jerome ${JNI_LIBRARIES})

target_link_libraries(Jerome4Java ${Jerome4Java_libraries} )

# -------------------------------------------------
# Jerome4Java

find_package(Java REQUIRED)
include(UseJava)

set(JAR_NAME J4JTest)

# J4JTest source files
file(GLOB_RECURSE J4JTest_source_files 
		"${PROJECT_SOURCE_DIR}/Jerome4Java/java/*/*.java")

# message("J4j source: ${J4JTest_source_files}")

add_jar(${JAR_NAME} ${J4JTest_source_files} 
  MANIFEST "${PROJECT_SOURCE_DIR}/Jerome4Java/java/app/META-INF/MANIFEST.MF"
  OUTPUT_DIR ${EXECUTABLE_OUTPUT_PATH})