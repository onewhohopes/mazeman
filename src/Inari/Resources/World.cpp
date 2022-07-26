#include "World.hpp"

#include <nlohmann/json.hpp>

#include "Inari/Utils/Colors.hpp"

namespace {
constexpr std::string_view levelPrefix = "Level_";

std::any createAny(const std::string& type, const nlohmann::json& value) {
    if (type == "Int") {
        return value.get<int>();
    }
    if (type == "Float") {
        return value.get<float>();
    }
    if (type == "Bool") {
        return value.get<bool>();
    }
    if (type == "Color") {
        return inari::colors::toGL<3>(value.get<std::string>());
    }
    if (type == "Point") {
        return glm::vec2(value.at("cx").get<int>(), value.at("cy").get<int>());
    }

    return value.get<std::string>();
}
}  // namespace

namespace inari {

void from_json(const nlohmann::json& j, LevelTile& tile) {
    j.at("px").at(0).get_to(tile.position.x);
    j.at("px").at(1).get_to(tile.position.y);
    j.at("src").at(0).get_to(tile.sourceRect.x);
    j.at("src").at(1).get_to(tile.sourceRect.y);
}

void from_json(const nlohmann::json& j, LevelEntityInstance& entity) {
    j.at("px").at(0).get_to(entity.position.x);
    j.at("px").at(1).get_to(entity.position.y);
}

void from_json(const nlohmann::json& j, LevelLayer& layer) {
    j.at("__identifier").get_to(layer.identifier);

    int gridSize = 0;
    j.at("__gridSize").get_to(gridSize);

    std::string tilesKey;
    if (j.contains("autoLayerTiles")) {
        tilesKey = "autoLayerTiles";
    } else if (j.contains("gridTiles")) {
        tilesKey = "gridTiles";
    }

    if (!tilesKey.empty()) {
        for (auto& element : j.at(tilesKey)) {
            LevelTile tile{};
            element.get_to(tile);
            tile.sourceRect.z = static_cast<float>(gridSize);
            tile.sourceRect.w = static_cast<float>(gridSize);
            layer.tiles.push_back(tile);
        }
    }

    for (auto& entity : j.at("entityInstances")) {
        auto levelEntity = entity.get<LevelEntityInstance>();

        if (entity.contains("fieldInstances")) {
            for (auto& field : entity.at("fieldInstances")) {
                levelEntity
                    .fields[field.at("__identifier").get<std::string>()] =
                    createAny(field.at("__type").get<std::string>(),
                              field.at("__value"));
            }
        }
        layer.entityInstances[entity.at("__identifier").get<std::string>()] =
            levelEntity;
    }
}

void from_json(const nlohmann::json& j, WorldLevel& level) {
    j.at("identifier").get_to(level.identifier);
    std::string bgColor;
    j.at("bgColor").get_to(bgColor);
    level.backgroundColor = colors::toGL<3>(bgColor);
    j.at("pxWid").get_to(level.size.x);
    j.at("pxHei").get_to(level.size.y);

    for (auto& element : j.at("layerInstances")) {
        LevelLayer layer;
        element.get_to(layer);
        level.layers[layer.identifier] = layer;
    }
}

struct World::Data {
    std::map<std::string, WorldLevel> levels;

    friend void from_json(const nlohmann::json& j, World::Data& data) {
        for (auto& element : j.at("levels")) {
            WorldLevel level;
            element.get_to(level);
            data.levels[level.identifier] = level;
        }
    }
};

std::shared_ptr<World> World::createFromData(const std::string_view& data) {
    auto jsonData = nlohmann::json::parse(data);
    auto worldData = std::make_unique<Data>(jsonData.get<World::Data>());
    return std::make_shared<World>(std::move(worldData));
}

World::World(std::unique_ptr<Data>&& data) : m_data(std::move(data)) {}
World::~World() = default;

const WorldLevel& World::getLevel(int idx) const {
    const auto it =
        m_data->levels.find(levelPrefix.data() + std::to_string(idx));
    assert(it != m_data->levels.end());
    return it->second;
}

}  // namespace inari