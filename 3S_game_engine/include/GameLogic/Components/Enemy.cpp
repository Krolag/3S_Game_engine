#include "Enemy.h"
#include "GameLogic/Proctor.h"
#include "Application/Randomizer.h"
#include <iostream>

namespace GameLogic
{
	/* Default enemy values */
	constexpr float MAX_HEALTH_DEFAULT = 100.0f;
	constexpr float DAMAGE_DEFAULT = 26.0f;
	constexpr float SIGHT_RADIUS_DEFAULT = 15.0f;
	constexpr float ATTACK_RADIUS_DEFAULT = 0.5f;
	constexpr float MAX_VELOCITY_DEFAULT = 0.05f;
	constexpr float ACCELERATION_DEFAULT = 0.001f;
	constexpr float MAX_GRAVITY_DEFAULT = 0.05f;
	constexpr float GRAVITY_ACCELERATION_DEFAULT = 0.01f;

	/* Constructors and destructors */
	// Constructor initialize enemy by default values
	Enemy::Enemy(Proctor* _proctor, Proctor* _playerOneRef, Proctor* _playerTwoRef) : Component(C_ENEMY, _proctor),
		playerOneRef(_playerOneRef), playerTwoRef(_playerTwoRef), currentlyChasedPlayer(nullptr),
		maxHealth(MAX_HEALTH_DEFAULT), currentHealth(MAX_HEALTH_DEFAULT), damage(DAMAGE_DEFAULT), sightRadius(SIGHT_RADIUS_DEFAULT), attackRadius(ATTACK_RADIUS_DEFAULT),
		maxVelocity(MAX_VELOCITY_DEFAULT), currentVelocity(0.0f), acceleration(ACCELERATION_DEFAULT),
		maxGravity(MAX_GRAVITY_DEFAULT), currentGravity(0.0f), gravityAcceleration(GRAVITY_ACCELERATION_DEFAULT),
		currentState(STOIC_STATE), wanderDirection(glm::vec3(0.0f)), newWanderDirectionTimer(0.0f)
	{
		if (_proctor != NULL)
		{
			proctor->addComponent(this);
		}
	}

	// Constructor initialize enemy by the given type
	Enemy::Enemy(Proctor* _proctor, Proctor* _playerOneRef, Proctor* _playerTwoRef, std::string _enemyType) : Component(C_ENEMY, _proctor),
		playerOneRef(_playerOneRef), playerTwoRef(_playerTwoRef), currentlyChasedPlayer(nullptr),
		currentState(STOIC_STATE), wanderDirection(glm::vec3(0.0f)), newWanderDirectionTimer(0.0f)
	{
		if(_enemyType == "locals_00" || _enemyType == "locals_01")
		{
			// Initialize normal enemy
			maxHealth = MAX_HEALTH_DEFAULT;
			currentHealth = MAX_HEALTH_DEFAULT;
			damage = DAMAGE_DEFAULT;
			sightRadius = SIGHT_RADIUS_DEFAULT;
			attackRadius = ATTACK_RADIUS_DEFAULT;
			maxVelocity = MAX_VELOCITY_DEFAULT;
			currentVelocity = 0.0f;
			acceleration = ACCELERATION_DEFAULT;
			maxGravity = MAX_GRAVITY_DEFAULT;
			currentGravity = 0.0f;
			gravityAcceleration = GRAVITY_ACCELERATION_DEFAULT;
		}
		else
		{
			// Initialize heavy enemy
			maxHealth = MAX_HEALTH_DEFAULT * 2.0f;
			currentHealth = MAX_HEALTH_DEFAULT * 2.0f;
			damage = DAMAGE_DEFAULT * 1.5f;
			sightRadius = SIGHT_RADIUS_DEFAULT;
			attackRadius = ATTACK_RADIUS_DEFAULT * 1.5f;
			maxVelocity = MAX_VELOCITY_DEFAULT * 0.6f;
			currentVelocity = 0.0f;
			acceleration = ACCELERATION_DEFAULT * 0.7f;
			maxGravity = MAX_GRAVITY_DEFAULT;
			currentGravity = 0.0f;
			gravityAcceleration = GRAVITY_ACCELERATION_DEFAULT;
		}
		if (_proctor != NULL)
		{
			proctor->addComponent(this);
		}
	}

