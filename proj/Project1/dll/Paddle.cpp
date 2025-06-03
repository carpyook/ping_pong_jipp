#include "Paddle.h"

namespace PongEngine {
    Paddle::Paddle(float width, float height) : speed(300.0f) {
        shape.setSize({ width, height });  
        shape.setOrigin({ width / 2.0f, height / 2.0f });  
        shape.setFillColor(sf::Color::White);
    }


    Paddle::~Paddle() {

    }

    void Paddle::setPosition(float x, float y) {
        shape.setPosition({ x, y }); 
    }

    void Paddle::setPosition(const sf::Vector2f& position) {
        shape.setPosition(position);
    }

    void Paddle::setColor(const sf::Color& color) {
        shape.setFillColor(color);
    }

    void Paddle::moveUp(float deltaTime) {
        shape.move({ 0.0f, -speed * deltaTime });
    }

    void Paddle::moveDown(float deltaTime) {
        shape.move({ 0.0f, speed * deltaTime });
    }

    void Paddle::update(float deltaTime) {
    }

    sf::FloatRect Paddle::getBounds() const {
        return shape.getGlobalBounds();
    }

    void Paddle::draw(sf::RenderWindow& window) {
        window.draw(shape);
    }

    sf::Vector2f Paddle::getPosition() const {
        return shape.getPosition();
    }

    float Paddle::getHeight() const {
        return shape.getSize().y;
    }

    float Paddle::getWidth() const {
        return shape.getSize().x;
    }
}