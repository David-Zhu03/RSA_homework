#include "RsaCrypto.h"
#include <algorithm>
#include <stdexcept>

BigNumber bytesToBigNumber(const std::vector<uint8_t>& bytes) {
    BigNumber result("0");
    BigNumber base("128");

    for (uint8_t byte : bytes) {
        result = result * base + BigNumber(std::to_string(byte));
    }
    return result;
}

std::vector<uint8_t> bigNumberToBytes(BigNumber number) {
    std::vector<uint8_t> bytes;
    BigNumber base("128");
    BigNumber zero("0");

    while (number > zero) {
        BigNumber remainder = number % base;
        number = number / base;
        bytes.push_back(static_cast<uint8_t>(std::stoi(remainder.toString())));
    }

    std::reverse(bytes.begin(), bytes.end());
    return bytes;
}

std::vector<BigNumber> rsaEncryptChunks(const std::string& message, const BigNumber& e, const BigNumber& n, int keyBits) {
    int k = keyBits / 8;
    if (k < 2) throw std::invalid_argument("Key size too small for padding");

    std::vector<BigNumber> ciphertexts;
    size_t fullBlocks = message.size() / (k - 1);
    size_t lastBlockLen = message.size() % (k - 1);

    for (size_t i = 0; i < fullBlocks; ++i) {
        std::vector<uint8_t> chunk;
        chunk.push_back(k - 1);
        chunk.insert(chunk.end(), message.begin() + i * (k - 1), message.begin() + (i + 1) * (k - 1));

        BigNumber m = bytesToBigNumber(chunk);
        if (m >= n) throw std::runtime_error("Block too large");
        ciphertexts.push_back(m.powmod(e, n));
    }

    if (lastBlockLen > 0) {
        std::vector<uint8_t> lastChunk;
        lastChunk.push_back(static_cast<uint8_t>(lastBlockLen));
        lastChunk.insert(lastChunk.end(), message.end() - lastBlockLen, message.end());
        lastChunk.resize(k, 0);

        BigNumber m = bytesToBigNumber(lastChunk);
        if (m >= n) throw std::runtime_error("Last block too large after padding");
        ciphertexts.push_back(m.powmod(e, n));
    }

    return ciphertexts;
}

std::string rsaDecryptChunks(const std::vector<BigNumber>& ciphertexts, const BigNumber& d, const BigNumber& n) {
    std::string result;
    int k = 0;
    for (const auto& c : ciphertexts) {
        BigNumber m = c.powmod(d, n);
        std::vector<uint8_t> bytes = bigNumberToBytes(m);

        if (k == 0) k = bytes.size();
        if (bytes.empty()) continue;

        uint8_t plainLen = bytes[0];
        if (plainLen > k - 1) {
            throw std::runtime_error("Invalid padding length");
        }

        for (size_t i = 1; i <= plainLen; ++i) {
            result.push_back(static_cast<char>(bytes[i]));
        }
    }
    return result;
}

std::vector<BigNumber> rsaSignChunks(const std::string& message, const BigNumber& d, const BigNumber& n, int keyBits) {
    int k = keyBits / 8;
    if (k < 2) throw std::invalid_argument("Key size too small for padding");

    std::vector<BigNumber> signatures;
    size_t fullBlocks = message.size() / (k - 1);
    size_t lastBlockLen = message.size() % (k - 1);

    for (size_t i = 0; i < fullBlocks; ++i) {
        std::vector<uint8_t> chunk;
        chunk.push_back(k - 1);
        chunk.insert(chunk.end(), message.begin() + i * (k - 1), message.begin() + (i + 1) * (k - 1));
        BigNumber m = bytesToBigNumber(chunk);
        if (m >= n) throw std::runtime_error("Block too large");
        signatures.push_back(m.powmod(d, n));
    }

    if (lastBlockLen > 0) {
        std::vector<uint8_t> lastChunk;
        lastChunk.push_back(static_cast<uint8_t>(lastBlockLen));
        lastChunk.insert(lastChunk.end(), message.end() - lastBlockLen, message.end());
        lastChunk.resize(k, 0);
        BigNumber m = bytesToBigNumber(lastChunk);
        if (m >= n) throw std::runtime_error("Last block too large after padding");
        signatures.push_back(m.powmod(d, n));
    }

    return signatures;
}

bool rsaVerifyChunks(const std::string& message, const std::vector<BigNumber>& signature, const BigNumber& e, const BigNumber& n) {
    std::string recovered;
    int k = 0;
    for (const auto& c : signature) {
        BigNumber m = c.powmod(e, n);
        std::vector<uint8_t> bytes = bigNumberToBytes(m);

        if (k == 0) k = bytes.size();
        if (bytes.empty()) continue;

        uint8_t plainLen = bytes[0];
        if (plainLen > k - 1) {
            throw std::runtime_error("Invalid padding length");
        }

        for (size_t i = 1; i <= plainLen; ++i) {
            recovered.push_back(static_cast<char>(bytes[i]));
        }
    }
    return recovered == message;
}