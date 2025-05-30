add_subdirectory(indexer)
add_subdirectory(search/ast)
add_subdirectory(search/kql)
add_subdirectory(search/sql)

set(
        CLP_SOURCES
        ../clp/aws/AwsAuthenticationSigner.cpp
        ../clp/aws/AwsAuthenticationSigner.hpp
        ../clp/BoundedReader.cpp
        ../clp/BoundedReader.hpp
        ../clp/CurlDownloadHandler.cpp
        ../clp/CurlDownloadHandler.hpp
        ../clp/CurlEasyHandle.hpp
        ../clp/CurlGlobalInstance.cpp
        ../clp/CurlGlobalInstance.hpp
        ../clp/CurlOperationFailed.hpp
        ../clp/CurlStringList.hpp
        ../clp/cli_utils.cpp
        ../clp/cli_utils.hpp
        ../clp/Defs.h
        ../clp/ErrorCode.hpp
        ../clp/ffi/ir_stream/decoding_methods.cpp
        ../clp/ffi/ir_stream/decoding_methods.hpp
        ../clp/ffi/ir_stream/Deserializer.hpp
        ../clp/ffi/ir_stream/encoding_methods.cpp
        ../clp/ffi/ir_stream/encoding_methods.hpp
        ../clp/ffi/ir_stream/ir_unit_deserialization_methods.cpp
        ../clp/ffi/ir_stream/ir_unit_deserialization_methods.hpp
        ../clp/ffi/ir_stream/Serializer.cpp
        ../clp/ffi/ir_stream/Serializer.hpp
        ../clp/ffi/ir_stream/search/AstEvaluationResult.hpp
        ../clp/ffi/ir_stream/search/ErrorCode.cpp
        ../clp/ffi/ir_stream/search/ErrorCode.hpp
        ../clp/ffi/ir_stream/search/NewProjectedSchemaTreeNodeCallbackReq.hpp
        ../clp/ffi/ir_stream/search/QueryHandler.hpp
        ../clp/ffi/ir_stream/search/QueryHandlerImpl.cpp
        ../clp/ffi/ir_stream/search/QueryHandlerImpl.hpp
        ../clp/ffi/ir_stream/search/QueryHandlerReq.hpp
        ../clp/ffi/ir_stream/search/utils.cpp
        ../clp/ffi/ir_stream/search/utils.hpp
        ../clp/ffi/ir_stream/utils.cpp
        ../clp/ffi/ir_stream/utils.hpp
        ../clp/ffi/KeyValuePairLogEvent.cpp
        ../clp/ffi/KeyValuePairLogEvent.hpp
        ../clp/ffi/SchemaTree.cpp
        ../clp/ffi/SchemaTree.hpp
        ../clp/ffi/Value.hpp
        ../clp/FileDescriptor.cpp
        ../clp/FileDescriptor.hpp
        ../clp/FileReader.cpp
        ../clp/FileReader.hpp
        ../clp/hash_utils.cpp
        ../clp/hash_utils.hpp
        ../clp/ir/constants.hpp
        ../clp/ir/EncodedTextAst.cpp
        ../clp/ir/EncodedTextAst.hpp
        ../clp/ir/parsing.cpp
        ../clp/ir/parsing.hpp
        ../clp/NetworkReader.cpp
        ../clp/NetworkReader.hpp
        ../clp/networking/socket_utils.cpp
        ../clp/networking/socket_utils.hpp
        ../clp/ReaderInterface.cpp
        ../clp/ReaderInterface.hpp
        ../clp/ReadOnlyMemoryMappedFile.cpp
        ../clp/ReadOnlyMemoryMappedFile.hpp
        ../clp/spdlog_with_specializations.hpp
        ../clp/streaming_archive/ArchiveMetadata.cpp
        ../clp/streaming_archive/ArchiveMetadata.hpp
        ../clp/streaming_archive/Constants.hpp
        ../clp/streaming_compression/zstd/Decompressor.cpp
        ../clp/streaming_compression/zstd/Decompressor.hpp
        ../clp/Thread.cpp
        ../clp/Thread.hpp
        ../clp/TraceableException.hpp
        ../clp/time_types.hpp
        ../clp/type_utils.hpp
        ../clp/utf8_utils.cpp
        ../clp/utf8_utils.hpp
        ../clp/WriterInterface.cpp
        ../clp/WriterInterface.hpp
)

