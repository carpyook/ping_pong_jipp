#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "../dll/GameState.h"
#include "ResourceManager.h"
#include "ScoreManager.h"
#include <memory>
#include <string>
#include <optional>


enum class GameScreen {
    START_DIALOG,
    PLAYING,
    GAME_OVER
};

class Game {
private:
    sf::RenderWindow window;
    std::unique_ptr<PongEngine::GameState> gameState;
    sf::Text scoreText;
    sf::Text pauseText;
    sf::Text winnerText;
    sf::Clock clock;
    sf::Sound hitSound;
    sf::Sound scoreSound;
    bool gameOver;
    int winner;
    ResourceManager& resourceManager;
    ScoreManager scoreManager;


    GameScreen currentScreen;


    sf::Text titleText;
    sf::Text resumeOption;
    sf::Text newGameOption;
    sf::RectangleShape dialogBackground;
    bool resumeSelected;
    bool hasSavedGame;


    bool loadResources();
    void updateScoreText();
    void handleEvents();
    void update();
    void render();


    void initializeStartDialog();
    void renderStartDialog();
    void moveDialogSelection();
    void checkSavedGame();

public:
    Game();
    ~Game();
    void run();
    void restart();
};