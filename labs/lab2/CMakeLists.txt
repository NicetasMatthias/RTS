cmake_minimum_required(VERSION 3.0)

project(lab2 LANGUAGES C)

set(LAB_NUMBER "2")

file(GLOB_RECURSE SRC_LIST_2
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable(lab2
    ${SRC_LIST_2})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(lab2 PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(lab2 "${CMAKE_THREAD_LIBS_INIT}")
endif()

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
        DESTINATION "${REPORTS_DIR}")
file(RENAME "${REPORTS_DIR}/main.c" "${REPORTS_DIR}/T${LAB_NUMBER}_${NAME}_${GROUP}.c")

