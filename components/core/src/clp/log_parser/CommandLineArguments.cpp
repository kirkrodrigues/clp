#include "CommandLineArguments.hpp"

#include <iostream>

#include <boost/program_options.hpp>

#include "../spdlog_with_specializations.hpp"

namespace po = boost::program_options;
using std::cerr;
using std::endl;
using std::exception;
using std::invalid_argument;
using std::string;

namespace clp::make_dictionaries_readable {
constexpr char cCliArgNameLogFilePath[]{"log-file-path"};
constexpr char cCliArgNameOutputFilePath[]{"output-file-path"};
constexpr std::string_view cCliHumanReadableArgNameLogFilePath{"LOG_FILE_PATH"};
constexpr std::string_view cCliHumanReadableArgNameOutputFilePath{"OUTPUT_FILE_PATH"};

CommandLineArgumentsBase::ParsingResult
CommandLineArguments::parse_arguments(int argc, char const* argv[]) {
    // Print out basic usage if user doesn't specify any options
    if (1 == argc) {
        print_basic_usage();
        return ParsingResult::Failure;
    }

    // Define general options
    po::options_description options_general("General Options");
    options_general.add_options()("help,h", "Print help");

    // Define visible options
    po::options_description visible_options;
    visible_options.add(options_general);

    // Define hidden positional options (not shown in Boost's program options help message)
    po::options_description hidden_positional_options;
    // clang-format off
        hidden_positional_options.add_options()
                (cCliArgNameLogFilePath, po::value<string>(&m_log_file_path))
                (cCliArgNameOutputFilePath, po::value<string>(&m_output_file_path));
    // clang-format on
    po::positional_options_description positional_options_description;
    positional_options_description.add(cCliArgNameLogFilePath, 1);
    positional_options_description.add(cCliArgNameOutputFilePath, 1);

    // Aggregate all options
    po::options_description all_options;
    all_options.add(options_general);
    all_options.add(hidden_positional_options);

    // Parse options
    try {
        // Parse options specified on the command line
        po::parsed_options parsed = po::command_line_parser(argc, argv)
                                            .options(all_options)
                                            .positional(positional_options_description)
                                            .run();
        po::variables_map parsed_command_line_options;
        store(parsed, parsed_command_line_options);

        notify(parsed_command_line_options);

        // Handle --help
        if (parsed_command_line_options.count("help")) {
            if (argc > 2) {
                SPDLOG_WARN("Ignoring all options besides --help.");
            }

            print_basic_usage();

            cerr << visible_options << endl;
            return ParsingResult::InfoCommand;
        }

        // Validate required parameters
        if (m_log_file_path.empty()) {
            throw invalid_argument(
                    string{cCliHumanReadableArgNameLogFilePath} + " not specified or empty."
            );
        }
        if (m_output_file_path.empty()) {
            throw invalid_argument(
                    string{cCliHumanReadableArgNameOutputFilePath} + " not specified or empty."
            );
        }
    } catch (exception& e) {
        SPDLOG_ERROR("{}", e.what());
        print_basic_usage();
        return ParsingResult::Failure;
    }

    return ParsingResult::Success;
}

void CommandLineArguments::print_basic_usage() const {
    cerr << "Usage: " << get_program_name() << " [OPTIONS] " << cCliHumanReadableArgNameLogFilePath
         << ' ' << cCliHumanReadableArgNameOutputFilePath << endl;
}
}  // namespace clp::make_dictionaries_readable
