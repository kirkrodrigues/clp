#include "ColumnWriter.hpp"

namespace clp_s {
void Int64ColumnWriter::add_value(ParsedMessage::variable_t& value, size_t& size) {
    size = sizeof(int64_t);
    m_values.push_back(std::get<int64_t>(value));
}

size_t Int64ColumnWriter::store(PassthroughCompressor& compressor) {
    size_t size = m_values.size() * sizeof(int64_t);
    compressor.write(reinterpret_cast<char const*>(m_values.data()), size);
    return size;
}

void FloatColumnWriter::add_value(ParsedMessage::variable_t& value, size_t& size) {
    size = sizeof(double);
    m_values.push_back(std::get<double>(value));
}

size_t FloatColumnWriter::store(PassthroughCompressor& compressor) {
    size_t size = m_values.size() * sizeof(double);
    compressor.write(reinterpret_cast<char const*>(m_values.data()), size);
    return size;
}

void BooleanColumnWriter::add_value(ParsedMessage::variable_t& value, size_t& size) {
    size = sizeof(uint8_t);
    m_values.push_back(std::get<bool>(value) ? 1 : 0);
}

size_t BooleanColumnWriter::store(PassthroughCompressor& compressor) {
    size_t size = m_values.size() * sizeof(uint8_t);
    compressor.write(reinterpret_cast<char const*>(m_values.data()), size);
    return size;
}

void ClpStringColumnWriter::add_value(ParsedMessage::variable_t& value, size_t& size) {
    size = sizeof(int64_t);
    std::string string_var = std::get<std::string>(value);
    uint64_t id;
    uint64_t offset = m_encoded_vars.size();
    VariableEncoder::encode_and_add_to_dictionary(
            string_var,
            m_logtype_entry,
            *m_var_dict,
            m_encoded_vars
    );
    m_log_dict->add_entry(m_logtype_entry, id);
    auto encoded_id = encode_log_dict_id(id, offset);
    m_logtypes.push_back(encoded_id);
    size += sizeof(int64_t) * (m_encoded_vars.size() - offset);
}

size_t ClpStringColumnWriter::store(PassthroughCompressor& compressor) {
    size_t logtypes_size = m_logtypes.size() * sizeof(int64_t);
    compressor.write(reinterpret_cast<char const*>(m_logtypes.data()), logtypes_size);
    size_t encoded_vars_size = m_encoded_vars.size() * sizeof(int64_t);
    size_t num_encoded_vars = m_encoded_vars.size();
    compressor.write_numeric_value(num_encoded_vars);
    compressor.write(reinterpret_cast<char const*>(m_encoded_vars.data()), encoded_vars_size);
    return logtypes_size + sizeof(num_encoded_vars) + encoded_vars_size;
}

void VariableStringColumnWriter::add_value(ParsedMessage::variable_t& value, size_t& size) {
    size = sizeof(int64_t);
    std::string string_var = std::get<std::string>(value);
    uint64_t id;
    m_var_dict->add_entry(string_var, id);
    m_variables.push_back(id);
}

size_t VariableStringColumnWriter::store(PassthroughCompressor& compressor) {
    size_t size = m_variables.size() * sizeof(int64_t);
    compressor.write(reinterpret_cast<char const*>(m_variables.data()), size);
    return size;
}

void DateStringColumnWriter::add_value(ParsedMessage::variable_t& value, size_t& size) {
    size = 2 * sizeof(int64_t);
    auto encoded_timestamp = std::get<std::pair<uint64_t, epochtime_t>>(value);
    m_timestamps.push_back(encoded_timestamp.second);
    m_timestamp_encodings.push_back(encoded_timestamp.first);
}

size_t DateStringColumnWriter::store(PassthroughCompressor& compressor) {
    size_t timestamps_size = m_timestamps.size() * sizeof(int64_t);
    compressor.write(reinterpret_cast<char const*>(m_timestamps.data()), timestamps_size);
    size_t encodings_size = m_timestamp_encodings.size() * sizeof(int64_t);
    compressor.write(reinterpret_cast<char const*>(m_timestamp_encodings.data()), encodings_size);
    return timestamps_size + encodings_size;
}
}  // namespace clp_s
