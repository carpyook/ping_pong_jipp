#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <string_view>
#include <memory>
#include <stdexcept>
#include <optional>

class ResourceManager {
private:
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> soundBuffers;
    static ResourceManager* instance;


    ResourceManager() = default;

public:

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;


    static ResourceManager& getInstance();


    ~ResourceManager();

    const sf::Font& loadFont(std::string_view id, std::string_view filePath);
    const sf::Texture& loadTexture(std::string_view id, std::string_view filePath);
    const sf::SoundBuffer& loadSoundBuffer(std::string_view id, std::string_view filePath);

    void loadSoundBufferFromMemory(std::string_view id, const sf::SoundBuffer& buffer);
    void copySoundBuffer(std::string_view sourceId, std::string_view targetId);
    void fallbackFont(std::string_view id);
    bool hasFallbackFont(std::string_view id) const;

    const sf::Font& getFont(std::string_view id);
    const sf::Texture& getTexture(std::string_view id);
    const sf::SoundBuffer& getSoundBuffer(std::string_view id);

    bool hasFont(std::string_view id) const;
    bool hasTexture(std::string_view id) const;
    bool hasSoundBuffer(std::string_view id) const;

    void removeFont(std::string_view id);
    void removeTexture(std::string_view id);
    void removeSoundBuffer(std::string_view id);

    void clear();
};