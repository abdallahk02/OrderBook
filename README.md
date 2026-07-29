# Order Book

A compact C++20 limit order book that demonstrates price-time-priority matching.

## Features

- Buy and sell limit orders
- Market orders with unfilled-quantity reporting
- Automatic matching at compatible prices
- Order cancellation and modification
- Aggregated volume display by price level

## Build and Run

Requires a C++20-compatible compiler.

```sh
g++ -std=c++20 -Wall -Wextra -pedantic orderbook.cpp -o orderbook
./orderbook
```

On Windows, run the executable with:

```powershell
.\orderbook.exe
```

The `main` function in `orderbook.cpp` contains a short example that adds,
matches, modifies, and displays orders.
