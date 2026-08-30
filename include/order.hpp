#pragma once
#include<iostream>
#include<cstdint>

using OrderId=uint64_t;
using Price=uint64_t;
using Quantity=uint64_t;

enum class Side{BUY,SELL};

struct Order{
  OrderId id;
  Side side;
  Price price;
  Quantity quantity;
};