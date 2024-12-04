#include <vector>
#include <string>
#include "../Base64/base64.h" // ʹ�� cpp-base64 ��

// Varint���뺯��
namespace varint {
    // ��������ΪVarint��ʽ
    size_t encode(uint64_t value, std::vector<uint8_t>& buffer, std::vector<uint8_t>::iterator& it) {
        size_t bytes_written = 0;
        do {
            uint8_t byte = value & 0x7F; // ȡ��7λ
            value >>= 7;
            if (value) {
                byte |= 0x80; // �������ʣ�����ݣ��������λΪ1
            }
            it = buffer.insert(it, byte); // �ڵ�ǰλ�ò����ֽ�
            ++it;                         // �ƶ�����һ��λ��
            ++bytes_written;
        } while (value);
        return bytes_written;
    }

    // ����Varint��ʽΪ����
    uint64_t decode(const std::vector<uint8_t>& buffer, std::vector<uint8_t>::const_iterator& it) {
        uint64_t result = 0;
        int shift = 0;
        size_t bytes_read = 0;
        while (it != buffer.end()) {
            uint8_t byte = *it++;
            result |= (uint64_t(byte & 0x7F) << shift);
            ++bytes_read;
            if ((byte & 0x80) == 0) break; // ������λ�Ƿ�Ϊ0
            shift += 7;
            if (shift >= 64) throw std::overflow_error("Varint too large");
        }
        if (bytes_read == 0) throw std::runtime_error("Varint decode error: no data read");
        return result;
    }
}

// BufferWriter��
class BufferWriter {
private:
    std::vector<uint8_t> buffer;

public:
    BufferWriter() {}

    void null() {
        buffer.push_back(0); // ֱ�Ӳ���0
    }

    void varint(uint64_t value) {
        auto it = buffer.end();
        varint::encode(value, buffer, it); // ʹ�õ��������б���
    }

    std::string toString() const {
        std::string binary(buffer.begin(), buffer.end());
        return base64_encode(binary); // Base64����
    }
};

// BufferReader��
class BufferReader {
private:
    std::vector<uint8_t> buffer;
    std::vector<uint8_t>::const_iterator it;

public:
    BufferReader(const std::string& encodedString) {
        std::string binary = base64_decode(encodedString);
        buffer.assign(binary.begin(), binary.end());
        it = buffer.begin(); // ��ʼ��������
    }

    uint8_t nextByte() {
        if (it == buffer.end()) throw std::out_of_range("Index out of range");
        return *it++;
    }

    uint64_t nextVarint() {
        return varint::decode(buffer, it); // ʹ�õ���������
    }
};