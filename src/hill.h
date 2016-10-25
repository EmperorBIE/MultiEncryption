#ifndef HILL_H
#define HILL_H

#include "encryptable.h"
#include <string>
#include <iosfwd>
#include <sstream>

class Hill : public Encryptable {
public:
    Hill();
    ~Hill();

    void setKeyword(const std::string& keyword) { keyword;}

    const std::string encrypt(const std::string&) const;
    const std::string decrypt(const std::string&) const;

    const std::string toString() const
    {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }

    friend std::ostream& operator<<(std::ostream&, const Hill&);

private:
    std::size_t rank;
    int **keyMatrix;
    int **inverseKeyMatrix;
};

#endif // HILL_H
