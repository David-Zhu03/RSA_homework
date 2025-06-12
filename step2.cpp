#include <iostream>
#include "GenerateKey.h"

void printBigNumber(const std::string& label, const BigNumber& bn) {
    std::cout << label << ": " << bn.toString() << std::endl;
}

int main() {
    int bitSizes[] = {512, 768, 1024, 2048};

    for (int bits : bitSizes) {
        std::cout << "=== Generating RSA key pair with " << bits << " bits ===" << std::endl;

        BigNumber e, d, n;
        generateRSAKeyPair(bits, e, d, n);

        printBigNumber("Public exponent e", e);
        printBigNumber("Private exponent d", d);
        printBigNumber("Modulus n", n);
        std::cout << std::endl;
    }

    return 0;
}