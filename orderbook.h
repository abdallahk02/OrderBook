#ifndef ORDERBOOK_H
#define ORDERBOOK_H
#include <list>

enum class Side{Buy, Sell};

struct Order{
    int id;
    double price;
    int quantity; 
    Side side;
};

struct PriceLevel{
    double price;
    int volume = 0;
    std::list<Order> orders;
};



#endif