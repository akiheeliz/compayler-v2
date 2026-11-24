#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <cctype>
#include <iomanip>
#include <random>

#include "useraccount.h"
#include "balances.h"
#include "homepage.h"


using namespace std;

// Safely reads a full line of text with spaces...
string readLineInput(bool skipLeadingNewline = true) {
    if (skipLeadingNewline) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    string value;
    getline(cin, value);
    return value;
}

// Check if MPIN is 6 digits...
bool isValidMpin(const string& mpin) {
    if (mpin.length() != 6) {
        cout << "Error: MPIN must be exactly 6 digits long!\n";
        return false;
    }
    for (char c : mpin) {
        if (!isdigit(static_cast<unsigned char>(c))) {
            cout << "Error: MPIN must contain only digits!\n";
            return false;
        }
    }
    return true;
}

int generateUniqueAccountNumber(const string& filename = "users.csv") {
    vector<UserAccount> existing = loadUsersFromCsv(filename);
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<int> dist(100000, 999999);

    while (true) {
        int candidate = dist(gen);
        bool taken = false;
        for (const auto& acc : existing) {
            if (acc.accountNumber == candidate) {
                taken = true;
                break;
            }
        }
        if (!taken) {
            return candidate;
        }
    }
}

bool userExists(const string& username) {
    vector<UserAccount> accounts = loadUsersFromCsv("users.csv");
    for (const auto& acc : accounts) {
        if (acc.username == username) {
            return true;
        }
    }
    return false;
}

bool registerUser() {
    string givenname, lastname, address, mobile, username, password, repeatPassword, mpin, confirmMpin;
    int age;

    cout << "\n===== REGISTRATION =====\n";

    // 1. given name
    cout << "Enter given name: ";
    givenname = readLineInput(true);
    if (givenname.empty()) {
        cout << "Error: Given name cannot be empty!\n";
        return false;
    }

    // 2. last name
    cout << "Enter last name: ";
    lastname = readLineInput(false);
    if (lastname.empty()) {
        cout << "Error: Last name cannot be empty!\n";
        return false;
    }

    // 3. age
    cout << "Enter age: ";
    if (!(cin >> age)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Error: Please enter a valid number for age!\n";
        return false;
    }

    if (age < 18) {
        cout << "Error: You must be 18 or older to register!\n";
        return false;
    }

    // 4. address
    cout << "Enter address: ";
    address = readLineInput(true);
    if (address.empty()) {
        cout << "Error: Address cannot be empty!\n";
        return false;
    }

    // 5. mobile number
    cout << "Enter mobile number: ";
    mobile = readLineInput(false);
    if (mobile.empty()) {
        cout << "Error: Mobile number cannot be empty!\n";
        return false;
    }

    // 6. username
    cout << "Enter username: ";
    cin >> username;

    if (userExists(username)) {
        cout << "Error: Username '" << username << "' already exists!\n";
        return false;
    }

    // 7. password
    cout << "Enter password: ";
    password = readLineInput(true);
    if (password.empty()) {
        cout << "Error: Password cannot be empty!\n";
        return false;
    }

    // 8. repeat password
    cout << "Repeat password: ";
    repeatPassword = readLineInput(false);

    if (password != repeatPassword) {
        cout << "Error: Passwords do not match!\n";
        return false;
    }

    // 9. mpin
    cout << "Enter MPIN (6 digits only): ";
    cin >> mpin;

    if (!isValidMpin(mpin)) {
        return false;
    }

    // 10. confirm mpin
    cout << "Confirm MPIN: ";
    cin >> confirmMpin;

    if (mpin != confirmMpin) {
        cout << "Error: MPINs do not match!\n";
        return false;
    }

    int accountNumber = generateUniqueAccountNumber("users.csv");

    // Display all entered information for user confirmation (1 = Yes, 0 = No)
    cout << "\n===== CONFIRM YOUR REGISTRATION DETAILS =====\n";
    cout << "Given Name     : " << givenname << "\n";
    cout << "Last Name      : " << lastname << "\n";
    cout << "Age            : " << age << "\n";
    cout << "Address        : " << address << "\n";
    cout << "Mobile Number  : " << mobile << "\n";
    cout << "Username       : " << username << "\n";
    cout << "Password       : " << password << "\n";
    cout << "MPIN           : " << mpin << "\n";
    cout << "Account Number : " << accountNumber << "\n";
    cout << "===========================================\n";

    int confirmChoice;
    cout << "Confirm registration? (1 = Yes, 0 = No): ";
    if (!(cin >> confirmChoice) || (confirmChoice != 0 && confirmChoice != 1)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid choice! Registration cancelled. Nothing was saved.\n";
        return false;
    }

    if (confirmChoice == 0) {
        cout << "Registration cancelled. Nothing was saved.\n";
        return false;
    }

    ofstream file("users.csv", ios::app);
    if (!file) {
        cout << "Error: Could not open file for writing!\n";
        return false;
    }

    file << escapeCsv(givenname) << ","
         << escapeCsv(lastname) << ","
         << age << ","
         << escapeCsv(address) << ","
         << escapeCsv(mobile) << ","
         << escapeCsv(username) << ","
         << escapeCsv(password) << ","
         << escapeCsv(mpin) << ","
         << accountNumber << "\n";
    file.close();

    setAccountBalance(username, 0.0, "balances.csv");

    cout << "Registration successful! Your Account Number is: " << accountNumber << "\n";
    cout << "You can now log in.\n";
    return true;
}

