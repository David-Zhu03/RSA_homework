#include <iostream>
#include <cassert>
#include <openssl/bn.h>
#include "BigNumber.h"

std::string openssl_op(const std::string& a, const std::string& b, char op) {
    BN_CTX* ctx = BN_CTX_new();
    BIGNUM *aa = BN_new(), *bb = BN_new(), *res = BN_new();

    BN_dec2bn(&aa, a.c_str());
    BN_dec2bn(&bb, b.c_str());

    switch (op) {
        case '+': BN_add(res, aa, bb); break;
        case '-': BN_sub(res, aa, bb); break;
        case '*': BN_mul(res, aa, bb, ctx); break;
        case '/': BN_div(res, nullptr, aa, bb, ctx); break;
        case '%': BN_mod(res, aa, bb, ctx); break;
        default: throw std::invalid_argument("Invalid operation");
    }

    char* result_str = BN_bn2dec(res);
    std::string result(result_str);

    OPENSSL_free(result_str);
    BN_free(aa); BN_free(bb); BN_free(res); BN_CTX_free(ctx);

    return result;
}

void testBinaryOp(const std::string& a, const std::string& b, char op, const std::string& name) {
    BigNumber A(a), B(b), result;
    switch (op) {
        case '+': result = A + B; break;
        case '-': result = A - B; break;
        case '*': result = A * B; break;
        case '/': result = A / B; break;
        case '%': result = A % B; break;
        default: throw std::invalid_argument("Unknown op");
    }

    std::string expected = openssl_op(a, b, op);
    std::cout << "BigNumber " << name << ": " << result.toString() << "\n";
    std::cout << "OpenSSL   " << name << ": " << expected << "\n";
    assert(result.toString() == expected);
    std::cout << "[PASS] " << name << " comparison\n\n";
}

std::string openssl_powmod(const std::string& base, const std::string& exp, const std::string& mod) {
    BN_CTX* ctx = BN_CTX_new();
    BIGNUM *b = BN_new(), *e = BN_new(), *m = BN_new(), *res = BN_new();

    BN_dec2bn(&b, base.c_str());
    BN_dec2bn(&e, exp.c_str());
    BN_dec2bn(&m, mod.c_str());

    BN_mod_exp(res, b, e, m, ctx);

    char* result_str = BN_bn2dec(res);
    std::string result(result_str);

    OPENSSL_free(result_str);
    BN_free(b); BN_free(e); BN_free(m); BN_free(res); BN_CTX_free(ctx);
    return result;
}

std::string openssl_modinv(const std::string& a, const std::string& mod) {
    BN_CTX* ctx = BN_CTX_new();
    BIGNUM *aa = BN_new(), *mm = BN_new(), *res = BN_new();

    BN_dec2bn(&aa, a.c_str());
    BN_dec2bn(&mm, mod.c_str());

    BN_mod_inverse(res, aa, mm, ctx);
    char* result_str = BN_bn2dec(res);
    std::string result(result_str);

    OPENSSL_free(result_str);
    BN_free(aa); BN_free(mm); BN_free(res); BN_CTX_free(ctx);
    return result;
}

void testPowmodWithOpenSSL(const std::string& base, const std::string& exp, const std::string& mod) {
    BigNumber a(base), e(exp), m(mod);
    BigNumber res = a.powmod(e, m);
    std::string expected = openssl_powmod(base, exp, mod);
    std::cout << "BigNumber powmod: " << res.toString() << "\n";
    std::cout << "OpenSSL   powmod: " << expected << "\n";
    assert(res.toString() == expected);
    std::cout << "[PASS] powmod comparison\n\n";
}

void testModinvWithOpenSSL(const std::string& a, const std::string& mod) {
    BigNumber x(a), m(mod);
    BigNumber inv = x.modinv(m);
    std::string expected = openssl_modinv(a, mod);
    std::cout << "BigNumber modinv: " << inv.toString() << "\n";
    std::cout << "OpenSSL   modinv: " << expected << "\n";
    assert(inv.toString() == expected);
    std::cout << "[PASS] modinv comparison\n\n";
}

int main() {
    std::cout << "=== Basic Arithmetic Tests ===\n";
    testBinaryOp("12345678901234567890", "98765432109876543210", '+', "Addition");
    testBinaryOp("98765432109876543210", "12345678901234567890", '-', "Subtraction");
    testBinaryOp("123456789", "987654321", '*', "Multiplication");
    testBinaryOp("98765432109876543210", "123456789", '/', "Division");
    testBinaryOp("98765432109876543210", "123456789", '%', "Modulus");

    std::cout << "=== Modular Arithmetic Tests ===\n";
    testPowmodWithOpenSSL("4", "13", "497");
    testPowmodWithOpenSSL("123456789", "65537", "987654321987654321");
    testModinvWithOpenSSL("3", "11");
    testModinvWithOpenSSL("123456789", "1000000007");

    std::cout << "All BigNumber <=> OpenSSL tests passed.\n";
    return 0;
}