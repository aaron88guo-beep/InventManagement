#pragma once
#include "Inventory.h"
#include <string>
#include <ctime>

class Transaction
{
private:
    int transactionId;
    int itemId;
    std::string transactionType; // "ADD" or "REMOVE"
    int quantity;
    std::time_t timestamp;
    std::string description;

public:
    Transaction();
    Transaction(int txnId, int itemId, const std::string& type, int qty, const std::string& desc);

    // Getters
    int getTransactionId() const;
    int getItemId() const;
    std::string getTransactionType() const;
    int getQuantity() const;
    std::time_t getTimestamp() const;
    std::string getDescription() const;

    // Setters
    void setTransactionId(int id);
    void setItemId(int id);
    void setTransactionType(const std::string& type);
    void setQuantity(int qty);
    void setDescription(const std::string& desc);

    // Display
    void displayTransaction() const;
};