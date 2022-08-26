#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string.h>
#include <map>
#include <cassert> 
#include <algorithm>
#include <filesystem>
#include <bitset>

using namespace std;

void multiplication()
{
    unsigned int multiplicand, multiplier;

    cout << "Enter multiplicand: ";
    cin >> multiplicand;
    cout << "Binary form: " << bitset<32>(multiplicand) << endl;

    cout << "Enter multiplier: ";
    cin >> multiplier;
    cout << "Binary form: " << bitset<32>(multiplier) << endl;

    unsigned long long result = multiplier;

    for (size_t i = 0; i < 32; i++)
    {
        cout << "Step " << (i + 1) << ":  \t result register: " << bitset<64>(result) << endl;

        if (result & 1)
        {
            result += static_cast<unsigned long long>(multiplicand) << 32LL;
        }
        result >>= 1;
    }

    cout << "Result: " << result << " (" << bitset<64>(result) << ")" << endl;
}

void division()
{
    unsigned int dividend;
    unsigned long long divisor;

    cout << "Enter dividend: ";
    cin >> dividend;
    cout << "Binary form: " << bitset<32>(dividend) << endl;

    cout << "Enter divisor: ";
    cin >> divisor;
    cout << "Binary form: " << bitset<32>(divisor) << endl;

    divisor <<= 32;

    unsigned int remainder = dividend;
    unsigned int quotient = 0;

    for (size_t i = 0; i < 32; i++)
    {
        cout << "Step " << (i + 1) << ":" << endl;
        cout << "    remainder:   " << string(32, ' ')  << bitset<32>(remainder) << endl;
        cout << "    divisor:     " << bitset<64>(divisor) << endl;
        cout << "    quotient:    " << string(32, ' ') << bitset<32>(quotient) << endl;

        quotient <<= 1;
        divisor >>= 1;
        if (divisor <= remainder)
        {
            quotient += 1;
            remainder -= divisor;
        }
    }

    cout << "Result: quotient = " << quotient << ", remainder = " << remainder << endl;
}

void fpMultiplication()
{
    float multiplicand, multiplier;

    cout << "Enter multiplicand: ";
    cin >> multiplicand;
    unsigned int multiplicandBin = *reinterpret_cast<unsigned int*>(&multiplicand);
    cout << "Binary form: " << bitset<32>(multiplicandBin) << endl;

    cout << "Enter multiplier: ";
    cin >> multiplier;
    unsigned int multiplierBin = *reinterpret_cast<unsigned int*>(&multiplier);
    cout << "Binary form: " << bitset<32>(multiplierBin) << endl;

    float realResult = 0;
    unsigned int tempResult = 0;

    // Check if one / both operands = 0 or infinity
    if (multiplicand == 0 || multiplier == 0)
    {
        cout << "Result: " << 0 << endl;
        return;
    }
    else if (isinf(multiplier) || isinf(multiplier))
    {
        cout << "Result: " << multiplier << endl;
        return;
    }

    // sign
    tempResult |= (multiplicandBin & (1LL << 31)) ^ (multiplierBin & (1LL << 31));

    // fracs
    unsigned int m1 = multiplicandBin & ((1 << 23) - 1);
    unsigned int m2 = multiplierBin   & ((1 << 23) - 1);

    // Add hidden ones
    m1 |= 1 << 23;
    m2 |= 1 << 23;

    unsigned long long mResult = static_cast<unsigned long long>(m1) * m2;

    bool shouldAddToExponent = mResult & (1LL << 47);

    // trunc & normalize
    mResult >>= 23;
    if (shouldAddToExponent)
    {
        mResult >>= 1;
    }
    mResult &= (1LL << 23) - 1;
    
    const unsigned int exponentMask = ((1LL << 31) - 1) - (((1LL << 23) - 1) - 1);

    // compute the exponent
    unsigned int exp1 = (multiplicandBin & exponentMask) >> 23;
    unsigned int exp2 = (multiplierBin & exponentMask) >> 23;

    unsigned int newExponent = exp1 + exp2 - 127 + (shouldAddToExponent ? 1 : 0);

    // construct the float
    tempResult |= mResult;
    tempResult |= newExponent << 23;

    realResult = *reinterpret_cast<float*>(&tempResult);

    cout << "Result: " << realResult << endl;
}

int main(int argc, char** argv) {
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1000);

    cout << "Enter operation (1 - multiplication, 2 - division, 3 - floating point multiplication): ";
    int operation;
    cin >> operation;

    switch (operation)
    {
    case 1:
        multiplication();
        break;
    case 2:
        division();
        break;
    case 3:
        fpMultiplication();
        break;
    default:
        cout << "Wrong operation, terminating..." << endl;
        break;
    }

    system("pause");

    return 0;
}