#ifndef GENERATE_KEY_H
#define GENERATE_KEY_H

#include "BigNumber.h"

BigNumber generateRandomOddBigNumber(int bits);
BigNumber generateRandomOddBigNumber_optimization(int bits);
bool isProbablyPrime(const BigNumber& n, int k = 3);
bool isProbablyPrime_optimization(const BigNumber& n, int k = 5);
void generateRSAKeyPair(int bits, BigNumber& e, BigNumber& d, BigNumber& n);
void generateRSAKeyPair_optimization(int bits, BigNumber& e, BigNumber& d, BigNumber& n);

#endif // GENERATE_KEY_H
