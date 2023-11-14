// market_project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <conio.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>

#include "sqlite3.h"

using namespace std;


struct UserData {
    std::string username;
    std::string password;
};

bool test(string un, string pw) {
    //std::string un = "123";

    std::vector<UserData> users;

    // Open the SQLite database
    sqlite3* db;
    if (sqlite3_open("market_place.db", &db) == SQLITE_OK) {
        // Use a stringstream to construct the SQL query
        std::stringstream ss;
        ss << "SELECT * FROM user001 WHERE userID = \"" << un << "\";";
        std::string query = ss.str();

        // Execute the SQL query
        //int result = sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr);

        sqlite3_stmt* stmt;

        // compile sql statement to binary
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
            printf("ERROR: while compiling sql: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            sqlite3_finalize(stmt);
            return false;
        }

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            UserData data;
            data.username = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            data.password = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));

            users.push_back(data);
        }

        sqlite3_finalize(stmt);

        if (users.empty()) {
            return false;
        }

        if (users.size() > 1) {
            return false;
        }

        if ((users[0].username == un) && (users[0].password == pw))
        {
            return true;
        }

        // Close the database
        sqlite3_close(db);
    }
    else {
        std::cerr << "Error opening database." << std::endl;
    }

    return 0;
}

struct ItemData {
    std::string id;
    std::string description;
    int quantity;
    double price;
    std::string userId;
};

bool getItemsFromDB(std::vector<ItemData> &items) {

    // Open the SQLite database
    sqlite3* db;
    if (sqlite3_open("market_place.db", &db) == SQLITE_OK) {

        std::string query = "SELECT * FROM item001";

        sqlite3_stmt* stmt;

        // compile sql statement to binary
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
            printf("ERROR: while compiling sql: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            sqlite3_finalize(stmt);
            return false;
        }

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            ItemData data;
            data.id = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            data.description = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            data.quantity = sqlite3_column_int(stmt, 2);
            data.price = sqlite3_column_double(stmt, 3);
            data.userId = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));

            items.push_back(data);
        }

        sqlite3_finalize(stmt);

        // Close the database
        sqlite3_close(db);

        return true;
    }
    else {
        std::cerr << "Error opening database." << std::endl;
    }

    return false;
}

void printItems(const std::vector<ItemData> &items) {
    std::cout << "ID" << setw(20) << left <<" DESCRIPTION" << setw(20) << left << "QUANTITY" << setw(20) << left << "PRICE" << setw(20) << left << "USER ID" << std::endl;
    for (int i = 0; i < items.size(); i++) {
        std::cout << setw(20) << left << items[i].id << setw(10) << left << items[i].description << setw(20) << left << items[i].quantity << setw(20) << left << items[i].price << setw(20) << left << items[i].userId << std::endl;
    }
}


int partition(std::vector<ItemData>& items, int start, int end)
{


    ItemData pivot = items[start];

    int count = 0;
    for (int i = start + 1; i <= end; i++) {
        if (items[i].price <= pivot.price)
            count++;
    }

    // Giving pivot element its correct position
    int pivotIndex = start + count;
    swap(items[pivotIndex], items[start]);

    // Sorting left and right parts of the pivot element
    int i = start, j = end;

    while (i < pivotIndex && j > pivotIndex) {

        while (items[i].price <= pivot.price) {
            i++;
        }

        while (items[j].price > pivot.price) {
            j--;
        }

        if (i < pivotIndex && j > pivotIndex) {
            swap(items[i++], items[j--]);
        }
    }

    return pivotIndex;
}

void quickSort(std::vector<ItemData>& items, int start, int end)
{

    // base case
    if (start < end)
    {

        // partitioning the array
        int p = partition(items, start, end);

        // Sorting the left part 
        quickSort(items, start, p - 1);

        // Sorting the right part
        quickSort(items, p + 1, end);

    }
}

void bubbleSort(std::vector<ItemData>& items) {
    int n = items.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (items[j].price > items[j + 1].price) {
                // Swap arr[j] and arr[j + 1]
                std::swap(items[j], items[j + 1]);
            }
        }
    }
}

void selectionSort(std::vector<ItemData>& items) {
    int n = items.size();
    for (int i = 0; i < n - 1; ++i) {
        int minIndex = i;
        for (int j = i + 1; j < n; ++j) {
            if (items[j].price < items[minIndex].price) {
                minIndex = j;
            }
        }
        // Swap arr[i] and arr[minIndex]
        std::swap(items[i], items[minIndex]);
    }
}

void insertionSort(std::vector<ItemData>& items) {
    int n = items.size();
    for (int i = 1; i < n; ++i) {
        int key = items[i].price;
        int j = i - 1;

        // Move elements of arr[0..i-1] that are greater than key to one position ahead of their current position
        while (j >= 0 && items[j].price > key) {
            items[j + 1].price = items[j].price;
            --j;
        }
        items[j + 1].price = key;
    }
}


int main()
{
    std::string uname;
    std::cout << "--------------------Welcome to TRU Market--------------------\n" << std::endl;
    std::cout << "\n----------Please login below---------- " << std::endl;
    std::cout << "User name: ";
    std::getline(cin, uname);

    std::string password;
    char ch;

    std::cout << "Enter password: ";

    while (true) {
        ch = _getch();  // Use _getch() to get a character without echoing

        if (ch == 13) {  // Enter key
            break;
        }
        else if (ch == 8 && !password.empty()) {  // Backspace key
            std::cout << "\b \b";  // Move the cursor back, print a space, move back again
            password.pop_back();
        }
        else {
            std::cout << '*';
            password.push_back(ch);
        }
    }

    std::cout << std::endl;


    bool cor = test(uname, password);

    if (cor) {
        std::cout << "User " << uname << " successfully logged in" << std::endl;
    }
    else {
        std::cout << "Login failed" << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<ItemData> items;
    if (!getItemsFromDB(items)) {
        std::cout << "Getting items from database failed" << std::endl;
        return EXIT_FAILURE;
    }

    if (items.empty()) {
        std::cout << "No items in database" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Items before sorting: " << std::endl;
    printItems(items);

    /*
   clock_t start, end;

    start = clock();

    std::cout << "Items after sorting by price: " << std::endl;
    quickSort(items, 0, items.size() - 1);
    printItems(items);

    end = clock();

    double time_taken = double(end - start)/ double(CLOCKS_PER_SEC);
    cout << "\nTime of execution for quick sort is : " << fixed << time_taken << setprecision(6) << endl;
*/
   
    clock_t start, end;

    start = clock();
    std::cout << "Items after bubble sort by price: " << std::endl;
    bubbleSort(items);
    printItems(items);
    end = clock();
    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    cout << "\nTime of execution for bubble sort is : " << fixed << time_taken << setprecision(6) << endl;
/*
    start = clock();
    std::cout << "Items after selection sort by price: " << std::endl;
    selectionSort(items);
    printItems(items);
    end = clock();
     time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    cout << "\nTime of execution for selection sort is : " << fixed << time_taken << setprecision(6) << endl;

    start = clock();
   std::cout << "Items after insertion sort by price: " << std::endl;
    insertionSort(items);
    printItems(items);
    end = clock();
     time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    cout << "\nTime of execution for insterion  sort is : " << fixed << time_taken << setprecision(6) << endl;

   */ 


    return (0);
}
