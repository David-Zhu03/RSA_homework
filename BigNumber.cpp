#include "BigNumber.h"

BigNumber::BigNumber() : isNegative(false) {
    digits.push_back(0);
}

BigNumber::BigNumber(int val) {
    isNegative = val < 0;
    if (val == 0) {
        digits.push_back(0);
        isNegative = false;
    } else {
        val = std::abs(val);
        while (val > 0) {
            digits.push_back(val % 10);
            val /= 10;
        }
    }
}

BigNumber::BigNumber(const std::string& str) {
    isNegative = false;
    digits.clear();
    int i = 0;

    if (str.empty()) {
        digits.push_back(0);
        return;
    }

    if (str[0] == '-') {
        isNegative = true;
        i = 1;
    }

    for (int j = str.size() - 1; j >= i; --j) {
        if (isdigit(str[j])) {
            digits.push_back(str[j] - '0');
        } else {
            throw std::invalid_argument("Invalid digit in string.");
        }
    }

    removeLeadingZeros();

    if (digits.size() == 1 && digits[0] == 0) {
        isNegative = false;
    }
}

std::string BigNumber::toString() const {
    std::string res;
    if (isNegative) res += "-";
    for (int i = digits.size() - 1; i >= 0; --i) {
        res += (digits[i] + '0');
    }
    return res;
}

void BigNumber::removeLeadingZeros() {
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
}

int BigNumber::absCompare(const BigNumber& a, const BigNumber& b) {
    if (a.digits.size() != b.digits.size())
        return (a.digits.size() < b.digits.size()) ? -1 : 1;
    for (int i = a.digits.size() - 1; i >= 0; --i) {
        if (a.digits[i] != b.digits[i])
            return (a.digits[i] < b.digits[i]) ? -1 : 1;
    }
    return 0;
}

BigNumber BigNumber::absAdd(const BigNumber& a, const BigNumber& b) {
    BigNumber result;
    result.digits.clear();

    int carry = 0, sum = 0;
    size_t maxLen = std::max(a.digits.size(), b.digits.size());

    for (size_t i = 0; i < maxLen || carry; ++i) {
        int digitA = i < a.digits.size() ? a.digits[i] : 0;
        int digitB = i < b.digits.size() ? b.digits[i] : 0;
        sum = digitA + digitB + carry;
        result.digits.push_back(sum % 10);
        carry = sum / 10;
    }

    return result;
}

