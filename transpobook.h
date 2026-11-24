#pragma once
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <cmath>
#include <functional>
#include <algorithm>

#include "balances.h"
#include "useraccount.h"
#include "transactions.h"
#include "vouchers.h"

// -----------------------------------------
// TRANSPORTATION DATA
struct Driver {
    std::string name;
    std::string plate;
};

struct Vehicle {
    int code;
    std::string type;
    double baseRate;
    double ratePerKm;
    std::vector<Driver> drivers;
};

struct Ride {
    std::string vehicle;
    std::string pickup;
    std::string dropoff;
    std::string driver;
    std::string plate;
    double distance;
    double baseFare;
    double vat;
    double totalFare;
};

// -----------------------------------------
// RANDOM NUMBER GENERATOR
inline std::mt19937& getRng() {
    static std::mt19937 rng(std::random_device{}());
    return rng;
}

inline int randomNumber(int min, int max) {
    return std::uniform_int_distribution<int>(min, max)(getRng());
}

// -----------------------------------------
// MONEY FORMAT
inline double roundMoney(double amount) {
    return std::round(amount * 100.0) / 100.0;
}

inline std::string money(double amount) {
    std::ostringstream out;
    out << "PHP " << std::fixed << std::setprecision(2) << amount;
    return out.str();
}

// -----------------------------------------
// INPUT VALIDATION
inline std::string readRequired(const std::string& prompt) {
    std::string input;

    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);

        if (!input.empty() &&
            input.find_first_not_of(" \t") != std::string::npos) {
            return input;
        }

        std::cout << "Error: This field cannot be empty.\n";
    }
}

