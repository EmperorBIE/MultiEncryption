#include "playfair.h"
#include <set>
#include <ctype.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <qDebug>
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
    set<char> alphaSet;
    size_t i = 0, j = 0;

    alphaPosMap.clear();

    // firstly insert keyword to alpha matrix
    for(string::const_iterator keyIter = keyword.begin();
            keyIter != keyword.end(); keyIter++) {
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

    // last, to speed up the encryption, we fill the edge
    // of alpha matrix
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
    qDebug() << ch << '[' << mapIter->second.first
             << ',' << mapIter->second.second << ']';
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

    string::const_iterator pIter = plainText.begin();
    auto cipherBIter = back_inserter(cipherText);

    while(pIter != plainText.end()) {
        char ch1 = toupper(*pIter);
        char ch2 = (ch1 == 'K') ? 'Z' : 'K';

        // skip nonalpha char
        if(!isalpha(ch1)) {
            *cipherBIter++ = ch1;
            pIter++;
            continue;
        }

        string symbols;
        symbols.reserve(5);

        bool hasInnerSymbol = false;

        if((pIter+1) != plainText.end()) { // has next one
            if(!isalpha(*(pIter+1))) {// the second one isn't alpha
                pIter++;
                while(pIter != plainText.end() &&
                        !isalpha(*pIter)) {
                    symbols += *pIter;
                    pIter++;
                }
                // If finally we get a unique alpha,
                // replace ch2 with it's upper
                if(pIter != plainText.end()) {
                    if(toupper(*pIter) != ch1) {
                        ch2 = toupper(*pIter);
                        pIter++;
                    }
                }
                hasInnerSymbol = true;
            }else if(*pIter != *(pIter+1)) {// both are unique alpha
                ch2 = toupper(*(pIter+1));
                pIter += 2;
            }else { // both are the same alpha
                pIter++;
            }
        }else {
            pIter++;
        }
        // actually encryption work in here
        doEncrypt(ch1, ch2);

        *cipherBIter++ = ch1;
        // we append those symbols back
        if(hasInnerSymbol) {
            copy(symbols.begin(), symbols.end(), cipherBIter);
        }
        *cipherBIter++ = ch2;
    }
    return cipherText;
}

const std::string Playfair::decrypt(
                const std::string& cipherText) const
{
    if(cipherText.empty()) return "";

    string plainText;
    plainText.reserve(cipherText.size() * 2);

    string::const_iterator cIter = cipherText.begin();
    auto plainBIter = back_inserter(plainText);

    while(cIter != cipherText.end()) {
        char ch1 = toupper(*cIter);
        char ch2 = ' ';

        // skip nonalpha char
        if(!isalpha(ch1)) {
            *plainBIter++ = ch1;
            cIter++;
            continue;
        }

        string symbols;
        symbols.reserve(5);

        bool hasInnerSymbol = false;

        if((cIter+1) != cipherText.end()) { // has next one
            if(!isalpha(*(cIter+1))) {// the second one isn't alpha
                cIter++;
                while(cIter != cipherText.end() &&
                        !isalpha(*cIter)) {
                    symbols += *cIter;
                    cIter++;
                }
                // finally we get a unique alpha
                if(cIter != cipherText.end()) {
                    ch2 = toupper(*cIter);
                    cIter++;
                }
                hasInnerSymbol = true;
            }else {
                ch2 = toupper(*(cIter+1));
                cIter += 2;
            }
        }else { // it shouldn't goes to here
            cIter++;
            qDebug() << "Playfair::decrypt: Error: "
                "number of alpha in cipher text is not even";
        }
        // actually decryption work in here
        doDecrypt(ch1, ch2);

        *plainBIter++ = ch1;
        // we append those symbols back
        if(hasInnerSymbol) {
            copy(symbols.begin(), symbols.end(), plainBIter);
        }

        // check whether ch2 is filled character

        if(cIter == cipherText.end()) { // check tail
            if( (ch1 == 'K' && ch2 == 'Z') ||
                (ch1 != 'K' && ch2 == 'K') )
                break; // drop ch2
            else
                *plainBIter++ = ch2;
        }else {
            *plainBIter++ = ch2;
        }
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
