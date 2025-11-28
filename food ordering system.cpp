// FOOD ORDERING SYSTEM WITH MANAGER FEATURES

#include <iostream>
#include <vector>
#include <unordered_map>
#include <iomanip> // For formatted output (setw, setprecision)
#include <limits>  // For numeric_limits
#include <string>  // For getline and string operations

#ifdef _WIN32
#include <windows.h> // For colors on Windows
#endif

using namespace std;

// Function to clear the console screen
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// STRUCTURE to represent a Menu Item
struct MenuItem {
    int id;
    string name;
    float price;
    int stock;
    int prepTime;
};

// STRUCTURE to represent a Customer Order
struct Order {
    int itemId;
    int quantity;
};

// CLASS to represent the Menu
class Menu {
    vector<MenuItem> items; // [DSA: Vector]

public:
    void addItem(const MenuItem& item) {
        items.push_back(item);
    }

    void display() {
        cout << "\n\t\t+------------------------------------------------------------------+\n";
        cout << "\t\t|                           -- MENU --                             |\n";
        cout << "\t\t+------------------------------------------------------------------+\n";
        cout << "\t\t| " << left << setw(5) << "ID" << "| " << setw(20) << "Item Name" << "| "
             << setw(10) << "Price" << "| " << setw(10) << "Stock" << "| "
             << setw(15) << "Prep Time (m)" << " |\n";
        cout << "\t\t+------------------------------------------------------------------+\n";

        for (const auto& item : items) {
            cout << "\t\t| " << left << setw(5) << item.id << "| "
                 << setw(20) << item.name << "| $"
                 << fixed << setprecision(2) << setw(8) << item.price << "| "
                 << setw(10) << item.stock << "| "
                 << setw(15) << item.prepTime << " |\n";
        }
        cout << "\t\t+------------------------------------------------------------------+\n";
    }

    bool updateStock(int itemId, int quantity) {
        for (auto& item : items) {
            if (item.id == itemId && item.stock >= quantity) {
                item.stock -= quantity;
                return true;
            }
        }
        return false;
    }

    MenuItem* getItem(int id) {
        for (auto& item : items) {
            if (item.id == id) return &item;
        }
        return nullptr;
    }

    void managerEditMenu() {
        int choice;
        do {
            clearScreen();
            cout << "\n\t\t  *** MANAGER CONTROL PANEL ***\n";
            cout << "\t\t=================================\n";
            cout << "\t\t[1] Add New Menu Item\n";
            cout << "\t\t[2] Edit Item Price\n";
            cout << "\t\t[3] Edit Item Stock\n";
            cout << "\t\t[4] Edit Item Prep Time\n";
            cout << "\t\t[5] Display Full Menu\n";
            cout << "\t\t[0] Exit to Main Menu\n";
            cout << "\t\t=================================\n";
            cout << "\t\tEnter your choice: ";
            cin >> choice;

            if (cin.fail()) {
                cout << "\n\t\tInvalid input. Please enter a number.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            if (choice == 1) {
                clearScreen();
                MenuItem newItem;
                cout << "\n\t\t--- Add New Item ---\n";
                cout << "\t\tEnter Item ID: "; cin >> newItem.id;
                cout << "\t\tEnter Item Name: "; cin.ignore(); getline(cin, newItem.name);
                cout << "\t\tEnter Price ($): "; cin >> newItem.price;
                cout << "\t\tEnter Initial Stock: "; cin >> newItem.stock;
                cout << "\t\tEnter Prep Time (mins): "; cin >> newItem.prepTime;
                addItem(newItem);
                cout << "\n\t\tSUCCESS: Item '" << newItem.name << "' added!\n";
            } else if (choice >= 2 && choice <= 4) {
                display();
                int id;
                cout << "\n\t\tEnter the ID of the item to edit: "; cin >> id;
                MenuItem* item = getItem(id);
                if (item) {
                    if (choice == 2) {
                        cout << "\t\tEnter new price for " << item->name << ": "; cin >> item->price;
                    } else if (choice == 3) {
                        cout << "\t\tEnter new stock for " << item->name << ": "; cin >> item->stock;
                    } else if (choice == 4) {
                        cout << "\t\tEnter new prep time for " << item->name << ": "; cin >> item->prepTime;
                    }
                    cout << "\n\t\tSUCCESS: Item updated.\n";
                } else {
                    cout << "\n\t\tERROR: Item not found.\n";
                }
            } else if (choice == 5) {
                display();
            } else if (choice != 0) {
                cout << "\n\t\tInvalid choice. Please try again.\n";
            }
            if(choice != 0) {
                cout << "\n\t\tPress Enter to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
            }
        } while (choice != 0);
    }
};

