#include <iostream>
#include <chrono>
#include "GenerateKey.h"

int main() {
    const int bits = 768;
    BigNumber e, d, n;

    auto start = std::chrono::steady_clock::now();

    generateRSAKeyPair_optimization(bits, e, d, n);

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    std::cout << "Generated RSA " << bits << "-bit key pair in " << elapsed_seconds.count() << " seconds." << std::endl;
    return 0;
}
