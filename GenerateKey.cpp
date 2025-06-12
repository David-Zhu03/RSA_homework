#include "GenerateKey.h"
#include <random>
#include <ctime>
#include <thread>
#include <atomic>
#include <mutex>

std::mutex prime_mutex;
std::atomic<bool> found_p(false), found_q(false);
BigNumber global_p, global_q;

void generatePrimeCandidate(int bits, BigNumber& result, std::atomic<bool>& foundFlag) {
    while (!foundFlag.load()) {
        BigNumber candidate = generateRandomOddBigNumber(bits / 2);
        if (isProbablyPrime(candidate)) {
            std::lock_guard<std::mutex> lock(prime_mutex);
            if (!foundFlag.load()) {
                result = candidate;
                foundFlag.store(true);
            }
            return;
        }
    }
}

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

BigNumber generateRandomOddBigNumber_optimization(int bits) {
    if (bits < 2) throw std::invalid_argument("Bit length must be at least 2");

    static std::mt19937_64 gen(std::random_device{}());
    std::vector<char> binaryDigits;

    binaryDigits.reserve(bits);
    binaryDigits.push_back(1);

    std::uniform_int_distribution<> dist(0, 1);
    for (int i = 1; i < bits - 1; ++i) {
        binaryDigits.push_back(dist(gen));
    }

    binaryDigits.push_back(1);
    BigNumber result(0);
    BigNumber two(2);

    for (char bit : binaryDigits) {
        result = result * two + BigNumber(bit);
    }
    return result;
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
    static const BigNumber ONE(1);
    static const BigNumber TWO(2);
    static const BigNumber THREE(3);

    if (n == TWO || n == THREE) return true;
    if (n < TWO || (n % TWO == BigNumber(0))) return false;

    static const int smallPrimes[] = {
        3, 5, 7, 11, 13, 17, 19, 23, 29, 31,
        37, 41, 43, 47, 53, 59, 61, 67, 71, 73,
        79, 83, 89, 97, 101, 103, 107, 109, 113, 127
    };
    for (int p : smallPrimes) {
        BigNumber prime(p);
        if (n == prime) return true;
        if (n % prime == BigNumber(0)) return false;
    }

    BigNumber d = n - ONE;
    int r = 0;
    while (d % TWO == BigNumber(0)) {
        d = d / TWO;
        ++r;
    }

    BarrettReducer reducer(n);
    thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(2, 1 << 16);

    for (int i = 0; i < k; ++i) {
        BigNumber a = BigNumber(dist(gen)) % (n - BigNumber(4)) + TWO;
        BigNumber x = a.powmod(d, n);
        if (x == ONE || x == n - ONE) continue;
        bool passed = false;
        for (int j = 0; j < r - 1; ++j) {
            x = x.powmod(TWO, n);
            if (x == n - ONE) {
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
    e = BigNumber(65537);
    std::thread t1(generatePrimeCandidate, bits, std::ref(global_p), std::ref(found_p));
    std::thread t2(generatePrimeCandidate, bits, std::ref(global_q), std::ref(found_q));

    t1.join();
    t2.join();

    while (global_q == global_p) {
        found_q.store(false);
        generatePrimeCandidate(bits, global_q, found_q);
    }

    BigNumber p = global_p;
    BigNumber q = global_q;

    n = p * q;
    BigNumber phi = (p - BigNumber(1)) * (q - BigNumber(1));
    while (phi % e == BigNumber(0)) {
        e = e + BigNumber(2);
    }

    d = e.modinv(phi);
}