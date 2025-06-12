#ifndef RSA_CRYPTO_H
#define RSA_CRYPTO_H

#include "BigNumber.h"
#include <string>
#include <vector>
#include <cstdint>

BigNumber bytesToBigNumber(const std::vector<uint8_t>& bytes);
std::vector<uint8_t> bigNumberToBytes(BigNumber number);

std::vector<BigNumber> rsaEncryptChunks(const std::string& message, const BigNumber& e, const BigNumber& n, int keyBits);
std::string rsaDecryptChunks(const std::vector<BigNumber>& ciphertexts, const BigNumber& d, const BigNumber& n);

std::vector<BigNumber> rsaSignChunks(const std::string& message, const BigNumber& d, const BigNumber& n, int keyBits);
bool rsaVerifyChunks(const std::string& message, const std::vector<BigNumber>& signature, const BigNumber& e, const BigNumber& n);

#endif // RSA_CRYPTO_H
