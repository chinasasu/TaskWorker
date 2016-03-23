cmake_minimum_required(VERSION 3.0)

PROJECT(task_worker)

function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

include_directories(base)
# base
set(SRC_LIST 
	callback/bind.h
	callback/bind_callback.h
	callback/bind_callback.cpp
	callback/bind_storage.h
	callback/bind_helpers.h
	base/notification/observer_list.h
	base/notification/notification_types.h
	base/notification/notification_source.h
	base/notification/notification_service_impl.h
	base/notification/notification_service_impl.cpp
	base/notification/notification_service.h
	base/notification/notification_registrar.h
	base/notification/notification_registrar.cpp
	base/notification/notification_observer.h
	base/notification/notification_details.h
	)

add_library(base STATIC  ${SRC_LIST})
assign_source_group(${SRC_LIST})
target_include_directories(base PUBLIC 
	".")



# base_unittest
set(SRC_UNITTEST
	callback/bind_unittest.cpp
	test_suite.cpp
)

add_executable(base_unittest ${SRC_UNITTEST})
add_dependencies(base_unittest 
	base
	)

target_link_libraries(base_unittest PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/gtest.lib"
	base
	)
	
target_include_directories(base_unittest PUBLIC 
"3rdparty/gtest-1.7.0/include"
".")

assign_source_group(${SRC_UNITTEST})

