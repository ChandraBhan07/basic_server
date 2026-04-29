// extend later

#pragma once
#include <string>

class uuid
{
    std::string id;

public:
    uuid();
    ~uuid();
    const std::string &toString() const;

    // Equality operator - it checks if key exists
    bool operator==(const uuid &other) const
    {
        return id == other.id;
    }
};

struct uuidHash
{
    std::size_t operator()(const uuid &u) const noexcept
    {
        return std::hash<std::string>()(u.toString());
    }
};