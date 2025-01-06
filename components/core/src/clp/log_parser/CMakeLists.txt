set(
        LOG_PARSER_SOURCES
        ../dictionary_utils.cpp
        ../dictionary_utils.hpp
        ../DictionaryEntry.hpp
        ../DictionaryReader.hpp
        ../FileDescriptor.cpp
        ../FileDescriptor.hpp
        ../FileReader.cpp
        ../FileReader.hpp
        ../FileWriter.cpp
        ../FileWriter.hpp
        ../MessageParser.cpp
        ../ir/parsing.cpp
        ../ir/parsing.hpp
        ../LogTypeDictionaryEntry.cpp
        ../LogTypeDictionaryEntry.hpp
        ../LogTypeDictionaryReader.hpp
        ../ParsedMessage.cpp
        ../ParsedMessage.hpp
        ../ReaderInterface.cpp
        ../ReaderInterface.hpp
        ../ReadOnlyMemoryMappedFile.cpp
        ../ReadOnlyMemoryMappedFile.hpp
        ../spdlog_with_specializations.hpp
        ../streaming_compression/Decompressor.hpp
        ../streaming_compression/passthrough/Decompressor.cpp
        ../streaming_compression/passthrough/Decompressor.hpp
        ../streaming_compression/zstd/Decompressor.cpp
        ../streaming_compression/zstd/Decompressor.hpp
        ../TimestampPattern.cpp
        ../Utils.cpp
        ../Utils.hpp
        ../VariableDictionaryEntry.cpp
        ../VariableDictionaryEntry.hpp
        ../VariableDictionaryReader.hpp
        ../WriterInterface.cpp
        ../WriterInterface.hpp
        "${PROJECT_SOURCE_DIR}/submodules/date/include/date/date.h"
        CommandLineArguments.cpp
        CommandLineArguments.hpp
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
        ZStd::ZStd
)
# Put the built executable at the root of the build directory
set_target_properties(
        log-parser
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
