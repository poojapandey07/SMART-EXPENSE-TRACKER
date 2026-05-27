#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <limits>
#include <iomanip>

using namespace std;

struct Transaction {
    string type;      // "Income" or "Expense"
    string category;
    double amount;
};

vector<Transaction> records;
const string FILE_NAME = "expenses.txt";

// --- File I/O (pipe-delimited to support spaces in categories) ---

void saveToFile() {
    ofstream file(FILE_NAME);
    for (const auto& t : records)
        file << t.type << "|" << t.category << "|" << t.amount << "\n";
}

void loadFromFile() {
    ifstream file(FILE_NAME);
    string line;
    while (getline(file, line)) {
        size_t p1 = line.find('|');
        size_t p2 = line.rfind('|');
        if (p1 == string::npos || p1 == p2) continue; // skip malformed lines
        Transaction t;
        t.type     = line.substr(0, p1);
        t.category = line.substr(p1 + 1, p2 - p1 - 1);
        try {
            t.amount = stod(line.substr(p2 + 1));
        } catch (...) {
            continue; // skip lines with bad amount
        }
        records.push_back(t);
    }
}

// --- Input helpers ---

double getAmount() {
    double amount;
    while (true) {
        cin >> amount;
        if (cin.fail() || amount <= 0) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid amount. Enter a positive number: ";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return amount;
        }
    }
}

string getCategory() {
    string category;
    getline(cin, category);
    return category;
}

// --- Core features ---

void addTransaction(const string& type) {
    Transaction t;
    t.type = type;
    cout << "Enter category: ";
    t.category = getCategory();
    cout << "Enter amount: ";
    t.amount = getAmount();
    records.push_back(t);
    saveToFile();
    cout << type << " added successfully!\n";
}

void showBalance() {
    double income = 0, expense = 0;
    for (const auto& t : records) {
        if (t.type == "Income")  income  += t.amount;
        else                     expense += t.amount;
    }
    cout << fixed << setprecision(2);
    cout << "\nTotal Income  : " << income  << "\n"
         << "Total Expense : " << expense << "\n"
         << "Balance       : " << income - expense << "\n";
}

void showTransactions() {
    if (records.empty()) { cout << "No transactions found.\n"; return; }
    cout << "\n----- All Transactions -----\n";
    cout << fixed << setprecision(2);
    int i = 1;
    for (const auto& t : records)
        cout << i++ << ". " << t.type << " | " << t.category << " | " << t.amount << "\n";
}

void categoryReport() {
    map<string, double> totals;
    for (const auto& t : records)
        if (t.type == "Expense")
            totals[t.category] += t.amount;  // works for ANY category

    cout << "\n----- Expense by Category -----\n";
    cout << fixed << setprecision(2);
    if (totals.empty()) { cout << "No expenses recorded.\n"; return; }
    for (const auto& pair : totals)
        cout << pair.first << ": " << pair.second << "\n";
}

void deleteTransaction() {
    showTransactions();
    if (records.empty()) return;
    cout << "Enter transaction number to delete: ";
    int idx;
    cin >> idx;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (idx < 1 || idx > (int)records.size()) {
        cout << "Invalid number.\n"; return;
    }
    records.erase(records.begin() + idx - 1);
    saveToFile();
    cout << "Transaction deleted.\n";
}

// --- Main ---

int main() {
    loadFromFile();
    int choice;
    do {
        cout << "\n====== SMART EXPENSE TRACKER ======\n"
             << "1. Add Income\n"
             << "2. Add Expense\n"
             << "3. Show Balance\n"
             << "4. View Transactions\n"
             << "5. Category Report\n"
             << "6. Delete Transaction\n"
             << "7. Exit\n"
             << "Enter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (choice) {
            case 1: addTransaction("Income");  break;
            case 2: addTransaction("Expense"); break;
            case 3: showBalance();             break;
            case 4: showTransactions();        break;
            case 5: categoryReport();          break;
            case 6: deleteTransaction();       break;
            case 7: cout << "Goodbye!\n";      break;
            default: cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 7);

    return 0;
}