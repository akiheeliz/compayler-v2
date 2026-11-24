#pragma once
#include <string>
#include <iostream>
#include "useraccount.h"
#include "balances.h"
#include "transactions.h"

int depwithtrans(const UserAccount& selected) {
    double balance = getAccountBalance(selected.username);

    std::string recipientUsername;
    int recipientAccountNumber;
    double recipientBalance;

    int depositCode = 111;
    int withdrawCode = 222;
    int transferCode = 333;

    int choice, mode, code, enteredMpin, accountNumber;
    double amount;
    std::string username;

        std::cout << "\n=== HOME PAGE ===\n";
        std::cout << "1 - Deposit\n2 - Withdrawal\n3 - Transfer\n";
        std::cout << "Choose: ";
        std::cin >> choice;

        while (choice != 1 && choice != 2 && choice != 3) {
            std::cout << "Error: Invalid choice.\n";
            std::cout << "Choose: ";
            std::cin >> choice;
        }

        if (choice == 1) {

            std::cout << "\n1 - Over-the-counter\n2 - Online bank transfer\n3 - E-wallet\n";
            std::cout << "Mode of deposit: ";
            std::cin >> mode;
            while (mode != 1 && mode != 2 && mode != 3) {
                std::cout << "Error: Invalid mode.\n";
                std::cout << "Mode of deposit: ";
                std::cin >> mode;
            }

            std::cout << "Enter amount of deposit: ";
            std::cin >> amount;
            while (amount <= 0) {
                std::cout << "Error: Amount must be greater than 0.\n";
                std::cout << "Enter amount of deposit: ";
                std::cin >> amount;
            }

            std::cout << "Enter deposit verification code: ";
            std::cin >> code;
            while (code != depositCode) {
                std::cout << "Error: Wrong verification code.\n";
                std::cout << "Enter deposit verification code: ";
                std::cin >> code;
            }

            std::cout << "Enter MPIN: ";
            std::cin >> enteredMpin;
            while (enteredMpin != std::stoi(selected.mpin)) {
                std::cout << "Error: Wrong MPIN.\n";
                std::cout << "Enter MPIN: ";
                std::cin >> enteredMpin;
            }

            balance = balance + amount;
            setAccountBalance(selected.username, balance);
            recordTransaction(selected.username, "Deposit", amount);
            cout << endl;
            cout << "                     **" << endl;
            cout << "                    **" << endl;
            cout << "                   **" << endl;
            cout << "                  **" << endl;
            cout << "        **       **" << endl;
            cout << " TRANSACTION SUCCESSFUL!" << endl;
            cout << "          **   **" << endl;
            cout << "           ** **" << endl;
            cout << "            **" << endl;
            cout << "\nDeposit successful. New balance: " << balance << "\n";
        }

        else if (choice == 2) {

            std::cout << "\n1 - ATM\n2 - Over-the-counter\n";
            std::cout << "Mode of withdrawal: ";
            std::cin >> mode;
            while (mode != 1 && mode != 2) {
                std::cout << "Error: Invalid mode.\n";
                std::cout << "Mode of withdrawal: ";
                std::cin >> mode;
            }

            std::cout << "Enter amount of withdrawal: ";
            std::cin >> amount;
            while (amount > balance || amount <= 0) {
                cout << endl << endl;
                cout << "       **            **"<<endl;
                cout << "         **        **"<<endl;
                cout << "           **    **"<<endl;
                cout << "              **"<<endl;
                cout << "     INSUFFICIENT BALANCE!"<<endl;
                cout << "              **   "<<endl;
                cout << "           **    **"<<endl;
                cout << "         **        **"<<endl;
                cout << "       **            **\n"<<endl;
                std::cout << "Enter amount of withdrawal: ";
                std::cin >> amount;
            }

            std::cout << "Enter withdrawal verification code: ";
            std::cin >> code;
            while (code != withdrawCode) {
                std::cout << "Error: Wrong verification code.\n";
                std::cout << "Enter withdrawal verification code: ";
                std::cin >> code;
            }

            std::cout << "Enter MPIN: ";
            std::cin >> enteredMpin;
            while (enteredMpin != std::stoi(selected.mpin)) {
                std::cout << "Error: Wrong MPIN.\n";
                std::cout << "Enter MPIN: ";
                std::cin >> enteredMpin;
            }

            balance = balance - amount;
            setAccountBalance(selected.username, balance);
recordTransaction(selected.username, "Withdrawal", amount);

            cout << endl;
                cout << "                     **" << endl;
                cout << "                    **" << endl;
                cout << "                   **" << endl;
                cout << "                  **" << endl;
                cout << "        **       **" << endl;
                cout << " TRANSACTION SUCCESSFUL!" << endl;
                cout << "          **   **" << endl;
                cout << "           ** **" << endl;
                cout << "            **" << endl;
            cout << "\nWithdrawal successful. New balance: " << balance << "\n";
        }

        else {

            std::cout << "\nEnter recipient username: ";
            std::cin >> username;
            std::cout << "Enter recipient account number: ";
            std::cin >> accountNumber;
  

            std::cout << "Enter amount to transfer: ";
            std::cin >> amount;
            while (amount > balance || amount <= 0) {
            cout << endl << endl;
            cout << "       **            **"<<endl;
            cout << "         **        **"<<endl;
            cout << "           **    **"<<endl;
            cout << "              **"<<endl;
            cout << "     INSUFFICIENT BALANCE!"<<endl;
            cout << "              **   "<<endl;
            cout << "           **    **"<<endl;
            cout << "         **        **"<<endl;
            cout << "       **            **\n"<<endl;
                std::cout << "Enter amount to transfer: ";
                std::cin >> amount;
            }

            std::cout << "Enter transfer verification code: ";
            std::cin >> code;
            while (code != transferCode) {
                std::cout << "Error: Wrong verification code.\n";
                std::cout << "Enter transfer verification code: ";
                std::cin >> code;
            }

            std::cout << "Enter MPIN: ";
            std::cin >> enteredMpin;
            while (enteredMpin != std::stoi(selected.mpin)) {
                std::cout << "Error: Wrong MPIN.\n";
                std::cin >> enteredMpin;
            }

            balance = balance - amount;
            setAccountBalance(selected.username, balance);
            recipientBalance = recipientBalance + amount;
            recordTransaction(selected.username, "Transfer Out", amount,
                  "To " + recipientUsername + " (#" + std::to_string(recipientAccountNumber) + ")");
recordTransaction(recipientUsername, "Transfer In", amount,
                  "From " + selected.username + " (#" + std::to_string(selected.accountNumber) + ")");

      cout << endl;
            cout << "                     **" << endl;
            cout << "                    **" << endl;
            cout << "                   **" << endl;
            cout << "                  **" << endl;
            cout << "        **       **" << endl;
            cout << " TRANSACTION SUCCESSFUL!" << endl;
            cout << "          **   **" << endl;
            cout << "           ** **" << endl;
            cout << "            **" << endl;
            cout << "\nTransfer successful. Sent " << amount << " to " << recipientUsername << ".\n";
            cout << "Your new balance: " << balance << "\n";
        }
    
    return 0;
}