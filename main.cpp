#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <ctime>
#include <sstream>
#include <conio.h>

using namespace std;

struct Account {
    int accountNumber;
    string name;
    string pin;
    double balance;
};

// Read a PIN without displaying its digits (Windows/Dev-C++ TDM-GCC).
string getMaskedPIN() {
    string pin;
    char ch;

    while (true) {
        ch = (char)_getch();

        if (ch == '\r' || ch == '\n') {
            cout << "\n";
            break;
        } else if (ch == '\b') {
            if (!pin.empty()) {
                pin.erase(pin.length() - 1);
                cout << "\b \b";
            }
        } else if (ch >= '0' && ch <= '9' && pin.length() < 4) {
            pin += ch;
            cout << "*";
        }
    }

    return pin;
}

bool validPIN(string pin) {
    if (pin.length() != 4)
        return false;

    for (int i = 0; i < 4; i++) {
        if (pin[i] < '0' || pin[i] > '9')
            return false;
    }
    return true;
}

// Find the next account number using the existing account file.
int generateAccountNumber() {
    ifstream file("accounts.txt");
    int lastAccountNumber = 1000;
    string line;

    while (getline(file, line)) {
        if (line.empty())
            continue;

        size_t pos = line.find('|');
        if (pos != string::npos) {
            int number = atoi(line.substr(0, pos).c_str());
            if (number > lastAccountNumber)
                lastAccountNumber = number;
        }
    }

    return lastAccountNumber + 1;
}

// Append a new account to persistent storage.
bool saveAccount(Account account) {
    ofstream file("accounts.txt", ios::app);
    if (!file) {
        cout << "\nError: Unable to open account storage file.\n";
        return false;
    }

    file << account.accountNumber << "|"
         << account.name << "|"
         << account.pin << "|"
         << fixed << setprecision(2) << account.balance << "\n";

    bool saved = (bool)file;
    file.close();
    return saved;
}

// Find an account by account number.
bool findAccount(int accountNumber, Account &account) {
    ifstream file("accounts.txt");
    string line;

    while (getline(file, line)) {
        if (line.empty())
            continue;

        size_t p1 = line.find('|');
        if (p1 == string::npos) continue;
        size_t p2 = line.find('|', p1 + 1);
        if (p2 == string::npos) continue;
        size_t p3 = line.find('|', p2 + 1);
        if (p3 == string::npos) continue;

        int storedNumber = atoi(line.substr(0, p1).c_str());
        if (storedNumber == accountNumber) {
            account.accountNumber = storedNumber;
            account.name = line.substr(p1 + 1, p2 - p1 - 1);
            account.pin = line.substr(p2 + 1, p3 - p2 - 1);
            account.balance = atof(line.substr(p3 + 1).c_str());
            return true;
        }
    }

    return false;
}

// Rewrite the account file with the updated account balance.
bool updateAccount(Account updatedAccount) {
    ifstream inputFile("accounts.txt");
    if (!inputFile) {
        cout << "\nError: Unable to read account storage file.\n";
        return false;
    }

    ofstream tempFile("temp.txt");
    if (!tempFile) {
        cout << "\nError: Unable to create temporary storage file.\n";
        return false;
    }

    string line;
    bool accountFound = false;

    while (getline(inputFile, line)) {
        if (line.empty())
            continue;

        size_t p1 = line.find('|');
        if (p1 == string::npos) {
            tempFile << line << "\n";
            continue;
        }

        int number = atoi(line.substr(0, p1).c_str());

        if (number == updatedAccount.accountNumber) {
            tempFile << updatedAccount.accountNumber << "|"
                     << updatedAccount.name << "|"
                     << updatedAccount.pin << "|"
                     << fixed << setprecision(2)
                     << updatedAccount.balance << "\n";
            accountFound = true;
        } else {
            tempFile << line << "\n";
        }
    }

    inputFile.close();
    tempFile.close();

    if (!tempFile || !accountFound) {
        remove("temp.txt");
        cout << "\nError: Account update could not be prepared.\n";
        return false;
    }

    // Keep a backup so the original is recoverable if replacement fails.
    remove("accounts_backup.txt");
    if (rename("accounts.txt", "accounts_backup.txt") != 0) {
        cout << "\nError: Could not back up account storage file.\n";
        remove("temp.txt");
        return false;
    }

    if (rename("temp.txt", "accounts.txt") != 0) {
        cout << "\nError: Could not finalize account storage update.\n";
        rename("accounts_backup.txt", "accounts.txt");
        remove("temp.txt");
        return false;
    }

    remove("accounts_backup.txt");
    return true;
}

