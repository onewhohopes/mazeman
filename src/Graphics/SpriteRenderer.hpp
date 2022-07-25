#pragma once

#include <glm/mat4x4.hpp>

#include <map>
#include <memory>

class Shader;
class Sprite;
class Camera2D;

class SpriteRenderer
{
public:
    SpriteRenderer();
    ~SpriteRenderer();

    void begin(const glm::vec4 clearColor, const std::shared_ptr<Camera2D>& camera);
    void draw(const std::shared_ptr<Sprite>& sprite);
    void end();

private:
    uint32_t m_vao;
    uint32_t m_vbo;
    std::shared_ptr<Shader> m_shader;

    glm::mat4 m_cameraMatrix;
};
