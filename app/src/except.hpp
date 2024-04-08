#pragma once

namespace StreichfettSse
{

class ContinuableException : public std::exception
{
public:
    ContinuableException(const char* message);
};

class UncontinuableException : public std::runtime_error
{
public:
    UncontinuableException(const char* message);
};

} // StreichfettSse
