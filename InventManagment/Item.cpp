#include "Item.h"
#include <iostream>

Item::Item() : itemId(0), itemName(""), description(""), price(0.0), quantity(0) {}

Item::Item(int id, const std::string& name, const std::string& desc, double price, int qty)
    : itemId(id), itemName(name), description(desc), price(price), quantity(qty) {}

int Item::getItemId() const { return itemId; }
std::string Item::getItemName() const { return itemName; }
std::string Item::getDescription() const { return description; }
double Item::getPrice() const { return price; }
int Item::getQuantity() const { return quantity; }

void Item::setItemId(int id) { itemId = id; }
void Item::setItemName(const std::string& name) { itemName = name; }
void Item::setDescription(const std::string& desc) { description = desc; }
void Item::setPrice(double p) { price = p; }
void Item::setQuantity(int qty) { quantity = qty; }

void Item::displayItem() const
{
    std::cout << "ID: " << itemId << " | Name: " << itemName
        << " | Description: " << description << " | Price: $" << price
        << " | Quantity: " << quantity << std::endl;
}