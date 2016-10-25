#include "hill.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <cctype>
#include <QDebug>

using namespace std;

Hill::Hill() : rank(3), keyMatrix(new int*[3]),
               inverseKeyMatrix(new int*[3])
{
    for(size_t i = 0; i < rank; i++) {
        keyMatrix[i] = new int[rank];
        inverseKeyMatrix[i] = new int[rank];
    }
    int keyArray[9] =
                   {17, 17, 5,
                    21, 18, 21,
                    2,  2,  19};
    int inverseKeyArray[9] =
                   {4,  9,  15,
                    15, 17, 6,
                    24, 0, 17};

    for(size_t row = 0; row < rank; row++) {
        for(size_t col = 0; col < rank; col++) {
            keyMatrix[row][col] = keyArray[row*rank + col];
            inverseKeyMatrix[row][col] =
                    inverseKeyArray[row*rank + col];
        }
    }
}

Hill::~Hill()
{
    for(size_t i = 0; i < rank; i++) {
        delete [] keyMatrix[i];
        delete [] inverseKeyMatrix[i];
    }
    delete [] keyMatrix;
    delete [] inverseKeyMatrix;
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

namespace {
void SquareMatrixMulToColumnVector(
        size_t rank,
        const int * const * const squareMatrix, // squareMatrix[rank][rank]
        const int * const rightMatrix,   // rightMatrix[rank][1]
        int * const resultMatrix)  // resultMatrix[rank][1]
{
    for(size_t row = 0; row < rank; row++) {
        int sum = 0;
        for(size_t col = 0; col < rank; col++)
            sum += squareMatrix[row][col] * rightMatrix[col];
        resultMatrix[row] = sum;
    }
}
}


const std::string Hill::encrypt(
                const std::string& plainText) const
{
    if(plainText.empty()) return "";

    string cipherText;
    cipherText.reserve(plainText.size());

    string::const_iterator pIter = plainText.begin(),
                           pIterEnd = plainText.end();
    auto cipherBInserter = back_inserter(cipherText);

    int *plainAlphaGroup = new int[rank];
    int *cipherAlphaGroup = new int[rank];

    while(pIter != pIterEnd) {
        // collect alphas
        string::const_iterator tmpIter = pIter;
        size_t cnt = 0;
        while(tmpIter != pIterEnd && cnt < rank) {
            if(isalpha(*tmpIter)) {
                plainAlphaGroup[cnt] = toupper(*tmpIter) - 'A';
                cnt++;
            }
            tmpIter++;
        }
        // if not have enough alpha to encrypt, we must add filled alpha
        while(cnt < rank) {
            plainAlphaGroup[cnt] = 'X' - 'A';
            cnt++;
        }

        stringstream ss;
        for(size_t i = 0; i < rank; i++)
            ss << plainAlphaGroup[i] << " ";
        qDebug() << "Before encrypt: (" << ss.str().c_str() << ')';

        // encrypt alpha
        SquareMatrixMulToColumnVector(rank, keyMatrix,
                                      plainAlphaGroup,
                                      cipherAlphaGroup);

        // insert encrypted characters to cipher text
        for(cnt = 0; pIter != pIterEnd && cnt < rank; pIter++) {
            if(isalpha(*pIter)) {
                *cipherBInserter++ = cipherAlphaGroup[cnt]%26 + 'A';
                cnt++;
            }else {
                *cipherBInserter++ = *pIter;
            }
        }
        // insert the filled alpha
        while(cnt < rank) {
            *cipherBInserter++ = cipherAlphaGroup[cnt]%26 + 'A';
            cnt++;
        }
    }
    delete plainAlphaGroup;
    delete cipherAlphaGroup;
    return cipherText;
}

const std::string Hill::decrypt(
                const std::string& cipherText) const
{
    if(cipherText.empty()) return "";

    string plainText;
    plainText.reserve(cipherText.size());

    string::const_iterator cipherIter = cipherText.begin(),
                           cipherIterEnd = cipherText.end();
    auto plainBInserter = back_inserter(plainText);

    int *cipherAlphaGroup = new int[rank];
    int *plainAlphaGroup = new int[rank];

    while(cipherIter != cipherIterEnd) {
        // collect alphas
        string::const_iterator tmpIter = cipherIter;
        size_t cnt = 0;
        while(tmpIter != cipherIterEnd && cnt < rank) {
            if(isalpha(*tmpIter)) {
                cipherAlphaGroup[cnt] = toupper(*tmpIter) - 'A';
                cnt++;
            }
            tmpIter++;
        }

        // decrypt alpha
        SquareMatrixMulToColumnVector(rank, inverseKeyMatrix,
                                      cipherAlphaGroup,
                                      plainAlphaGroup);

        // write decrypted characters to plain text
        for(cnt = 0; cipherIter != cipherIterEnd && cnt < rank;
                            cipherIter++) {
            if(isalpha(*cipherIter)) {
                *plainBInserter++ = tolower(
                            plainAlphaGroup[cnt]%26 + 'A');
                cnt++;
            }else {
                *plainBInserter++ = *cipherIter;
            }
        }
    }
    delete plainAlphaGroup;
    delete cipherAlphaGroup;
    return plainText;
}
