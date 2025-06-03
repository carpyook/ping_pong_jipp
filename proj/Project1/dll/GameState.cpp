#include "GameState.h"
#include <fstream>
#include <iostream>
#include <random>

namespace PongEngine {
    GameState::GameState(sf::Vector2u windowSize)
        : windowSize(windowSize),
        leftScore(0),
        rightScore(0),
        paused(false),
        scoreFilePath("scores.dat"),
        collisionManager(windowSize),
        soundLoaded(false) {

        ball.setPosition({ windowSize.x / 2.0f, windowSize.y / 2.0f });

        leftPaddle.setPosition({ 30.0f, windowSize.y / 2.0f });
        rightPaddle.setPosition({ windowSize.x - 30.0f, windowSize.y / 2.0f });

        try {
            if (winSoundBuffer.loadFromFile("resources/score.wav")) {
                winSound.setBuffer(winSoundBuffer);
                winSound.setVolume(75.0f);
                soundLoaded = true;
            }
            else {
                std::cerr << "Failed to load win sound" << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Exception loading sound: " << e.what() << std::endl;
        }

        loadScoreFromFile();
    }

    void GameState::playWinSound() {
        if (soundLoaded) {
            winSound.play();
        }
    }

    void GameState::resetPaddlePositions() {
        leftPaddle.setPosition({ 30.0f, windowSize.y / 2.0f });
        rightPaddle.setPosition({ windowSize.x - 30.0f, windowSize.y / 2.0f });
    }

    void GameState::resetBall() {
        ball.setPosition({ windowSize.x / 2.0f, windowSize.y / 2.0f });

        resetPaddlePositions();

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> angleDist(0, 1);
        std::uniform_int_distribution<> degreeDist(-45, 45);

        float angle;
        if (angleDist(gen) == 0) {
            angle = degreeDist(gen);
        }
        else {
            angle = degreeDist(gen) + 180;
        }

        float speed = 300.0f;
        float radians = angle * 3.14159f / 180.0f;
        ball.setVelocity({ std::cos(radians) * speed, std::sin(radians) * speed });
    }

    void GameState::resetScores() {
        leftScore = 0;
        rightScore = 0;
        resetPaddlePositions();
        std::cout << "Scores reset to 0-0" << std::endl;
    }

    void GameState::update(float deltaTime) {
        if (paused) return;

        ball.update(deltaTime);
        leftPaddle.update(deltaTime);
        rightPaddle.update(deltaTime);

        collisionManager.checkWallCollision(ball);

        collisionManager.checkPaddleCollision(ball, leftPaddle);
        collisionManager.checkPaddleCollision(ball, rightPaddle);

        bool leftSideOut = false;
        if (collisionManager.checkBallOutOfBounds(ball, leftSideOut)) {
            if (leftSideOut) {
                rightScore++;
            }
            else {
                leftScore++;
            }
            saveScoreToFile();
            resetBall();
        }

        auto leftPos = leftPaddle.getPosition();
        auto rightPos = rightPaddle.getPosition();

        float leftHeight = leftPaddle.getHeight() / 2.0f;
        float rightHeight = rightPaddle.getHeight() / 2.0f;

        if (leftPos.y - leftHeight < 0) {
            leftPaddle.setPosition({ leftPos.x, leftHeight });
        }
        if (leftPos.y + leftHeight > windowSize.y) {
            leftPaddle.setPosition({ leftPos.x, windowSize.y - leftHeight });
        }

        if (rightPos.y - rightHeight < 0) {
            rightPaddle.setPosition({ rightPos.x, rightHeight });
        }
        if (rightPos.y + rightHeight > windowSize.y) {
            rightPaddle.setPosition({ rightPos.x, windowSize.y - rightHeight });
        }
    }

    void GameState::handleInput(const sf::RenderWindow& window, float deltaTime) {

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
            leftPaddle.moveUp(deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
            leftPaddle.moveDown(deltaTime);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            rightPaddle.moveUp(deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            rightPaddle.moveDown(deltaTime);
        }
    }

    void GameState::draw(sf::RenderWindow& window) {
        ball.draw(window);
        leftPaddle.draw(window);
        rightPaddle.draw(window);
    }

    int GameState::getLeftScore() const {
        return leftScore;
    }

    int GameState::getRightScore() const {
        return rightScore;
    }

    void GameState::togglePause() {
        paused = !paused;
    }

    bool GameState::isPaused() const {
        return paused;
    }

    void GameState::setScoreFilePath(std::string_view path) {
        scoreFilePath = path;
    }

    bool GameState::saveGameState() {
        return saveScoreToFile();
    }

    bool GameState::loadGameState() {
        return loadScoreFromFile();
    }

    bool GameState::saveScoreToFile() {
        try {
            std::ofstream file(scoreFilePath, std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "Failed to open file for writing: " << scoreFilePath << std::endl;
                return false;
            }

            file.write(reinterpret_cast<const char*>(&leftScore), sizeof(leftScore));
            file.write(reinterpret_cast<const char*>(&rightScore), sizeof(rightScore));

            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception during save: " << e.what() << std::endl;
            return false;
        }
    }

    bool GameState::loadScoreFromFile() {
        try {
            if (!std::filesystem::exists(scoreFilePath)) {
                std::cerr << "Score file does not exist." << std::endl;
                return false;
            }

            std::ifstream file(scoreFilePath, std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "Score file cannot be opened." << std::endl;
                return false;
            }

            file.read(reinterpret_cast<char*>(&leftScore), sizeof(leftScore));
            file.read(reinterpret_cast<char*>(&rightScore), sizeof(rightScore));

            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception during load: " << e.what() << std::endl;
            return false;
        }
    }

    bool GameState::checkWinCondition(int& winner) const {
        const int winScore = 10;
        if (leftScore >= winScore) {
            winner = 1;
            return true;
        }
        else if (rightScore >= winScore) {
            winner = 2;
            return true;
        }
        return false;
    }
}