	// Constructor initialize enemy with given values
	Enemy::Enemy(Proctor* _proctor, Proctor* _playerOneRef, Proctor* _playerTwoRef, float _maxHealth, float _damage, float _sightRadius, float _attackRadius,
		float _maxVelocity, float _acceleration, float _maxGravity, float _gravityAcceleration) :
		Component(C_ENEMY, _proctor),
		playerOneRef(_playerOneRef), playerTwoRef(_playerTwoRef), currentlyChasedPlayer(nullptr),
		maxHealth(_maxHealth), currentHealth(_maxHealth), damage(_damage), sightRadius(_sightRadius), attackRadius(_attackRadius),
		maxVelocity(_maxVelocity), currentVelocity(0.0f), acceleration(_acceleration),
		maxGravity(_maxGravity), currentGravity(0.0f), gravityAcceleration(_gravityAcceleration),
		currentState(STOIC_STATE),
		wanderDirection(glm::vec3(0.0f)), newWanderDirectionTimer(0.0f)
	{
		if (_proctor != NULL)
		{
			proctor->addComponent(this);
		}
	}

	Enemy::~Enemy()
	{
	}

	void Enemy::update()
	{
		if (currentState != DEAD_STATE)
		{
			switch (currentState)
			{
			case STOIC_STATE:
				stoicBehaviour();
				break;
			case CHASING_STATE:
				chasingBehaviour();
				break;
			case ATTACK_STATE:
				attackBehaviour();
				break;
			case DYING_STATE:
				dyingBehaviour();
				break;
			}
		}
	}

	void Enemy::takeDamage(Proctor* playerRef, float damage)
	{
		currentHealth -= damage;
		if (currentHealth < 0.0f)
		{
			// PLAY DEATH ANIMATION HERE
			currentState = DEAD_STATE;
			currentlyChasedPlayer = nullptr;
			return;
		}
		currentlyChasedPlayer = playerRef;
		currentState = CHASING_STATE;
	}

	void Enemy::stoicBehaviour()
	{
		/* Enemy wandering */
		/* If appropriate amount of time passed, then calculate new random wander direction */
		newWanderDirectionTimer += proctor->getParentHierarchy()->getDeltaTime();
		if (newWanderDirectionTimer > 4)
		{
			Application::Randomizer randomizer;
			
			wanderDirection.x = randomizer.randomInt() % 2;
			wanderDirection.y = 0.0f;
			wanderDirection.z = randomizer.randomInt() % 2;

			if (randomizer.randomInt() % 2 != 0)
				wanderDirection.x *= -1;
			if (randomizer.randomInt() % 2 != 0)
				wanderDirection.z *= -1;
			
			newWanderDirectionTimer = 0.0f;
			currentVelocity = 0.0f;
		}

		/* If enemy is staying in place, then rotate him left and right, else move and rotate enemy in wander direction */
		if (wanderDirection.x == 0.0f && wanderDirection.z == 0.0f)
		{
			if (newWanderDirectionTimer > 1.33)
			{
				/* Apply rotation */
				glm::quat enemyRotation = proctor->getRotation();
				enemyRotation.y += glm::radians(1.0f);
				proctor->setRotation(enemyRotation);
			}
			else
			{
				glm::quat enemyRotation = proctor->getRotation();
				enemyRotation.y -= glm::radians(1.0f);
				proctor->setRotation(enemyRotation);
			}
		}
		else
		{
			currentVelocity += acceleration;
			glm::vec3 enemyNormalDown = glm::vec3(0.0f, 0.0f, -1.0f);

			/* Chase player */
			proctor->setPosition(proctor->getPosition() -= glm::vec3(wanderDirection.x * currentVelocity, 0.0f, wanderDirection.z * currentVelocity));

			/* Calc rotation to look at the chased player */
			float dot = enemyNormalDown.x * wanderDirection.x + enemyNormalDown.z * wanderDirection.z;	// dot product between enemy starting rotation and direction to player
			float det = enemyNormalDown.x * wanderDirection.z - wanderDirection.x * enemyNormalDown.z;	// determinant
			float angle = atan2(det, dot);											// angle between enemy starting position and direction to player in radians

			/* Apply rotation */
			glm::quat enemyStartRotation = glm::quat(1.0f, 0.0f, -angle, 0.0f);
			proctor->setRotation(enemyStartRotation);
		}
		
		/* If first player is in sight radius then chase him */
		if(distanceToPlayer(playerOneRef) <= sightRadius)
		{
			currentlyChasedPlayer = playerOneRef;
			currentState = CHASING_STATE;
			return;
		}

		/* If second player is in sight radius then chase him */
		if (distanceToPlayer(playerTwoRef) <= sightRadius)
		{
			currentlyChasedPlayer = playerTwoRef;
			currentState = CHASING_STATE;
			return;
		}

		/* If all conditions are false then be stoic still */
		currentState = STOIC_STATE;
	}

