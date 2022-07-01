#ifndef UTILS_GET_FLOAT_VARS_COMMANDLINEARGUMENTS_HPP
#define UTILS_GET_FLOAT_VARS_COMMANDLINEARGUMENTS_HPP

// C++ libraries
#include <string>

// Boost libraries
#include <boost/asio.hpp>

// Project headers
#include "../../CommandLineArgumentsBase.hpp"

namespace utils { namespace get_float_vars {
    class CommandLineArguments : public CommandLineArgumentsBase {
    public:
        // Constructors
        explicit CommandLineArguments (const std::string& program_name) : CommandLineArgumentsBase(program_name) {}

        // Methods
        ParsingResult parse_arguments (int argc, const char* argv[]) override;

        const std::string& get_input_path () const { return m_input_path; }

    private:
        // Methods
        void print_basic_usage () const override;

        // Variables
        std::string m_input_path;
    };
} }

#endif // UTILS_GET_FLOAT_VARS_COMMANDLINEARGUMENTS_HPP
