#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include "useraccount.h"
#include "balances.h"
#include "transactions.h"
#include "vouchers.h"

using namespace std;

inline int shopping(const UserAccount& selected) {

    string Item_Code, Product_Name, Choice;
    string Voucher_Code, Has_Voucher;
    int User_MPIN;

    int Quantity;

    float Amount_Per_Item, Line_Total;
    float Subtotal = 0, VAT, Total_Cost;
    float Discount, Discounted_Total;
    float Tendered = 0, Change = 0;
    double Available_Balance = getAccountBalance(selected.username);

    cout << fixed << setprecision(2);

    cout << "========== WELCOME TO COMPAYLER! ==========\n\n";

    cout << "AVAILABLE ITEMS AND PRICE LISTS\n";
    cout << "01 - Baggy Pants - P299.00\n";
    cout << "02 - Wireless Headphones - P399.00\n";
    cout << "03 - Mini Vacuum Cleaner - P350.00\n";
    cout << "04 - Perfume - P250.00\n";
    cout << "05 - Shoulder Bag - P299.00\n";
    cout << "06 - Oversized T-Shirt - P199.00\n";
    cout << "07 - Sneakers - P299.00\n";
    cout << "08 - Wristwatch - P250.00\n";
    cout << "09 - Water Bottle - P160.00\n";
    cout << "10 - Power Bank - P499.00\n";
    cout << "11 - Wallet - P180.00\n";
    cout << "12 - Desk Lamp - P275.00\n";

    // =====================================
    // SHOPPING PROCESS
    // =====================================

    do {

        cout << "\nEnter Item Code: ";
        cin >> Item_Code;

        if (Item_Code == "01") {
            Product_Name = "Baggy Pants";
            Amount_Per_Item = 299.00;
        }
        else if (Item_Code == "02") {
            Product_Name = "Wireless Headphones";
            Amount_Per_Item = 399.00;
        }
        else if (Item_Code == "03") {
            Product_Name = "Mini Vacuum Cleaner";
            Amount_Per_Item = 350.00;
        }
        else if (Item_Code == "04") {
            Product_Name = "Perfume";
            Amount_Per_Item = 250.00;
        }
        else if (Item_Code == "05") {
            Product_Name = "Shoulder Bag";
            Amount_Per_Item = 299.00;
        }
        else if (Item_Code == "06") {
            Product_Name = "Oversized T-Shirt";
            Amount_Per_Item = 199.00;
        }
        else if (Item_Code == "07") {
            Product_Name = "Sneakers";
            Amount_Per_Item = 299.00;
        }
        else if (Item_Code == "08") {
            Product_Name = "Wristwatch";
            Amount_Per_Item = 250.00;
        }
        else if (Item_Code == "09") {
            Product_Name = "Water Bottle";
            Amount_Per_Item = 160.00;
        }
        else if (Item_Code == "10") {
            Product_Name = "Power Bank";
            Amount_Per_Item = 499.00;
        }
        else if (Item_Code == "11") {
            Product_Name = "Wallet";
            Amount_Per_Item = 180.00;
        }
        else if (Item_Code == "12") {
            Product_Name = "Desk Lamp";
            Amount_Per_Item = 275.00;
        }
        else {
            cout << "Error: Invalid Item Code\n";
            return 0;
        }

        cout << "Enter Quantity: ";
        cin >> Quantity;

        if (Quantity <= 0) {
            cout << "Error: Invalid Quantity\n";
            return 0;
        }

        Line_Total = Quantity * Amount_Per_Item;
        Subtotal = Subtotal + Line_Total;

        cout << "\nProduct Added Successfully!\n";
        cout << "Product Name: " << Product_Name << endl;
        cout << "Quantity: " << Quantity << endl;
        cout << "Price per Item: P" << Amount_Per_Item << endl;
        cout << "Line Total: P" << Line_Total << endl;
        cout << "Current Subtotal: P" << Subtotal << endl;

        cout << "\nDo you want to purchase another item? (YES/NO): ";
        cin >> Choice;

    } while (Choice == "YES" || Choice == "yes" ||
             Choice == "Yes");

    // =====================================
    // PAYMENT SUMMARY
    // =====================================

    VAT = Subtotal * 0.10;
    Total_Cost = Subtotal + VAT;

    cout << "\n========== PAYMENT SUMMARY ==========\n";
    cout << "Subtotal: P" << Subtotal << endl;
    cout << "VAT (10%): P" << VAT << endl;
    cout << "Total Cost: P" << Total_Cost << endl;

    // =====================================
    // VOUCHER PROCESS
    // =====================================

    Discount = 0;
    bool voucherApplied = false;
    int voucherPercent = 0;

    printAvailableVouchersForService(selected.username, "Shopping");

    cout << "\nDo you have a voucher? (YES/NO): ";
    cin >> Has_Voucher;

    if (Has_Voucher == "YES" || Has_Voucher == "yes" ||
        Has_Voucher == "Yes") {

        cout << "Enter Voucher Code: ";
        cin >> Voucher_Code;
        Voucher_Code = toUpperVoucherStr(Voucher_Code);

        double discountedVal = previewVoucherDiscount(
            selected.username,
            "Shopping",
            Voucher_Code,
            static_cast<double>(Total_Cost),
            voucherApplied,
            voucherPercent
        );

        Discounted_Total = static_cast<float>(discountedVal);
        Discount = Total_Cost - Discounted_Total;

        if (voucherApplied) {
            cout << "\nVoucher Successfully Applied (" << voucherPercent << "% OFF)!\n";
        }
        else {
            cout << "\nNo discount applied.\n";
        }
    }
    else {
        Discounted_Total = Total_Cost;
        cout << "\nNo voucher applied.\n";
    }

    cout << "\nDiscount: P" << Discount << endl;
    cout << "Final Total: P" << Discounted_Total << endl;
    cout << "Available Balance: P" << Available_Balance << endl;

    // =====================================
    // MPIN VERIFICATION
    // =====================================

    cout << "Enter MPIN: ";
    cin >> User_MPIN;

    if (User_MPIN != stoi(selected.mpin)) {

        cout << "\nError: Incorrect PIN\n";
        cout << "Transaction Cancelled\n";

        return 0;
    }

    if (Available_Balance < Discounted_Total) {

        cout << "\nError: Insufficient Amount Tendered\n\n";
        cout << "       **            **"<<endl;
        cout << "         **        **"<<endl;
        cout << "           **    **"<<endl;
        cout << "              **"<<endl;
        cout << "     INSUFFICIENT BALANCE!"<<endl;
        cout << "              **   "<<endl;
        cout << "           **    **"<<endl;
        cout << "         **        **"<<endl;
        cout << "       **            **\n"<<endl;
        cout << "Transaction Cancelled\n";

        return 0;
    }
    else {

        Tendered = Discounted_Total;
        Change = Tendered - Discounted_Total;

        Available_Balance = Available_Balance - Discounted_Total;
        setAccountBalance(selected.username, Available_Balance);
        if (voucherApplied) {
            markVoucherUsed(selected.username, Voucher_Code, "Shopping");
        }
        recordTransaction(selected.username, "Shopping", Discounted_Total);

        // =====================================
        // COMPAYLER RECEIPT
        // =====================================

        cout << "\n============= COMPAYLER RECEIPT ==========\n";
        cout << "Subtotal: P" << Subtotal << endl;
        cout << "VAT (10%): P" << VAT << endl;
        cout << "Discount: P" << Discount << endl;
        cout << "Final Total: P" << Discounted_Total << endl;
        cout << "Amount Tendered: P" << Tendered << endl;
        cout << "Change: P" << Change << endl;
        cout << "Remaining Balance: P" << Available_Balance << endl;
        cout << "=======================================\n\n";
        cout << "                     **" << endl;
        cout << "                    **" << endl;
        cout << "                   **" << endl;
        cout << "                  **" << endl;
        cout << "        **       **" << endl;
        cout << " TRANSACTION SUCCESSFUL!" << endl;
        cout << "          **   **" << endl;
        cout << "           ** **" << endl;
        cout << "            **" << endl;

        cout << "\nTransaction Successful!\n";
    }

    cout << "\nShopping Session Ended\n";

    return 0;
}