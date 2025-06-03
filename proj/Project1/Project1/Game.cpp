#include "Game.h"
#include <iostream>
#include <stdexcept>
#include <optional>
#include <Windows.h>
#include <filesystem>

Game::Game() :
    gameOver(false),
    winner(0),
    resourceManager(ResourceManager::getInstance()),
    scoreManager("highscores.dat"),
    currentScreen(GameScreen::START_DIALOG),
    resumeSelected(false) {
    try {

        window.create(sf::VideoMode(800, 600), "Pong Game", sf::Style::Default);
        window.setFramerateLimit(60);


        gameState = std::make_unique<PongEngine::GameState>(window.getSize());

      
        if (!loadResources()) {
            throw std::runtime_error("Failed to load critical game resources!");
        }

        try {
           
            scoreText.setFont(resourceManager.getFont("main"));
            scoreText.setCharacterSize(30);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setPosition(window.getSize().x / 2.0f - 50, 10);

            pauseText.setFont(resourceManager.getFont("main"));
            pauseText.setString("PAUSED");
            pauseText.setCharacterSize(50);
            pauseText.setFillColor(sf::Color::Yellow);
            pauseText.setPosition(window.getSize().x / 2.0f - pauseText.getGlobalBounds().width / 2.0f,
                window.getSize().y / 2.0f - pauseText.getGlobalBounds().height / 2.0f);

            winnerText.setFont(resourceManager.getFont("main"));
            winnerText.setCharacterSize(40);
            winnerText.setFillColor(sf::Color::Green);
            winnerText.setPosition(window.getSize().x / 2.0f - 150, window.getSize().y / 2.0f - 20);

            
            initializeStartDialog();
        }
        catch (const std::exception& e) {
            std::cerr << "Warning: Failed to setup text UI: " << e.what() << std::endl;

        }

        try {
            
            if (resourceManager.hasSoundBuffer("hit")) {
                hitSound.setBuffer(resourceManager.getSoundBuffer("hit"));
            }
            if (resourceManager.hasSoundBuffer("score")) {
                scoreSound.setBuffer(resourceManager.getSoundBuffer("score"));
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Warning: Failed to setup sounds: " << e.what() << std::endl;
           
        }


        clock.restart();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception during Game initialization: " << e.what() << std::endl;
        throw;
    }
}

void Game::initializeStartDialog() {

    checkSavedGame();

    // Konfiguracja t³a dialogu
    dialogBackground.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    dialogBackground.setFillColor(sf::Color(0, 0, 100, 230)); 


    titleText.setFont(resourceManager.getFont("main"));
    titleText.setString("PINGPONG");
    titleText.setCharacterSize(72);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleRect = titleText.getLocalBounds();
    titleText.setOrigin(titleRect.left + titleRect.width / 2.0f, titleRect.top + titleRect.height / 2.0f);
    titleText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 3.0f);

    resumeOption.setFont(resourceManager.getFont("main"));
    resumeOption.setString("Wznow gre");
    resumeOption.setCharacterSize(36);
 
    resumeOption.setFillColor(hasSavedGame ? sf::Color::White : sf::Color(100, 100, 100));
    sf::FloatRect resumeRect = resumeOption.getLocalBounds();
    resumeOption.setOrigin(resumeRect.left + resumeRect.width / 2.0f, resumeRect.top + resumeRect.height / 2.0f);
    resumeOption.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);


    newGameOption.setFont(resourceManager.getFont("main"));
    newGameOption.setString("Nowa gra");
    newGameOption.setCharacterSize(36);

    newGameOption.setFillColor(sf::Color::Green);
    sf::FloatRect newGameRect = newGameOption.getLocalBounds();
    newGameOption.setOrigin(newGameRect.left + newGameRect.width / 2.0f, newGameRect.top + newGameRect.height / 2.0f);
    newGameOption.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f + 60.0f);
}


