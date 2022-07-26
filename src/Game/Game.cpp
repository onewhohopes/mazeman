#include "Game.hpp"

#include <SDL_keycode.h>

// inari
#include "Inari/ECS/Components/Sprite.hpp"
#include "Inari/ECS/Components/Transform.hpp"
#include "Inari/ECS/EntityRegistry.hpp"
#include "Inari/ECS/SystemRegistry.hpp"
#include "Inari/ECS/Systems/AnimationSystem.hpp"
#include "Inari/ECS/Systems/PhysicsSystem.hpp"
#include "Inari/ECS/Systems/SpriteRenderSystem.hpp"

#include "Inari/Graphics/SpriteBatch.hpp"
#include "Inari/Graphics/Window.hpp"

#include "Inari/Resources/ResourceManager.hpp"
#include "Inari/Resources/Texture2D.hpp"
#include "Inari/Resources/World.hpp"

#include "Inari/Utils/Camera2D.hpp"
#include "Inari/Utils/Colors.hpp"

#include "Inari/InputManager.hpp"
// inari

// game
#include "Game/Components/Collision.hpp"
#include "Game/Components/Player.hpp"
#include "Game/Prefabs/Mazeman.hpp"
#include "Game/Systems/CollisionSystem.hpp"
#include "Game/Systems/InputSystem.hpp"
// game

namespace constants {
constexpr std::string_view title = "MazeMan";
constexpr int screenFps = 30;
constexpr glm::ivec2 windowSize(1280, 720);
constexpr std::string_view worldFilename = "res/world.ldtk";
}  // namespace constants

Game::Game()
    : m_entityRegistry(std::make_shared<inari::EntityRegistry>()),
      m_systemRegistry(std::make_unique<inari::SystemRegistry>()),
      m_camera(nullptr) {}

Game::~Game() = default;

bool Game::init() {
    if (IGame::init()) {
        // Init window
        const auto& window = getWindow();
        window->setWindowSize(constants::windowSize);
        window->setTitle(constants::title);
        window->setFrameLimit(constants::screenFps);

        // Init camera
        m_camera = std::make_unique<inari::Camera2D>(constants::windowSize);

        // Init systems
        m_systemRegistry->addSystem<inari::AnimationSystem>(m_entityRegistry);
        m_systemRegistry->addSystem<inari::PhysicsSystem>(m_entityRegistry);
        m_systemRegistry->addSystem<inari::SpriteRenderSystem>(
            m_entityRegistry, getSpriteBatch());
        m_systemRegistry->addSystem<InputSystem>(m_entityRegistry,
                                                 getInputManager());
        m_systemRegistry->addSystem<CollisionSystem>(m_entityRegistry);

        return true;
    }
    return false;
}

void Game::loadResources() {
    auto world =
        getResourceManager()->load<inari::World>(constants::worldFilename);
    if (world) {
        const inari::WorldLevel& level = world->getLevel(0);
        m_camera->setScale(
            glm::vec2(level.size.y / m_camera->getWindowSize().y));

        {
            auto it = level.layers.find("Collisions");
            for (const auto& tile : it->second.tiles) {
                inari::Sprite sprite;
                sprite.texture = getResourceManager()->load<inari::Texture2D>(
                    "res/walls.png");
                sprite.sourceRect = tile.sourceRect;

                inari::Transform transform;
                transform.position = tile.position;
                transform.size =
                    glm::vec2(tile.sourceRect.w, tile.sourceRect.z);

                Collision collision;
                collision.isDynamic = false;

                auto tileEntity = m_entityRegistry->createEntity();
                m_entityRegistry->emplaceComponent(tileEntity, sprite);
                m_entityRegistry->emplaceComponent(tileEntity, transform);
                m_entityRegistry->emplaceComponent(tileEntity, collision);
            }
        }

        {
            auto it = level.layers.find("Spawns")->second.entityInstances.find(
                "MazeMan");
            const inari::LevelEntityInstance entityInstance = it->second;
            prefabs::createMazeman(
                m_entityRegistry,
                getResourceManager()->load<inari::Texture2D>("res/mazeman.png"),
                entityInstance.position, entityInstance.get<float>("angle"));
        }
    }
}

void Game::unloadResources() {
    getResourceManager()->unload<inari::Texture2D>("res/mazeman.png");
}

void Game::handleWindowResized(const glm::ivec2& size) {
    m_camera->setWindowSize(size);

    auto world =
        getResourceManager()->load<inari::World>(constants::worldFilename);
    if (world) {
        const inari::WorldLevel& level = world->getLevel(0);
        m_camera->setScale(
            glm::vec2(level.size.y / m_camera->getWindowSize().y));
    }
}

void Game::update(const inari::GameTime& gameTime) {
    m_systemRegistry->updateSystem<InputSystem>(gameTime);
    m_systemRegistry->updateSystem<CollisionSystem>(gameTime);
    m_systemRegistry->updateSystem<inari::PhysicsSystem>(gameTime);
    m_systemRegistry->updateSystem<inari::AnimationSystem>(gameTime);

    if (getInputManager()->isKeyPressed(SDLK_F1)) {
        getSpriteBatch()->toggleWireframeMode();
    }
}

void Game::draw(const inari::GameTime& gameTime) {
    glm::vec3 bgColor(0.0f);
    auto world =
        getResourceManager()->load<inari::World>(constants::worldFilename);
    if (world) {
        const inari::WorldLevel& level = world->getLevel(0);
        bgColor = level.backgroundColor;
    }
    getWindow()->clear(bgColor);

    auto spriteRenderSystem =
        m_systemRegistry->getSystem<inari::SpriteRenderSystem>();
    if (spriteRenderSystem) {
        spriteRenderSystem->draw(gameTime, m_camera->getTransform());
    }

    getWindow()->display();
}
