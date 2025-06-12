#include "GenerateKey.h"
#include <random>
#include <ctime>

BigNumber generateRandomOddBigNumber(int bits) {
    if (bits < 2) throw std::invalid_argument("Bit length must be at least 2");

    int digits = bits / 3.32 + 1;
    std::string numStr;
    static std::mt19937 gen(time(0));
    std::uniform_int_distribution<> dist(0, 9);

    numStr += '1' + dist(gen) % 9;
    for (int i = 1; i < digits - 1; ++i)
        numStr += '0' + dist(gen);

    static const char odds[] = {'1', '3', '5', '7', '9'};
    numStr += odds[dist(gen) % 5];

    return BigNumber(numStr);
}

bool isProbablyPrime(const BigNumber& n, int k) {
    if (n == BigNumber(2) || n == BigNumber(3)) return true;
    if (n < BigNumber(2) || (n % BigNumber(2) == BigNumber(0))) return false;

    BigNumber d = n - BigNumber(1);
    int r = 0;
    while (d % BigNumber(2) == BigNumber(0)) {
        d = d / BigNumber(2);
        r++;
    }

    static std::mt19937 gen(time(0));
    std::uniform_int_distribution<int> dist(2, 9);

    for (int i = 0; i < k; ++i) {
        BigNumber a = BigNumber(dist(gen)) % (n - BigNumber(4)) + BigNumber(2);
        BigNumber x = a.powmod(d, n);
        if (x == BigNumber(1) || x == n - BigNumber(1)) continue;

        bool passed = false;
        for (int j = 0; j < r - 1; ++j) {
            x = x.powmod(BigNumber(2), n);
            if (x == n - BigNumber(1)) {
                passed = true;
                break;
            }
        }

        if (!passed) return false;
    }

    return true;
}

bool isProbablyPrime_optimization(const BigNumber& n, int k) {
    if (n == BigNumber(2) || n == BigNumber(3)) return true;
    if (n < BigNumber(2) || (n % BigNumber(2) == BigNumber(0))) return false;

    BigNumber d = n - BigNumber(1);
    int r = 0;
    BigNumber two(2);
    while ((d % two) == BigNumber(0)) {
        d = d / two;
        r++;
    }

    static std::mt19937_64 gen(time(0));
    std::uniform_int_distribution<uint64_t> dist;

    for (int i = 0; i < k; ++i) {
        // 这里生成随机 a，范围在 [2, n-2]
        BigNumber a;
        do {
            uint64_t rnd64 = dist(gen);
            a = BigNumber(std::to_string(rnd64 % (std::stoull(n.toString()) - 3) + 2));
        } while (a >= n - BigNumber(2) || a < two);

        BigNumber x = a.powmod(d, n);
        if (x == BigNumber(1) || x == n - BigNumber(1)) continue;

        bool passed = false;
        for (int j = 0; j < r - 1; ++j) {
            x = (x * x) % n;  // 快速平方模替代powmod(2,n)
            if (x == n - BigNumber(1)) {
                passed = true;
                break;
            }
        }

        if (!passed) return false;
    }
    return true;
}

void generateRSAKeyPair(int bits, BigNumber& e, BigNumber& d, BigNumber& n) {
    BigNumber p, q, phi;

    do {
        p = generateRandomOddBigNumber(bits / 2);
        while (!isProbablyPrime(p)) {
            p = generateRandomOddBigNumber(bits / 2);
        }

        q = generateRandomOddBigNumber(bits / 2);
        while (!isProbablyPrime(q) || q == p) {
            q = generateRandomOddBigNumber(bits / 2);
        }

        n = p * q;
        phi = (p - BigNumber(1)) * (q - BigNumber(1));
        e = BigNumber(65537);
    } while (phi % e == BigNumber(0));

    d = e.modinv(phi);
}

void generateRSAKeyPair_optimization(int bits, BigNumber& e, BigNumber& d, BigNumber& n) {
    BigNumber p, q, phi;

    do {
        p = generateRandomOddBigNumber(bits / 2);
        while (!isProbablyPrime(p)) {
            p = generateRandomOddBigNumber(bits / 2);
        }

        q = generateRandomOddBigNumber(bits / 2);
        while (!isProbablyPrime(q) || q == p) {
            q = generateRandomOddBigNumber(bits / 2);
        }

        n = p * q;
        phi = (p - BigNumber(1)) * (q - BigNumber(1));
        e = BigNumber(65537);
    } while (phi % e == BigNumber(0));

    d = e.modinv(phi);
}