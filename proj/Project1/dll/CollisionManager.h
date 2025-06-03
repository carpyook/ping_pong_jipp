#pragma once
#include "Ball.h"
#include "Paddle.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <string_view>
#include <memory>

#ifdef PONGENGINE_EXPORTS
#define PONGENGINE_API __declspec(dllexport)
#else
#define PONGENGINE_API __declspec(dllimport)
#endif

namespace PongEngine {
    class PONGENGINE_API CollisionManager {
    private:
        sf::Vector2u windowSize;
        sf::SoundBuffer paddleHitBuffer;
        sf::SoundBuffer wallHitBuffer;
        std::unique_ptr<sf::Sound> paddleHitSound;
        std::unique_ptr<sf::Sound> wallHitSound;
        bool soundEnabled;

    public:
        CollisionManager(sf::Vector2u winSize);
        ~CollisionManager(); 

        bool checkWallCollision(Ball& ball);
        bool checkPaddleCollision(Ball& ball, Paddle& paddle);
        bool checkBallOutOfBounds(const Ball& ball, bool& leftSideOut);
        void enableSound(bool enable);
        bool loadSoundResources(std::string_view paddleHitPath, std::string_view wallHitPath);
    };
}