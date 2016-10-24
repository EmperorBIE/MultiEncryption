#ifndef ENCRYPTABLE_H
#define ENCRYPTABLE_H

#include <string>

struct Encryptable {
    virtual void setKeyword(const std::string&) = 0;
    virtual const std::string encrypt(const std::string&) const = 0;
    virtual const std::string decrypt(const std::string&) const = 0;
    virtual ~Encryptable() { }
};

#endif // ENCRYPTABLE_H
