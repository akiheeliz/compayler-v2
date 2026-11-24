// vouchers.h
#pragma once

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "useraccount.h"

struct Voucher {
    std::string code;
    std::string username;
    std::string service; // "Shopping", "Transportation", or "Hotel"
    int discountPercent;
    bool used;
};

inline std::string trimVoucherStr(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        --end;
    }
    return s.substr(start, end - start);
}

inline std::string toUpperVoucherStr(const std::string& s) {
    std::string out = trimVoucherStr(s);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return out;
}

inline std::vector<Voucher> getAllVouchers(const std::string& filename = "vouchers.csv") {
    std::vector<Voucher> list;
    std::ifstream file(filename);
    if (!file.is_open()) {
        return list;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<std::string> cols = parseCsvLine(line);
        if (cols.size() < 5) continue;

        try {
            Voucher v;
            v.code = trimVoucherStr(cols[0]);
            v.username = trimVoucherStr(cols[1]);
            v.service = trimVoucherStr(cols[2]);
            v.discountPercent = std::stoi(trimVoucherStr(cols[3]));
            v.used = (std::stoi(trimVoucherStr(cols[4])) != 0);
            list.push_back(v);
        } catch (...) {
            continue;
        }
    }

    file.close();
    return list;
}

inline std::vector<Voucher> getUserVouchers(const std::string& username,
                                          const std::string& filename = "vouchers.csv") {
    std::vector<Voucher> mine;
    std::string targetUser = trimVoucherStr(username);
    for (const auto& v : getAllVouchers(filename)) {
        if (trimVoucherStr(v.username) == targetUser) {
            mine.push_back(v);
        }
    }
    return mine;
}

inline void printAvailableVouchersForService(const std::string& username,
                                             const std::string& service,
                                             const std::string& filename = "vouchers.csv") {
    bool found = false;
    std::string targetService = toUpperVoucherStr(service);

    for (const auto& v : getUserVouchers(username, filename)) {
        if (!v.used && toUpperVoucherStr(v.service) == targetService) {
            if (!found) {
                std::cout << "\nAvailable " << service << " Vouchers:\n";
                found = true;
            }
            std::cout << "  - " << v.code << " (" << v.discountPercent << "% OFF)\n";
        }
    }

    if (!found) {
        std::cout << "\nNo unused " << service << " vouchers available.\n";
    }
}

inline bool saveAllVouchers(const std::vector<Voucher>& all,
                            const std::string& filename = "vouchers.csv") {
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) {
        std::cout << "Error: Could not update " << filename << ".\n";
        return false;
    }

    for (const auto& v : all) {
        file << escapeCsv(trimVoucherStr(v.code)) << ","
             << escapeCsv(trimVoucherStr(v.username)) << ","
             << escapeCsv(trimVoucherStr(v.service)) << ","
             << v.discountPercent << ","
             << (v.used ? 1 : 0) << "\n";
    }

    file.flush();
    file.close();
    return true;
}

inline Voucher createEveryThirdTxVoucher(const std::string& username,
                                       int totalTransactions,
                                       int discountPercent = 10,
                                       const std::string& filename = "vouchers.csv") {
    const std::string services[] = {"Shopping", "Transportation", "Hotel"};
    int milestone = totalTransactions / 3;
    std::string service = services[(milestone - 1) % 3];

    int nextNum = static_cast<int>(getAllVouchers(filename).size()) + 1;
    std::ostringstream codeStream;
    codeStream << "VOUCH-" << std::setw(3) << std::setfill('0') << nextNum;

    Voucher v{codeStream.str(), trimVoucherStr(username), service, discountPercent, false};

    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        file << escapeCsv(v.code) << ","
             << escapeCsv(v.username) << ","
             << escapeCsv(v.service) << ","
             << v.discountPercent << ","
             << (v.used ? 1 : 0) << "\n";
        file.flush();
        file.close();
    }

    return v;
}

inline bool markVoucherUsed(const std::string& username,
                            const std::string& code,
                            const std::string& service = "",
                            const std::string& filename = "vouchers.csv") {
    std::vector<Voucher> all = getAllVouchers(filename);
    std::string targetUser = trimVoucherStr(username);
    std::string targetCode = toUpperVoucherStr(code);
    std::string targetService = toUpperVoucherStr(service);

    bool updated = false;
    for (auto& v : all) {
        bool serviceMatches = targetService.empty() || (toUpperVoucherStr(v.service) == targetService);
        if (!v.used &&
            trimVoucherStr(v.username) == targetUser &&
            toUpperVoucherStr(v.code) == targetCode &&
            serviceMatches) {
            v.used = true;
            updated = true;
            break;
        }
    }

    if (!updated) {
        return false;
    }

    return saveAllVouchers(all, filename);
}

inline double previewVoucherDiscount(const std::string& username,
                                     const std::string& service,
                                     const std::string& code,
                                     double originalAmount,
                                     bool& appliedOut,
                                     int& percentOut,
                                     const std::string& filename = "vouchers.csv") {
    appliedOut = false;
    percentOut = 0;

    std::string targetUser = trimVoucherStr(username);
    std::string targetCode = toUpperVoucherStr(code);
    std::string targetService = toUpperVoucherStr(service);

    if (targetCode.empty()) {
        return originalAmount;
    }

    for (const auto& v : getAllVouchers(filename)) {
        if (trimVoucherStr(v.username) == targetUser &&
            toUpperVoucherStr(v.code) == targetCode) {
            if (v.used) {
                std::cout << "That voucher has already been used.\n";
                return originalAmount;
            }
            if (toUpperVoucherStr(v.service) != targetService) {
                std::cout << "That voucher is only valid for " << v.service << ".\n";
                return originalAmount;
            }

            appliedOut = true;
            percentOut = v.discountPercent;
            double discounted = originalAmount * (1.0 - (v.discountPercent / 100.0));
            return std::round(discounted * 100.0) / 100.0;
        }
    }

    std::cout << "Voucher code not found for your account.\n";
    return originalAmount;
}