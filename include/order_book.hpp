#pragma once
#include "order.hpp"
#include "trade.hpp"
#include<vector>
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
  vector<Trade> trades;
  
  bool can_match_buy(const Order &order,Price ask_price) const;
  bool can_match_ask(const Order &order,Price buy_price) const;

  void match_buy(Order& order);
  void match_ask(Order& order);
    
  public:
  void add_order(Order order);
  bool cancel_order(OrderId order_id);
  void print_book() const;
  void print_trades()const;
  void modify_order(OrderId id,Quantity new_quantity);
  void modify_order(OrderId,Quantity new_quantity,Price new_price);
};

