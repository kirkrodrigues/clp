#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <spdlog/sinks/stdout_sinks.h>

#include "../FileWriter.hpp"
#include "../ir/types.hpp"
#include "../spdlog_with_specializations.hpp"
#include "CommandLineArguments.hpp"
#include "ffi/ir_stream/decoding_methods.hpp"
#include "MessageParser.hpp"

using clp::CommandLineArgumentsBase;
using clp::FileWriter;
using clp::ir::VariablePlaceholder;
using clp::segment_id_t;
using std::string;
using std::vector;

std::string convert_to_json_str(std::string const& unescaped) {
    std::string escaped;
    escaped.reserve(unescaped.length());

    for (auto c : unescaped) {
        switch (c) {
            case '"':
                escaped += "\\\"";
                break;
            case '\\':
                escaped += "\\\\";
                break;
            case '\b':
                escaped += "\\b";
                break;
            case '\f':
                escaped += "\\f";
                break;
            case '\n':
                escaped += "\\n";
                break;
            case '\r':
                escaped += "\\r";
                break;
            case '\t':
                escaped += "\\t";
                break;
            default:
                if (static_cast<unsigned char>(c) <= 0x1F) {
                    char buffer[7];
                    snprintf(buffer, sizeof(buffer), "\\u%04x", c);
                    escaped += buffer;
                } else {
                    escaped += c;
                }
                break;
        }
    }
    return escaped;
}

string escape_braces(std::string_view unescaped) {
    string escaped;
    escaped.reserve(unescaped.length());

    for (auto c : unescaped) {
        if (c == '{' || c == '}') {
            escaped += '\\';
        }
        escaped += c;
    }
    return escaped;
}

int main(int argc, char const* argv[]) {
    // Program-wide initialization
    try {
        auto stderr_logger = spdlog::stderr_logger_st("stderr");
        spdlog::set_default_logger(stderr_logger);
        spdlog::set_pattern("%Y-%m-%d %H:%M:%S,%e [%l] %v");
    } catch (std::exception& e) {
        // NOTE: We can't log an exception if the logger couldn't be constructed
        return -1;
    }
    clp::TimestampPattern::init();

    clp::make_dictionaries_readable::CommandLineArguments command_line_args("log-parser");
    auto parsing_result = command_line_args.parse_arguments(argc, argv);
    switch (parsing_result) {
        case CommandLineArgumentsBase::ParsingResult::Failure:
            return -1;
        case CommandLineArgumentsBase::ParsingResult::InfoCommand:
            return 0;
        case CommandLineArgumentsBase::ParsingResult::Success:
            // Continue processing
            break;
    }

    clp::FileReader file_reader{command_line_args.get_log_file_path()};
    clp::MessageParser msg_parser;
    clp::ParsedMessage parsed_msg;

    while (msg_parser.parse_next_message(true, file_reader, parsed_msg)) {
        // std::cerr << parsed_msg.get_ts() << ':' << parsed_msg.get_content() <<
        // '\n';

        // Parse the message content
        auto content = parsed_msg.get_content();
        size_t var_begin_pos{0};
        size_t var_end_pos{0};
        size_t constant_begin_pos{0};
        string logtype;
        vector<string> vars;
        logtype.reserve(content.length());
        while (clp::ir::get_bounds_of_next_var(content, var_begin_pos, var_end_pos)) {
            std::string_view constant{
                    &content[constant_begin_pos],
                    var_begin_pos - constant_begin_pos
            };
            constant_begin_pos = var_end_pos;
            logtype += escape_braces(constant);

            logtype += "{}";
            std::string_view var{&content[var_begin_pos], var_end_pos - var_begin_pos};
            vars.push_back(string{var});
        }
        if (constant_begin_pos < content.length()) {
            std::string_view constant{
                    &content[constant_begin_pos],
                    content.length() - constant_begin_pos
            };
            logtype += constant;
        }

        string msg = parsed_msg.get_content();
        auto ts_patt = parsed_msg.get_ts_patt();
        if (nullptr != ts_patt) {
            ts_patt->insert_formatted_timestamp(parsed_msg.get_ts(), msg);
        }
        auto escaped_msg = convert_to_json_str(msg);

        auto escaped_logtype = convert_to_json_str(logtype);
        std::cout << "{"
                  << "\"timestamp\":\"" << parsed_msg.get_ts() << "\","
                  << "\"logtype\":\"" << escaped_logtype << "\","
                  << "\"vars\":[";
        for (size_t i = 0; i < vars.size(); ++i) {
            std::cout << "\"" << vars[i] << "\"";
            if (i < vars.size() - 1) {
                std::cout << ",";
            }
        }
        std::cout << "],";
        std::cout << "\"original_msg\":\"" << escaped_msg << "\"";
        std::cout << "}\n";
    }
    return 0;
}
