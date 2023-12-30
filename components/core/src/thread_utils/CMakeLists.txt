set(
        THREAD_UTILS_HEADER_LIST
        "${CLP_SOURCE_DIR}/include/thread_utils/Thread.hpp"
        "${CLP_SOURCE_DIR}/src/ErrorCode.hpp"
        "${CLP_SOURCE_DIR}/src/TraceableException.hpp"
)
add_library(
        thread_utils
        Thread.cpp
        ${THREAD_UTILS_HEADER_LIST}
)
add_library(thread_utils::thread_utils ALIAS thread_utils)
target_include_directories(thread_utils PUBLIC ../../include)
target_compile_features(thread_utils PRIVATE cxx_std_17)
target_link_libraries(
        thread_utils
        PRIVATE
        spdlog::spdlog
)
