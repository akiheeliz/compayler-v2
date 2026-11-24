// cooldown.h
#pragma once

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>

struct CooldownState {
    bool active = false;
    std::chrono::steady_clock::time_point unlockTime;
};

inline std::unordered_map<std::string, CooldownState>& getCooldownMap() {
    static std::unordered_map<std::string, CooldownState> cooldowns;
    return cooldowns;
}

inline int countStoredTransactions(const std::string& username,
                                   const std::string& filename = "transactions.csv") {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return 0;
    }

    int count = 0;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        size_t commaPos = line.find(',');
        std::string rowUser = (commaPos == std::string::npos) ? line : line.substr(0, commaPos);
        if (rowUser.size() >= 2 && rowUser.front() == '"' && rowUser.back() == '"') {
            rowUser = rowUser.substr(1, rowUser.size() - 2);
        }

        if (rowUser == username) {
            count++;
        }
    }

    return count;
}

inline void enforceCooldownIfActive(const std::string& username) {
    auto& cooldowns = getCooldownMap();
    auto it = cooldowns.find(username);
    if (it == cooldowns.end() || !it->second.active) {
        return;
    }

    auto now = std::chrono::steady_clock::now();
    if (now >= it->second.unlockTime) {
        it->second.active = false;
        return;
    }

    std::cout << "\n========================================\n";
    std::cout << "      TRANSACTION COOLDOWN ACTIVE\n";
    std::cout << "========================================\n";
    std::cout << "You exceeded 5 transactions.\n";
    std::cout << "Please wait 10 seconds before continuing...\n";

    while (true) {
        now = std::chrono::steady_clock::now();
        if (now >= it->second.unlockTime) {
            break;
        }

        auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
            it->second.unlockTime - now
        ).count() + 1;

        std::cout << "\rTime remaining: " << remaining << " second(s)   " << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    it->second.active = false;
    std::cout << "\rCooldown complete! You may continue.      \n";
    std::cout << "========================================\n";
}

inline void triggerCooldownIfExceeded(const std::string& username,
                                      int cooldownSeconds = 10,
                                      const std::string& filename = "transactions.csv") {
    int totalTx = countStoredTransactions(username, filename);
    if (totalTx > 5) {
        auto& state = getCooldownMap()[username];
        state.active = true;
        state.unlockTime = std::chrono::steady_clock::now() + std::chrono::seconds(cooldownSeconds);
        enforceCooldownIfActive(username);
    }
}