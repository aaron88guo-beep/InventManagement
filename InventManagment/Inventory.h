#pragma once
#include "Item.h"
#include <vector>
#include <memory>

class Inventory
{
private:
    std::vector<std::shared_ptr<Item>> items;

public:
    Inventory();

    // Item Management
    void addItem(const std::shared_ptr<Item>& item);
    void removeItem(int itemId);
    std::shared_ptr<Item> findItemById(int itemId);
    std::shared_ptr<Item> findItemByName(const std::string& name);

    // Inventory Operations
    void increaseStock(int itemId, int quantity);
    void decreaseStock(int itemId, int quantity);
    int getTotalItems() const;
    double getTotalValue() const;

    // Display
    void displayInventory() const;
    void displayItemDetails(int itemId) const;
};