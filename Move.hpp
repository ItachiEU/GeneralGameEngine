#pragma once
#include <memory>

class Move
{
public:
    virtual bool eq(std::shared_ptr<Move> other) = 0;
};