bool loginUser() {
    syncBalancesWithUsers(0.0, "users.csv", "balances.csv");
    vector<UserAccount> accounts = loadUsersFromCsv("users.csv");

    if (accounts.empty()) {
        cout << "\nError: No registered users found. Please register first.\n";
        return false;
    }

    cout << "\n===== SAVED ACCOUNTS =====\n";
    for (size_t i = 0; i < accounts.size(); ++i) {
        cout << "#" << setw(3) << setfill('0') << (i + 1) << setfill(' ')
             << ": " << accounts[i].username
             << " (" << accounts[i].givenname << " " << accounts[i].lastname << ")\n";
    }
    cout << "0. Cancel\n";
    cout << "Select account number to login: ";

    int selection;
    if (!(cin >> selection)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input! Please enter a number.\n";
        return false;
    }

    if (selection == 0) {
        cout << "Login cancelled.\n";
        return false;
    }

    if (selection < 1 || static_cast<size_t>(selection) > accounts.size()) {
        cout << "Error: Invalid account selection!\n";
        return false;
    }

    const UserAccount& selected = accounts[selection - 1];

    string password, mpin;
    cout << "\nLogging in as: " << selected.username << "\n";
    cout << "Enter password: ";
    password = readLineInput(true);

    cout << "Enter MPIN: ";
    cin >> mpin;

    if (password == selected.password && mpin == selected.mpin) {
        cout << "\nLogin successful! Welcome, " << selected.givenname << "!\n";

        // INPUT FEATURES
        homepage(selected);
    }

    cout << "Error: Invalid password or MPIN!\n";
    return false;
}

int main() {
    syncBalancesWithUsers(0.0, "users.csv", "balances.csv");

    int choice;

    while (true) {
        cout << endl;
        cout << "            ** **" << endl;
        cout << "            ** **" << endl;
        cout << "          ********* " << endl;
        cout << "        **  ** **  **  " << endl;
        cout << "       **   ** **" << endl;
        cout << "       **   ** **" << endl;
        cout << "       **   ** **" << endl;
        cout << "       **   ** **" << endl;
        cout << "        **  ** **  **   " << endl;
        cout << "          *********" << endl;
        cout << "            ** **" << endl;
        cout << "            ** **" << endl;
        cout << "\n==============================\n";
        cout << "     REGISTRATION & LOGIN     \n";
        cout << "==============================\n";
        
    cout << "Welcome to Compayler!" << endl;
    cout << "Compile and Pay!" << endl;

        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                loginUser();
                break;
            case 3:
                cout << "Goodbye!\n";
                return 0;
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    }

    return 0;
}