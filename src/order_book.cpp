#include "order_book.hpp"
#include<iostream>
#include<cstdint>
#include<iomanip>
#include<stdbool.h>
#include<cmath>
using namespace std;
void OrderBook::add_order(Order order){
  if(order.side==Side::BUY){
    match_buy(order);
    if(order.quantity>0 && order.type==OrderType::LIMIT){
      auto &orders=bids[order.price];
      orders.push_back(order);
      auto iterator=prev(orders.end());
      order_lookup[order.id]={
        Side::BUY,
        order.price,
        iterator
      };
    }
  }
  else{
    match_ask(order);
    if(order.quantity>0 && order.type==OrderType::LIMIT){
      auto &orders=asks[order.price];
      orders.push_back(order);
      auto iterator=prev(orders.end());
      order_lookup[order.id]={
        Side::SELL,
        order.price,
        iterator
      };
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
    // this->print_trades();
  }
  else{
    auto level=asks.find(location.price);
    if(level!=asks.end()){
      level->second.erase(location.iterator);
      if(level->second.empty()){
      asks.erase(level);
    }
    }
    // this->print_trades();
  }
  order_lookup.erase(lookup);
  return true;
}

void OrderBook::print_book() const{
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
  bool flag1=false;
  bool flag2=false;
  if(!bids.empty()){cout<<"BEST BID->"<<bb->first<<endl; 
    flag1=true;}
  if(!asks.empty()){cout<<"BEST ASK->"<<ba->first<<endl; 
    flag2=true;}
  if(flag1 && flag2){cout<<"SPREAD->"<<(ba->first)-(bb->first)<<endl;
  cout<<"MID PRICE->"<<static_cast<double>((ba->first)+(bb->first))/2<<endl;}
}

void OrderBook::print_trades() const{
  cout << "\n";
    cout << "---------------------------- TRADES ----------------------------\n";
    cout<<"  ================================================================\n";
    cout << "BUY ORDER       SELL ORDER       PRICE       QUANTITY\n";
    for(const auto & trade:trades){
      cout << setw(12) << trade.buy_id
             << setw(17) << trade.sell_id
             << setw(14) << trade.price
             << setw(14) << trade.quantity
             << '\n';
    }
    cout<<"------------------------------------------------------------------\n";
}

bool OrderBook::can_match_buy(const Order &order,Price ask_price) const{
  if (order.type==OrderType::MARKET){
    return true;
  }
  return ask_price<=order.price;
}

bool OrderBook::can_match_ask(const Order &order,Price bid_price) const{
  if(order.type==OrderType::MARKET){
    return true;
  }
  return bid_price>=order.price;
}

void OrderBook::match_buy(Order &order){
  while(order.quantity>0 && !asks.empty()){
    auto best_level=asks.begin();
    if(!can_match_buy(order,best_level->first)){
      break;
    }
    auto &orders=best_level->second;
    auto &resting_order=orders.front();
    Quantity traded_quantity=min(resting_order.quantity,order.quantity);
    Trade trade{order.id,resting_order.id,resting_order.price,traded_quantity};
    trades.push_back(trade);
    cout << "TRADE "<< traded_quantity<< " @ "<< best_level->first<< '\n';
    order.quantity-=traded_quantity;
    resting_order.quantity-=traded_quantity;
    if(resting_order.quantity==0){
      order_lookup.erase(resting_order.id);
      orders.pop_front();
    }
    if(orders.empty()){
      asks.erase(best_level);
    }
  }
}

void OrderBook::match_ask(Order &order){
  while(order.quantity>0 && !bids.empty()){
    auto best_level=bids.begin();
    if(!can_match_ask(order,best_level->first)){break;}
    auto &orders=best_level->second;
    auto &resting_order=orders.front();
    Quantity traded_quantity=min(order.quantity,resting_order.quantity);
    Trade trade{resting_order.id,order.id,resting_order.price,traded_quantity};
    trades.push_back(trade);
    order.quantity-=traded_quantity;
    resting_order.quantity-=traded_quantity;
    if(resting_order.quantity==0){
      order_lookup.erase(resting_order.id);
      orders.pop_front();
    }
    if(orders.empty()){
      bids.erase(best_level->first);
    }
  }
}

void OrderBook::modify_order(OrderId id,Quantity new_quantity){
  if(order_lookup.find(id)==order_lookup.end()){
    return;
  }
  auto & order_location=order_lookup[id];
  auto iter=order_location.iterator;
  if(new_quantity==0){
    cancel_order(iter->id);
    return;
  }
  iter->quantity=new_quantity;
}

void OrderBook::modify_order(OrderId id,Price new_price,Quantity new_quantity){
  if(order_lookup.find(id)==order_lookup.end()){
    return;
  }
  auto &order_location=order_lookup[id];
  auto iter=order_location.iterator;
  if(new_quantity==0){
    cancel_order(id);
    return;
  }
  if(new_price==iter->price){
    modify_order(id,new_quantity);
    return ;
  }
  Order new_order={
    iter->id,
    iter->side,
    iter->type,
    new_price,
    new_quantity
  };
  cancel_order(iter->id);
  add_order(new_order);
}

void OrderBook::print_bid_depth() const{
  cout<<"===============================BID DEPTH====================="<<endl;
  cout<<"   PRICE         QUANTITY\n";
  for(const auto &levels: bids){
    Quantity total_quantity=0;
    for(const auto &orders:levels.second){
       total_quantity+=orders.quantity;
    }
  cout<<"    "<<levels.first<<"             "<<total_quantity<<"\n";
  }
}

void OrderBook::print_ask_depth() const{
  cout<<"===============================ASK DEPTH====================="<<endl;
  cout<<"   PRICE         QUANTITY\n";
  for(const auto &levels:asks){
    Quantity total_quantity=0;
    for(const auto &orders: levels.second){
      total_quantity+=orders.quantity;
    }
  cout<<"    "<<levels.first<<"             "<<total_quantity<<"\n";
  }
}

double OrderBook:: calculate_imbalance() const{
  if(bids.empty() || asks.empty()){
    return 0.0;
  }
  const auto &level_bid=*bids.begin();
  Quantity total_bid=0;
  for(const auto &order:level_bid.second){
    total_bid+=order.quantity;
  }
  const auto &level_ask=*asks.begin();
  Quantity total_ask=0;
  for(const auto &order:level_ask.second){
    total_ask+=order.quantity;
  }
  double q_bid=static_cast<double>(total_bid);
  double q_ask=static_cast<double>(total_ask);
  return (q_bid-q_ask)/(q_bid+q_ask);
}

double OrderBook:: calculate_imbalance(int n) const{
  if(bids.empty() || asks.empty()){
    return 0.0;
  }
  if(n<=0){
    return 0.0;
  }
  Quantity total_bid=0,total_ask=0;
  auto bid_levels=static_cast<int>(bids.size());
  int bid_level=min(n,bid_levels);
  auto it_bid=bids.begin();
  while(bid_level--){
    for(const auto &order:it_bid->second){
      total_bid+=order.quantity;
    }
    it_bid++;
  }
  auto ask_levels=static_cast<int>(asks.size());
  int ask_level=min(n,ask_levels);
  auto it_ask=asks.begin();
  while(ask_level--){
    for(const auto &order:it_ask->second){
      total_ask+=order.quantity;
    }
    it_ask++;
  }
  return ( static_cast<double>(total_bid)-static_cast<double>(total_ask))/(static_cast<double>(total_bid)+static_cast<double>(total_ask));
}

double OrderBook:: calculate_weighted_imbalance(int n,double lambda) const{
  if(bids.empty() || asks.empty()){
    return 0.0;
  }
  if(n<=0){
    return 0.0;
  }
  if(lambda<0){
    return 0.0;
  }
  double total_bid=0,total_ask=0;
  auto bid_levels=static_cast<int>(bids.size());
  int bid_level=min(n,bid_levels);
  auto it_bid=bids.begin();
  int i=1;
  while(bid_level--){
    double weight=exp(-lambda*(i-1));
    for(const auto &order:it_bid->second){
      total_bid=total_bid+weight*static_cast<double>(order.quantity);
    }
    it_bid++;
    i++;
  }

  auto ask_levels=static_cast<int>(asks.size());
  int ask_level=min(n,ask_levels);
  auto it_ask=asks.begin();
  i=1;
  while(ask_level--){
    double weight=exp(-lambda*(i-1));
    for(const auto &order:it_ask->second){
      total_ask=total_ask+weight*static_cast<double>(order.quantity);
    }
    it_ask++;
    i++;
  }
  if(total_ask+total_bid==0){
    return 0.0;
  }
  return ((total_bid)-(total_ask))/((total_bid)+(total_ask));
}