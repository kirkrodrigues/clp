set(
        LOG_PARSER_SOURCES
        ../FileReader.cpp
        ../MessageParser.cpp
        ../ir/parsing.cpp
        ../ParsedMessage.cpp
        ../ReaderInterface.cpp
        ../TimestampPattern.cpp
        ../Utils.cpp
        "${PROJECT_SOURCE_DIR}/submodules/date/include/date/date.h"
        CommandLineArguments.cpp
        log-parser.cpp
)

add_executable(log-parser ${LOG_PARSER_SOURCES})
target_compile_features(log-parser PRIVATE cxx_std_20)
target_include_directories(log-parser PRIVATE "${PROJECT_SOURCE_DIR}/submodules")
target_link_libraries(log-parser
        PRIVATE
        Boost::filesystem Boost::program_options
        log_surgeon::log_surgeon
        spdlog::spdlog
        clp::string_utils
)

# Put the built executable at the root of the build directory
set_target_properties(
        log-parser
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
