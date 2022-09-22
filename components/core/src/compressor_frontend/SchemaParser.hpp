#ifndef COMPRESSOR_FRONTEND_SCHEMAPARSER_HPP
#define COMPRESSOR_FRONTEND_SCHEMAPARSER_HPP

// Boost libraries
#include <boost/filesystem/path.hpp>
#include <utility>

// Project headers
#include "../ReaderInterface.hpp"
#include "LALR1Parser.hpp"

namespace compressor_frontend {
    // ASTs used in SchemaParser AST
    class SchemaFileAST : public ParserAST {
    public:
        // Constructor
        SchemaFileAST () = default;

        /**
         * Sets the list of delimiters when a delimiters line is found in the schema
         * Will overwrite any existing delimiters
         * @param delimiters_in
         */
        void add_delimiters (std::unique_ptr<ParserAST> delimiters_in) {
            m_delimiters = std::move(delimiters_in);
        }

        /**
         * Add a schema variable to the list of schema vars when a variable line is found in the schema
         * @param schema_var
         */
        void add_schema_var (std::unique_ptr<ParserAST> schema_var) {
            m_schema_vars.push_back(std::move(schema_var));
        }

        std::vector<std::unique_ptr<ParserAST>> m_schema_vars;
        std::unique_ptr<ParserAST> m_delimiters;
        std::string m_file_path;
    };
    
    class IdentifierAST : public ParserAST {
    public:
        // Constructor
        explicit IdentifierAST (char character) {
            m_name.push_back(character);
        }

        /**
         * Add a character to the name of a variable in the schema
         * @param character
         */
        void add_character (char character) {
            m_name.push_back(character);
        }
        
        std::string m_name;
    };
    
    class SchemaVarAST : public ParserAST {
    public:
        //Constructor
        SchemaVarAST (std::string name, std::unique_ptr<RegexAST> regex_ptr, uint32_t line_num) : m_name(std::move(name)), m_regex_ptr(std::move(regex_ptr)),
                                                                                                  m_line_num(line_num) {}

        uint32_t m_line_num;
        std::string m_name;
        std::unique_ptr<RegexAST> m_regex_ptr;
    };

    class DelimiterStringAST : public ParserAST {
    public:
        // Constructor
        explicit DelimiterStringAST (uint32_t delimiter) {
            m_delimiters.push_back(delimiter);
        }

        /**
         * Add a delimiter to the string of delimiters specified on a delimiters line of the schema
         * @param delimiter
         */
        void add_delimiter (uint32_t delimiter) {
            m_delimiters.push_back(delimiter);
        }

        std::vector<uint32_t> m_delimiters;
    };

    // Schema Parser itself

    class SchemaParser : public LALR1Parser {
    public:
        // Constructor
        SchemaParser ();

        /**
         * A semantic rule that needs access to soft_reset()
         * @param m
         * @return std::unique_ptr<SchemaFileAST>
         */
        std::unique_ptr<SchemaFileAST> existing_schema_file_rule (NonTerminal* m);

        /**
         * Parse a user defined schema to generate a schema AST used for generating the log lexer
         * @param reader
         * @return std::unique_ptr<SchemaFileAST>
         */
        std::unique_ptr<SchemaFileAST> generate_schema_ast (ReaderInterface& reader);

        /**
         * Wrapper around generate_schema_ast() including error checking on opening the input schema file and computing schema checksum
         * @param schema_file_path
         * @return std::unique_ptr<SchemaFileAST>
         */
        static std::unique_ptr<SchemaFileAST> try_schema_file (const std::string& schema_file_path);

    private:
        /**
         * Add all lexical rules needed for schema lexing
         */
        void add_lexical_rules ();

        /**
         * Add all productions needed for schema parsing
         */
        void add_productions ();
    };
}

#endif // COMPRESSOR_FRONTEND_SCHEMAPARSER_HPP
