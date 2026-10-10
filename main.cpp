#include <iostream>

#include <fstream>

#include <string>

#include <iomanip>

#include <cstdio>

#include <cstdlib>

#include <limits>

#include <conio.h>
charan/sprint-1-balance-storage

#include <sstream>

#include <ctime>

=======
#include <ctime>
#include <sstream>
main
using namespace std;

struct Account {

    int accountNumber;

    string name;

    string pin;

    double balance;

};

 charan/sprint-1-balance-storage
// Record a transaction after a successful balance update.
void recordTransaction(int accountNumber,
                      string transactionType,
                      double amount,
                      double balanceAfterTransaction);

=======
// Record a transaction in persistent storage
void recordTransaction(int accountNumber,
                       string transactionType,
                       double amount,
                       double balanceAfterTransaction) {

    ofstream file("transactions.txt", ios::app);

    if (!file) {
        cout << "\nError: Unable to open transaction file.\n";
        return;
    }

    time_t now = time(0);
    tm *localTime = localtime(&now);

    char timestamp[30];

    strftime(timestamp, sizeof(timestamp),
             "%Y-%m-%d %H:%M:%S", localTime);

    file << accountNumber << "|"
         << timestamp << "|"
         << transactionType << "|"
         << fixed << setprecision(2) << amount << "|"
         << fixed << setprecision(2)
         << balanceAfterTransaction << "\n";

    file.close();
}
main

// --------------------------------------------------

// Generate a new account number

// --------------------------------------------------

int generateAccountNumber() {

    ifstream file("accounts.txt");

    int lastAccountNumber = 1000;

    string line;

    while (getline(file, line)) {

        if (line.empty())

            continue;

        size_t pos = line.find('|');

        if (pos != string::npos) {

            int accountNumber = atoi(line.substr(0, pos).c_str());

            if (accountNumber > lastAccountNumber) {

                lastAccountNumber = accountNumber;

            }

        }

    }

    file.close();

    return lastAccountNumber + 1;

}

// --------------------------------------------------

// Save an account to the file

// --------------------------------------------------

void saveAccount(Account account) {

    ofstream file("accounts.txt", ios::app);

    if (!file) {

        cout << "\nError: Unable to open account storage file.\n";

        return;

    }

    file << account.accountNumber << "|"

         << account.name << "|"

         << account.pin << "|"

         << fixed << setprecision(2)

         << account.balance << "\n";

    file.close();

}

// --------------------------------------------------

// Find account by account number

// --------------------------------------------------

bool findAccount(int accountNumber, Account &account) {

    ifstream file("accounts.txt");

    string line;

    while (getline(file, line)) {

        if (line.empty())

            continue;

        size_t p1 = line.find('|');

        size_t p2 = line.find('|', p1 + 1);

        size_t p3 = line.find('|', p2 + 1);

        if (p1 == string::npos ||

            p2 == string::npos ||

            p3 == string::npos) {

            continue;

        }

        int storedAccountNumber =

            atoi(line.substr(0, p1).c_str());

        if (storedAccountNumber == accountNumber) {

            account.accountNumber = storedAccountNumber;

            account.name =

                line.substr(p1 + 1, p2 - p1 - 1);

            account.pin =

                line.substr(p2 + 1, p3 - p2 - 1);

            account.balance =

                atof(line.substr(p3 + 1).c_str());

            file.close();

            return true;

        }

    }

    file.close();

    return false;

}

// --------------------------------------------------

// Update an account in the file

// --------------------------------------------------

void updateAccount(Account updatedAccount) {

    ifstream inputFile("accounts.txt");

    ofstream tempFile("temp.txt");

    string line;

    while (getline(inputFile, line)) {

        if (line.empty())

            continue;

        size_t p1 = line.find('|');

        size_t p2 = line.find('|', p1 + 1);

        size_t p3 = line.find('|', p2 + 1);

        if (p1 == string::npos ||

            p2 == string::npos ||

            p3 == string::npos) {

            continue;

        }

        int accountNumber =

            atoi(line.substr(0, p1).c_str());

        if (accountNumber == updatedAccount.accountNumber) {

            tempFile << updatedAccount.accountNumber << "|"

                     << updatedAccount.name << "|"

                     << updatedAccount.pin << "|"

                     << fixed << setprecision(2)

                     << updatedAccount.balance << "\n";

        }

        else {

            tempFile << line << "\n";

        }

    }

    inputFile.close();

    tempFile.close();

    remove("accounts.txt");

    rename("temp.txt", "accounts.txt");

}

