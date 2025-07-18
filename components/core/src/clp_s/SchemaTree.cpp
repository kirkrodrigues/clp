#include "SchemaTree.hpp"

#include "archive_constants.hpp"
#include "FileWriter.hpp"
#include "search/ast/Literal.hpp"
#include "ZstdCompressor.hpp"

namespace clp_s {
auto node_to_literal_type(NodeType type) -> clp_s::search::ast::LiteralType {
    // TODO: properly support NodeType::Object once we add LiteralType::ObjectT when implementing
    // type-per-token support.
    switch (type) {
        case NodeType::Integer:
        case NodeType::DeltaInteger:
            return clp_s::search::ast::LiteralType::IntegerT;
        case NodeType::Float:
            return clp_s::search::ast::LiteralType::FloatT;
        case NodeType::ClpString:
            return clp_s::search::ast::LiteralType::ClpStringT;
        case NodeType::VarString:
            return clp_s::search::ast::LiteralType::VarStringT;
        case NodeType::Boolean:
            return clp_s::search::ast::LiteralType::BooleanT;
        case NodeType::UnstructuredArray:
            return clp_s::search::ast::LiteralType::ArrayT;
        case NodeType::NullValue:
            return clp_s::search::ast::LiteralType::NullT;
        case NodeType::DateString:
            return clp_s::search::ast::LiteralType::EpochDateT;
        case NodeType::Metadata:
        case NodeType::Unknown:
        default:
            return clp_s::search::ast::LiteralType::UnknownT;
    }
}

int32_t SchemaTree::add_node(int32_t parent_node_id, NodeType type, std::string_view const key) {
    auto node_it = m_node_map.find({parent_node_id, key, type});
    if (node_it != m_node_map.end()) {
        auto node_id = node_it->second;
        m_nodes[node_id].increase_count();
        return node_id;
    }

    int32_t node_id = m_nodes.size();
    auto& node = m_nodes.emplace_back(parent_node_id, node_id, std::string{key}, type, 0);
    node.increase_count();
    if (constants::cRootNodeId == parent_node_id) {
        m_namespace_and_type_to_subtree_id.emplace(
                std::make_pair(node.get_key_name(), type),
                node_id
        );
    }

    if (constants::cRootNodeId != parent_node_id) {
        auto& parent_node = m_nodes[parent_node_id];
        node.set_depth(parent_node.get_depth() + 1);
        parent_node.add_child(node_id);
    }
    m_node_map.emplace(std::make_tuple(parent_node_id, node.get_key_name(), type), node_id);

    return node_id;
}

auto SchemaTree::get_subtree_node_id(std::string_view subtree_namespace, NodeType type) const
        -> int32_t {
    auto it = m_namespace_and_type_to_subtree_id.find({subtree_namespace, type});
    if (m_namespace_and_type_to_subtree_id.end() != it) {
        return it->second;
    }
    return -1;
}

int32_t SchemaTree::get_metadata_field_id(std::string_view const field_name) const {
    auto const metadata_subtree_id = get_metadata_subtree_node_id();
    if (metadata_subtree_id < 0) {
        return -1;
    }

    auto& metadata_subtree_node = m_nodes.at(metadata_subtree_id);
    for (auto child_id : metadata_subtree_node.get_children_ids()) {
        auto& child_node = m_nodes.at(child_id);
        if (child_node.get_key_name() == field_name) {
            return child_id;
        }
    }

    return -1;
}

size_t SchemaTree::store(std::string const& archives_dir, int compression_level) {
    FileWriter schema_tree_writer;
    ZstdCompressor schema_tree_compressor;

    schema_tree_writer.open(
            archives_dir + constants::cArchiveSchemaTreeFile,
            FileWriter::OpenMode::CreateForWriting
    );
    schema_tree_compressor.open(schema_tree_writer, compression_level);

    schema_tree_compressor.write_numeric_value(m_nodes.size());
    for (auto const& node : m_nodes) {
        schema_tree_compressor.write_numeric_value(node.get_parent_id());

        auto key = node.get_key_name();
        schema_tree_compressor.write_numeric_value(key.size());
        schema_tree_compressor.write(key.begin(), key.size());
        schema_tree_compressor.write_numeric_value(node.get_type());
    }

    schema_tree_compressor.close();
    size_t compressed_size = schema_tree_writer.get_pos();
    schema_tree_writer.close();
    return compressed_size;
}

int32_t SchemaTree::find_matching_subtree_root_in_subtree(
        int32_t const subtree_root_node,
        int32_t node,
        NodeType type
) const {
    int32_t earliest_match = -1;
    while (subtree_root_node != node) {
        auto const& schema_node = get_node(node);
        if (schema_node.get_type() == type) {
            earliest_match = node;
        }
        node = schema_node.get_parent_id();
    }
    return earliest_match;
}
}  // namespace clp_s
