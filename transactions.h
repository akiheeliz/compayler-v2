// transactions.h
#pragma once

#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "cooldown.h"
#include "useraccount.h"
#include "vouchers.h"

struct TransactionRecord {
    std::string username;
    int txNumber;
    std::string category;
    double amount;
    std::string timestamp;
};

inline std::string getCurrentDateTime() {
    std::time_t now = std::time(nullptr);
    std::tm localTm{};
#if defined(_WIN32)
    localtime_s(&localTm, &now);
#else
    localtime_r(&now, &localTm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&localTm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

inline std::vector<TransactionRecord> loadTransactionsFromCsv(const std::string& filename = "transactions.csv") {
    std::vector<TransactionRecord> records;
    std::ifstream file(filename);
    if (!file.is_open()) {
        return records;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<std::string> fields = parseCsvLine(line);
        if (fields.size() < 5) continue;

        TransactionRecord rec;
        rec.username = fields[0];
        try {
            rec.txNumber = std::stoi(fields[1]);
            rec.amount = std::stod(fields[3]);
        } catch (...) {
            continue;
        }
        rec.category = fields[2];
        rec.timestamp = fields[4];

        records.push_back(rec);
    }

    return records;
}

inline std::vector<TransactionRecord> getUserTransactions(const std::string& username,
                                                        const std::string& filename = "transactions.csv") {
    std::vector<TransactionRecord> userTx;
    for (const auto& rec : loadTransactionsFromCsv(filename)) {
        if (rec.username == username) {
            userTx.push_back(rec);
        }
    }
    return userTx;
}

inline bool recordTransaction(const std::string& username,
                              const std::string& category,
                              double amount,
                              const std::string& filename = "transactions.csv") {
    int nextTxNumber = static_cast<int>(getUserTransactions(username, filename).size()) + 1;
    std::string timestamp = getCurrentDateTime();

    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cout << "Error: Could not open " << filename << " for writing!\n";
        return false;
    }

    std::ostringstream amtStream;
    amtStream << std::fixed << std::setprecision(2) << amount;

    file << escapeCsv(username) << ","
         << nextTxNumber << ","
         << escapeCsv(category) << ","
         << escapeCsv(amtStream.str()) << ","
         << escapeCsv(timestamp) << "\n";
    file.close();

    int totalTx = nextTxNumber;
    if (totalTx > 0 && totalTx % 3 == 0) {
        Voucher v = createEveryThirdTxVoucher(username, totalTx, 10);
        std::cout << "\n========================================\n";
        std::cout << "     DISCOUNT VOUCHER GENERATED!\n";
        std::cout << "========================================\n";
        std::cout << "Transactions Completed: " << totalTx << "\n";
        std::cout << "Voucher Code:         " << v.code << "\n";
        std::cout << "Valid For:            " << v.service << "\n";
        std::cout << "Discount:             " << v.discountPercent << "% OFF\n";
        std::cout << "========================================\n";
    }

    triggerCooldownIfExceeded(username, 10, filename);
    return true;
}