// --------------------------------------------------

// Validate 4-digit PIN

// --------------------------------------------------

bool validPIN(string pin) {

    if (pin.length() != 4)

        return false;

    for (int i = 0; i < 4; i++) {

        if (pin[i] < '0' || pin[i] > '9')

            return false;

    }

    return true;

}

string getMaskedPIN() {

    string pin;

    char ch;

    while (true) {

        ch = _getch();

        if (ch == '\r') {

            break;

        }

        if (ch == '\b') {

            if (!pin.empty()) {

                pin.erase(pin.length() - 1);

                cout << "\b \b";

            }

        }

        else if (ch >= '0' && ch <= '9') {

            if (pin.length() < 4) {

                pin += ch;

                cout << '*';

            }

        }

    }

    cout << endl;

    return pin;

}


// --------------------------------------------------

// Create Account

// --------------------------------------------------

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

    saveAccount(account);

    cout << "\nAccount created successfully!\n";

    cout << "Your Account Number: "

         << account.accountNumber << "\n";

    cout << "Initial Balance: Rs. "

         << fixed << setprecision(2)

         << account.balance << "\n";

}

// --------------------------------------------------

// Login

// --------------------------------------------------

bool login(Account &loggedInAccount) {

    int accountNumber;

    string pin;

    cout << "\n====================================\n";

    cout << "             LOGIN\n";

    cout << "====================================\n";

    cout << "Enter Account Number: ";

    cin >> accountNumber;
 charan/sprint-1-balance-storage

    if (cin.fail() || accountNumber <= 0) {

    cin.clear();

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "\nInvalid account number.\n";

    return false;

}

=======

main
    cout << "Enter PIN: ";

    pin = getMaskedPIN();

    Account account;

    if (findAccount(accountNumber, account)) {

        if (account.pin == pin) {

            loggedInAccount = account;

            cout << "\nLogin successful!\n";

            cout << "Welcome, " << account.name << "!\n";

            return true;

        }

    }

    cout << "\nInvalid account number or PIN.\n";

    return false;

}

// --------------------------------------------------

// Deposit Money

// --------------------------------------------------

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

updateAccount(account);

charan/sprint-1-balance-storage
recordTransaction(
=======
    updateAccount(account);
    account.balance += amount;

    updateAccount(account);

    recordTransaction(
    account.accountNumber,
    "DEPOSIT",
    amount,
    account.balance
    );
main

    account.accountNumber,

    "DEPOSIT",

    amount,

    account.balance

);

cout << "\nDeposit successful!\n";

    cout << "Deposited: Rs. "

         << fixed << setprecision(2)

         << amount << "\n";

    cout << "Updated Balance: Rs. "

         << fixed << setprecision(2)

         << account.balance << "\n";

}

// --------------------------------------------------

// Withdraw Money

// --------------------------------------------------

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

             << fixed << setprecision(2)

             << account.balance << "\n";

        return;

    }

account.balance -= amount;

updateAccount(account);

recordTransaction(

    account.accountNumber,

    "WITHDRAWAL",

    amount,

    account.balance

);

cout << "\nWithdrawal successful!\n";

    cout << "Withdrawn: Rs. "

         << fixed << setprecision(2)

         << amount << "\n";

    cout << "Updated Balance: Rs. "

         << fixed << setprecision(2)

         << account.balance << "\n";
charan/sprint-1-balance-storage

=======
         account.balance -= amount;

    updateAccount(account);

    recordTransaction(
    account.accountNumber,
    "WITHDRAWAL",
    amount,
    account.balance
    );
}

