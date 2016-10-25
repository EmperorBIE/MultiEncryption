#include "playfair.h"
#include <set>
#include <ctype.h>
#include <algorithm>
#include <iostream>
#include <iterator>
using namespace std;

Playfair::Playfair(const std::string& keyword)
{
    setKeyword(keyword);
}

bool Playfair::fillMatrixAndIsFull(char ch, std::size_t& row,
                                  std::size_t& col)
{
    // we don't insert 'J', wherever replacing 'J' with 'I'
    if(ch == 'J')
        return false;

    alphaMatrix[row][col] = ch;
    alphaPosMap.insert(make_pair(ch,
                                 make_pair(row, col)));
    if(++col == RANK) {
        col = 0;
        if(++row == RANK)
            return true;
    }
    return false;
}

void Playfair::setKeyword(const std::string& keyword)
{
    // filter nonalphabetic characters
    std::string key;
    key.reserve(keyword.size());
    auto keyBInserter = back_inserter(key);
    copy_if(keyword.begin(), keyword.end(), keyBInserter,
                isalpha);

    set<char> alphaSet;
    size_t i = 0, j = 0;

    alphaPosMap.clear();

    // firstly insert keyword to alpha matrix
    for(string::const_iterator keyIter = key.begin();
            keyIter != key.end(); keyIter++) {
        char ch = toupper(*keyIter);
        if(alphaSet.find(ch) == alphaSet.end()) {
            if(fillMatrixAndIsFull(ch, i, j))
                break;
            alphaSet.insert(ch);
        }
    }

    // secondly fill the alpha matrix with remaining alphas
    for(char ch = 'A'; ch != 'Z' + 1; ch++) {
        if(alphaSet.find(ch) == alphaSet.end()) {
            if(fillMatrixAndIsFull(ch, i, j))
                break;
            alphaSet.insert(ch);
        }
    }

    // last, to speed up the encryption, we fill the edge of alpha matrix
    for(size_t k = 0; k < RANK; k++)
        alphaMatrix[RANK][k] = alphaMatrix[0][k];
    for(size_t k = 0; k < RANK; k++)
        alphaMatrix[k][RANK] = alphaMatrix[k][0];

    alphaMatrix[RANK][RANK] = ' ';
}


const std::pair<size_t, size_t> Playfair::getRowAndCol(char ch) const
{
    if('J' == ch) ch = 'I';

    map<char, pair<size_t, size_t> >::const_iterator
            mapIter = alphaPosMap.find(ch);
    if(mapIter == alphaPosMap.end())
        return make_pair(RANK, RANK);
    return mapIter->second;
}

void Playfair::doEncrypt(char& ch1, char& ch2) const
{
    const pair<size_t, size_t> pair1 = getRowAndCol(ch1),
                         pair2 = getRowAndCol(ch2);
    if(pair1.first == pair2.first) {// ch1 & ch2 in the same row
        ch1 = alphaMatrix[pair1.first][pair1.second + 1];
        ch2 = alphaMatrix[pair2.first][pair2.second + 1];
    }else if(pair1.second == pair2.second) {// in the same col
        ch1 = alphaMatrix[pair1.first + 1][pair1.second];
        ch2 = alphaMatrix[pair2.first + 1][pair2.second];
    }else { // not in the same row or col
        ch1 = alphaMatrix[pair1.first][pair2.second];
        ch2 = alphaMatrix[pair2.first][pair1.second];
    }
}

void Playfair::doDecrypt(char& ch1, char& ch2) const
{
    const pair<size_t, size_t> pair1 = getRowAndCol(ch1),
                         pair2 = getRowAndCol(ch2);
    if(pair1.first == pair2.first) {// ch1 & ch2 in the same row
        ch1 = alphaMatrix[pair1.first][(RANK+pair1.second-1)%RANK];
        ch2 = alphaMatrix[pair2.first][(RANK+pair2.second-1)%RANK];
    }else if(pair1.second == pair2.second) {// in the same col
        ch1 = alphaMatrix[(RANK+pair1.first-1)%RANK][pair1.second];
        ch2 = alphaMatrix[(RANK+pair2.first-1)%RANK][pair2.second];
    }else { // not in the same row or col
        ch1 = alphaMatrix[pair1.first][pair2.second];
        ch2 = alphaMatrix[pair2.first][pair1.second];
    }
}

