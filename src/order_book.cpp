#include "order_book.hpp"
#include<iostream>
#include<cstdint>
#include<iomanip>
using namespace std;
void OrderBook::add_order(Order order)
{
  cout<<"ADD_ORDER FUNCTION CALLED"<<endl;
  if (order.side == Side::BUY)
  {
    while (order.quantity > 0 && !asks.empty())
    {
      auto best_level = asks.begin();
      if (best_level->first > order.price)
      {
        break;
      }
      auto &orders = best_level->second;
      auto &resting_order = orders.front();
      Quantity traded_quantity = min(order.quantity, resting_order.quantity);
      cout << "TRADE " << traded_quantity << " @ " << best_level->first << "\n"
           << "---------------------------------------------------------------";
      cout << endl;
      order.quantity -= traded_quantity;
      resting_order.quantity -= traded_quantity;

      if (resting_order.quantity == 0)
      {
        order_lookup.erase(resting_order.id);
        orders.pop_front();
      }
      if (orders.empty())
      {
        asks.erase(best_level);
      }
    }
    if (order.quantity > 0)
    {
      auto &orders = bids[order.price];
      orders.push_back(order);
      auto iterator = prev(orders.end());
      order_lookup[order.id] = {
          Side::BUY,
          order.price,
          iterator};
    }
  }
  else
  {
    while (order.quantity > 0 && !bids.empty())
    {
      auto best_level = bids.begin();
      if (best_level->first < order.price)
      {
        break;
      }
      auto &orders = best_level->second;
      auto &resting_order = orders.front();
      Quantity traded_quantity = min(resting_order.quantity, order.quantity);
      order.quantity -= traded_quantity;
      resting_order.quantity -= traded_quantity;
      cout << "TRADE " << traded_quantity << " @ " << best_level->first << "\n"
             << "---------------------------------------------------------------"
             << endl;
      if (resting_order.quantity == 0)
      {
        order_lookup.erase(resting_order.id);
        orders.pop_front();
      }
      if (orders.empty())
      {
        bids.erase(best_level);
      }
    }
    if (order.quantity > 0)
    {
      auto &orders = asks[order.price];
      orders.push_back(order);
      auto iterator = prev(orders.end());
      order_lookup[order.id] = {
          Side::SELL,
          order.price,
          iterator};
    }
  }
}

bool OrderBook::cancel_order(OrderId order_id)
{
  auto lookup = order_lookup.find(order_id);
  if (lookup == order_lookup.end())
  {
    return false;
  }
  // cout<<"function for cancelling order called"<<endl;
  auto location = lookup->second;
  if (location.side == Side::BUY)
  {
    auto level = bids.find(location.price);
    if (level != bids.end())
    {
      level->second.erase(location.iterator);
      if (level->second.empty())
      {
        bids.erase(level);
      }
    }
  }
  else{
    auto level=asks.find(location.price);
    if(level!=asks.end()){
      level->second.erase(location.iterator);
      if(level->second.empty()){
      asks.erase(level);
    }
    }
  }
  order_lookup.erase(lookup);
  return true;
}

void OrderBook::print_book(OrderBook const ob){
  cout<<"\n";
  cout<<"---------------------------------ORDERBOOK----------------------------------"<<endl;
  cout<<"============================================================================"<<endl;
  cout<<endl;
  cout<<"       ASK SIZE             PRICE                BID SIZE\n";
  cout<<endl;
  for(auto it=asks.rbegin();it!=asks.rend();++it){
    Quantity total_quantity=0;
    for(const auto &order:it->second){
      total_quantity+=order.quantity;
    }
    cout << setw(16) << total_quantity
             << setw(17) << it->first
             << setw(20) << ""
             << '\n';
  }
  cout<<"----------------------------------------------------------------------------"<<endl;
  for(const auto & level:bids){
    Quantity total_quantity=0;
    for(const auto &order:level.second){
      total_quantity+=order.quantity;
    }
    cout << setw(16) << ""
             << setw(17) << level.first
             << setw(20) << total_quantity
             << '\n';
  }
  cout<<"============================================================================"<<endl;
  auto bb=bids.begin();
  auto ba=asks.begin();
  cout<<"BEST BID->"<<bb->first<<endl;
  cout<<"BEST ASK->"<<ba->first<<endl;
  cout<<"SPREAD->"<<(ba->first)-(bb->first)<<endl;
  cout<<"MID PRICE->"<<static_cast<double>((ba->first)+(bb->first))/2<<endl;
}