// Display saved transaction history
void showTransactionHistory(int accountNumber) {

    ifstream file("transactions.txt");

    if (!file) {
        cout << "\nNo transaction history available.\n";
        return;
    }

    string line;
    bool found = false;

    cout << "\n====================================\n";
    cout << "        TRANSACTION HISTORY\n";
    cout << "====================================\n";

    while (getline(file, line)) {

        stringstream ss(line);

        string storedAccount;
        string timestamp;
        string type;
        string amount;
        string balance;

        getline(ss, storedAccount, '|');
        getline(ss, timestamp, '|');
        getline(ss, type, '|');
        getline(ss, amount, '|');
        getline(ss, balance, '|');

      stringstream accountNumberStream;
      accountNumberStream << accountNumber;

      if (storedAccount == accountNumberStream.str()) {

            cout << "\nDate: " << timestamp;
            cout << "\nType: " << type;
            cout << "\nAmount: Rs. " << amount;
            cout << "\nBalance after transaction: Rs. "
                 << balance << "\n";

            found = true;
        }
    }

    if (!found) {
        cout << "\nNo transactions found for this account.\n";
    }

    file.close();
    main
}

// --------------------------------------------------

// Balance Inquiry

// --------------------------------------------------

void showBalance(Account account) {

    cout << "\n====================================\n";

    cout << "          BALANCE INQUIRY\n";

    cout << "====================================\n";

    cout << "Account Number: "

         << account.accountNumber << "\n";

    cout << "Account Holder: "

         << account.name << "\n";

    cout << "Current Balance: Rs. "

         << fixed << setprecision(2)

         << account.balance << "\n";
charan/sprint-1-balance-storage

}

void recordTransaction(int accountNumber,

                       string transactionType,

                       double amount,

                       double balanceAfterTransaction) {

    ofstream file("transactions.txt", ios::app);

    if (!file) {

        cout << "\nError: Unable to open transaction file.\n";

        return;

    }

    time_t now = time(0);

    tm *localTime = localtime(&now);

    char timestamp[30];

    strftime(timestamp, sizeof(timestamp),

             "%Y-%m-%d %H:%M:%S", localTime);

    file << accountNumber << "|"

         << timestamp << "|"

         << transactionType << "|"

         << fixed << setprecision(2) << amount << "|"

         << fixed << setprecision(2)

         << balanceAfterTransaction << "\n";

    file.close();

}

void showTransactionHistory(int accountNumber) {

    ifstream file("transactions.txt");

    if (!file) {

        cout << "\nNo transaction history available.\n";

        return;

    }

    string line;

    bool found = false;

    cout << "\n====================================\n";

    cout << "        TRANSACTION HISTORY\n";

    cout << "====================================\n";

    while (getline(file, line)) {

        stringstream ss(line);

        string storedAccount, timestamp, type, amount, balance;

        getline(ss, storedAccount, '|');

        getline(ss, timestamp, '|');

        getline(ss, type, '|');

        getline(ss, amount, '|');

        getline(ss, balance, '|');

        if (storedAccount == to_string(accountNumber)) {

            cout << "\nDate: " << timestamp;

            cout << "\nType: " << type;

            cout << "\nAmount: Rs. " << amount;

            cout << "\nBalance after transaction: Rs. "

                 << balance << "\n";

            found = true;

        }

    }

    if (!found) {

        cout << "\nNo transactions found for this account.\n";

    }

    file.close();

=======
    showTransactionHistory(account.accountNumber);
main
}

// --------------------------------------------------

// Customer Menu

// --------------------------------------------------

void customerMenu(Account &account) {

    int choice;

    do {

        cout << "\n====================================\n";

        cout << "          CUSTOMER MENU\n";

        cout << "====================================\n";

        cout << "1. Deposit Money\n";

        cout << "2. Withdraw Money\n";

        cout << "3. Balance Inquiry\n";

        cout << "4. Transaction History\n";

        cout << "5. Logout\n";

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

                showTransactionHistory(account.accountNumber);

                break;

            case 5:

                cout << "\nLogged out successfully.\n";

                break;

            default:

                cout << "\nInvalid choice. Please try again.\n";

        }

    } while (choice != 5);

}

// --------------------------------------------------

// Main

// --------------------------------------------------

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

            case 2:

            {

                Account loggedInAccount;

                if (login(loggedInAccount)) {

                    customerMenu(loggedInAccount);

                }

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
