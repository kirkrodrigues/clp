#include "QueryParser.hpp"

QueryParser::QueryParser(std::unique_ptr<ParserASTSchemaFile> schema_ast) {
    add_tokens();
    add_productions(schema_ast);
    generate();
}

void QueryParser::add_tokens() {
    add_token("Star", '*');
    add_token("Backslash", '\\');

    add_token_group("WhiteSpace", std::make_unique<RegexASTGroup>(RegexASTGroup({ ' ', '\t', '\r', '\n', '\v', '\f' })));

    // make sure to add this after any noteable characters
    add_token_group("UncaughtChar", std::make_unique<RegexASTGroup>(RegexASTGroup(0, RegexASTGroup::UNICODE_MAX)));
}

void QueryParser::add_productions(std::unique_ptr<ParserASTSchemaFile> const& schema_ast) {
    add_production("PartialQuery", { "PartialQuery", "Delimniter" }, nullptr);
    add_production("PartialQuery", { "PartialQuery", "QueryToken" }, nullptr);
    add_production("PartialQuery", { "QueryToken" }, nullptr);

    add_production("QueryToken", { "TextStar", "Delimiter" }, nullptr);
    add_production("QueryToken", { "StarText", "Delimiter" }, nullptr);
    add_production("QueryToken", { "StaticText", "Delimiter" }, nullptr);

    add_production("TextStar", { "StarText", "Star" }, nullptr);
    add_production("TextStar", { "TextStar", "StaticText" }, nullptr);
    add_production("StarText", { "Star", "StaticText" }, nullptr);
    add_production("TextStar", { "StaticText", "Star" }, nullptr);

    add_production("Delimiter", { "Delimiter", Lexer::TOKEN_END }, nullptr);
    add_production("Delimiter", { "Delimiter", "WhiteSpace" }, nullptr);
    add_production("Delimiter", { Lexer::TOKEN_END }, nullptr);
    add_production("Delimiter", { "WhiteSpace" }, nullptr);
    add_production("StaticText", { "StaticText", "Char" }, nullptr);
    add_production("StaticText", { "Char" }, nullptr);
    add_production("Char", { "Backslash", "Star" }, nullptr);
    add_production("Char", { "Backslash", "Backslash" }, nullptr);
    add_production("Char", { "Backslash", "WhiteSpace" }, nullptr);
}