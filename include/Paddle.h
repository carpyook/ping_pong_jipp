#pragma once
#include <SFML/Graphics.hpp>

#ifdef PONGENGINE_EXPORTS
#define PONGENGINE_API __declspec(dllexport)
#else
#define PONGENGINE_API __declspec(dllimport)
#endif

namespace PongEngine {
    class PONGENGINE_API Paddle {
    private:
        sf::RectangleShape shape;
        float speed;

    public:
        Paddle(float width = 15.0f, float height = 80.0f);
        ~Paddle(); // Jawny destruktor
        void setPosition(float x, float y);
        void setPosition(const sf::Vector2f& position);
        void setColor(const sf::Color& color);
        void moveUp(float deltaTime);
        void moveDown(float deltaTime);
        void update(float deltaTime);
        sf::FloatRect getBounds() const;
        void draw(sf::RenderWindow& window);
        sf::Vector2f getPosition() const;
        float getHeight() const;
        float getWidth() const;
    };
}