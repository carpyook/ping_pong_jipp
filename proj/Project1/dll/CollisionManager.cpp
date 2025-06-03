#include "CollisionManager.h"
#include <iostream>

namespace PongEngine {
    CollisionManager::CollisionManager(sf::Vector2u winSize)
        : windowSize(winSize), soundEnabled(true) {
        loadSoundResources("resources/hit.wav", "resources/wall.wav");

        if (paddleHitBuffer.getSampleCount() > 0) {
            paddleHitSound = std::make_unique<sf::Sound>(paddleHitBuffer);
        }
        if (wallHitBuffer.getSampleCount() > 0) {
            wallHitSound = std::make_unique<sf::Sound>(wallHitBuffer);
        }
    }


    CollisionManager::~CollisionManager() {

    }

    bool CollisionManager::checkWallCollision(Ball& ball) {
        bool collided = false;
        auto pos = ball.getPosition();
        float radius = ball.getRadius();

        if (pos.y - radius < 0) {
            ball.setPosition({ pos.x, radius });
            ball.reverseY();
            collided = true;
        }
        else if (pos.y + radius > windowSize.y) {
            ball.setPosition({ pos.x, windowSize.y - radius });
            ball.reverseY();
            collided = true;
        }

        if (collided && soundEnabled && wallHitSound && wallHitBuffer.getSampleCount() > 0) {
            wallHitSound->play();
        }

        return collided;
    }

    bool CollisionManager::checkPaddleCollision(Ball& ball, Paddle& paddle) {
        float ballRadius = ball.getRadius();
        sf::Vector2f ballPos = ball.getPosition();
        sf::Vector2f paddlePos = paddle.getPosition();
        float paddleWidth = paddle.getWidth();
        float paddleHeight = paddle.getHeight();

        float ballLeft = ballPos.x - ballRadius;
        float ballRight = ballPos.x + ballRadius;
        float ballTop = ballPos.y - ballRadius;
        float ballBottom = ballPos.y + ballRadius;

        float paddleLeft = paddlePos.x - paddleWidth / 2.0f;
        float paddleRight = paddlePos.x + paddleWidth / 2.0f;
        float paddleTop = paddlePos.y - paddleHeight / 2.0f;
        float paddleBottom = paddlePos.y + paddleHeight / 2.0f;

        bool collides = !(
            ballRight < paddleLeft ||
            ballLeft > paddleRight ||
            ballBottom < paddleTop ||
            ballTop > paddleBottom
            );

        if (collides) {
            ball.reverseX();

            float hitPosition = (ballPos.y - paddlePos.y) / paddleHeight;
            hitPosition = hitPosition * 2.0f - 1.0f;

            sf::Vector2f velocity = ball.getVelocity();
            velocity.y = hitPosition * std::abs(velocity.x) * 0.75f;
            ball.setVelocity(velocity);

            if (soundEnabled && paddleHitSound && paddleHitBuffer.getSampleCount() > 0) {
                paddleHitSound->play();
            }

            return true;
        }

        return false;
    }

    bool CollisionManager::checkBallOutOfBounds(const Ball& ball, bool& leftSideOut) {
        auto pos = ball.getPosition();

        if (pos.x < 0) {
            leftSideOut = true;
            return true;
        }
        else if (pos.x > windowSize.x) {
            leftSideOut = false;
            return true;
        }

        return false;
    }

    void CollisionManager::enableSound(bool enable) {
        soundEnabled = enable;
    }

    bool CollisionManager::loadSoundResources(std::string_view paddleHitPath, std::string_view wallHitPath) {
        try {
            bool result = true;

            if (!paddleHitBuffer.loadFromFile(std::string(paddleHitPath))) {
                std::cerr << "Failed to load paddle hit sound: " << paddleHitPath << std::endl;
                result = false;
            }

            if (!wallHitBuffer.loadFromFile(std::string(wallHitPath))) {
                std::cerr << "Failed to load wall hit sound: " << wallHitPath << std::endl;
                result = false;
            }

            return result;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception during sound loading: " << e.what() << std::endl;
            return false;
        }
    }
}