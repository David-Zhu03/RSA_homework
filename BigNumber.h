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
    void rightShift1();
    BigNumber rightShiftDecimal(int n) const;

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
    static BigNumber absMultiply(const BigNumber& a, const BigNumber& b);
};

struct BarrettReducer {
    BigNumber modulus;
    BigNumber mu;

    BarrettReducer(const BigNumber& m) : modulus(m) {
        int k = m.toString().length();
        BigNumber b2k("1");
        for (int i = 0; i < 2 * k; ++i) {
            b2k = b2k * BigNumber(10);
        }
        mu = b2k / modulus;
    }

    BigNumber reduce(const BigNumber& x) const {
        int k = modulus.toString().length();
        int shift1 = k - 1;
        int shift2 = k + 1;

        BigNumber q1 = x.rightShiftDecimal(shift1);
        BigNumber q2 = q1 * mu;
        BigNumber q3 = q2.rightShiftDecimal(shift2);

        BigNumber r = x - q3 * modulus;

        while (r < BigNumber(0)) r = r + modulus;
        while (r >= modulus) r = r - modulus;

        return r;
    }
};
#endif // BIGNUMBER_H
