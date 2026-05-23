#pragma once


// ==============================================================
// Ficheiro:    Object.h
// Descrição:   Declaração da classe Object
// Autor:       Duarte Duque
// Data:        21/07/2025
// Versão:      1.0.0
// Dependências: Transform.h
// Observações:
// Esta classe representa um objeto no jogo.
// ==============================================================


#include <vector> // Para std::vector

#include "Transform.h" // Inclui a classe 'Transform'


namespace game_engine_p3d {

	// Não é necessário incluir os ficheiros de cabeçalho completos, apenas fazer declarações antecipadas (forward declarations).
	// Isto é suficiente quando se usa apenas apontadores ou referências a essas classes.
	// Esta abordagem reduz o tempo de compilação e evita dependências circulares.
	class Game; // Declaração antecipada da classe 'Game'
	class Camera; // Declaração antecipada da classe 'Camera'
	class Light; // Declaração antecipada da classe 'Light'
	class Renderer; // Declaração antecipada da classe 'Renderer'
	class Behaviour; // Declaração antecipada da classe 'Behaviour'


	class Object {
	public:
		// ------------------------------------------------------------
		// Construtores e destrutores
		// ------------------------------------------------------------
		Object(std::string name, Layer layer, Behaviour* behaviour, Renderer* renderer, 
			float x, float y, float z, float pitch = 0.0f, float yaw = 0.0f, float roll = 0.0f,
			float scale_x = 1.0f, float scale_y = 1.0f, float scale_z = 1.0f); // Construtor com parâmetros
		~Object() = default;	// Destrutor padrão (indico ao compilador para gerar o destrutor padrão)
		// ------------------------------------------------------------
		// Accessors e mutators
		// ------------------------------------------------------------
		int id() const { return id_; }				// Retorna o ID do objeto
		std::string name() const { return name_; }	// Retorna o nome do objeto
		Layer layer() const { return layer_; }		// Retorna a layer do objeto
		void set_game(Game* game) { game_ = game; }	// Associa a instância do jogo ao objeto
		Game* game() const { return game_; }		// Retorna a instância do jogo associada ao objeto
		Transform& model() { return model_; }		// Retorna a transformação associada ao objeto (matriz Model)
		// ------------------------------------------------------------
		// Funções de ciclo de vida do objeto
		// ------------------------------------------------------------
		void Start();
		void Update();
		void FixedUpdate();
		void PhysicsUpdate();
		void LateUpdate();
		void OnCollisionEnter(Object& other);
		void OnCollisionExit(Object& other);
		void OnTriggerEnter(Object& other);
		void OnTriggerExit(Object& other);
		void OnDestroy();
		void Draw(Camera& camera, std::vector<Light*>& lights);

	private:
		// ------------------------------------------------------------
		// Dados-membro privados
		// ------------------------------------------------------------
		static int object_count_;	// Contador de objetos
		int id_ = 0;				// ID do objeto
		std::string name_{};		// Nome do objeto (não pode ser vazio)
		Layer layer_ = "Default";	// Layer do objeto (padrão: "Default")
		Game* game_ = nullptr;		// Apontador para o jogo ao qual o objeto pertence
		Transform model_;			// Transformação associada ao objeto (matriz Model)
		Behaviour* behaviour_ = nullptr;	// Comportamento associado ao objeto (pode ser nulo se não houver comportamento)
		Renderer* renderer_ = nullptr;		// Renderizador associado ao objeto (usado para desenhar o objeto). Pode ser nulo, se não houver renderização associada.
	};
}