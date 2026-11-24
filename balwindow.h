#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include "balances.h"
#include "useraccount.h"
using namespace std;

string belowThousand(int n) {
    string ones[20] = {"", "One", "Two", "Three", "Four", "Five", "Six",
                       "Seven", "Eight", "Nine", "Ten", "Eleven", "Twelve",
                       "Thirteen", "Fourteen", "Fifteen", "Sixteen",
                       "Seventeen", "Eighteen", "Nineteen"};
    string tens[10] = {"", "", "Twenty", "Thirty", "Forty", "Fifty",
                       "Sixty", "Seventy", "Eighty", "Ninety"};
    string result = "";

    if (n >= 100) {
        result += ones[n / 100] + " Hundred ";
        n %= 100;
    }
    if (n >= 20) {
        result += tens[n / 10];
        if (n % 10 != 0) result += "-" + ones[n % 10];
    }
    else if (n > 0) {
        result += ones[n];
    }
    return result;
}

string numberToWords(long long n) {
    if (n == 0) return "Zero";

    string result = "";
    if (n >= 1000000) {
        result += belowThousand(n / 1000000) + " Million ";
        n %= 1000000;
    }
    if (n >= 1000) {
        result += belowThousand(n / 1000) + " Thousand ";
        n %= 1000;
    }
    if (n > 0) {
        result += belowThousand(n);
    }
    return result;
}

int balwindow(const UserAccount& selected){
string userName = selected.username;
    string givenName = selected.givenname, lastName = selected.lastname;
    double available_balance = getAccountBalance(selected.username);
    int selectChoice;

    cout << fixed << setprecision(2);

    cout << "\n========== BALANCE WINDOW ==========\n";
    cout << "Username: " << userName << endl;
    cout << "Name: " << givenName << " " << lastName << endl;
    cout << "Available Balance: P" << available_balance << endl;

    long long pesos = (long long)available_balance;
    int centavos = (int)((available_balance - pesos) * 100 + 0.5);

    cout << "Balance in Words: " << numberToWords(pesos) << " Pesos";
    if (centavos > 0) {
        cout << " and " << numberToWords(centavos) << " Centavos";
    }
    cout << endl;

    // =====================================
    // EVEN / ODD
    // =====================================

    if (pesos % 2 == 0) {
        cout << "\nEven amount of balance\n";
    }
    else {
        cout << "\nOdd amount of balance\n";
    }

    // =====================================
    // BALANCE LEVEL
    // =====================================

    if (available_balance <= 100) {
        cout << "Account balance read is poor balance\n";
    }
    else if (available_balance <= 500) {
        cout << "Account balance read is low balance\n";
    }
    else if (available_balance <= 1000) {
        cout << "Account balance read is medium balance\n";
    }
    else if (available_balance <= 5000) {
        cout << "Account balance read is high\n";
    }
    else {
        cout << "Account balance read is very high\n";
    }

    // =====================================
    // RETURN TO HOME PAGE
    // =====================================

    do {
        cout << "\nSelect 0 to return to HOME PAGE: ";
        cin >> selectChoice;

        if (selectChoice != 0) {
            cout << "Error: Invalid Selection\n";
        }
    } while (selectChoice != 0);

    cout << "\nReturning to HOME PAGE...\n";

return 0;
}