﻿#pragma once

#include <glm/ext/vector_int2.hpp>

#include <memory>
#include <string>

namespace inari {
class SpriteBatch;
class Window;
class ResourceManager;
class InputManager;
class GameTime;

class IGame {
   public:
    IGame();

    IGame(const IGame&) = delete;
    IGame(IGame&&) = delete;

    void run();

   protected:
    virtual bool init();

    virtual void loadResources() = 0;
    virtual void unloadResources() = 0;

    virtual void handleWindowResized(const glm::ivec2& size) = 0;

    virtual void update(const GameTime& gameTime) = 0;
    virtual void draw(const GameTime& gameTime) = 0;

    const std::shared_ptr<Window>& getWindow() const;
    const std::shared_ptr<SpriteBatch>& getSpriteBatch() const;
    const std::shared_ptr<ResourceManager>& getResourceManager() const;
    const std::shared_ptr<InputManager>& getInputManager() const;

   private:
    void handleEvents();

    bool m_isRunning;

    std::shared_ptr<Window> m_window;
    std::shared_ptr<SpriteBatch> m_spriteBatch;
    std::shared_ptr<ResourceManager> m_resources;
    std::shared_ptr<InputManager> m_inputManager;
};
}  // namespace inari
