#include <vector>
#include <string>
#include "../Base64/base64.h" // 使用 cpp-base64 库

// Varint编码函数
namespace varint {
    // 编码整数为Varint格式
    size_t encode(uint64_t value, std::vector<uint8_t>& buffer, std::vector<uint8_t>::iterator& it) {
        size_t bytes_written = 0;
        do {
            uint8_t byte = value & 0x7F; // 取低7位
            value >>= 7;
            if (value) {
                byte |= 0x80; // 如果还有剩余数据，设置最高位为1
            }
            it = buffer.insert(it, byte); // 在当前位置插入字节
            ++it;                         // 移动到下一个位置
            ++bytes_written;
        } while (value);
        return bytes_written;
    }

    // 解码Varint格式为整数
    uint64_t decode(const std::vector<uint8_t>& buffer, std::vector<uint8_t>::const_iterator& it) {
        uint64_t result = 0;
        int shift = 0;
        size_t bytes_read = 0;
        while (it != buffer.end()) {
            uint8_t byte = *it++;
            result |= (uint64_t(byte & 0x7F) << shift);
            ++bytes_read;
            if ((byte & 0x80) == 0) break; // 检查最高位是否为0
            shift += 7;
            if (shift >= 64) throw std::overflow_error("Varint too large");
        }
        if (bytes_read == 0) throw std::runtime_error("Varint decode error: no data read");
        return result;
    }
}

// BufferWriter类
class BufferWriter {
private:
    std::vector<uint8_t> buffer;

public:
    BufferWriter() {}

    void null() {
        buffer.push_back(0); // 直接插入0
    }

    void varint(uint64_t value) {
        auto it = buffer.end();
        varint::encode(value, buffer, it); // 使用迭代器进行编码
    }

    std::string toString() const {
        std::string binary(buffer.begin(), buffer.end());
        return base64_encode(binary); // Base64编码
    }
};

// BufferReader类
class BufferReader {
private:
    std::vector<uint8_t> buffer;
    std::vector<uint8_t>::const_iterator it;

public:
    BufferReader(const std::string& encodedString) {
        std::string binary = base64_decode(encodedString);
        buffer.assign(binary.begin(), binary.end());
        it = buffer.begin(); // 初始化迭代器
    }

    uint8_t nextByte() {
        if (it == buffer.end()) throw std::out_of_range("Index out of range");
        return *it++;
    }

    uint64_t nextVarint() {
        return varint::decode(buffer, it); // 使用迭代器解码
    }
};