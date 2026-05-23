#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"

namespace game_engine_p3d {

    class Material {
    public:
        Material(const Shader* shader, const std::string obj_filename);
        Material(glm::vec3 ka, glm::vec3 kd, glm::vec3 ks, glm::vec3 ke, float Ns, float Ni, float d, int illum,
            Shader* shader, std::vector<Texture> textures);

        // Deixamos apenas a declaração limpa aqui
        void Use() const;

        Shader* GetShader() const { return const_cast<Shader*>(shader_); }

    private:
        bool findMaterialFile(const std::string& obj_filename);
        void LoadMaterialFromFile(void);

        const Shader* shader_ = nullptr;
        std::string mtl_filename_;

        glm::vec3 ka_{ 0.0f };
        glm::vec3 kd_{ 0.0f };
        glm::vec3 ks_{ 0.0f };
        glm::vec3 ke_{ 0.0f };

        float Ns_ = 0.0f;
        float Ni_ = 1.0f;
        float d_ = 1.0f;
        int illum_ = 2;

        std::vector<Texture> texture_;
    };

}