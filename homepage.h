#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "useraccount.h"
#include "balances.h"
#include "transactions.h"
#include "cooldown.h"
#include "vouchers.h"

#include "balwindow.h"
#include "depwithtrans.h"
#include "shopping.h"
#include "hotelreserv.h"
#include "transpobook.h"

using namespace std;

int homepage(const UserAccount& selected) {
    char menuChoice;

    cout << fixed << setprecision(2);

    do {
        enforceCooldownIfActive(selected.username);
        cout << endl << endl;
        cout << "\t           ** **" << endl;
        cout << "\t           ** **" << endl;
        cout << "\t         ********* " << endl;
        cout << "\t       **  ** **  **  " << endl;
        cout << "\t      **   ** **" << endl;
        cout << "\t      **   ** **" << endl;
        cout << "\t      **   ** **" << endl;
        cout << "\t      **   ** **" << endl;
        cout << "\t       **  ** **  **   " << endl;
        cout << "\t         *********" << endl;
        cout << "\t           ** **" << endl;
        cout << "\t           ** **" << endl << endl;
        cout << "========== WELCOME TO COMPAYLER! ==========\n\n";
        cout << "Username: " << selected.username << endl;
        cout << "Account No.: " << selected.accountNumber << endl;
        cout << "Name: " << selected.givenname << " " << selected.lastname << endl;
        cout << "Date & Time: " << getCurrentDateTime() << endl;

        // =====================================
        // TRANSACTION HISTORY (PER USER FROM CSV)
        // =====================================

        vector<TransactionRecord> userTx = getUserTransactions(selected.username);

        cout << "\n========== TRANSACTION HISTORY ==========\n";
        cout << left << setw(12) << "Tx No."
             << setw(22) << "Category"
             << setw(15) << "Amount"
             << "Date / Time\n";

        if (userTx.empty()) {
            cout << "No transactions yet.\n";
        } else {
            for (const auto& tx : userTx) {
                cout << left << setw(12) << tx.txNumber
                     << setw(22) << tx.category
                     << "P" << setw(14) << tx.amount
                     << tx.timestamp << endl;
            }
        }

        std::cout << "\n--- YOUR VOUCHERS ---\n";
auto myVouchers = getUserVouchers(selected.username);
if (myVouchers.empty()) {
    std::cout << "No vouchers yet (1 generated every 3 transactions).\n";
} else {
    for (const auto& v : myVouchers) {
        std::cout << v.code << " | " << v.discountPercent << "% OFF | "
                  << v.service << " | " << (v.used ? "USED" : "AVAILABLE") << "\n";
    }
}

        // =====================================
        // MENU
        // =====================================
        cout << "\n========== MENU ==========\n";
        cout << "A - Balance Window\n";
        cout << "B - Deposit/Withdrawal/Transfer\n";
        cout << "C - Shopping\n";
        cout << "D - Hotel Reservations\n";
        cout << "E - Transportation\n";
        cout << "0 - Log-out\n";
        cout << "Select an Option: ";
        cin >> menuChoice;

        if (menuChoice == 'A' || menuChoice == 'a') {
            cout << "\nGoing to BALANCE WINDOW...\n";
            balwindow(selected);
        } else if (menuChoice == 'B' || menuChoice == 'b') {
            cout << "\nGoing to DEPOSIT/WITHDRAWAL/TRANSFER...\n";
            depwithtrans(selected);
        } else if (menuChoice == 'C' || menuChoice == 'c') {
            cout << "\nGoing to SHOPPING...\n";
            shopping(selected);
        } else if (menuChoice == 'D' || menuChoice == 'd') {
            cout << "\nGoing to HOTEL RESERVATIONS...\n";
            hotelreserv(selected);
        } else if (menuChoice == 'E' || menuChoice == 'e') {
            cout << "\nGoing to TRANSPORTATION...\n";
            transpobook(selected);
        } else if (menuChoice == '0') {
            cout << "\nLogging out...\n";
        } else {
            cout << "Error: Invalid Selection\n";
        }

    } while (menuChoice != '0');

    return true;
}