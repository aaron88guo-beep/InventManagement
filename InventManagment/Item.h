#pragma once
#include <string>

class Item
{
private:
    int itemId;
    std::string itemName;
    std::string description;
    double price;
    int quantity;

public:
    Item();
    Item(int id, const std::string& name, const std::string& desc, double price, int qty);

    // Getters
    int getItemId() const;
    std::string getItemName() const;
    std::string getDescription() const;
    double getPrice() const;
    int getQuantity() const;

    // Setters
    void setItemId(int id);
    void setItemName(const std::string& name);
    void setDescription(const std::string& desc);
    void setPrice(double price);
    void setQuantity(int qty);

    // Display
    void displayItem() const;
};