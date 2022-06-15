#pragma once

#include <memory>

class Texture2D;
class Shader;

class SpriteRenderer
{
public:
    explicit SpriteRenderer(const std::shared_ptr<Shader>& shader);
    ~SpriteRenderer();

    void draw(const std::shared_ptr<Texture2D>& texture);

private:
    std::shared_ptr<Shader> m_shader;
    uint32_t m_quadVAO;
};