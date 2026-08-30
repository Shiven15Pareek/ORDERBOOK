#include "include/order.hpp"
#include "include/order_book.hpp"
#include <iostream>

using namespace std;

int main()
{
    cout << "PROGRAM STARTED\n";

    OrderBook orderbook;

    // -------------------------
    // Add BUY orders
    // -------------------------

    Order o1{
        1,
        Side::BUY,
        100,
        50
    };

    Order o2{
        2,
        Side::BUY,
        100,
        100
    };

    Order o3{
        17,
        Side::BUY,
        99,
        200
    };

    orderbook.add_order(o1);
    orderbook.add_order(o2);
    orderbook.add_order(o3);
    orderbook.print_book(orderbook);
    // -------------------------
    // Add SELL orders
    // -------------------------

    Order o4{
        24,
        Side::SELL,
        101,
        75
    };

    Order o5{
        12,
        Side::SELL,
        101,
        50
    };

    Order o6{
        36,
        Side::SELL,
        102,
        100
    };

    orderbook.add_order(o4);
    orderbook.add_order(o5);
    orderbook.add_order(o6);

    cout << "INITIAL ORDERS ADDED\n";
        orderbook.print_book(orderbook);

    // -------------------------
    // Incoming SELL
    // -------------------------

    Order o7{
        7,
        Side::SELL,
        100,
        120
    };

    cout << "ADDING O7\n";

    orderbook.add_order(o7);

    cout << "O7 PROCESSED\n";
        orderbook.print_book(orderbook);

    cout<<"testing order cancellations"<<endl;
    auto cancelled =orderbook.cancel_order(17);
    if(cancelled){
      cout<<"order cancelled "<<endl;
    }
    else{
      cout<<"order not cancelled"<<endl;
    }
        orderbook.print_book(orderbook);

    return 0;
}