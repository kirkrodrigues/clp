#ifndef CLP_MAKE_DICTIONARIES_READABLE_COMMANDLINEARGUMENTS_HPP
#define CLP_MAKE_DICTIONARIES_READABLE_COMMANDLINEARGUMENTS_HPP

#include "../CommandLineArgumentsBase.hpp"

namespace clp::make_dictionaries_readable {
class CommandLineArguments : public CommandLineArgumentsBase {
public:
    // Constructors
    explicit CommandLineArguments(std::string const& program_name)
            : CommandLineArgumentsBase(program_name) {}

    // Methods
    ParsingResult parse_arguments(int argc, char const* argv[]) override;

    std::string const& get_log_file_path() const { return m_log_file_path; }

    std::string const& get_output_file_path() const { return m_output_file_path; }

private:
    // Methods
    void print_basic_usage() const override;

    // Variables
    std::string m_log_file_path;
    std::string m_output_file_path;
};
}  // namespace clp::make_dictionaries_readable

#endif  // CLP_MAKE_DICTIONARIES_READABLE_COMMANDLINEARGUMENTS_HPP