// CLASS to handle orders
class OrderSystem {
    Menu& menu;
    vector<Order> cart; // [DSA: Vector]
    unordered_map<int, int> orderCount; // [DSA: Hash Map]

public:
    OrderSystem(Menu& m) : menu(m) {}

    void placeOrder() {
        char more = 'Y';
        while (more == 'Y' || more == 'y') {
            clearScreen();
            cout << "\n\t\t   *** PLACE YOUR ORDER ***\n";
            menu.display();

            int itemId, qty;
            cout << "\n\t\t> Enter Item ID to order: ";
            cin >> itemId;
            cout << "\t\t> Enter quantity: ";
            cin >> qty;

            if (menu.getItem(itemId) != nullptr && menu.updateStock(itemId, qty)) {
                cart.push_back({itemId, qty});
                orderCount[itemId] += qty;
                cout << "\n\t\tSUCCESS: Added to your order!\n";
            } else {
                cout << "\n\t\tERROR: Invalid item ID or insufficient stock.\n";
            }

            cout << "\n\t\t> Order another item? (Y/N): ";
            cin >> more;
        }
    }

    void showBill() {
        clearScreen();
        float total = 0;
        cout << "\n\t\t+------------------------------------------+\n";
        cout << "\t\t|              FINAL RECEIPT               |\n";
        cout << "\t\t+------------------------------------------+\n";
        cout << "\t\t| " << left << setw(20) << "Item" << "| " << setw(8) << "Qty" << "| " << setw(10) << "Cost" << " |\n";
        cout << "\t\t+------------------------------------------+\n";

        for (const auto& order : cart) {
            MenuItem* item = menu.getItem(order.itemId);
            if (item) {
                float cost = item->price * order.quantity;
                cout << "\t\t| " << left << setw(20) << item->name << "| "
                     << setw(8) << order.quantity << "| $"
                     << fixed << setprecision(2) << setw(8) << cost << " |\n";
                total += cost;
            }
        }
        cout << "\t\t+------------------------------------------+\n";
        cout << "\t\t| " << left << setw(29) << "TOTAL:" << "| $" << fixed << setprecision(2) << setw(8) << total << " |\n";
        cout << "\t\t+------------------------------------------+\n";
        cout << "\n\t\t     Thank you for your order! :)\n\n";
    }

    void showPopularItem() {
        if (orderCount.empty()) return;

        int maxId = -1;
        int maxQty = 0;
        for (auto& p : orderCount) {
            if (p.second > maxQty) {
                maxQty = p.second;
                maxId = p.first;
            }
        }
        if (maxId != -1) {
            MenuItem* item = menu.getItem(maxId);
            cout << "\n\t\t-------------------------------------------\n";
            cout << "\t\t  Today's Most Popular Item: " << item->name << "\n";
            cout << "\t\t  (Ordered " << maxQty << " times)\n";
            cout << "\t\t-------------------------------------------\n";
        }
    }
};

int main() {
    #ifdef _WIN32
    // Optional: Set console title and text color for a better look
    SetConsoleTitle("Food Ordering System");
    // system("color 0F"); // Bright White on Black
    #endif

    Menu menu;
    menu.addItem({1, "Margherita Pizza", 8.99, 10, 15});
    menu.addItem({2, "Cheeseburger", 4.99, 20, 10});
    menu.addItem({3, "Carbonara Pasta", 6.49, 15, 12});
    menu.addItem({4, "Seasoned Fries", 2.99, 25, 5});
    menu.addItem({5, "Chicken Nuggets", 3.49, 18, 7});

    OrderSystem system(menu);

    int userType = 0;
    do {
        clearScreen();
        cout << "\n\n\n";
        cout << "\t\t======================================\n";
        cout << "\t\t   WELCOME TO THE FOOD ORDERING SYSTEM   \n";
        cout << "\t\t======================================\n\n";
        cout << "\t\t   [1] Manager Login\n";
        cout << "\t\t   [2] Customer Login\n";
        cout << "\t\t   [0] Exit Application\n\n";
        cout << "\t\t   Please choose your login type: ";
        cin >> userType;

        if (cin.fail()) {
            cout << "\n\t\tInvalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            userType = -1; // Set to a value that will loop
            cout << "\n\t\tPress Enter to continue...";
            cin.get();
            continue;
        }

        switch (userType) {
            case 1:
                menu.managerEditMenu();
                break;
            case 2:
                system.placeOrder();
                if(!cin.fail()){ // Check if cart is not empty before showing bill
                   system.showBill();
                   system.showPopularItem();
                }
                cout << "\n\t\tPress Enter to return to the main menu...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
                break;
            case 0:
                cout << "\n\t\tThank you for using the system. Goodbye!\n\n";
                break;
            default:
                cout << "\n\t\tInvalid choice. Please try again.\n";
                cout << "\n\t\tPress Enter to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
                break;
        }

    } while (userType != 0);

    return 0;
}
