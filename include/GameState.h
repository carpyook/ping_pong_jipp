#pragma once
#include "Ball.h"
#include "Paddle.h"
#include "CollisionManager.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <string_view>
#include <stdexcept>
#include <optional>
#include <filesystem>

#ifdef PONGENGINE_EXPORTS
#define PONGENGINE_API __declspec(dllexport)
#else
#define PONGENGINE_API __declspec(dllimport)
#endif

namespace PongEngine {
    class PONGENGINE_API GameState {
    private:
        Ball ball;
        Paddle leftPaddle;
        Paddle rightPaddle;
        CollisionManager collisionManager;
        int leftScore;
        int rightScore;
        sf::Vector2u windowSize;
        bool paused;

        // File handling
        std::filesystem::path scoreFilePath;
        bool saveScoreToFile();
        bool loadScoreFromFile();

    public:
        GameState(sf::Vector2u windowSize);
        void resetBall();
        void update(float deltaTime);
        void handleInput(const sf::RenderWindow& window, float deltaTime);
        void draw(sf::RenderWindow& window);
        int getLeftScore() const;
        int getRightScore() const;
        void togglePause();
        bool isPaused() const;

        // Exception-safe file operations
        void setScoreFilePath(std::string_view path);
        bool saveGameState();
        bool loadGameState();

        // Game logic
        bool checkWinCondition(int& winner) const;
    };
}