BigNumber BigNumber::absSubtract(const BigNumber& a, const BigNumber& b) {
    BigNumber result;
    result.digits.clear();

    int borrow = 0;
    for (size_t i = 0; i < a.digits.size(); ++i) {
        int digitA = a.digits[i];
        int digitB = i < b.digits.size() ? b.digits[i] : 0;
        int sub = digitA - digitB - borrow;
        if (sub < 0) {
            sub += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result.digits.push_back(sub);
    }

    result.removeLeadingZeros();
    return result;
}

BigNumber BigNumber::operator+(const BigNumber& other) const {
    BigNumber result;

    if (!isNegative && !other.isNegative) {
        result = absAdd(*this, other);
        result.isNegative = false;
    } else if (isNegative && other.isNegative) {
        result = absAdd(*this, other);
        result.isNegative = true;
    } else {
        int cmp = absCompare(*this, other);
        if (cmp == 0) {
            return BigNumber(0);
        } else if (cmp > 0) {
            result = absSubtract(*this, other);
            result.isNegative = isNegative;
        } else {
            result = absSubtract(other, *this);
            result.isNegative = other.isNegative;
        }
    }

    return result;
}

BigNumber BigNumber::operator-(const BigNumber& other) const {
    BigNumber negOther = other;
    negOther.isNegative = !other.isNegative;
    return *this + negOther;
}

BigNumber BigNumber::operator*(const BigNumber& other) const {
    BigNumber result = absMultiply(*this, other);
    result.isNegative = (isNegative != other.isNegative);

    if (result.digits.size() == 1 && result.digits[0] == 0)
        result.isNegative = false;

    return result;
}

BigNumber BigNumber::divide(const BigNumber& dividend, const BigNumber& divisor, BigNumber& remainder) {
    if (divisor == BigNumber(0)) {
        throw std::invalid_argument("Division by zero");
    }

    BigNumber result;
    result.digits.resize(dividend.digits.size(), 0);
    result.isNegative = (dividend.isNegative != divisor.isNegative);
    remainder = BigNumber(0);

    for (int i = dividend.digits.size() - 1; i >= 0; --i) {
        remainder.digits.insert(remainder.digits.begin(), dividend.digits[i]);
        remainder.removeLeadingZeros();

        int x = 0, low = 0, high = 9;
        while (low <= high) {
            int mid = (low + high) / 2;
            BigNumber midVal = divisor * BigNumber(mid);
            if (midVal <= remainder) {
                x = mid;
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }

        result.digits[i] = x;
        remainder = remainder - divisor * BigNumber(x);
    }

    result.removeLeadingZeros();
    remainder.removeLeadingZeros();

    if (result.digits.size() == 1 && result.digits[0] == 0)
        result.isNegative = false;
    if (remainder.digits.size() == 1 && remainder.digits[0] == 0)
        remainder.isNegative = false;

    return result;
}

BigNumber BigNumber::operator/(const BigNumber& other) const {
    BigNumber remainder;
    return divide(*this, other, remainder);
}

BigNumber BigNumber::operator%(const BigNumber& other) const {
    BigNumber remainder;
    divide(*this, other, remainder);
    if (remainder.isNegative) {
        remainder = remainder + other;
    }
    return remainder;
}

BigNumber BigNumber::powmod(const BigNumber& exponent, const BigNumber& modulus) const {
    if (modulus == BigNumber(0))
        throw std::invalid_argument("Modulo by zero");

    BarrettReducer reducer(modulus);

    BigNumber base = reducer.reduce(*this);
    BigNumber exp = exponent;
    BigNumber result(1);

    while (exp > BigNumber(0)) {
        if (exp.digits[0] % 2 == 1) {
            result = reducer.reduce(result * base);
        }
        base = reducer.reduce(base * base);
        exp.rightShift1();
    }

    return result;
}

BigNumber BigNumber::modinv(const BigNumber& modulus) const {
    BigNumber a = *this, m = modulus;
    BigNumber m0 = m;
    BigNumber y(0), x(1);

    if (m == BigNumber(0))
        throw std::invalid_argument("Modulo by zero");

    while (a > BigNumber(1)) {
        BigNumber q = a / m;
        BigNumber t = m;

        m = a % m;
        a = t;
        t = y;

        y = x - q * y;
        x = t;
    }

    if (x < BigNumber(0))
        x = x + m0;

    return x;
}

bool BigNumber::operator==(const BigNumber& other) const {
    return isNegative == other.isNegative && digits == other.digits;
}

bool BigNumber::operator<(const BigNumber& other) const {
    if (isNegative != other.isNegative)
        return isNegative;

    if (digits.size() != other.digits.size())
        return isNegative ? digits.size() > other.digits.size() : digits.size() < other.digits.size();

    for (int i = digits.size() - 1; i >= 0; --i) {
        if (digits[i] != other.digits[i])
            return isNegative ? digits[i] > other.digits[i] : digits[i] < other.digits[i];
    }
    return false;
}

bool BigNumber::operator<=(const BigNumber& other) const {
    return *this < other || *this == other;
}

bool BigNumber::operator>(const BigNumber& other) const {
    return !(*this <= other);
}

bool BigNumber::operator>=(const BigNumber& other) const {
    return !(*this < other);
}

BigNumber BigNumber::shiftLeft(int m) const {
    if (*this == BigNumber(0)) return BigNumber(0);
    BigNumber result(*this);
    result.digits.insert(result.digits.begin(), m, 0);
    return result;
}

void BigNumber::rightShift1() {
    int carry = 0;
    for (int i = digits.size() - 1; i >= 0; --i) {
        int current = digits[i] + carry * 10;
        digits[i] = current / 2;
        carry = current % 2;
    }
    removeLeadingZeros();
}

// BigNumber.cpp 中实现：
BigNumber BigNumber::rightShiftDecimal(int n) const {
    if (n >= digits.size()) return BigNumber(0);
    BigNumber result;
    result.digits.assign(digits.begin() + n, digits.end());
    result.isNegative = isNegative;
    result.removeLeadingZeros();
    return result;
}

BigNumber BigNumber::absMultiply(const BigNumber& a, const BigNumber& b) {
    if (a.digits.size() < 32 || b.digits.size() < 32) {
        BigNumber result;
        size_t n = a.digits.size();
        size_t m = b.digits.size();
        result.digits.assign(n + m, 0);

        for (size_t i = 0; i < n; ++i) {
            int carry = 0;
            for (size_t j = 0; j < m || carry; ++j) {
                int digitA = a.digits[i];
                int digitB = (j < m ? b.digits[j] : 0);
                int64_t mul = result.digits[i + j] + digitA * digitB + carry;
                result.digits[i + j] = mul % 10;
                carry = mul / 10;
            }
        }

        result.removeLeadingZeros();
        return result;
    }

    size_t n = std::max(a.digits.size(), b.digits.size());
    size_t m = n / 2;

    BigNumber a0, a1, b0, b1;
    a0.digits.assign(a.digits.begin(), a.digits.begin() + std::min(a.digits.size(), m));
    a1.digits.assign(a.digits.begin() + std::min(a.digits.size(), m), a.digits.end());
    b0.digits.assign(b.digits.begin(), b.digits.begin() + std::min(b.digits.size(), m));
    b1.digits.assign(b.digits.begin() + std::min(b.digits.size(), m), b.digits.end());

    a0.removeLeadingZeros();
    a1.removeLeadingZeros();
    b0.removeLeadingZeros();
    b1.removeLeadingZeros();

    BigNumber z0 = absMultiply(a0, b0);
    BigNumber z1 = absMultiply(a1, b1);
    BigNumber z2 = absMultiply(a0 + a1, b0 + b1) - z0 - z1;

    BigNumber result;
    result.digits = std::vector<char>(2 * m + z1.digits.size(), 0);
    for (size_t i = 0; i < z1.digits.size(); ++i)
        result.digits[i + 2 * m] = z1.digits[i];

    for (size_t i = 0; i < z2.digits.size(); ++i) {
        if (i + m >= result.digits.size())
            result.digits.resize(i + m + 1, 0);
        result.digits[i + m] += z2.digits[i];
    }

    for (size_t i = 0; i < z0.digits.size(); ++i) {
        if (i >= result.digits.size())
            result.digits.resize(i + 1, 0);
        result.digits[i] += z0.digits[i];
    }

    int carry = 0;
    for (size_t i = 0; i < result.digits.size(); ++i) {
        int sum = result.digits[i] + carry;
        result.digits[i] = sum % 10;
        carry = sum / 10;
    }
    if (carry) result.digits.push_back(carry);

    result.removeLeadingZeros();
    return result;
}