set(
        CLP_S_SOURCES
        archive_constants.hpp
        ArchiveReader.cpp
        ArchiveReader.hpp
        ArchiveReaderAdaptor.cpp
        ArchiveReaderAdaptor.hpp
        ArchiveWriter.cpp
        ArchiveWriter.hpp
        BufferViewReader.hpp
        ColumnReader.cpp
        ColumnReader.hpp
        ColumnWriter.cpp
        ColumnWriter.hpp
        CommandLineArguments.cpp
        CommandLineArguments.hpp
        Compressor.hpp
        Decompressor.hpp
        Defs.hpp
        DictionaryEntry.cpp
        DictionaryEntry.hpp
        DictionaryReader.hpp
        DictionaryWriter.cpp
        DictionaryWriter.hpp
        ErrorCode.hpp
        FileReader.cpp
        FileReader.hpp
        FileWriter.cpp
        FileWriter.hpp
        InputConfig.cpp
        InputConfig.hpp
        JsonConstructor.cpp
        JsonConstructor.hpp
        JsonFileIterator.cpp
        JsonFileIterator.hpp
        JsonParser.cpp
        JsonParser.hpp
        JsonSerializer.hpp
        kv_ir_search.cpp
        kv_ir_search.hpp
        PackedStreamReader.cpp
        PackedStreamReader.hpp
        ParsedMessage.hpp
        RangeIndexWriter.cpp
        RangeIndexWriter.hpp
        ReaderUtils.cpp
        ReaderUtils.hpp
        Schema.cpp
        Schema.hpp
        SchemaMap.cpp
        SchemaMap.hpp
        SchemaReader.cpp
        SchemaReader.hpp
        SchemaTree.cpp
        SchemaTree.hpp
        SchemaWriter.cpp
        SchemaWriter.hpp
        TimestampDictionaryReader.cpp
        TimestampDictionaryReader.hpp
        TimestampDictionaryWriter.cpp
        TimestampDictionaryWriter.hpp
        TimestampEntry.cpp
        TimestampEntry.hpp
        TraceableException.hpp
        Utils.cpp
        Utils.hpp
        VariableDecoder.cpp
        VariableDecoder.hpp
        VariableEncoder.cpp
        VariableEncoder.hpp
        ZstdCompressor.cpp
        ZstdCompressor.hpp
        ZstdDecompressor.cpp
        ZstdDecompressor.hpp
)

set(
        CLP_S_SEARCH_SOURCES
        search/AddTimestampConditions.cpp
        search/AddTimestampConditions.hpp
        search/clp_search/EncodedVariableInterpreter.cpp
        search/clp_search/EncodedVariableInterpreter.hpp
        search/clp_search/Grep.cpp
        search/clp_search/Grep.hpp
        search/clp_search/Query.cpp
        search/clp_search/Query.hpp
        search/EvaluateTimestampIndex.cpp
        search/EvaluateTimestampIndex.hpp
        search/Output.cpp
        search/Output.hpp
        search/OutputHandler.cpp
        search/OutputHandler.hpp
        search/Projection.cpp
        search/Projection.hpp
        search/QueryRunner.cpp
        search/QueryRunner.hpp
        search/SchemaMatch.cpp
        search/SchemaMatch.hpp
)

set(
        REDUCER_SOURCES
        ../reducer/BufferedSocketWriter.cpp
        ../reducer/BufferedSocketWriter.hpp
        ../reducer/ConstRecordIterator.hpp
        ../reducer/CountOperator.cpp
        ../reducer/CountOperator.hpp
        ../reducer/DeserializedRecordGroup.cpp
        ../reducer/DeserializedRecordGroup.hpp
        ../reducer/GroupTags.hpp
        ../reducer/network_utils.cpp
        ../reducer/network_utils.hpp
        ../reducer/Operator.cpp
        ../reducer/Operator.hpp
        ../reducer/Pipeline.cpp
        ../reducer/Pipeline.hpp
        ../reducer/Record.hpp
        ../reducer/RecordGroup.hpp
        ../reducer/RecordGroupIterator.hpp
        ../reducer/RecordTypedKeyIterator.hpp
        ../reducer/types.hpp
)

set(
        CLP_S_TIMESTAMP_PATTERN_SOURCES
        Defs.hpp
        ErrorCode.hpp
        TimestampPattern.cpp
        TimestampPattern.hpp
        TraceableException.hpp
)

add_library(
        clp_s_TimestampPattern
        ${CLP_S_TIMESTAMP_PATTERN_SOURCES}
)
add_library(clp_s::TimestampPattern ALIAS clp_s_TimestampPattern)
target_compile_features(clp_s_TimestampPattern PRIVATE cxx_std_20)
target_link_libraries(
        clp_s_TimestampPattern
        PRIVATE
        clp::string_utils
        date::date
        spdlog::spdlog
)

add_executable(
        clp-s
        clp-s.cpp
        ${CLP_SOURCES}
        ${CLP_S_SOURCES}
        ${CLP_S_SEARCH_SOURCES}
        ${REDUCER_SOURCES}
)
target_compile_features(clp-s PRIVATE cxx_std_20)
target_link_libraries(
        clp-s
        PRIVATE
        absl::flat_hash_map
        Boost::iostreams Boost::program_options Boost::regex Boost::url
        ${CURL_LIBRARIES}
        clp::string_utils
        clp_s::search::ast
        clp_s::TimestampPattern
        kql
        MariaDBClient::MariaDBClient
        ${MONGOCXX_TARGET}
        msgpack-cxx
        nlohmann_json::nlohmann_json
        OpenSSL::Crypto
        simdjson::simdjson
        spdlog::spdlog
        yaml-cpp
        ystdlib::containers
        ystdlib::error_handling
        ZStd::ZStd
)
target_include_directories(clp-s
        PRIVATE
        "${CLP_OUTCOME_INCLUDE_DIRECTORY}"
)
set_target_properties(
        clp-s
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
