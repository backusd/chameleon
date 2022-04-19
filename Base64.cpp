#include "Base64.h"

const uint8_t Base64::tableDecodeBase64[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 64, 0, 0,
    0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0,
    0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0
};

const char* Base64::tableEncodeBase64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

inline char Base64::EncodeChar(uint8_t b) {
    return tableEncodeBase64[size_t(b)];
}

inline uint8_t Base64::DecodeChar(char c) {
    if (c & 0x80) {
        std::ostringstream oss;
        oss << "Invalid base64 char value: " << sizeof(c);
        throw Base64Exception(__LINE__, __FILE__, oss.str());
    }
    return Base64::tableDecodeBase64[size_t(c & 0x7F)]; // TODO faster with lookup table or ifs?
}

void Base64::Encode(const uint8_t* in, size_t inLength, std::string& out) {
    size_t outLength = ((inLength + 2) / 3) * 4;

    size_t j = out.size();
    out.resize(j + outLength);

    for (size_t i = 0; i < inLength; i += 3) {
        uint8_t b = (in[i] & 0xFC) >> 2;
        out[j++] = EncodeChar(b);

        b = (in[i] & 0x03) << 4;
        if (i + 1 < inLength) {
            b |= (in[i + 1] & 0xF0) >> 4;
            out[j++] = EncodeChar(b);

            b = (in[i + 1] & 0x0F) << 2;
            if (i + 2 < inLength) {
                b |= (in[i + 2] & 0xC0) >> 6;
                out[j++] = EncodeChar(b);

                b = in[i + 2] & 0x3F;
                out[j++] = EncodeChar(b);
            }
            else {
                out[j++] = EncodeChar(b);
                out[j++] = '=';
            }
        }
        else {
            out[j++] = EncodeChar(b);
            out[j++] = '=';
            out[j++] = '=';
        }
    }
}

void Base64::Encode(const std::vector<uint8_t>& in, std::string& out) {
    Base64::Encode(in.data(), in.size(), out);
}

std::string Base64::Encode(const std::vector<uint8_t>& in) {
    std::string encoded;
    Base64::Encode(in, encoded);
    return encoded;
}

size_t Base64::Decode(const char* in, size_t inLength, uint8_t*& out) {
    if (inLength % 4 != 0) {
        std::ostringstream oss;
        oss << "Invalid base64 encoded data: \"" << std::string(in, std::min(size_t(32), inLength)) << "\", length: " << inLength;
        throw Base64Exception(__LINE__, __FILE__, oss.str());
    }

    if (inLength < 4) {
        out = nullptr;
        return 0;
    }

    int nEquals = int(in[inLength - 1] == '=') +
        int(in[inLength - 2] == '=');

    size_t outLength = (inLength * 3) / 4 - nEquals;
    out = new uint8_t[outLength];
    memset(out, 0, outLength);

    size_t i, j = 0;

    for (i = 0; i + 4 < inLength; i += 4) {
        uint8_t b0 = DecodeChar(in[i]);
        uint8_t b1 = DecodeChar(in[i + 1]);
        uint8_t b2 = DecodeChar(in[i + 2]);
        uint8_t b3 = DecodeChar(in[i + 3]);

        out[j++] = (uint8_t)((b0 << 2) | (b1 >> 4));
        out[j++] = (uint8_t)((b1 << 4) | (b2 >> 2));
        out[j++] = (uint8_t)((b2 << 6) | b3);
    }

    {
        uint8_t b0 = DecodeChar(in[i]);
        uint8_t b1 = DecodeChar(in[i + 1]);
        uint8_t b2 = DecodeChar(in[i + 2]);
        uint8_t b3 = DecodeChar(in[i + 3]);

        out[j++] = (uint8_t)((b0 << 2) | (b1 >> 4));
        if (b2 < 64) out[j++] = (uint8_t)((b1 << 4) | (b2 >> 2));
        if (b3 < 64) out[j++] = (uint8_t)((b2 << 6) | b3);
    }

    return outLength;
}

size_t Base64::Decode(const std::string& in, std::vector<uint8_t>& out) {
    uint8_t* outPtr = nullptr;
    size_t decodedSize = Base64::Decode(in.data(), in.size(), outPtr);
    if (outPtr == nullptr) {
        return 0;
    }
    out.assign(outPtr, outPtr + decodedSize);
    delete[] outPtr;
    return decodedSize;
}

std::vector<uint8_t> Base64::Decode(const std::string& in) {
    std::vector<uint8_t> result;
    Base64::Decode(in, result);
    return result;
}