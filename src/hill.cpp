#include "hill.h"
#include <algorithm>
#include <iostream>
using namespace std;

Hill::Hill() : rank(3), keyMatrix(new int*[3])
{
    for(size_t i = 0; i < rank; i++)
        keyMatrix[i] = new int[rank];
    int keyArray[9] =
                   {17, 17, 5,
                    21, 18, 21,
                    2,  2,  19};
    for(size_t row = 0; row < rank; row++) {
        for(size_t col = 0; col < rank; col++) {
            keyMatrix[row][col] = keyArray[row*rank + col];
        }
    }
}

Hill::~Hill()
{
    for(size_t i = 0; i < rank; i++)
        delete [] keyMatrix[i];
    delete [] keyMatrix;
}

std::ostream& operator<<(std::ostream& os,
                         const Hill& hill)
{
    for(size_t row = 0; row < hill.rank; row++) {
        for(size_t col = 0; col < hill.rank; col++) {
            os << hill.keyMatrix[row][col] << '\t';
        }
        os << endl;
    }
    return os;
}

const std::string Hill::encrypt(
                const std::string& plainText) const
{
    return plainText;
}

const std::string Hill::decrypt(
                const std::string& cipherText) const
{
    return cipherText;
}