// Append a timestamped transaction to persistent transaction history.
// Format: account|timestamp|type|amount|balance-after
void recordTransaction(int accountNumber, string transactionType,
                       double amount, double balanceAfterTransaction) {
    ofstream file("transactions.txt", ios::app);

    if (!file) {
        cout << "\nWarning: Unable to open transaction history file.\n";
        return;
    }

    time_t now = time(0);
    tm *localTime = localtime(&now);
    char timestamp[30];

    if (localTime != NULL) {
        strftime(timestamp, sizeof(timestamp),
                 "%Y-%m-%d %H:%M:%S", localTime);
    } else {
        snprintf(timestamp, sizeof(timestamp), "timestamp-unavailable");
    }

    file << accountNumber << "|"
         << timestamp << "|"
         << transactionType << "|"
         << fixed << setprecision(2) << amount << "|"
         << fixed << setprecision(2) << balanceAfterTransaction << "\n";

    if (!file)
        cout << "\nWarning: Transaction record could not be saved.\n";

    file.close();
}

// Display persistent transaction history for one account.
void showTransactionHistory(int accountNumber) {
    ifstream file("transactions.txt");

    cout << "\n====================================\n";
    cout << "        TRANSACTION HISTORY\n";
    cout << "====================================\n";

    if (!file) {
        cout << "No transaction history available yet.\n";
        return;
    }

    string line;
    bool found = false;

    while (getline(file, line)) {
        if (line.empty())
            continue;

        stringstream ss(line);
        string storedAccount, timestamp, type, amount, balance;

        getline(ss, storedAccount, '|');
        getline(ss, timestamp, '|');
        getline(ss, type, '|');
        getline(ss, amount, '|');
        getline(ss, balance, '|');

        stringstream accountNumberStream;
        accountNumberStream << accountNumber;

        if (storedAccount == accountNumberStream.str()) {
            cout << "Date: " << timestamp << "\n";
            cout << "Type: " << type << "\n";
            cout << "Amount: Rs. " << amount << "\n";
            cout << "Balance after transaction: Rs. " << balance << "\n";
            cout << "------------------------------------\n";
            found = true;
        }
    }

    if (!found)
        cout << "No transactions found for this account.\n";

    file.close();
}

// Create a new account.
void createAccount() {
    Account account;
    account.accountNumber = generateAccountNumber();

    cout << "\n====================================\n";
    cout << "          CREATE ACCOUNT\n";
    cout << "====================================\n";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter your full name: ";
    getline(cin, account.name);

    while (account.name.empty()) {
        cout << "Name cannot be empty.\n";
        cout << "Enter your full name: ";
        getline(cin, account.name);
    }

    cout << "Create a 4-digit PIN: ";
    account.pin = getMaskedPIN();

    while (!validPIN(account.pin)) {
        cout << "Invalid PIN. PIN must contain exactly 4 digits.\n";
        cout << "Enter a 4-digit PIN: ";
        account.pin = getMaskedPIN();
    }

    account.balance = 0.0;

    if (!saveAccount(account)) {
        cout << "Account creation failed because storage was unavailable.\n";
        return;
    }

    cout << "\nAccount created successfully!\n";
    cout << "Your Account Number: " << account.accountNumber << "\n";
    cout << "Initial Balance: Rs. "
         << fixed << setprecision(2) << account.balance << "\n";
}

// Customer login.
bool login(Account &loggedInAccount) {
    int accountNumber;
    string pin;

    cout << "\n====================================\n";
    cout << "             LOGIN\n";
    cout << "====================================\n";
    cout << "Enter Account Number: ";
    cin >> accountNumber;

    if (cin.fail() || accountNumber <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\nInvalid account number.\n";
        return false;
    }

    cout << "Enter PIN: ";
    pin = getMaskedPIN();

    Account account;
    if (findAccount(accountNumber, account) && account.pin == pin) {
        loggedInAccount = account;
        cout << "\nLogin successful!\n";
        cout << "Welcome, " << account.name << "!\n";
        return true;
    }

    cout << "\nInvalid account number or PIN.\n";
    return false;
}