void Game::checkSavedGame() {

    hasSavedGame = std::filesystem::exists("scores.dat");


    if (hasSavedGame) {
        try {
            std::uintmax_t fileSize = std::filesystem::file_size("scores.dat");

            if (fileSize < 8) {
                hasSavedGame = false;
                std::cout << "Found (" << fileSize << " bytes)" << std::endl;
            }
            else {
                std::cout << "Found (size: " << fileSize << " bytes)" << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error while saving game: " << e.what() << std::endl;
            hasSavedGame = false;
        }
    }
}

void Game::moveDialogSelection() {

    if (hasSavedGame) {
        resumeSelected = !resumeSelected;

        if (resumeSelected) {

            resumeOption.setFillColor(sf::Color::Green);

            newGameOption.setFillColor(sf::Color::White);
        }
        else {

            resumeOption.setFillColor(sf::Color::White);

            newGameOption.setFillColor(sf::Color::Green);
        }
    }
}

void Game::renderStartDialog() {

    window.clear(sf::Color(25, 25, 112));


    window.draw(dialogBackground);


    window.draw(titleText);
    window.draw(resumeOption);
    window.draw(newGameOption);


    sf::Text controlsText;
    controlsText.setFont(resourceManager.getFont("main"));
    controlsText.setString("Uzyj strzalek GORA/DOL aby wybrac i ENTER aby zatwierdzic");
    controlsText.setCharacterSize(18);
    controlsText.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect controlsRect = controlsText.getLocalBounds();
    controlsText.setOrigin(controlsRect.left + controlsRect.width / 2.0f, controlsRect.top + controlsRect.height / 2.0f);
    controlsText.setPosition(window.getSize().x / 2.0f, window.getSize().y * 3.0f / 4.0f);
    window.draw(controlsText);
}

Game::~Game() {

    if (gameState && currentScreen == GameScreen::PLAYING) {
        gameState->saveGameState();
        std::cout << "Stan gry zapisany przy zamkniêciu" << std::endl;
    }

}

bool Game::loadResources() {
    try {
        std::cout << "Trying to load resources..." << std::endl;


        char currentDir[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, currentDir);
        std::cout << "Current working directory: " << currentDir << std::endl;

        bool success = true;
        bool fontLoaded = false;


        try {
            resourceManager.loadFont("main", "C:\\Windows\\Fonts\\arial.ttf");
            std::cout << "Loaded font from Windows system fonts" << std::endl;
            fontLoaded = true;
        }
        catch (const std::exception& e) {
            std::cout << "Failed to load system font: " << e.what() << std::endl;


            try {
                resourceManager.loadFont("main", "resources/arial.ttf");
                std::cout << "Loaded font from resources folder" << std::endl;
                fontLoaded = true;
            }
            catch (const std::exception&) {

                try {
                    resourceManager.loadFont("main", "arial.ttf");
                    std::cout << "Loaded font from root folder" << std::endl;
                    fontLoaded = true;
                }
                catch (const std::exception& e) {
                    std::cerr << "Failed to load font from any location: " << e.what() << std::endl;
                    success = false;
                }
            }
        }


        if (!fontLoaded) {
            return false;
        }


        bool hitSoundLoaded = false;
        try {
            sf::SoundBuffer buffer;
            if (buffer.loadFromFile("resources/hit.wav")) {
                resourceManager.loadSoundBuffer("hit", "resources/hit.wav");
                std::cout << "Loaded hit sound from resources folder" << std::endl;
                hitSoundLoaded = true;
            }
        }
        catch (const std::exception& e) {
            std::cout << "Couldn't load hit.wav from resources: " << e.what() << std::endl;
        }

        if (!hitSoundLoaded) {
            try {
                sf::SoundBuffer buffer;
                if (buffer.loadFromFile("hit.wav")) {
                    resourceManager.loadSoundBuffer("hit", "hit.wav");
                    std::cout << "Loaded hit sound from root folder" << std::endl;
                    hitSoundLoaded = true;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Failed to load hit sound: " << e.what() << std::endl;
                
            }
        }


        if (!hitSoundLoaded) {
            sf::SoundBuffer buffer;
            sf::Int16 samples[4000]; 
            for (int i = 0; i < 4000; i++) {
                samples[i] = i < 2000 ? 10000 : -10000;
            }
            buffer.loadFromSamples(samples, 4000, 1, 44100);
            resourceManager.loadSoundBufferFromMemory("hit", buffer);
            std::cout << "Created fallback hit sound" << std::endl;
        }


        bool scoreSoundLoaded = false;
        try {
            sf::SoundBuffer buffer;
            if (buffer.loadFromFile("resources/score.wav")) {
                resourceManager.loadSoundBuffer("score", "resources/score.wav");
                std::cout << "Loaded score sound from resources folder" << std::endl;
                scoreSoundLoaded = true;
            }
        }
        catch (const std::exception&) {
            try {
                sf::SoundBuffer buffer;
                if (buffer.loadFromFile("score.wav")) {
                    resourceManager.loadSoundBuffer("score", "score.wav");
                    std::cout << "Loaded score sound from root folder" << std::endl;
                    scoreSoundLoaded = true;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Failed to load score sound: " << e.what() << std::endl;
            }
        }


        if (!scoreSoundLoaded) {
            sf::SoundBuffer buffer;
            sf::Int16 samples[8000]; 
            for (int i = 0; i < 8000; i++) {
                samples[i] = (i / 20) % 2 == 0 ? 15000 : -15000; 
            }
            buffer.loadFromSamples(samples, 8000, 1, 44100);
            resourceManager.loadSoundBufferFromMemory("score", buffer);
            std::cout << "Created fallback score sound" << std::endl;
        }


        try {
            resourceManager.loadSoundBuffer("wall", "resources/wall.wav");
            std::cout << "Loaded wall sound from resources folder" << std::endl;
        }
        catch (const std::exception&) {
            try {
                resourceManager.loadSoundBuffer("wall", "wall.wav");
                std::cout << "Loaded wall sound from root folder" << std::endl;
            }
            catch (const std::exception&) {
                std::cout << "Wall sound not available, using hit sound instead" << std::endl;

                if (hitSoundLoaded || resourceManager.hasSoundBuffer("hit")) {
                    resourceManager.copySoundBuffer("hit", "wall");
                }
            }
        }

        return true; 
    }
    catch (const std::exception& e) {
        std::cerr << "Exception during resource loading: " << e.what() << std::endl;
        return false;
    }
}

void Game::updateScoreText() {

    int leftScore = gameState->getLeftScore();
    int rightScore = gameState->getRightScore();


    std::string leftScoreStr = (leftScore < 10) ? "0" + std::to_string(leftScore) : std::to_string(leftScore);
    std::string rightScoreStr = (rightScore < 10) ? "0" + std::to_string(rightScore) : std::to_string(rightScore);


    scoreText.setString(leftScoreStr + " : " + rightScoreStr);


}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {

            if (currentScreen == GameScreen::PLAYING) {
                gameState->saveGameState();
                std::cout << "Game state before closing" << std::endl;
            }
            window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {

            switch (currentScreen) {
            case GameScreen::START_DIALOG:

                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down) {
                    moveDialogSelection();
                }
                else if (event.key.code == sf::Keyboard::Return) {

                    if (resumeSelected && hasSavedGame) {

                        if (gameState->loadGameState()) {
                            std::cout << "Success" << std::endl;
                            updateScoreText();
                        }
                        else {
                            std::cerr << "Error" << std::endl;

                            gameState->resetScores();
                            gameState->resetBall();
                        }
                    }
                    else {

                        gameState->resetScores();
                        gameState->resetBall();
                    }

                    currentScreen = GameScreen::PLAYING;

                    clock.restart();
                }
                else if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                break;

            case GameScreen::PLAYING:
            case GameScreen::GAME_OVER:

                switch (event.key.code) {
                case sf::Keyboard::Escape:

                    if (currentScreen == GameScreen::PLAYING) {
                        gameState->saveGameState();
                        std::cout << "Game saved before exit" << std::endl;
                    }
                    window.close();
                    break;
                case sf::Keyboard::Space:
                    if (currentScreen == GameScreen::PLAYING) {
                        gameState->togglePause();
                    }
                    break;
                case sf::Keyboard::R:
                    restart();
                    if (currentScreen == GameScreen::GAME_OVER) {
                        currentScreen = GameScreen::PLAYING;
                    }
                    break;
                default:
                    break;
                }
                break;
            }
        }
    }
}

void Game::update() {
    if (window.isOpen()) {

        float deltaTime = clock.restart().asSeconds();

        if (currentScreen == GameScreen::PLAYING) {

            gameState->handleInput(window, deltaTime);

            if (!gameState->isPaused()) {
                gameState->update(deltaTime);
                updateScoreText();


                if (gameState->checkWinCondition(winner)) {
                    gameOver = true;
                    currentScreen = GameScreen::GAME_OVER;

                    gameState->playWinSound();

                    winnerText.setString("Gracz " + std::to_string(winner) + " wygrywa!");

                    scoreManager.addScore("Gracz " + std::to_string(winner),
                        (winner == 1) ? gameState->getLeftScore() : gameState->getRightScore());

                    gameState->resetScores();
                    gameState->saveGameState();
                }
            }
        }
    }
}

void Game::render() {
    switch (currentScreen) {
    case GameScreen::START_DIALOG:
        renderStartDialog();
        break;

    case GameScreen::PLAYING:
    case GameScreen::GAME_OVER:
        window.clear(sf::Color(25, 25, 112));

        sf::RectangleShape centerLine(sf::Vector2f(5, window.getSize().y));
        centerLine.setPosition(window.getSize().x / 2.0f - 2.5f, 0);
        centerLine.setFillColor(sf::Color(100, 100, 100));
        window.draw(centerLine);

        gameState->draw(window);

        sf::RectangleShape scoreBoard(sf::Vector2f(140, 45));
        scoreBoard.setFillColor(sf::Color(50, 50, 150, 200));
        scoreBoard.setOutlineThickness(2.0f);
        scoreBoard.setOutlineColor(sf::Color::White);
        scoreBoard.setPosition(window.getSize().x / 2.0f - 70, 10);
        scoreBoard.setOrigin(0, 0);

        sf::RectangleShape scoreBoardShadow(sf::Vector2f(140, 45));
        scoreBoardShadow.setFillColor(sf::Color(0, 0, 0, 100));
        scoreBoardShadow.setPosition(window.getSize().x / 2.0f - 70 + 4, 10 + 4);
        window.draw(scoreBoardShadow);


        window.draw(scoreBoard);


        std::string leftScoreStr = std::to_string(gameState->getLeftScore());
        std::string rightScoreStr = std::to_string(gameState->getRightScore());

 
        if (gameState->getLeftScore() < 10) leftScoreStr = "0" + leftScoreStr;
        if (gameState->getRightScore() < 10) rightScoreStr = "0" + rightScoreStr;


        scoreText.setString(leftScoreStr + " : " + rightScoreStr);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setCharacterSize(28);
        scoreText.setStyle(sf::Text::Bold);


        sf::FloatRect textRect = scoreText.getLocalBounds();
        scoreText.setPosition(
            window.getSize().x / 2.0f - textRect.width / 2.0f,
            10 + (45 - textRect.height) / 2.0f - 5
        );


        window.draw(scoreText);

        sf::Text infoText;
        infoText.setFont(resourceManager.getFont("main"));
        infoText.setFillColor(sf::Color(200, 200, 200));
        infoText.setCharacterSize(14);
        infoText.setString("W/S - Gracz 1   |   GORA/DOL - Gracz 2   |   SPACJA - Pauza   |   ESC - Wyjscie");
        infoText.setPosition(
            window.getSize().x / 2.0f - infoText.getLocalBounds().width / 2.0f,
            window.getSize().y - 30
        );
        window.draw(infoText);

        // Komunikat o pauzie
        if (gameState->isPaused() && currentScreen == GameScreen::PLAYING) {
            sf::RectangleShape pauseBackground(sf::Vector2f(200, 70));
            pauseBackground.setFillColor(sf::Color(0, 0, 0, 180));
            pauseBackground.setOutlineThickness(2.0f);
            pauseBackground.setOutlineColor(sf::Color::Yellow);
            pauseBackground.setPosition(window.getSize().x / 2.0f - 100, window.getSize().y / 2.0f - 35);
            window.draw(pauseBackground);

            pauseText.setPosition(window.getSize().x / 2.0f - pauseText.getLocalBounds().width / 2.0f,
                window.getSize().y / 2.0f - pauseText.getLocalBounds().height / 2.0f - 10);
            window.draw(pauseText);
        }

        // Komunikat o zwyciêstwie
        if (currentScreen == GameScreen::GAME_OVER) {
            sf::RectangleShape winnerBackground(sf::Vector2f(450, 180));
            winnerBackground.setFillColor(sf::Color(0, 0, 100, 200));
            winnerBackground.setOutlineThickness(3.0f);
            winnerBackground.setOutlineColor(sf::Color::Green);
            winnerBackground.setPosition(window.getSize().x / 2.0f - 225, window.getSize().y / 2.0f - 90);
            window.draw(winnerBackground);


            winnerText.setPosition(window.getSize().x / 2.0f - winnerText.getLocalBounds().width / 2.0f,
                window.getSize().y / 2.0f - winnerText.getLocalBounds().height / 2.0f - 30); // Przesuwamy wy¿ej
            window.draw(winnerText);


            sf::Text restartInstruction;
            restartInstruction.setFont(resourceManager.getFont("main"));
            restartInstruction.setCharacterSize(24);
            restartInstruction.setFillColor(sf::Color::Yellow);
            restartInstruction.setString("Nacisnij R aby zrestartowac gre");
            restartInstruction.setPosition(
                window.getSize().x / 2.0f - restartInstruction.getLocalBounds().width / 2.0f,
                window.getSize().y / 2.0f + 20 
            );
            window.draw(restartInstruction);
        }
        break;
    }

    window.display();
}

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void Game::restart() {
    try {

        gameState = std::make_unique<PongEngine::GameState>(window.getSize());


        gameOver = false;
        winner = 0;
        currentScreen = GameScreen::PLAYING;


        try {
            gameState->resetScores();
        }
        catch (const std::exception& e) {
            std::cerr << "Error calling resetScores: " << e.what() << std::endl;
        }


        gameState->resetBall();


        updateScoreText();

        std::cout << "Restarted" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error while restarting " << e.what() << std::endl;
    }
}