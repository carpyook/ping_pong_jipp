#pragma once
#include <SFML/Graphics.hpp>

#ifdef PONGENGINE_EXPORTS
#define PONGENGINE_API __declspec(dllexport)
#else
#define PONGENGINE_API __declspec(dllimport)
#endif

namespace PongEngine {
    class PONGENGINE_API Ball {
    private:
        sf::CircleShape shape;
        sf::Vector2f velocity;
        float speed;

    public:
        Ball(float radius = 10.0f);
        ~Ball(); 
        void setPosition(float x, float y);
        void setPosition(const sf::Vector2f& position);
        void setVelocity(float vx, float vy);
        void setVelocity(const sf::Vector2f& velocity);
        void setColor(const sf::Color& color);
        void update(float deltaTime);
        sf::FloatRect getBounds() const;
        void reverseX();
        void reverseY();
        void draw(sf::RenderWindow& window);
        sf::Vector2f getPosition() const;
        sf::Vector2f getVelocity() const;
        float getRadius() const;
    };
}