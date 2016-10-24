#ifndef PLAYFAIR_H
#define PLAYFAIR_H

#include <string>
#include <utility>
#include <iosfwd>
#include <sstream>
#include "encryptable.h"
#include <map>

class Playfair : public Encryptable {
public:
    static const std::size_t RANK = 5;
    explicit Playfair(const std::string& keyword = "monarchy");

    void setKeyword(const std::string& keyword);

    friend std::ostream& operator<<(std::ostream& os,
                                    const Playfair& p);
    const std::string toString() const
    {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }

    const std::string encrypt(const std::string&) const;
    const std::string decrypt(const std::string&) const;

private:
    char alphaMatrix[RANK+1][RANK+1];
    std::map<char, std::pair<std::size_t, std::size_t> >
            alphaPosMap;

    bool fillMatrixAndIsFull(char ch, std::size_t& row,
                                      std::size_t& col);
    const std::pair<std::size_t, std::size_t> getRowAndCol(
                         char ch) const;

    void doEncrypt(char& ch1, char& ch2) const;
    void doDecrypt(char& ch1, char& ch2) const;
};

#endif // PLAYFAIR_H
