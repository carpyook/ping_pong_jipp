#include "ScoreManager.h"
#include <algorithm>
#include <iostream>

ScoreManager::ScoreManager(std::string_view filename) : highScoreFile(filename) {
    loadHighScores();
}

void ScoreManager::sortHighScores() {
    std::sort(highScores.begin(), highScores.end(),
        [](const auto& a, const auto& b) {
            return a.second > b.second; 
        });

    if (highScores.size() > maxHighScores) {
        highScores.resize(maxHighScores);
    }
}

void ScoreManager::addScore(const std::string& playerName, int score) {
    highScores.emplace_back(playerName, score);
    sortHighScores();
    saveHighScores();
}

bool ScoreManager::isHighScore(int score) const {
    if (highScores.size() < maxHighScores) {
        return true;
    }

    return score > highScores.back().second;
}

bool ScoreManager::saveHighScores() {
    try {
        std::ofstream file(highScoreFile, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open high score file for writing: " << highScoreFile << std::endl;
            return false;
        }

        size_t count = highScores.size();
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (const auto& [name, score] : highScores) {

            size_t nameLength = name.length();
            file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));

            file.write(name.c_str(), nameLength);

            file.write(reinterpret_cast<const char*>(&score), sizeof(score));
        }

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception during saving high scores: " << e.what() << std::endl;
        return false;
    }
}

bool ScoreManager::loadHighScores() {
    try {
        if (!std::filesystem::exists(highScoreFile)) {

            return false;
        }

        std::ifstream file(highScoreFile, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open high score file for reading." << std::endl;
            return false;
        }


        highScores.clear();


        size_t count;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));


        count = std::min(count, static_cast<size_t>(maxHighScores));


        for (size_t i = 0; i < count; ++i) {

            size_t nameLength;
            file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));


            std::string name(nameLength, '\0');
            file.read(&name[0], nameLength);


            int score;
            file.read(reinterpret_cast<char*>(&score), sizeof(score));


            highScores.emplace_back(name, score);
        }


        sortHighScores();

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception during loading high scores: " << e.what() << std::endl;
        return false;
    }
}

const std::vector<std::pair<std::string, int>>& ScoreManager::getHighScores() const {
    return highScores;
}

void ScoreManager::clearHighScores() {
    highScores.clear();
    saveHighScores();
}