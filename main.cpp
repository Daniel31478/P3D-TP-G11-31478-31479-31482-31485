#include <iostream>
#include <array>
#include <vector>
#include <string>
#include "Game.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"
#include "Shader.h"
#include "Renderer.h"
#include "Material.h"
#include "SnookerController.h"

namespace gep3d = game_engine_p3d;

int main() {
	// 1. Configuração Regional
	try {
#ifdef __linux__
		std::locale::global(std::locale("pt_PT"));
#else
		std::locale::global(std::locale("pt-PT"));
#endif
	}
	catch (const std::exception& e) {
		std::cerr << "Erro ao definir locale: " << e.what() << std::endl;
	}

	// 2. Inicialização do Jogo
	gep3d::Game game(1280, 720, "IPCA P3D - Mesa de Snooker");

	// 3. Configuração da Câmara
	gep3d::Camera camera;
	camera.set_background_color(glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));
	camera.LookAt(glm::vec3(0.0f, 11.0f, 8.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	camera.Prespective(45.0f, static_cast<float>(game.width()) / game.height(), 0.1f, 100.0f);
	camera.Viewport(game.width(), game.height(), 0, 0);

	camera.AddLayerToCullingMask("Default");
	camera.AddLayerToCullingMask("Environment");
	game.AddCamera(&camera);

	// 4. Criação das Luzes
	std::vector<gep3d::Light*> vetor_luzes;

	// Luz 1 - Ambiente (Força 4.0f para brilhar intensamente nas texturas)
	gep3d::Light* luz_ambiente = new gep3d::Light(glm::vec3(4.0f, 4.0f, 4.0f));

	// Luz 2 - Direcional
	gep3d::Light* luz_direcional = new gep3d::Light(glm::vec3(0.0f, -1.0f, -0.5f), glm::vec3(0.0f), glm::vec3(0.7f), glm::vec3(0.7f));

	// Luz 3 - Pontual 
	gep3d::Light* luz_pontual = new gep3d::Light(glm::vec3(0.0f, 1.5f, -1.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.8f), 1.0f, 0.09f, 0.032f);

	// Luz 4 - Cónica 
	gep3d::Light* luz_conica = new gep3d::Light(glm::vec3(0.0f, 3.5f, 3.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.8f), 1.0f, 0.09f, 0.032f, 12.5f, 17.5f);

	game.AddLight(luz_ambiente);     vetor_luzes.push_back(luz_ambiente);
	game.AddLight(luz_direcional);   vetor_luzes.push_back(luz_direcional);
	game.AddLight(luz_pontual);      vetor_luzes.push_back(luz_pontual);
	game.AddLight(luz_conica);       vetor_luzes.push_back(luz_conica);

	// 5. Shader do Projeto
	std::vector<gep3d::ShaderSource> sources = {
		{GL_VERTEX_SHADER, "light.vert"},
		{GL_FRAGMENT_SHADER, "light.frag"}
	};
	gep3d::Shader* shader = new gep3d::Shader(sources, "SnookerShader");

	// 6. Fundo / Mesa
	gep3d::Renderer* mesa_renderer = new gep3d::Renderer(shader, "MesaSnooker.obj");
	gep3d::Object* mesa = new gep3d::Object("Mesa", "Environment", nullptr, mesa_renderer, 0.0f, 0.0f, 0.0f);
	game.AddObject(mesa);

	// 7. Triângulo de Bolas
	std::vector<gep3d::Object*> vetor_bolas;
	float espacamento_x = 0.55f;
	float espacamento_z = 0.48f;
	float z_inicial_triangulo = -1.0f;

	int indice_bola = 1;
	for (int row = 0; row < 5; ++row) {
		for (int col = 0; col <= row; ++col) {
			if (indice_bola > 15) break;

			std::string ficheiro_obj = "Ball" + std::to_string(indice_bola) + ".obj";
			gep3d::Renderer* ball_renderer = new gep3d::Renderer(shader, ficheiro_obj);

			float x = (col - row * 0.5f) * espacamento_x;
			float z = z_inicial_triangulo - row * espacamento_z;

			gep3d::Object* bola = new gep3d::Object(
				"Bola" + std::to_string(indice_bola), "Default", nullptr, ball_renderer,
				x, 0.0f, z,
				0.0f, 0.0f, 0.0f,
				0.5f, 0.5f, 0.5f
			);

			game.AddObject(bola);
			vetor_bolas.push_back(bola);
			indice_bola++;
		}
	}

	// 8. Bola Branca Isolada
	gep3d::Renderer* branca_renderer = new gep3d::Renderer(shader, "Ball0.obj");
	gep3d::Object* bola_branca = new gep3d::Object(
		"BolaBranca", "Default", nullptr, branca_renderer,
		0.0f, 0.0f, 3.0f,
		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f
	);
	game.AddObject(bola_branca);

	// 9. Controlador
	gep3d::SnookerController* controlador = new gep3d::SnookerController(bola_branca, vetor_bolas, vetor_luzes);
	gep3d::Object* sistema_controlo = new gep3d::Object("SistemaControlo", "Default", controlador, nullptr, 0.0f, 0.0f, 0.0f);
	game.AddObject(sistema_controlo);

	// 10. Run Engine
	LOG("Motor de jogo pronto a arrancar...");
	game.Run();

	return 0;
}