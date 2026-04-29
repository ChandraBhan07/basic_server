#include "uuid.h"
#include <random>

uuid::uuid()
{
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    // simple thread-local RNG, seeded once per thread
    static thread_local std::mt19937 rng{std::random_device{}()};

    // distribution over charset range
    static thread_local std::uniform_int_distribution<int> dist(0, sizeof(charset) - 2);

    for (int i = 0; i < 8; ++i)
    {
        id += charset[dist(rng)];
    }
}

const std::string &uuid::toString() const { return id; }

uuid::~uuid() = default;