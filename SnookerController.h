#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "Behaviour.h"
#include "Object.h"
#include "Light.h"

namespace game_engine_p3d {

    class SnookerController : public Behaviour {
    public:
        SnookerController(Object* bola_branca, const std::vector<Object*>& outras_bolas, const std::vector<Light*>& luzes)
            : bola_branca_(bola_branca), bolas_triangulo_(outras_bolas), luzes_(luzes), animacao_ativa_(false), z_atual_(3.0f), angulo_acumulado_(0.0f) {

            // As 4 luzes começam LIGADAS
            luz_estado_[0] = true;
            luz_estado_[1] = true;
            luz_estado_[2] = true;
            luz_estado_[3] = true;
        }

        void Start(Object& object) override {
            LOG("SnookerController Iniciado: Pressiona ESPAÇO para tacada. Teclas 1 a 4 alternam as luzes.");
        }

        void Update(Object& object) override {
            GLFWwindow* window = glfwGetCurrentContext();
            if (!window) return;

            // --- LÓGICA DAS LUZES ---
            static bool t1 = false, t2 = false, t3 = false, t4 = false;

            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !t1) { AlternarLuz(0); t1 = true; }
            else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) t1 = false;

            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !t2) { AlternarLuz(1); t2 = true; }
            else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) t2 = false;

            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !t3) { AlternarLuz(2); t3 = true; }
            else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) t3 = false;

            if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && !t4) { AlternarLuz(3); t4 = true; }
            else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) t4 = false;

            // --- LÓGICA DA ANIMAÇÃO ---
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !animacao_ativa_) {
                animacao_ativa_ = true;
                LOG("Tacada efetuada! Bola em movimento.");
            }

            if (animacao_ativa_ && bola_branca_) {
                float velocidade = 25.0f;
                float dt = 0.016f;

                z_atual_ -= velocidade * dt;
                angulo_acumulado_ -= velocidade * 150.0f * dt;

                glm::mat4 nova_matriz = glm::mat4(1.0f);
                nova_matriz = glm::translate(nova_matriz, glm::vec3(0.0f, 0.0f, z_atual_));
                nova_matriz = glm::rotate(nova_matriz, glm::radians(angulo_acumulado_), glm::vec3(1.0f, 0.0f, 0.0f));
                nova_matriz = glm::scale(nova_matriz, glm::vec3(0.5f, 0.5f, 0.5f));

                bola_branca_->model().matrix_ = nova_matriz;

                if (z_atual_ <= -0.55f) {
                    animacao_ativa_ = false;
                    LOG("Impacto! A bola atingiu o triângulo.");
                }
            }
        }

    private:
        Object* bola_branca_;
        std::vector<Object*> bolas_triangulo_;
        std::vector<Light*> luzes_;
        bool animacao_ativa_;
        float z_atual_;
        float angulo_acumulado_;

        bool luz_estado_[4];

        void AlternarLuz(size_t index) {
            if (index < luzes_.size() && luzes_[index]) {
                luz_estado_[index] = !luz_estado_[index];

                // LUZ 1 (Ambiente)
                if (index == 0) {
                    if (luz_estado_[index]) {
                        // FORÇA MÁXIMA 4.0f PARA COMPENSAR A ABSORÇÃO DA TEXTURA DAS BOLAS
                        luzes_[index]->set_ambient(glm::vec3(4.0f, 4.0f, 4.0f));
                        LOG("Luz 1 (Ambiente) LIGADA.");
                    }
                    else {
                        luzes_[index]->set_ambient(glm::vec3(0.0f));
                        LOG("Luz 1 (Ambiente) DESLIGADA.");
                    }
                }
                // LUZES 2, 3 e 4 (Direcional, Pontual, Cónica)
                else {
                    if (luz_estado_[index]) {
                        glm::vec3 brilho = (index == 1) ? glm::vec3(0.7f) : glm::vec3(1.0f);
                        luzes_[index]->set_diffuse(brilho);
                        luzes_[index]->set_specular(glm::vec3(0.8f));
                        LOG("Luz " << (index + 1) << " LIGADA.");
                    }
                    else {
                        luzes_[index]->set_diffuse(glm::vec3(0.0f));
                        luzes_[index]->set_specular(glm::vec3(0.0f));
                        LOG("Luz " << (index + 1) << " DESLIGADA.");
                    }
                }
            }
        }
    };
}