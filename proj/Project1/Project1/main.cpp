#include "Game.h"
#include <Windows.h>
#include <iostream>
#include <stdexcept>
#include <direct.h>


int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
) {
    
    AllocConsole();
    FILE* pConsole;
    freopen_s(&pConsole, "CONOUT$", "w", stdout);
    std::cout << "=== PONG GAME DEBUG CONSOLE ===" << std::endl;
    std::cout << "-------------------------------" << std::endl;

    try {
        
        char currentDir[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, currentDir);
        std::cout << "Current working directory: " << currentDir << std::endl;

        
        std::string resourcesPath = std::string(currentDir) + "\\resources";
        if (CreateDirectoryA(resourcesPath.c_str(), NULL) ||
            GetLastError() == ERROR_ALREADY_EXISTS) {
            std::cout << "Resources directory exists or was created" << std::endl;
        }
        else {
            std::cout << "Warning: Could not create resources directory" << std::endl;
        }

        
        std::string fontPath = resourcesPath + "\\arial.ttf";
        std::cout << "Checking for font at: " << fontPath << std::endl;

        
        bool fontFound = false;
        FILE* testFile = nullptr;


        if (fopen_s(&testFile, fontPath.c_str(), "r") == 0) {
            std::cout << "Font file found at: " << fontPath << std::endl;
            fclose(testFile);
            fontFound = true;
        }
        else {
            std::cout << "Font file NOT found at: " << fontPath << std::endl;

         
            fontPath = std::string(currentDir) + "\\arial.ttf";
            std::cout << "Checking for font at: " << fontPath << std::endl;

            if (fopen_s(&testFile, fontPath.c_str(), "r") == 0) {
                std::cout << "Font file found at: " << fontPath << std::endl;
                fclose(testFile);
                fontFound = true;
            }
            else {
                std::cout << "Font file NOT found at: " << fontPath << std::endl;

       
                fontPath = "C:\\Windows\\Fonts\\arial.ttf";
                std::cout << "Checking for font at: " << fontPath << std::endl;

                if (fopen_s(&testFile, fontPath.c_str(), "r") == 0) {
                    std::cout << "Font file found at: " << fontPath << std::endl;
                    fclose(testFile);
                    fontFound = true;
                }
                else {
                    std::cout << "Font file NOT found at: " << fontPath << std::endl;
                }
            }
        }

        if (!fontFound) {
            std::cout << "Warning: Arial font not found in any location!" << std::endl;
            std::cout << "Game will attempt to continue but may encounter errors." << std::endl;
        }


        const char* soundFiles[] = { "hit.wav", "score.wav", "wall.wav" };
        for (const char* soundFile : soundFiles) {
            std::string soundPath = resourcesPath + "\\" + soundFile;
            if (fopen_s(&testFile, soundPath.c_str(), "r") == 0) {
                std::cout << "Sound file found: " << soundPath << std::endl;
                fclose(testFile);
            }
            else {
                soundPath = std::string(currentDir) + "\\" + soundFile;
                if (fopen_s(&testFile, soundPath.c_str(), "r") == 0) {
                    std::cout << "Sound file found: " << soundPath << std::endl;
                    fclose(testFile);
                }
                else {
                    std::cout << "Sound file NOT found: " << soundFile << std::endl;
                    std::cout << "Game will use fallback sounds" << std::endl;
                }
            }
        }

        std::cout << "\nCreating game object..." << std::endl;
        Game game;
        std::cout << "Game object created successfully!" << std::endl;
        std::cout << "Running game..." << std::endl;
        game.run();

        std::cout << "Game finished normally" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        MessageBoxA(NULL, e.what(), "Fatal Error", MB_ICONERROR);
        std::cout << "FATAL ERROR: " << e.what() << std::endl;
        std::cout << "Press any key to exit..." << std::endl;
        std::cin.get();
        return 1;
    }
}