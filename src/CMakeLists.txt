cmake_minimum_required(VERSION 3.0)

PROJECT(task_worker)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

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
	message_queue/message_queue.h
	message_queue/message_queue.cpp
	message_queue/message_queue_ui.h
	message_queue/message_queue_ui.cpp
	win/win_utils.h
	win/win_utils.cpp
	looper.h
	looper.cpp
	pending_task.h
	pending_task.cpp
	waitable_event.h
	waitable_event.cpp
	tracker.h
	tracker.cpp
	task_runner.h
	task_runner.cpp
	worker_thread.h
	worker_thread.cpp
	)

add_library(base STATIC  ${SRC_LIST})
assign_source_group(${SRC_LIST})
target_include_directories(base PUBLIC 
	".")



# base_unittest
set(SRC_UNITTEST
	callback/bind_unittest.cpp
	test_suite.cpp
	#worker_thread_unittest.cpp
	#looper_test.cpp
	#message_queue_ui_test.cpp
)

add_definitions(-DUNICODE -D_UNICODE)
add_executable(base_unittest ${SRC_UNITTEST})
add_dependencies(base_unittest 
	base
	)

target_link_libraries(base_unittest PUBLIC
	debug "${CMAKE_SOURCE_DIR}/libraries/debug/gtest.lib"
	optimized "${CMAKE_SOURCE_DIR}/libraries/release/gtest.lib"
	base
	)
	
target_include_directories(base_unittest PUBLIC 
"${CMAKE_SOURCE_DIR}/3rdparty/gtest-1.8.0/include"
".")

assign_source_group(${SRC_UNITTEST})


