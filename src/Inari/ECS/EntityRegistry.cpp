#include "EntityRegistry.hpp"

#include <algorithm>

#include "Inari/Utils/Random.hpp"

struct NameComparator {
   public:
    explicit NameComparator(const std::string_view& name) : m_name(name) {}

    bool operator()(const inari::EntityPtr& entity) {
        return entity ? entity->name == m_name : false;
    }

   private:
    std::string m_name;
};

namespace inari {
EntityPtr EntityRegistry::createEntity(const std::string_view& name) {
    EntityPtr entity = std::make_shared<Entity>();
    entity->uuid = random::generateUUID();
    entity->name = name;
    m_entities.push_back(entity);
    return entity;
}

EntityPtr EntityRegistry::getEntity(const std::string_view& name) {
    if (!name.empty()) {
        auto it = std::find_if(m_entities.begin(), m_entities.end(),
                               NameComparator(name));
        if (it != m_entities.end()) {
            return *it;
        }
    }
    return nullptr;
}

void EntityRegistry::forEachEntity(
    const std::function<void(const EntityPtr& entity)>& handler) const {
    if (!handler) {
        return;
    }

    std::for_each(m_entities.begin(), m_entities.end(),
                  [handler](const EntityPtr& entity) {
                      if (entity == nullptr) {
                          return;
                      }
                      handler(entity);
                  });
}

bool EntityRegistry::anyOfEntity(
    const std::function<bool(const EntityPtr& entity)>& handler) {
    if (!handler) {
        return false;
    }

    return std::any_of(m_entities.begin(), m_entities.end(),
                       [handler](const EntityPtr& entity) {
                           if (entity == nullptr) {
                               return false;
                           }
                           return handler(entity);
                       });
}

EntityPtr EntityRegistry::findEntity(
    const std::function<bool(const EntityPtr& entity)>& handler) const {
    if (!handler) {
        return nullptr;
    }

    const auto it = std::find_if(m_entities.begin(), m_entities.end(),
                                 [handler](const EntityPtr& entity) {
                                     if (entity == nullptr) {
                                         return false;
                                     }
                                     return handler(entity);
                                 });
    if (it == m_entities.end()) {
        return nullptr;
    }
    return *it;
}

bool EntityRegistry::destroyEntity(const EntityPtr& entity) {
    assert(entity != nullptr && "Entity is empty");

    const bool result = m_components.erase(entity->uuid);
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it != m_entities.end()) {
        m_entities.erase(it);
        return result;
    }
    return result;
}

bool EntityRegistry::destroyEntity(const std::string_view& name) {
    if (!name.empty()) {
        auto it = std::find_if(m_entities.begin(), m_entities.end(),
                               NameComparator(name));
        if (it != m_entities.end()) {
            return destroyEntity(*it);
        }
    }

    return false;
}
}  // namespace inari