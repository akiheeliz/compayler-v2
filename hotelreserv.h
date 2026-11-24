#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include "useraccount.h"
#include "balances.h"
#include "transactions.h"
#include "vouchers.h"

using namespace std;

inline int hotelreserv(const UserAccount& selected){

    int roomCode, month, checkIn, checkOut;
    int days, totalSeconds, hours, minutes, seconds;
    double baseRate, baseCost, VAT, totalCost;
    double balance = getAccountBalance(selected.username);
    int userMPIN;
    int maxDays;
    bool unavailable;
    string monthName;
    string hasVoucher, voucherCode;
    double discount = 0.0;
    double discountedTotal = 0.0;
    bool voucherApplied = false;
    int voucherPercent = 0;

    cout << "====================================\n";
    cout << "       HOTEL RESERVATION SYSTEM     \n";
    cout << "====================================\n\n";

    cout << "AVAILABLE ROOMS\n";
    cout << "1. Standard Room - PHP 1500/day\n";
    cout << "2. Deluxe Room   - PHP 2500/day\n";
    cout << "3. Suite Room    - PHP 4000/day\n";

    cout << "\nEnter Room Code: ";
    cin >> roomCode;

    switch (roomCode) {
        case 1:
            baseRate = 1500;
            break;
        case 2:
            baseRate = 2500;
            break;
        case 3:
            baseRate = 4000;
            break;
        default:
            cout << "Error: Invalid Room Code\n";
            return 0;
    }

    cout << "\nMONTHS OF THE YEAR\n";
    cout << "1. January       7. July\n";
    cout << "2. February      8. August\n";
    cout << "3. March         9. September\n";
    cout << "4. April        10. October\n";
    cout << "5. May          11. November\n";
    cout << "6. June         12. December\n";

    cout << "\nSelect Month (1-12): ";
    cin >> month;

    if (month < 1 || month > 12) {
        cout << "Error: Invalid Month\n";
        return 0;
    }

    switch (month) {
        case 1: monthName = "January"; break;
        case 2: monthName = "February"; break;
        case 3: monthName = "March"; break;
        case 4: monthName = "April"; break;
        case 5: monthName = "May"; break;
        case 6: monthName = "June"; break;
        case 7: monthName = "July"; break;
        case 8: monthName = "August"; break;
        case 9: monthName = "September"; break;
        case 10: monthName = "October"; break;
        case 11: monthName = "November"; break;
        case 12: monthName = "December"; break;
    }

    if (month == 2) {
        maxDays = 28;
    }
    else if (month == 4 || month == 6 ||
             month == 9 || month == 11) {
        maxDays = 30;
    }
    else {
        maxDays = 31;
    }

    cout << "\n\t\t\tDATES\n\n";
    cout << "Sun\tMon\tTue\tWed\tThu\tFri\tSat\n";

    if (month == 11) {
        cout << "\t\t\t1\t2\t3\t4\n";
        cout << "5X\t6\t7\t8\t9\t10\t11\n";
        cout << "12X\t13\t14\t15\t16\t17\t18X\n";
        cout << "19\t20\t21\t22X\t23\t24\t25\n";
        cout << "26\t27X\t28\t29\t30\n";
    }
    else {
        cout << "\t1\t2\t3\t4\t5X\t6\n";
        cout << "7\t8\t9\t10\t11\t12X\t13\n";
        cout << "14\t15\t16\t17\t18X\t19\t20\n";
        cout << "21\t22X\t23\t24\t25\t26\t27X\n";
        cout << "28\t29\t30\t31\n";
    }

    cout << "\nX = OCCUPIED DATE\n";
    cout << "Other dates = AVAILABLE\n";

    int occupied[] = {5, 12, 18, 22, 27};

    do {
        unavailable = false;

        cout << "\nEnter Check-in Date: ";
        cin >> checkIn;

        cout << "Enter Check-out Date: ";
        cin >> checkOut;

        if (checkIn < 1 || checkIn > maxDays ||
            checkOut < 1 || checkOut > maxDays ||
            checkOut <= checkIn) {

            cout << "\nError: Invalid Date!\n";
            cout << "Please enter your dates again.\n";
            unavailable = true;
        }
        else {
            for (int i = 0; i < 5; i++) {

                if (checkIn <= occupied[i] &&
                    checkOut > occupied[i]) {

                    cout << "\nError: Selected dates are occupied!\n";
                    cout << "Please choose another date.\n";
                    unavailable = true;
                    break;
                }
            }
        }

    } while (unavailable);

    days = checkOut - checkIn;

    totalSeconds = days * 24 * 60 * 60;

    hours = totalSeconds / 3600;
    minutes = (totalSeconds % 3600) / 60;
    seconds = totalSeconds % 60;

    baseCost = baseRate * days;
    VAT = baseCost * 0.10;
    totalCost = baseCost + VAT;
    discountedTotal = totalCost;
    cout << fixed << setprecision(2);

    cout << "\n========== RESERVATION DETAILS ==========\n";

    cout << "Duration of Stay: " << days << " day(s)\n";
    cout << "Total Hours: " << hours << " hours\n";
    cout << "Total Minutes: " << totalSeconds / 60 << " minutes\n";
    cout << "Total Seconds: " << totalSeconds << " seconds\n";

    cout << "\nBase Room Cost: PHP " << baseCost << endl;
    cout << "VAT (10%): PHP " << VAT << endl;
    cout << "Total Room Cost: PHP " << totalCost << endl;

    // =====================================
    // VOUCHER PROCESS
    // =====================================

    printAvailableVouchersForService(selected.username, "Hotel");

    cout << "\nDo you have a voucher? (YES/NO): ";
    cin >> hasVoucher;

    if (hasVoucher == "YES" || hasVoucher == "yes" || hasVoucher == "Yes") {
        cout << "Enter Voucher Code: ";
        cin >> voucherCode;
        voucherCode = toUpperVoucherStr(voucherCode);

        discountedTotal = previewVoucherDiscount(
            selected.username,
            "Hotel",
            voucherCode,
            totalCost,
            voucherApplied,
            voucherPercent
        );
        discount = totalCost - discountedTotal;

        if (voucherApplied) {
            cout << "\nVoucher Successfully Applied (" << voucherPercent << "% OFF)!\n";
        } else {
            cout << "\nNo discount applied.\n";
        }
    } else {
        discountedTotal = totalCost;
        cout << "\nNo voucher applied.\n";
    }

    cout << "\nDiscount: PHP " << discount << endl;
    cout << "Final Total: PHP " << discountedTotal << endl;
    cout << "Available Balance: PHP " << balance << endl;

    if (balance < discountedTotal) {
        cout << "\nError: Insufficient Balance\n\n";
        cout << "       **            **"<<endl;
        cout << "         **        **"<<endl;
        cout << "           **    **"<<endl;
        cout << "              **"<<endl;
        cout << "     INSUFFICIENT BALANCE!"<<endl;
        cout << "              **   "<<endl;
        cout << "           **    **"<<endl;
        cout << "         **        **"<<endl;
        cout << "       **            **\n"<<endl;
        cout << "Reservation Cancelled.\n";
        return 0;
    }

    cout << "\nEnter your MPIN: ";
    cin >> userMPIN;

    if (userMPIN != stoi(selected.mpin)) {
        cout << "\nError: Incorrect PIN\n";
        cout << "Transaction Cancelled.\n";
        return 0;
    }

    balance = balance - discountedTotal;
    setAccountBalance(selected.username, balance);
    if (voucherApplied) {
        markVoucherUsed(selected.username, voucherCode, "Hotel");
    }
    recordTransaction(selected.username, "Hotel Reservations", discountedTotal);

    cout << "                     **" << endl;
    cout << "                    **" << endl;
    cout << "                   **" << endl;
    cout << "                  **" << endl;
    cout << "        **       **" << endl;
    cout << " TRANSACTION SUCCESSFUL!" << endl;
    cout << "          **   **" << endl;
    cout << "           ** **" << endl;
    cout << "            **\n" << endl;

    cout << "\n====================================\n";
    cout << "       RESERVATION CONFIRMED!       \n";
    cout << "====================================\n";

    cout << "Room Number: " << roomCode << endl;

    cout << "Check-in Date: " << monthName << " "
         << checkIn << endl;

    cout << "Check-out Date: " << monthName << " "
         << checkOut << endl;

    cout << "\nTOTAL DURATION OF STAY\n";

    cout << "Total Number of Days: "
         << days << " day(s)\n";

    cout << "Equivalent Hours: "
         << hours << " hour(s)\n";

    cout << "Equivalent Minutes: "
         << totalSeconds / 60 << " minute(s)\n";

    cout << "Equivalent Seconds: "
         << totalSeconds << " second(s)\n";

    cout << "\nDiscount: PHP " << discount << endl;
    cout << "Total Paid: PHP " << discountedTotal << endl;
    cout << "Remaining Balance: PHP " << balance << endl;

    cout << "\nThank you for choosing our ComPAYler hotel!\n";

    return 0;
}