// Deposit money.
void deposit(Account &account) {
    double amount;

    cout << "\n====================================\n";
    cout << "             DEPOSIT\n";
    cout << "====================================\n";
    cout << "Enter deposit amount: Rs. ";
    cin >> amount;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a numeric amount.\n";
        return;
    }

    if (amount <= 0) {
        cout << "Invalid amount. Deposit must be greater than zero.\n";
        return;
    }

    account.balance += amount;

    if (!updateAccount(account)) {
        account.balance -= amount;
        cout << "Deposit could not be saved. Please try again.\n";
        return;
    }

    recordTransaction(account.accountNumber, "DEPOSIT",
                      amount, account.balance);

    cout << "\nDeposit successful!\n";
    cout << "Deposited: Rs. " << fixed << setprecision(2) << amount << "\n";
    cout << "Updated Balance: Rs. "
         << fixed << setprecision(2) << account.balance << "\n";
}

// Withdraw money.
void withdrawMoney(Account &account) {
    double amount;

    cout << "\n====================================\n";
    cout << "            WITHDRAW\n";
    cout << "====================================\n";
    cout << "Enter withdrawal amount: Rs. ";
    cin >> amount;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a numeric amount.\n";
        return;
    }

    if (amount <= 0) {
        cout << "Invalid amount. Withdrawal must be greater than zero.\n";
        return;
    }

    if (amount > account.balance) {
        cout << "\nWithdrawal failed.\n";
        cout << "Insufficient balance.\n";
        cout << "Current Balance: Rs. "
             << fixed << setprecision(2) << account.balance << "\n";
        return;
    }

    account.balance -= amount;

    if (!updateAccount(account)) {
        account.balance += amount;
        cout << "Withdrawal could not be saved. Please try again.\n";
        return;
    }

    recordTransaction(account.accountNumber, "WITHDRAWAL",
                      amount, account.balance);

    cout << "\nWithdrawal successful!\n";
    cout << "Withdrawn: Rs. " << fixed << setprecision(2) << amount << "\n";
    cout << "Updated Balance: Rs. "
         << fixed << setprecision(2) << account.balance << "\n";
}

// Balance inquiry and saved transaction history.
void showBalance(Account account) {
    cout << "\n====================================\n";
    cout << "          BALANCE INQUIRY\n";
    cout << "====================================\n";
    cout << "Account Number: " << account.accountNumber << "\n";
    cout << "Account Holder: " << account.name << "\n";
    cout << "Current Balance: Rs. "
         << fixed << setprecision(2) << account.balance << "\n";

    showTransactionHistory(account.accountNumber);
}

// Customer menu.
void customerMenu(Account &account) {
    int choice;

    do {
        cout << "\n====================================\n";
        cout << "          CUSTOMER MENU\n";
        cout << "====================================\n";
        cout << "1. Deposit Money\n";
        cout << "2. Withdraw Money\n";
        cout << "3. Balance Inquiry and Transaction History\n";
        cout << "4. Logout\n";
        cout << "\nEnter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nInvalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1:
                deposit(account);
                break;
            case 2:
                withdrawMoney(account);
                break;
            case 3:
                showBalance(account);
                break;
            case 4:
                cout << "\nLogged out successfully.\n";
                break;
            default:
                cout << "\nInvalid choice. Please try again.\n";
        }
    } while (choice != 4);
}

int main() {
    int choice;

    do {
        cout << "\n====================================\n";
        cout << "       BANK MANAGEMENT SYSTEM\n";
        cout << "====================================\n";
        cout << "1. Create Account\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "\nEnter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nInvalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1:
                createAccount();
                break;
            case 2: {
                Account loggedInAccount;
                if (login(loggedInAccount))
                    customerMenu(loggedInAccount);
                break;
            }
            case 3:
                cout << "\nThank you for using Bank Management System.\n";
                break;
            default:
                cout << "\nInvalid choice. Please try again.\n";
        }
    } while (choice != 3);

    return 0;
}
