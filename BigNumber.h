#ifndef BIGNUMBER_H
#define BIGNUMBER_H

#include <iostream>
#include <vector>
#include <string>

class BigNumber {
public:
    BigNumber();
    BigNumber(const std::string& str);
    BigNumber(int val);

    BigNumber operator+(const BigNumber& other) const;
    BigNumber operator-(const BigNumber& other) const;
    BigNumber operator*(const BigNumber& other) const;
    BigNumber operator/(const BigNumber& other) const;
    BigNumber operator%(const BigNumber& mod) const;
    bool operator==(const BigNumber& other) const;
    bool operator!=(const BigNumber& other) const { return !(*this == other); }
    bool operator<(const BigNumber& other) const;
    bool operator>(const BigNumber& other) const;
    bool operator<=(const BigNumber& other) const;
    bool operator>=(const BigNumber& other) const;
    BigNumber shiftLeft( int m) const;

    BigNumber powmod(const BigNumber& exponent, const BigNumber& mod) const;
    BigNumber modinv(const BigNumber& mod) const;

    std::string toString() const;

private:
    std::vector<char> digits;
    bool isNegative;

    void removeLeadingZeros();

    static int absCompare(const BigNumber& a, const BigNumber& b);
    static BigNumber absAdd(const BigNumber& a, const BigNumber& b);
    static BigNumber absSubtract(const BigNumber& a, const BigNumber& b);
    static BigNumber divide(const BigNumber& dividend, const BigNumber& divisor, BigNumber& remainder);
};

#endif // BIGNUMBER_H
