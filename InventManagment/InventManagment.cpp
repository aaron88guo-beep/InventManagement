// InventManagment.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>

#include "Item.h"
#include "Inventory.h"
#include "Transaction.h"

static std::string trim(const std::string& s) {
    auto a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    auto b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

static bool approxEqual(double a, double b, double eps = 1e-6) {
    return std::fabs(a - b) <= eps;
}

static bool runTestSuite() {
    int passed = 0, failed = 0;
    auto pass = [&](const std::string& name) {
        ++passed;
        std::cout << "[PASS] " << name << "\n";
    };
    auto fail = [&](const std::string& name, const std::string& reason = "") {
        ++failed;
        std::cout << "[FAIL] " << name;
        if (!reason.empty()) std::cout << " - " << reason;
        std::cout << "\n";
    };

    std::cout << "Running internal test suite to verify inventory functions (boundary checks & error handling)...\n";

    // Test 1: Add normal item
    {
        Inventory inv;
        auto it = std::make_shared<Item>(201, "T1", "desc", 10.0, 2);
        inv.addItem(it);
        auto found = inv.findItemById(201);
        bool ok = (found != nullptr) && (found->getItemName() == "T1") && (found->getQuantity() == 2);
        ok = ok && (inv.getTotalItems() == 2) && approxEqual(inv.getTotalValue(), 20.0);
        if (ok) pass("Add normal item and totals");
        else fail("Add normal item and totals", found ? ("name=" + found->getItemName() + " qty=" + std::to_string(found->getQuantity())) : "not found");
    }

    // Test 2: Duplicate add should be rejected (no overwrite)
    {
        Inventory inv;
        auto a = std::make_shared<Item>(202, "Orig", "d", 5.0, 1);
        inv.addItem(a);
        auto dup = std::make_shared<Item>(202, "Dup", "d2", 100.0, 999);
        inv.addItem(dup); // should be rejected by Inventory::addItem
        auto found = inv.findItemById(202);
        if (found && found->getItemName() == "Orig" && found->getQuantity() == 1) pass("Duplicate add rejected (no overwrite)");
        else fail("Duplicate add rejected (no overwrite)", found ? ("name=" + found->getItemName() + " qty=" + std::to_string(found->getQuantity())) : "not found");
    }

    // Test 3: Increase stock normal
    {
        Inventory inv;
        auto a = std::make_shared<Item>(203, "IncTest", "d", 1.0, 2);
        inv.addItem(a);
        inv.increaseStock(203, 3);
        auto f = inv.findItemById(203);
        if (f && f->getQuantity() == 5) pass("Increase stock normal");
        else fail("Increase stock normal", f ? ("qty=" + std::to_string(f->getQuantity())) : "not found");
    }

    // Test 4: Decrease stock normal
    {
        Inventory inv;
        auto a = std::make_shared<Item>(204, "DecTest", "d", 1.0, 5);
        inv.addItem(a);
        inv.decreaseStock(204, 2);
        auto f = inv.findItemById(204);
        if (f && f->getQuantity() == 3) pass("Decrease stock normal");
        else fail("Decrease stock normal", f ? ("qty=" + std::to_string(f->getQuantity())) : "not found");
    }

    // Test 5: Decrease stock insufficient should not change quantity
    {
        Inventory inv;
        auto a = std::make_shared<Item>(205, "DecInsuff", "d", 1.0, 3);
        inv.addItem(a);
        inv.decreaseStock(205, 10); // should be rejected internally
        auto f = inv.findItemById(205);
        if (f && f->getQuantity() == 3) pass("Decrease insufficient stock rejected");
        else fail("Decrease insufficient stock rejected", f ? ("qty=" + std::to_string(f->getQuantity())) : "not found");
    }

    // Test 6: Increase non-existent item should not crash and item remains absent
    {
        Inventory inv;
        inv.increaseStock(9999, 1); // no item
        auto f = inv.findItemById(9999);
        if (!f) pass("Increase non-existent item handled");
        else fail("Increase non-existent item handled", "item unexpectedly present");
    }

    // Test 7: Remove item then verify absent
    {
        Inventory inv;
        auto a = std::make_shared<Item>(206, "RemTest", "d", 2.0, 4);
        inv.addItem(a);
        inv.removeItem(206);
        if (!inv.findItemById(206)) pass("Remove item and verify absent");
        else fail("Remove item and verify absent", "item still present");
    }

    // Test 8: Remove non-existent item should not crash
    {
        Inventory inv;
        inv.removeItem(207); // non-existent
        pass("Remove non-existent item handled (no crash)");
    }

    // Test 9: Adding item with negative quantity/price should be rejected — test will flag if validation missing
    {
        Inventory inv;
        auto bad = std::make_shared<Item>(208, "BadItem", "neg", -5.0, -10);
        inv.addItem(bad);
        auto f = inv.findItemById(208);
        if (!f) pass("Reject item with negative price/quantity (validation present)");
        else fail("Reject item with negative price/quantity (validation missing)", "item was added with negative values");
    }

    // Test 10: Increasing/decreasing with negative qty should be rejected — test flags if missing
    {
        Inventory inv;
        auto a = std::make_shared<Item>(209, "NegOp", "d", 1.0, 10);
        inv.addItem(a);
        inv.increaseStock(209, -5); // if code accepts negative, qty will change — that's a failure for validation
        auto f = inv.findItemById(209);
        if (f && f->getQuantity() == 10) pass("Reject negative increase (validation present)");
        else fail("Reject negative increase (validation missing)", f ? ("qty=" + std::to_string(f->getQuantity())) : "not found");
    }

    // Test 11: Transaction object correctness (fields)
    {
        Transaction t(3001, 209, "ADD", 3, "txn test");
        bool ok = (t.getTransactionId() == 3001) && (t.getItemId() == 209) &&
                  (t.getTransactionType() == "ADD") && (t.getQuantity() == 3) &&
                  (t.getDescription() == "txn test") && (t.getTimestamp() != 0);
        if (ok) pass("Transaction getters and constructor");
        else fail("Transaction getters and constructor", "field mismatch");
    }

    // Test 12: Totals calculation for multiple items
    {
        Inventory inv;
        auto a = std::make_shared<Item>(301, "A", "d", 2.0, 5); // value 10
        auto b = std::make_shared<Item>(302, "B", "d", 4.0, 3); // value 12
        inv.addItem(a);
        inv.addItem(b);
        if (inv.getTotalItems() == 8 && approxEqual(inv.getTotalValue(), 22.0)) pass("Totals correct for multiple items");
        else fail("Totals correct for multiple items", "items=" + std::to_string(inv.getTotalItems()) + " value=" + std::to_string(inv.getTotalValue()));
    }

    // Summary
    std::cout << "\nTest summary: " << passed << " passed, " << failed << " failed.\n\n";
    return failed == 0;
}           

int main()  
{
    // Run tests before allowing interactive commands
    bool testsPassed = runTestSuite();
    if (!testsPassed) {
        std::cout << "One or more tests failed. Continue to interactive mode anyway? (y/n): ";
        std::string answer;
        if (!std::getline(std::cin, answer)) return 1;
        answer = toLower(trim(answer));
        if (answer != "y" && answer != "yes") {
            std::cout << "Aborting due to failed tests.\n";
            return 1;
        }
        std::cout << "Continuing to interactive mode despite test failures.\n";
    } else {
        std::cout << "All internal tests passed. Entering interactive REPL.\n";
    }

    Inventory inventory;
    std::vector<Transaction> txnHistory;
    int nextTxnId = 1000;

    // Prepopulate with some items
    auto item1 = std::make_shared<Item>(101, "Laptop", "High-performance laptop", 999.99, 5);
    auto item2 = std::make_shared<Item>(102, "Mouse", "Wireless mouse", 29.99, 50);
    auto item3 = std::make_shared<Item>(103, "Keyboard", "Mechanical keyboard", 79.99, 30);

    inventory.addItem(item1);
    inventory.addItem(item2);
    inventory.addItem(item3);

    std::cout << "Inventory management REPL. Type 'help' for commands. Type 'exit' to quit.\n";

    while (true)
    {
        std::cout << "\ncommand> ";
        std::string line;
        if (!std::getline(std::cin, line)) break;
        line = trim(line);
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        std::string lc = toLower(cmd);

        if (lc == "exit") {
            std::cout << "Exiting.\n";
            break;
        }
        else if (lc == "help" || lc == "?") {
            std::cout << "Commands:\n"
                "  help                     Show this help\n"
                "  list                     Display full inventory\n"
                "  additem                  Add a new item (interactive prompts)\n"
                "  removeitem <id>          Remove an item entirely\n"
                "  inc <id> <qty>           Increase stock for item id\n"
                "  dec <id> <qty>           Decrease stock for item id\n"
                "  txn                      Interactive transaction (add/remove)\n"
                "  findid <id>              Show details for item id\n"
                "  findname <name>          Find item by name (exact match)\n"
                "  history                  Show transaction history\n"
                "  exit                     Quit\n";
        }
        else if (lc == "list" || lc == "ls") {
            inventory.displayInventory();
        }
        else if (lc == "additem" || lc == "add") {
            try {
                std::string s;
                std::cout << "Enter Item ID: ";
                std::getline(std::cin, s);
                int id = std::stoi(trim(s));

                if (inventory.findItemById(id)) {
                    std::cout << "Item with ID " << id << " already exists. Use 'inc' to add stock.\n";
                    continue;
                }

                std::cout << "Enter Name: ";
                std::string name; std::getline(std::cin, name);
                name = trim(name);

                std::cout << "Enter Description: ";
                std::string desc; std::getline(std::cin, desc);
                desc = trim(desc);

                std::cout << "Enter Price: ";
                std::getline(std::cin, s);
                double price = std::stod(trim(s));

                std::cout << "Enter Quantity: ";
                std::getline(std::cin, s);
                int qty = std::stoi(trim(s));

                // Basic boundary checks before calling Inventory
                if (price < 0.0 || qty < 0) {
                    std::cout << "Invalid price or quantity. Both must be non-negative.\n";
                    continue;
                }

                auto newItem = std::make_shared<Item>(id, name, desc, price, qty);
                inventory.addItem(newItem);

                Transaction txn(nextTxnId++, id, "ADD", qty, "New item added");
                txnHistory.push_back(txn);
                std::cout << "Transaction recorded.\n";
            }
            catch (...) {
                std::cout << "Invalid input. Aborting add.\n";
            }
        }
        else if (lc == "removeitem" || lc == "remove") {
            int id;
            if (!(iss >> id)) {
                std::cout << "Usage: removeitem <id>\n";
                continue;
            }
            auto item = inventory.findItemById(id);
            if (!item) {
                std::cout << "Item ID " << id << " not found.\n";
                continue;
            }
            int prevQty = item->getQuantity();
            inventory.removeItem(id);
            Transaction txn(nextTxnId++, id, "REMOVE", prevQty, "Item removed");
            txnHistory.push_back(txn);
            std::cout << "Transaction recorded.\n";
        }
        else if (lc == "inc" || lc == "increase") {
            int id, qty;
            if (!(iss >> id >> qty)) {
                std::cout << "Usage: inc <id> <qty>\n";
                continue;
            }
            if (qty < 0) {
                std::cout << "Quantity must be non-negative.\n";
                continue;
            }
            auto item = inventory.findItemById(id);
            if (!item) {
                std::cout << "Item ID " << id << " not found.\n";
                continue;
            }
            inventory.increaseStock(id, qty);
            Transaction txn(nextTxnId++, id, "ADD", qty, "Stock increased");
            txnHistory.push_back(txn);
            std::cout << "Transaction recorded.\n";
        }
        else if (lc == "dec" || lc == "decrease") {
            int id, qty;
            if (!(iss >> id >> qty)) {
                std::cout << "Usage: dec <id> <qty>\n";
                continue;
            }
            if (qty < 0) {
                std::cout << "Quantity must be non-negative.\n";
                continue;
            }
            auto item = inventory.findItemById(id);
            if (!item) {
                std::cout << "Item ID " << id << " not found.\n";
                continue;
            }
            if (item->getQuantity() < qty) {
                std::cout << "Insufficient stock. Current: " << item->getQuantity() << "\n";
                continue;
            }
            inventory.decreaseStock(id, qty);
            Transaction txn(nextTxnId++, id, "REMOVE", qty, "Stock decreased");
            txnHistory.push_back(txn);
            std::cout << "Transaction recorded.\n";
        }
        else if (lc == "txn" || lc == "transact" || lc == "transaction") {
            try {
                std::string s;
                std::cout << "Transaction Type (add/remove): ";
                std::getline(std::cin, s);
                std::string t = toLower(trim(s));
                if (t != "add" && t != "remove" && t != "a" && t != "r") {
                    std::cout << "Invalid transaction type.\n";
                    continue;
                }
                bool isAdd = (t == "add" || t == "a");

                std::cout << "Enter Item ID: ";
                std::getline(std::cin, s);
                int id = std::stoi(trim(s));

                auto item = inventory.findItemById(id);

                if (isAdd) {
                    if (!item) {
                        std::cout << "Item not found. Create new item? (y/n): ";
                        std::string yn; std::getline(std::cin, yn);
                        if (toLower(trim(yn)) == "y" || toLower(trim(yn)) == "yes") {
                            std::cout << "Enter Name: ";
                            std::string name; std::getline(std::cin, name); name = trim(name);

                            std::cout << "Enter Description: ";
                            std::string desc; std::getline(std::cin, desc); desc = trim(desc);

                            std::cout << "Enter Price: ";
                            std::getline(std::cin, s);
                            double price = std::stod(trim(s));
                            if (price < 0.0) { std::cout << "Price must be non-negative.\n"; continue; }

                            std::cout << "Enter Quantity to add: ";
                            std::getline(std::cin, s);
                            int qty = std::stoi(trim(s));
                            if (qty < 0) { std::cout << "Quantity must be non-negative.\n"; continue; }

                            auto newItem = std::make_shared<Item>(id, name, desc, price, qty);
                            inventory.addItem(newItem);

                            Transaction txn(nextTxnId++, id, "ADD", qty, "New item added via txn");
                            txnHistory.push_back(txn);
                            std::cout << "Transaction recorded.\n";
                        } else {
                            std::cout << "Aborting transaction.\n";
                        }
                    } else {
                        std::cout << "Enter Quantity to add: ";
                        std::getline(std::cin, s);
                        int qty = std::stoi(trim(s));
                        if (qty < 0) { std::cout << "Quantity must be non-negative.\n"; continue; }
                        inventory.increaseStock(id, qty);
                        Transaction txn(nextTxnId++, id, "ADD", qty, "Stock increased via txn");
                        txnHistory.push_back(txn);
                        std::cout << "Transaction recorded.\n";
                    }
                } else { // remove
                    if (!item) {
                        std::cout << "Item ID " << id << " not found.\n";
                        continue;
                    }
                    std::cout << "Enter Quantity to remove: ";
                    std::getline(std::cin, s);
                    int qty = std::stoi(trim(s));
                    if (qty < 0) { std::cout << "Quantity must be non-negative.\n"; continue; }
                    if (item->getQuantity() < qty) {
                        std::cout << "Insufficient stock. Current: " << item->getQuantity() << "\n";
                        continue;
                    }
                    inventory.decreaseStock(id, qty);
                    Transaction txn(nextTxnId++, id, "REMOVE", qty, "Stock decreased via txn");
                    txnHistory.push_back(txn);
                    std::cout << "Transaction recorded.\n";
                }
            }
            catch (...) {
                std::cout << "Invalid input. Aborting transaction.\n";
            }
        }
        else if (lc == "findid") {
            int id;
            if (!(iss >> id)) {
                std::cout << "Usage: findid <id>\n";
                continue;
            }
            inventory.displayItemDetails(id);
        }
        else if (lc == "findname") {
            std::string rest;
            std::getline(iss, rest);
            rest = trim(rest);
            if (rest.empty()) {
                std::cout << "Usage: findname <name>\n";
                continue;
            }
            auto item = inventory.findItemByName(rest);
            if (item) item->displayItem();
            else std::cout << "Item with name '" << rest << "' not found.\n";
        }
        else if (lc == "history") {
            if (txnHistory.empty()) {
                std::cout << "No transactions recorded.\n";
            } else {
                std::cout << "\n=== TRANSACTION HISTORY ===\n";
                for (const auto& t : txnHistory) t.displayTransaction();
            }
        }
        else {
            std::cout << "Unknown command. Type 'help' for list of commands.\n";
        }
    }

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