inline int readInt(const std::string& prompt, int min, int max) {
    int value;

    while (true) {
        std::cout << prompt;

        if (std::cin >> value && value >= min && value <= max) {
            std::cin.ignore(10000, '\n');
            return value;
        }

        std::cout << "Error: Enter a number from "
                  << min << " to " << max << ".\n";

        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
}

inline bool confirmBooking() {
    std::string answer;

    while (true) {
        std::cout << "Confirm booking? (YES/NO): ";
        std::getline(std::cin, answer);

        std::transform(answer.begin(), answer.end(),
                       answer.begin(), ::toupper);

        if (answer == "YES" || answer == "Y")
            return true;

        if (answer == "NO" || answer == "N")
            return false;

        std::cout << "Please enter YES or NO.\n";
    }
}

// -----------------------------------------
// VEHICLE LIST
inline std::vector<Vehicle> getVehicles() {
    return {
        {1, "Motorcycle", 50, 10,
            {{"Juan Dela Cruz", "NBC 1234"},
             {"Pedro Reyes", "ABC 5678"}}},

        {2, "Car", 80, 15,
            {{"Maria Santos", "DEF 2468"},
             {"Jose Rizal", "GHI 1357"}}},

        {3, "Van", 120, 20,
            {{"Andres Bonifacio", "VAN 9087"}}}
    };
}

// -----------------------------------------
// DISTANCE CALCULATION
inline double calculateDistance(const std::string& pickup, const std::string& dropoff) {
    size_t h = std::hash<std::string>{}(pickup) +
               std::hash<std::string>{}(dropoff);

    return 1.0 + (h % 250) / 10.0;
}

// -----------------------------------------
// TRANSPORTATION SYSTEM
inline void transportation(const UserAccount& user, double &balance) {

    std::cout << "\n========================================\n";
    std::cout << "       COMPAYLER TRANSPORTATION\n";
    std::cout << "========================================\n";

    if (balance <= 0) {
        std::cout << "Error: Insufficient Balance.\n";
        return;
    }

    std::vector<Vehicle> vehicles = getVehicles();

    std::cout << "\nAVAILABLE VEHICLES AND RATES\n";

    for (const auto &v : vehicles) {
        std::cout << std::setfill('0') << std::setw(2) << v.code
                  << std::setfill(' ') << " - " << v.type
                  << " - Base Rate: " << money(v.baseRate)
                  << " + " << money(v.ratePerKm) << " per km\n";
    }

    // VEHICLE SELECTION
    int code = readInt("\nEnter vehicle code (1-3): ", 1, 3);

    Vehicle selected = vehicles[code - 1];

    // PICKUP AND DROP-OFF
    std::string pickup = readRequired("Enter pickup location: ");
    std::string dropoff = readRequired("Enter drop-off location: ");

    // DISTANCE
    double distance = calculateDistance(pickup, dropoff);

    // FARE CALCULATION
    double baseFare = roundMoney(
        selected.baseRate + distance * selected.ratePerKm
    );

    double vat = roundMoney(baseFare * 0.10);

    double totalFare = roundMoney(baseFare + vat);

    // FARE DETAILS
    std::cout << "\n========== FARE SUMMARY ==========\n";
    std::cout << "Vehicle Type: " << selected.type << "\n";
    std::cout << "Pickup: " << pickup << "\n";
    std::cout << "Drop-off: " << dropoff << "\n";

    std::cout << std::fixed << std::setprecision(1);
    std::cout << "Distance: " << distance << " km\n";

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Base Fare: " << money(baseFare) << "\n";
    std::cout << "VAT (10%): " << money(vat) << "\n";
    std::cout << "Total Fare: " << money(totalFare) << "\n";
    std::cout << "Available Balance: " << money(balance) << "\n";
    std::cout << "==================================\n";

    // VOUCHER PROCESS
    printAvailableVouchersForService(user.username, "Transportation");

    std::string voucherCode;
    bool voucherApplied = false;
    int voucherPercent = 0;

    std::cout << "Enter Transportation voucher code (or press Enter to skip): ";
    std::getline(std::cin, voucherCode);
    voucherCode = toUpperVoucherStr(voucherCode);

    if (!voucherCode.empty()) {
        totalFare = previewVoucherDiscount(
            user.username,
            "Transportation",
            voucherCode,
            totalFare,
            voucherApplied,
            voucherPercent
        );
        if (voucherApplied) {
            std::cout << voucherPercent << "% discount applied! New Total: "
                      << money(totalFare) << "\n";
        }
    }

    // BALANCE VALIDATION
    if (balance < totalFare) {
        std::cout << "\nError: Insufficient Balance.\n";
        std::cout << "Booking Cancelled.\n";
        return;
    }

    // BOOKING CONFIRMATION
    if (!confirmBooking()) {
        std::cout << "\nBooking Cancelled.\n";
        return;
    }

    // DRIVER AVAILABILITY
    std::cout << "\nSearching for nearby drivers...\n";

    if (randomNumber(1, 100) <= 15) {
        std::cout << "Error: No Drivers Available.\n";
        std::cout << "Booking Cancelled. No amount deducted.\n";
        return;
    }

    // DRIVER ASSIGNMENT
    int index = randomNumber(
        0, (int)selected.drivers.size() - 1
    );

    Driver assigned = selected.drivers[index];

    // PAYMENT
    balance = roundMoney(balance - totalFare);
    setAccountBalance(user.username, balance);
    if (voucherApplied) {
        markVoucherUsed(user.username, voucherCode, "Transportation");
    }
    recordTransaction(user.username, "Transportation", totalFare);

    // RIDE RECEIPT
    std::cout << "\n========================================\n";
    std::cout << "          COMPAYLER RIDE RECEIPT\n";
    std::cout << "========================================\n";

    std::cout << "Vehicle Type: " << selected.type << "\n";
    std::cout << "Pickup Location: " << pickup << "\n";
    std::cout << "Drop-off Location: " << dropoff << "\n";
    std::cout << "Distance: " << distance << " km\n";

    std::cout << "\nDRIVER INFORMATION\n";
    std::cout << "Driver Name: " << assigned.name << "\n";
    std::cout << "Plate Number: " << assigned.plate << "\n";
    std::cout << "Estimated Arrival: "
              << randomNumber(3, 12) << " minutes\n";

    std::cout << "\nPAYMENT DETAILS\n";
    std::cout << "Base Fare: " << money(baseFare) << "\n";
    std::cout << "VAT (10%): " << money(vat) << "\n";
    std::cout << "Final Total: " << money(totalFare) << "\n";

    std::cout << "Remaining Balance: " << money(balance) << "\n";

    std::cout << "========================================\n";
    std::cout << "Booking Confirmed!\n";
    std::cout << "Thank you for riding with COMPAYLER!\n";
    std::cout << "========================================\n";
}

// -----------------------------------------
// MAIN FUNCTION
// Used only when running independently.
inline int transpobook(const UserAccount& selected) {

    double balance = getAccountBalance(selected.username);

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "WELCOME TO COMPAYLER!\n";
    std::cout << "Initial Balance: " << money(balance) << "\n";

    transportation(selected, balance);

    std::cout << "\nFinal Wallet Balance: "
              << money(balance) << "\n";

    std::cout << "\nThank you for using COMPAYLER!\n";

    return 0;
}