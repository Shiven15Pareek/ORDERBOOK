#pragma once
#include "order.hpp"
#include "order_book.hpp"

struct Trade{
  OrderId buy_id;
  OrderId sell_id;
  Price price;
  Quantity quantity;
};