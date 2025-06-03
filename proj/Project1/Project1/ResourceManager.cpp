#include "ResourceManager.h"
#include <iostream>

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager& ResourceManager::getInstance() {
    if (instance == nullptr) {
        instance = new ResourceManager();
    }
    return *instance;
}

ResourceManager::~ResourceManager() {
    clear();
}

const sf::Font& ResourceManager::loadFont(std::string_view id, std::string_view filePath) {
    std::unique_ptr<sf::Font> font = std::make_unique<sf::Font>();
    if (!font->loadFromFile(std::string(filePath))) {
        throw std::runtime_error("Failed to load font: " + std::string(filePath));
    }

    std::string idStr(id);
    fonts[idStr] = std::move(font);
    return *fonts[idStr];
}

const sf::Texture& ResourceManager::loadTexture(std::string_view id, std::string_view filePath) {
    std::unique_ptr<sf::Texture> texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(std::string(filePath))) {
        throw std::runtime_error("Failed to load texture: " + std::string(filePath));
    }

    std::string idStr(id);
    textures[idStr] = std::move(texture);
    return *textures[idStr];
}

const sf::SoundBuffer& ResourceManager::loadSoundBuffer(std::string_view id, std::string_view filePath) {
    std::unique_ptr<sf::SoundBuffer> buffer = std::make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(std::string(filePath))) {
        throw std::runtime_error("Failed to load sound buffer: " + std::string(filePath));
    }

    std::string idStr(id);
    soundBuffers[idStr] = std::move(buffer);
    return *soundBuffers[idStr];
}

void ResourceManager::loadSoundBufferFromMemory(std::string_view id, const sf::SoundBuffer& buffer) {
    std::string idStr(id);
    soundBuffers[idStr] = std::make_unique<sf::SoundBuffer>();
    *soundBuffers[idStr] = buffer;
}

void ResourceManager::copySoundBuffer(std::string_view sourceId, std::string_view targetId) {
    std::string sourceIdStr(sourceId);
    std::string targetIdStr(targetId);

    if (soundBuffers.find(sourceIdStr) != soundBuffers.end()) {
        soundBuffers[targetIdStr] = std::make_unique<sf::SoundBuffer>(*soundBuffers[sourceIdStr]);
    }
}

void ResourceManager::fallbackFont(std::string_view id) {
    std::string idStr(id);
    fonts[idStr] = std::make_unique<sf::Font>();

}

bool ResourceManager::hasFallbackFont(std::string_view id) const {
    return fonts.find(std::string(id)) != fonts.end();
}

const sf::Font& ResourceManager::getFont(std::string_view id) {
    std::string idStr(id);
    auto it = fonts.find(idStr);
    if (it == fonts.end()) {
        throw std::runtime_error("Font not found: " + idStr);
    }
    return *(it->second);
}

const sf::Texture& ResourceManager::getTexture(std::string_view id) {
    std::string idStr(id);
    auto it = textures.find(idStr);
    if (it == textures.end()) {
        throw std::runtime_error("Texture not found: " + idStr);
    }
    return *(it->second);
}

const sf::SoundBuffer& ResourceManager::getSoundBuffer(std::string_view id) {
    std::string idStr(id);
    auto it = soundBuffers.find(idStr);
    if (it == soundBuffers.end()) {
        throw std::runtime_error("Sound buffer not found: " + idStr);
    }
    return *(it->second);
}

bool ResourceManager::hasFont(std::string_view id) const {
    return fonts.find(std::string(id)) != fonts.end();
}

bool ResourceManager::hasTexture(std::string_view id) const {
    return textures.find(std::string(id)) != textures.end();
}

bool ResourceManager::hasSoundBuffer(std::string_view id) const {
    return soundBuffers.find(std::string(id)) != soundBuffers.end();
}

void ResourceManager::removeFont(std::string_view id) {
    fonts.erase(std::string(id));
}

void ResourceManager::removeTexture(std::string_view id) {
    textures.erase(std::string(id));
}

void ResourceManager::removeSoundBuffer(std::string_view id) {
    soundBuffers.erase(std::string(id));
}

void ResourceManager::clear() {
    fonts.clear();
    textures.clear();
    soundBuffers.clear();
}