#pragma once
#include "order.hpp"
#include<list>
#include<map>
#include<unordered_map>
#include<functional>
#include<stdbool.h>
using namespace std;
class OrderBook{
  private:
  using OrderList=list<Order>;
  using OrderIterator=OrderList::iterator;
  struct OrderLocation{
    Side side;
    Price price;
    OrderIterator iterator;
  };
  map<Price,OrderList,greater<Price>> bids;
  map<Price,OrderList> asks;
  unordered_map<OrderId,OrderLocation> order_lookup;
  public:
  void add_order(Order order);
  bool cancel_order(OrderId order_id);
  void print_book(OrderBook const ob) ;
};

