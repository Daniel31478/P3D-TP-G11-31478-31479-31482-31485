#define GLEW_STATIC // Definição necessária, antes de incluir 'GL\glew.h', sempre que se usa GLEW como uma biblioteca estática
#include <GL/glew.h> // Necessário para utilizar a função 'glDrawArrays()'
#include <glm/gtc/type_ptr.hpp> // Necessário para usar glm::value_ptr()
#include <glm\gtc\matrix_inverse.hpp> // glm::inverseTranspose()
#include "Common.h"
#include "Renderer.h"
#include "Shader.h"

namespace game_engine_p3d {

	Renderer::Renderer(const Shader* shader, const std::string obj_filename) :
		mesh_{ Mesh(shader, obj_filename) }, // Cria uma malha a partir do ficheiro OBJ especificado, carregando os dados para os buffers OpenGL, e associa os atributos (buffers) ao shader
		material_{ Material(shader, obj_filename) } // Cria o material com o shader fornecido e o caminho do objeto (o caminho do ficheiro MTL será extraído do ficheiro OBJ)
	{
		LOG("Creating Renderer with shader '" << shader->name() << "' and OBJ file: " + obj_filename);

		// Se o shader não for fornecido, encerra o programa com uma mensagem de erro
		if (!shader) {
			LOG("Error: Shader is null. Cannot create a Renderer without a shader.");
			exit(EXIT_FAILURE); // Encerra o programa com erro
		}
		// Se não for fornecido um caminho para o objeto, encerra o programa com uma mensagem de erro
		if (obj_filename.empty()) {
			LOG("Error: Object filename is empty. Cannot create a Renderer without an OBJ path.");
			exit(EXIT_FAILURE); // Encerra o programa com erro
		}

		LOG("Renderer created with shader '" << shader->name() << "' and OBJ path : " + obj_filename);
	}

	void Renderer::Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, std::string layer, std::vector<Light*> lights) {
		LOG("Draw called.");

		// Ativa o Vertex Array Object (VAO) associado à malha
		mesh_.Use();

		// Ativa o material (i.e., o programa shader e a(s) textura(s)) que será usado para renderizar o objeto e configura os uniforms do material no shader
		material_.Use();

		// Atualiza as matrizes de transformação no shader
		{
			// CORREÇÃO AQUI: Obtém o shader do material de forma segura usando a nova função GetShader()
			Shader* shader = material_.GetShader();

			if (shader) {
				// NOTE: Os nomes dos uniforms estão hardcoded (ver ficheiro "Common.h") e devem existir no shader
				// Atualiza a matriz Model do objeto
				shader->SetUniform<glm::mat4>(kModelMatrixName, model);
				// Atualiza a matriz View e Projection da câmara fornecida
				shader->SetUniform<glm::mat4>(kViewMatrixName, view);
				// Alternativamente, ou em complemento, pode usar a matriz ModelView (produto de Model e View)
				glm::mat4 modelview = view * model;
				shader->SetUniform<glm::mat4>(kModelViewMatrixName, modelview);
				// Atualiza a matriz Normal (matriz usada para transformar normais, que é a transposta da inversa da matriz ModelView)
				glm::mat3 NormalMatrix = glm::inverseTranspose(glm::mat3(modelview));
				// FIXME: adicionar esta string como constante em "Common.h"
				shader->SetUniform<glm::mat3>("NormalMatrix", modelview);
				// Atualiza a matriz Projection
				shader->SetUniform<glm::mat4>(kProjectionMatrixName, projection);
			}
		}

		// Para cada luz na lista de luzes fornecida, configura os uniforms no shader
		LOG("***************** Setting " << lights.size() << " Lights ********************");
		for (auto& light : lights) {
			// Se alguma das layers da luz for igual à layer do objeto, configura os uniforms da luz no shader
			if (light->IsInLayer(layer)) {

				LOG("Configuring a light source of type " << light->type_string());
				// Envia parâmetros da luz para o shader

				// CORREÇÃO AQUI: Obtém o shader de forma segura para as luzes usando GetShader()
				Shader* shader = material_.GetShader();

				if (shader) {
					// Se a luz for do tipo ambiente (Fonte de luz ambiente global)
					if (light->type() == LightType::kAmbient) {
						shader->SetUniform<glm::vec3>("ambientLight.ambient", light->ambient());
					}
					// Se a luz for do tipo direcional (Fonte de luz direcional)
					else if (light->type() == LightType::kDirectional) {
						shader->SetUniform<glm::vec3>("directionalLight.direction", light->direction());
						shader->SetUniform<glm::vec3>("directionalLight.ambient", light->ambient());
						shader->SetUniform<glm::vec3>("directionalLight.diffuse", light->diffuse());
						shader->SetUniform<glm::vec3>("directionalLight.specular", light->specular());
					}
					// Se a luz for do tipo pontual (Fonte de luz pontual)
					else if (light->type() == LightType::kPoint) {
						shader->SetUniform<glm::vec3>("pointLight[0].position", light->position());
						shader->SetUniform<glm::vec3>("pointLight[0].ambient", light->ambient());
						shader->SetUniform<glm::vec3>("pointLight[0].diffuse", light->diffuse());
						shader->SetUniform<glm::vec3>("pointLight[0].specular", light->specular());
						shader->SetUniform<float>("pointLight[0].constant", light->constant());
						shader->SetUniform<float>("pointLight[0].linear", light->linear());
						shader->SetUniform<float>("pointLight[0].quadratic", light->quadratic());
					}
					// Se a luz for do tipo cónica (Fonte de luz cónica)
					else if (light->type() == LightType::kSpotlight) {
						shader->SetUniform<glm::vec3>("spotLight.position", light->position());
						shader->SetUniform<glm::vec3>("spotLight.spotDirection", light->direction());
						shader->SetUniform<glm::vec3>("spotLight.ambient", light->ambient());
						shader->SetUniform<glm::vec3>("spotLight.diffuse", light->diffuse());
						shader->SetUniform<glm::vec3>("spotLight.specular", light->specular());
						shader->SetUniform<float>("spotLight.constant", light->constant());
						shader->SetUniform<float>("spotLight.linear", light->linear());
						shader->SetUniform<float>("spotLight.quadratic", light->quadratic());
						shader->SetUniform<float>("spotLight.spotCutoff", light->cutOff());
						shader->SetUniform<float>("spotLight.spotExponent", 12.0f);
					}
				}
			}
		}

		// Desenha a malha usando o shader e as propriedades do material
		glDrawArrays(GL_TRIANGLES, 0, mesh_.vertex_count());

		LOG("Draw completed.");
	}
}