	void Enemy::chasingBehaviour()
	{		
		/* If currently chased player will escape from sight radius then end chase */
		if (distanceToPlayer(currentlyChasedPlayer) > sightRadius)
		{
			currentlyChasedPlayer = nullptr;
			currentState = STOIC_STATE;
			return;
		}
		
		/* Increment current velocity by acceleration */
		if (currentVelocity < maxVelocity)
		{
			currentVelocity += acceleration;
		}

		/* Calculate direction to player */
		glm::vec3 playerPosition = currentlyChasedPlayer->getPosition();
		glm::vec3 enemyPosition = proctor->getPosition();
		glm::vec3 direction = glm::normalize(enemyPosition - playerPosition);
		glm::vec3 enemyNormalDown = glm::vec3(0.0f, 0.0f, -1.0f);
		
		/* Chase player */
		proctor->setPosition(enemyPosition -= glm::vec3(direction.x * currentVelocity, 0.0f, direction.z * currentVelocity));

		/* Calc rotation to look at the chased player */
		float dot = enemyNormalDown.x * direction.x + enemyNormalDown.z * direction.z;	// dot product between enemy starting rotation and direction to player
		float det = enemyNormalDown.x * direction.z - direction.x * enemyNormalDown.z;	// determinant
		float angle = atan2(det, dot);											// angle between enemy starting position and direction to player in radians

		/* Apply rotation */
		glm::quat enemyStartRotation = glm::quat(1.0f, 0.0f, -angle, 0.0f);
		proctor->setRotation(enemyStartRotation);
				
		/* If currently chased player is within attack radius then attack him */
		if(distanceToPlayer(currentlyChasedPlayer) < attackRadius)
		{
			currentState = ATTACK_STATE;
			// PLAY ATTACK ANIMATION HERE
			// TODO: @Kuba @Dawid ANIMACJE ... TA TA TA
		}
	}

	void Enemy::attackBehaviour()
	{
		std::cout << "ATTACKING\n";
		// IF ANIMATION HAS ENDED THEN CHECK IF CURRENTLY CHASED IS WITHIN ATTACK RADIUS
		// IF CURRENTLY CHASED IS WITHIN ATTACK RADIUS THEN APPLY DAMAGE TO HIM
		// CHASE CURRENTLY CHASED AGAIN
	}

	void Enemy::dyingBehaviour()
	{
		// IF ANIMATION HAS ENDED THEN SCALE TO 0 0 0 AND CHANGE STATE TO DEAD
	}

	float Enemy::distanceToPlayer(Proctor* player)
	{
		float distanceToPlayer = glm::sqrt(
			glm::pow(proctor->getPosition().x - player->getPosition().x, 2) +
			glm::pow(proctor->getPosition().y - player->getPosition().y, 2) +
			glm::pow(proctor->getPosition().z - player->getPosition().z, 2)
		);
		
		return distanceToPlayer;
	}
}
