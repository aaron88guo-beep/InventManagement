// InventManagment.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Item.h"
#include "Inventory.h"
#include "Transaction.h"
#include <memory>

int main()
{
    // Create inventory system
    Inventory inventory;

    // Create and add items
    auto item1 = std::make_shared<Item>(101, "Laptop", "High-performance laptop", 999.99, 5);
    auto item2 = std::make_shared<Item>(102, "Mouse", "Wireless mouse", 29.99, 50);
    auto item3 = std::make_shared<Item>(103, "Keyboard", "Mechanical keyboard", 79.99, 30);

    inventory.addItem(item1);
    inventory.addItem(item2);
    inventory.addItem(item3);

    // Display initial inventory
    inventory.displayInventory();

    // Perform transactions
    std::cout << "\n=== PERFORMING TRANSACTIONS ===" << std::endl;
    inventory.increaseStock(102, 20);
    inventory.decreaseStock(103, 5);

    // Display updated inventory
    inventory.displayInventory();

    // Display specific item
    inventory.displayItemDetails(101);

    // Create transaction records
    Transaction txn1(1001, 102, "ADD", 20, "Restock");
    Transaction txn2(1002, 103, "REMOVE", 5, "Sold to customer");

    std::cout << "\n=== TRANSACTION HISTORY ===" << std::endl;
    txn1.displayTransaction();
    txn2.displayTransaction();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
