#pragma once

#include "Inari/ECS/Systems/ISystem.hpp"

// Simple AABB collision detection system
class CollisionSystem : public inari::ISystem {
   public:
    explicit CollisionSystem(std::shared_ptr<inari::EntityRegistry> registry);

   protected:
    void update(float dt, const inari::EntityPtr& entity) override;
};
