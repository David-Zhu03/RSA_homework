#include <iostream>
#include <vector>
#include "BigNumber.h"
#include "GenerateKey.h"
#include "RsaCrypto.h"

void testRSA(const std::string& message, const BigNumber& e, const BigNumber& d, const BigNumber& n, int keyBits) {
    std::cout << "测试消息: " << message << std::endl;

    try {
        // 加密解密测试
        auto ciphertexts = rsaEncryptChunks(message, e, n, keyBits);
        std::string decrypted = rsaDecryptChunks(ciphertexts, d, n);

        std::cout << "解密后: " << decrypted << std::endl;
        std::cout << (message == decrypted ? "加密解密测试成功！" : "加密解密测试失败！") << std::endl;

        // 签名验签测试
        auto signature = rsaSignChunks(message, d, n, keyBits);
        bool verified = rsaVerifyChunks(message, signature, e, n);

        std::cout << (verified ? "签名验签测试成功！" : "签名验签测试失败！") << std::endl;
    } catch (const std::exception& ex) {
        std::cout << "测试过程中出现异常: " << ex.what() << std::endl;
    }

    std::cout << "------------------------------------" << std::endl;
}

int main() {
    BigNumber e, d, n;
    int keyBits = 512;

    std::cout << "生成 " << keyBits << " 位 RSA 密钥对..." << std::endl;
    generateRSAKeyPair(keyBits, e, d, n);
    std::cout << "生成完成！" << std::endl << std::endl;

    // 多组测试样例
    std::vector<std::string> testMessages = {
        "Hello, RSA encryption and signing!",                     // 简单英文
        "0123456789ABCDEF0123456789ABCDEF",                       // 16进制字符串
        "The quick brown fox jumps over the lazy dog.",           // 经典英文句子
        "This is a test message with numbers 1234567890 and symbols !@#$%^&*()", // 混合字符
        "",                                                       // 空字符串
        std::string(200, 'A'),                                    // 长字符串，200个A
    };

    for (const auto& msg : testMessages) {
        testRSA(msg, e, d, n, keyBits);
    }

    return 0;
}

// macbookpro@zhuzhihangdeMacBook-Pro rsa % ./step3_test
// 生成 512 位 RSA 密钥对...
// 生成完成！

// 测试消息: Hello, RSA encryption and signing!
// 解密后: Hello, RSA encryption and signing!
// 加密解密测试成功！
// 签名验签测试成功！
// ------------------------------------
// 测试消息: 0123456789ABCDEF0123456789ABCDEF
// 解密后: 0123456789ABCDEF0123456789ABCDEF
// 加密解密测试成功！
// 签名验签测试成功！
// ------------------------------------
// 测试消息: The quick brown fox jumps over the lazy dog.
// 解密后: The quick brown fox jumps over the lazy dog.
// 加密解密测试成功！
// 签名验签测试成功！
// ------------------------------------
// 测试消息: This is a test message with numbers 1234567890 and symbols !@#$%^&*()
// 解密后: This is a test message with numbers 1234567890 and symbols !@#$%^&*()
// 加密解密测试成功！
// 签名验签测试成功！
// ------------------------------------
// 测试消息: 
// 解密后: 
// 加密解密测试成功！
// 签名验签测试成功！
// ------------------------------------
// 测试消息: AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
// 解密后: AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
// 加密解密测试成功！
// 签名验签测试成功！
// ------------------------------------