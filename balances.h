// balance.h
#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "useraccount.h"

struct AccountBalanceRecord {
    std::string username;
    double balance;
};

inline std::vector<AccountBalanceRecord> loadBalancesFromCsv(const std::string& filename = "balances.csv") {
    std::vector<AccountBalanceRecord> records;
    std::ifstream file(filename);
    if (!file) {
        return records;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // CSV format: username,balance
        std::vector<std::string> fields = parseCsvLine(line);
        if (fields.size() < 2) continue;

        AccountBalanceRecord rec;
        rec.username = fields[0];
        try {
            rec.balance = std::stod(fields[1]);
        } catch (...) {
            continue;
        }

        records.push_back(rec);
    }

    file.close();
    return records;
}

inline bool saveBalancesToCsv(const std::vector<AccountBalanceRecord>& records,
                              const std::string& filename = "balances.csv") {
    std::ofstream file(filename, std::ios::trunc);
    if (!file) {
        std::cout << "Error: Could not open " << filename << " for writing!\n";
        return false;
    }

    for (const auto& rec : records) {
        std::ostringstream balStream;
        balStream << std::fixed << std::setprecision(2) << rec.balance;

        file << escapeCsv(rec.username) << ","
             << escapeCsv(balStream.str()) << "\n";
    }

    file.close();
    return true;
}

inline bool setAccountBalanceInCsv(const std::string& username,
                                   double newBalance,
                                   const std::string& filename = "balances.csv") {
    std::vector<AccountBalanceRecord> records = loadBalancesFromCsv(filename);
    bool found = false;

    for (auto& rec : records) {
        if (rec.username == username) {
            rec.balance = newBalance;
            found = true;
            break;
        }
    }

    if (!found) {
        records.push_back({username, newBalance});
    }

    return saveBalancesToCsv(records, filename);
}

inline void syncBalancesWithUsers(double defaultBalance = 0.0,
                                  const std::string& usersFile = "users.csv",
                                  const std::string& balancesFile = "balances.csv") {
    std::vector<UserAccount> users = loadUsersFromCsv(usersFile);
    std::vector<AccountBalanceRecord> records = loadBalancesFromCsv(balancesFile);
    bool updated = false;

    for (const auto& user : users) {
        bool found = false;
        for (const auto& rec : records) {
            if (rec.username == user.username) {
                found = true;
                break;
            }
        }
        if (!found) {
            records.push_back({user.username, defaultBalance});
            updated = true;
        }
    }

    if (updated) {
        saveBalancesToCsv(records, balancesFile);
    }
}

inline double getAccountBalance(const std::string& username,
                                double defaultBalance = 0.0,
                                const std::string& usersFile = "users.csv",
                                const std::string& balancesFile = "balances.csv") {
    syncBalancesWithUsers(defaultBalance, usersFile, balancesFile);
    std::vector<AccountBalanceRecord> records = loadBalancesFromCsv(balancesFile);

    for (const auto& rec : records) {
        if (rec.username == username) {
            return rec.balance;
        }
    }

    setAccountBalanceInCsv(username, defaultBalance, balancesFile);
    return defaultBalance;
}

inline bool setAccountBalance(const std::string& username,
                              double newBalance,
                              const std::string& balancesFile = "balances.csv") {
    return setAccountBalanceInCsv(username, newBalance, balancesFile);
}