const std::string Playfair::encrypt(
                const std::string& plainText) const
{
    if(plainText.empty()) return "";

    string cipherText;
    cipherText.reserve(plainText.size() * 2);

    string::const_iterator pIter = plainText.begin(),
                           pIterEnd = plainText.end();
    auto cipherBIter = back_inserter(cipherText);

    while(pIter != pIterEnd) {
        char ch1 = toupper(*pIter++);
        char ch2;
        bool hasInsertAlpha = false;

        // skip nonalphabetic characters
        if(!isalpha(ch1)) {
            *cipherBIter++ = ch1;
            continue;
        }

        string::const_iterator tmpIter = pIter;
        while(tmpIter != pIterEnd) {
            if(isalpha(*tmpIter)) {
                ch2 = toupper(*tmpIter);
                // nearby alpha is same, so we need to insert a filled
                // alpha in the middle of then
                if(ch1 == ch2)
                    ch2 = (ch1 == 'K') ? 'Z' : 'K' ;
                break;
            }
            tmpIter++;
        }
        // less two alpha, so we need append a filled alpha
        if(tmpIter == pIterEnd) {
            hasInsertAlpha = true;
            ch2 = (ch1 == 'K') ? 'Z' : 'K' ;
        }

        doEncrypt(ch1, ch2);

        *cipherBIter++ = ch1;
        while(pIter != pIterEnd) {
            if(isalpha(*pIter)) {
                *cipherBIter++ = ch2;
                pIter++;
                break;
            }
            *cipherBIter++ = *pIter++;
        }
        if(hasInsertAlpha)
            *cipherBIter++ = ch2;
    }
    return cipherText;
}

const std::string Playfair::decrypt(
                const std::string& cipherText) const
{
    if(cipherText.empty()) return "";

    string plainText;
    plainText.reserve(plainText.size());

    string::const_iterator cipherIter = cipherText.begin(),
                           cipherIterEnd = cipherText.end();
    auto plainBInserter = back_inserter(plainText);

    while(cipherIter != cipherIterEnd) {
        char ch1 = toupper(*cipherIter++);
        char ch2 = 'K';
        bool hasFillAlpha = false;

        // skip nonalphabetic characters
        if(!isalpha(ch1)) {
            *plainBInserter++ = ch1;
            continue;
        }

        string::const_iterator tmpIter = cipherIter;
        while(tmpIter != cipherIterEnd) {
            if(isalpha(*tmpIter)) {
                ch2 = toupper(*tmpIter);
                break;
            }
            tmpIter++;
        }
        if(tmpIter == cipherIterEnd)
            hasFillAlpha = true;

        doDecrypt(ch1, ch2);

        *plainBInserter++ = ch1;
        while(cipherIter != cipherIterEnd) {
            if(isalpha(*cipherIter)) {
                *plainBInserter++ = ch2;
                cipherIter++;
                break;
            }
            *plainBInserter++ = *cipherIter++;
        }
        if(hasFillAlpha)
            *plainBInserter++ = ch2;
    }
    return plainText;
}

std::ostream& operator<<(std::ostream& os,
                         const Playfair& p)
{
    for(size_t row = 0; row < Playfair::RANK+1; row++) {
        for(size_t col = 0; col < Playfair::RANK+1; col++) {
            char ch = p.alphaMatrix[row][col];
            if(ch == 'I')
                os << "I/J ";
            else
                os << ch << "   ";
        }
        os << endl;
        if(Playfair::RANK-1 == row)
            os << endl;
    }
    return os;
}
