// C libraries
#include <sys/stat.h>

// C++ libraries
#include <iostream>

// Boost libraries
#include <boost/filesystem.hpp>

// spdlog
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

// Project headers
#include "../../Defs.h"
#include "../../LogTypeDictionaryEntry.hpp"
#include "../../MessageParser.hpp"
#include "../../Profiler.hpp"
#include "../../streaming_archive/Constants.hpp"
#include "../../Utils.hpp"
#include "CommandLineArguments.hpp"

using utils::get_float_vars::CommandLineArguments;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::to_string;
using std::vector;

static bool get_all_file_paths (const string& input_path, vector<string>& file_paths) {
    try {
        if (false == boost::filesystem::is_directory(input_path)) {
            // path is a file
            file_paths.emplace_back(input_path);
            return true;
        }

        if (boost::filesystem::is_empty(input_path)) {
            // path is an empty directory
            return false;
        }

        // Iterate directory
        boost::filesystem::recursive_directory_iterator iter(input_path, boost::filesystem::symlink_option::recurse);
        boost::filesystem::recursive_directory_iterator end;
        for (; iter != end; ++iter) {
            // Check if current entry is an empty directory or a file
            if (boost::filesystem::is_directory(iter->path())) {
                if (boost::filesystem::is_empty(iter->path())) {
                    iter.no_push();
                }
            } else {
                file_paths.emplace_back(iter->path().string());
            }
        }
    } catch (boost::filesystem::filesystem_error& exception) {
        SPDLOG_ERROR("Failed to find files/directories at '{}' - {}.", input_path.c_str(), exception.what());
        return false;
    }

    return true;
}

static bool is_double (const string& var) {
    // Ensure string is not an integer
    int64_t var_as_integer;
    if (convert_string_to_int64(var, 0, var_as_integer)) {
        return false;
    }

    double var_as_double;
    return convert_string_to_double(var, var_as_double);
}

int main (int argc, const char* argv[]) {
    // Program-wide initialization
    try {
        auto stderr_logger = spdlog::stderr_logger_st("stderr");
        spdlog::set_default_logger(stderr_logger);
        spdlog::set_pattern("%Y-%m-%d %H:%M:%S,%e [%l] %v");
    } catch (std::exception& e) {
        // NOTE: We can't log an exception if the logger couldn't be constructed
        return -1;
    }
    PROFILER_INITIALIZE()
    TimestampPattern::init();

    CommandLineArguments command_line_args("get-float-vars");
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

    // Get paths of all files
    vector<string> file_paths;
    if (false == get_all_file_paths(command_line_args.get_input_path(), file_paths)) {
        return -1;
    }

    FileReader file_reader;
    MessageParser message_parser;
    ParsedMessage parsed_message;
    LogTypeDictionaryEntry logtype_dictionary_entry;
    string var_str;

    for (const auto& file_path : file_paths) {
        file_reader.open(file_path);
        parsed_message.clear();
        while (message_parser.parse_next_message(true, file_reader, parsed_message)) {
            const auto& message = parsed_message.get_content();
            size_t var_begin_pos = 0;
            size_t var_end_pos = 0;
            logtype_dictionary_entry.clear();
            while (logtype_dictionary_entry.parse_next_var(message, var_begin_pos, var_end_pos, var_str)) {
                if (is_double(var_str)) {
                    cout << var_str << endl;
                }
            }
        }
        file_reader.close();
    }

    return 0;
}
