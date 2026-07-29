#include "orderbook.h"
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <map>
#include <string>
#include <algorithm>
#include <limits>

class orderBook{
    private:
    // Heaps for buys and sells
    std::map<double, PriceLevel, std::greater<double>> buys;
    std::map<double, PriceLevel> sells;
    // Map for lookups
    std::unordered_map<int, std::list<Order>::iterator> orderSearch;

    int matchOrder(Side side, double price, int quantity){
        while(side == Side::Buy && !sells.empty() && sells.begin()->first <= price && quantity > 0){
            PriceLevel& level = sells.begin()->second;
            Order& match = level.orders.front();
            int ordersMatched = std::min(match.quantity, quantity);
            match.quantity -= ordersMatched;
            quantity -= ordersMatched;
            level.volume -= ordersMatched;
            if(match.quantity == 0){
                orderSearch.erase(match.id);
                level.orders.pop_front();     
            }
            if(level.volume == 0) sells.erase(sells.begin());
        }
        while(side == Side::Sell && !buys.empty() && buys.begin()->first >= price && quantity > 0){
            PriceLevel& level = buys.begin()->second;
            Order& match = level.orders.front();
            int ordersMatched = std::min(match.quantity, quantity);
            match.quantity -= ordersMatched;
            quantity -= ordersMatched;
            level.volume -= ordersMatched;
            if(match.quantity == 0){
                orderSearch.erase(match.id);
                level.orders.pop_front(); 
            }
            if(level.volume == 0) buys.erase(buys.begin());
        }

        return quantity;
    }

    
    public:

    void addOrder(int id, double price, int quantity, Side side){  // Limit Orders
        if(orderSearch.contains(id)){
            std::cout << "Duplicate id found, order not added" << std::endl;
            return;
        }

        quantity = matchOrder(side, price, quantity);
        if(quantity == 0){
            return;
        }
        Order order = {id, price, quantity, side};
        if(order.side == Side::Buy){
            PriceLevel& level = buys[order.price];
            level.price = order.price;
            level.volume += order.quantity;
            level.orders.push_back(order);
            orderSearch[order.id] = --level.orders.end();
        }else{
            PriceLevel& level = sells[order.price];
            level.price = order.price;
            level.volume += order.quantity;
            level.orders.push_back(order);
            orderSearch[order.id] = --level.orders.end();
        }
    }

    void addOrder(int id, int quantity, Side side){ // Market order
        if(orderSearch.contains(id)){
            std::cout << "Duplicate id found, order not added" << std::endl;
            return;
        }
        double mktPrice = 0.0;
        if(side == Side::Buy) mktPrice += std::numeric_limits<double>::infinity();
        
        int remaining = matchOrder(side, mktPrice, quantity);

        if(remaining > 0){
            std::cout << "Market order " << id << " had " << remaining << " shares unfilled" << std::endl;
        }
    }


    void cancelOrder(int id){
        if(!orderSearch.contains(id)){
            std::cout << "Order " << id << " not found" << std::endl;
            return;
        }
        
        auto it = orderSearch[id];
        Side side = it->side;
        double price = it->price;
        int qty = it->quantity;

        if(side == Side::Buy){
            PriceLevel& level = buys[price];
            level.volume -= qty;
            level.orders.erase(it);
            if(level.volume == 0) buys.erase(price);
        }else{
            PriceLevel& level = sells[price];
            level.volume -= qty;
            level.orders.erase(it);
            if(level.volume == 0) sells.erase(price);
        }

        orderSearch.erase(id);
    }

    void modifyOrder(int id, double updatedPrice){
        if(!orderSearch.contains(id)) return;

        auto it = orderSearch[id]; 
        int quantity = it->quantity;
        Side side = it->side;
        
        cancelOrder(id);
        addOrder(id, updatedPrice, quantity, side);
    }
    void modifyOrder(int id, int updatedQty){
        if(!orderSearch.contains(id)) return;

        auto it = orderSearch[id]; 
        double price = it->price;
        Side side = it->side;
        if(updatedQty > it->quantity){
            cancelOrder(id);
            addOrder(id, price, updatedQty, side);
        }else{
            int loss = it->quantity - updatedQty;
            if(side == Side::Buy) buys[price].volume -= loss;
            else sells[price].volume -= loss;
            it->quantity = updatedQty;
        }

    }
    void modifyOrder(int id, double updatedPrice, int updatedQty){
        if(!orderSearch.contains(id)) return;

        auto it = orderSearch[id];
        Side side = it->side;
        if(updatedQty > it->quantity || updatedPrice != it->price){
            cancelOrder(id);
            addOrder(id, updatedPrice, updatedQty, side);
        }else{
            int loss = it->quantity - updatedQty;
            if(side == Side::Buy) buys[it->price].volume -= loss;
            else sells[it->price].volume -= loss;
            it->quantity = updatedQty;
        }
    }

    void display() {
        std::cout << "\n========= MARKET BOOK =========" << std::endl;
        for (auto it = sells.rbegin(); it != sells.rend(); ++it)
            std::cout << "SELL: " << std::fixed << std::setprecision(2) << it->first << " | Vol: " << it->second.volume << std::endl;
        std::cout << "-------------------------------" << std::endl;
        for (auto const& [price, level] : buys)
            std::cout << "BUY:  " << std::fixed << std::setprecision(2) << price << " | Vol: " << level.volume << std::endl;
        std::cout << "===============================\n" << std::endl;
    }

    
};

int main(){

    orderBook exchange;

    // Add sells
    exchange.addOrder(1, 105.0, 10, Side::Sell);
    exchange.addOrder(2, 110.0, 15, Side::Sell);
    exchange.addOrder(3, 105.0, 20, Side::Sell);
    exchange.display();
    // Add buys
    exchange.addOrder(4, 95.0, 30, Side::Buy);
    exchange.addOrder(5, 105.0, 10, Side::Buy);
    exchange.addOrder(6, 100.0, 5, Side::Buy);
    exchange.display();
    // Change buys to match
    exchange.modifyOrder(4, 110.0);
    exchange.display();
    return 0;
}


