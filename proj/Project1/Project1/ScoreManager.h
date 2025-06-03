#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <utility>
#include <fstream>
#include <stdexcept>
#include <filesystem>

class ScoreManager {
private:
    std::filesystem::path highScoreFile;
    std::vector<std::pair<std::string, int>> highScores;
    const unsigned int maxHighScores = 10;

    void sortHighScores();

public:
    ScoreManager(std::string_view filename = "highscores.dat");
    ~ScoreManager() = default;

    void addScore(const std::string& playerName, int score);

    [[nodiscard]] bool isHighScore(int score) const;

    [[nodiscard]] bool saveHighScores();

    [[nodiscard]] bool loadHighScores();

    [[nodiscard]] const std::vector<std::pair<std::string, int>>& getHighScores() const;


    void clearHighScores();
};