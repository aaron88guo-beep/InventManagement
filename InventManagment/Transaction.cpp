#include "Transaction.h"
#include <iostream>
#include <iomanip>

Transaction::Transaction() : transactionId(0), itemId(0), transactionType(""), quantity(0), timestamp(0), description("") {}

Transaction::Transaction(int txnId, int itemId, const std::string& type, int qty, const std::string& desc)
    : transactionId(txnId), itemId(itemId), transactionType(type), quantity(qty), description(desc)
{
    timestamp = std::time(nullptr);
}

int Transaction::getTransactionId() const { return transactionId; }
int Transaction::getItemId() const { return itemId; }
std::string Transaction::getTransactionType() const { return transactionType; }
int Transaction::getQuantity() const { return quantity; }
std::time_t Transaction::getTimestamp() const { return timestamp; }
std::string Transaction::getDescription() const { return description; }

void Transaction::setTransactionId(int id) { transactionId = id; }
void Transaction::setItemId(int id) { itemId = id; }
void Transaction::setTransactionType(const std::string& type) { transactionType = type; }
void Transaction::setQuantity(int qty) { quantity = qty; }
void Transaction::setDescription(const std::string& desc) { description = desc; }

void Transaction::displayTransaction() const
{
    std::cout << "Transaction ID: " << transactionId << " | Item ID: " << itemId
        << " | Type: " << transactionType << " | Quantity: " << quantity
        << " | Description: " << description << std::endl;
}