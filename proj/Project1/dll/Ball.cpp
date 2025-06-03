#include "Ball.h"
#include <cmath>
#include <random>

namespace PongEngine {
    Ball::Ball(float radius) : speed(300.0f) {
        shape.setRadius(radius);
        shape.setOrigin({ radius, radius }); 
        shape.setFillColor(sf::Color::White);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 2.0f * 3.14159f);
        float angle = dist(gen);
        velocity.x = std::cos(angle) * speed;
        velocity.y = std::sin(angle) * speed;
    }

    Ball::~Ball() {

    }

    void Ball::setPosition(float x, float y) {
        shape.setPosition({ x, y }); 
    }

    void Ball::setPosition(const sf::Vector2f& position) {
        shape.setPosition(position);
    }

    void Ball::setVelocity(float vx, float vy) {
        velocity.x = vx;
        velocity.y = vy;
    }

    void Ball::setVelocity(const sf::Vector2f& velocity) {
        this->velocity = velocity;
    }

    void Ball::setColor(const sf::Color& color) {
        shape.setFillColor(color);
    }

    void Ball::update(float deltaTime) {
        shape.move(velocity * deltaTime);
    }

    sf::FloatRect Ball::getBounds() const {
        return shape.getGlobalBounds();
    }

    void Ball::reverseX() {
        velocity.x = -velocity.x;
    }

    void Ball::reverseY() {
        velocity.y = -velocity.y;
    }

    void Ball::draw(sf::RenderWindow& window) {
        window.draw(shape);
    }

    sf::Vector2f Ball::getPosition() const {
        return shape.getPosition();
    }

    sf::Vector2f Ball::getVelocity() const {
        return velocity;
    }

    float Ball::getRadius() const {
        return shape.getRadius();
    }
}