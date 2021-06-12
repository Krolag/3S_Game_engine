#pragma once
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include "GameLogic/Components/Component.h"

namespace GameLogic
{
	enum EnemyState
	{
		STOIC_STATE = 1,
		CHASING_STATE = 2,
		ATTACK_STATE = 3,
		DYING_STATE = 4,
		DEAD_STATE = 5
	};

	class Enemy : public Component
	{
	public:
		/* Public fields */

		/* Constructors/Destructors */
		Enemy(Proctor* _proctor, Proctor* _playerOneRef, Proctor* _playerTwoRef);
		Enemy(Proctor* _proctor, Proctor* _playerOneRef, Proctor* _playerTwoRef, float _maxHealth, float _damage, float _sightRadius, float _attackRadius, float _maxVelocity, float _acceleration, float _maxGravity, float _gravityAcceleration);
		~Enemy();

		/* Public methods */
		void update() override;
		void takeDamage(Proctor* playerRef, float damage);

		/* Getters */

		/* Setters */

	private:
		/* Private fields */
		float maxHealth;
		float currentHealth;
		float sightRadius;
		float damage;
		float attackRadius;
		float maxVelocity;
		float currentVelocity;
		float acceleration;
		float maxGravity;
		float currentGravity;
		float gravityAcceleration;
		
		EnemyState currentState;
		
		/* References to our players */
		Proctor* playerOneRef;
		Proctor* playerTwoRef;
		Proctor* currentlyChasedPlayer;

		glm::vec3 wanderDirection;
		float newWanderDirectionTimer;
		
		/* Private methods */
		void stoicBehaviour();
		void chasingBehaviour();
		void attackBehaviour();
		void dyingBehaviour();
		float distanceToPlayer(Proctor* player);
	};
}