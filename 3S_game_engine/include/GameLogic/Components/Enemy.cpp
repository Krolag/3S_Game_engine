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
	constexpr float ATTACK_RADIUS_DEFAULT = 4.3f;
	constexpr float DAMAGE_RADIUS_DEFAULT = 2.0f*ATTACK_RADIUS_DEFAULT;
	constexpr float MAX_VELOCITY_DEFAULT = 7.0f;
	constexpr float ACCELERATION_DEFAULT = 0.2f;
	constexpr float MAX_GRAVITY_DEFAULT = 8.0f;
	constexpr float GRAVITY_ACCELERATION_DEFAULT = 0.3f;
	constexpr float WANDER_STAY_TIME = 4.0f;
	constexpr float ATTACK_TIME = 1.0f;
	const float boundaries[4][4] = {
		{720.0f, 799.0f, 39.0f, -79.0f},
		{720.0f, 799.0f, 39.0f, -79.0f},
		{720.0f, 799.0f, 39.0f, -79.0f},
		{720.0f, 799.0f, 39.0f, -79.0f}
	};

	/* Constructors and destructors */
	// Constructor initialize enemy by default values
	Enemy::Enemy(Proctor* _proctor, Proctor* _playerOneRef, Proctor* _playerTwoRef, int _islandID) : Component(C_ENEMY, _proctor),
		playerOneRef(_playerOneRef), playerTwoRef(_playerTwoRef), islandID(_islandID), currentlyChasedPlayer(nullptr),
		maxHealth(MAX_HEALTH_DEFAULT), currentHealth(MAX_HEALTH_DEFAULT), damage(DAMAGE_DEFAULT),
		sightRadius(SIGHT_RADIUS_DEFAULT), attackRadius(ATTACK_RADIUS_DEFAULT), damageRadius(DAMAGE_RADIUS_DEFAULT),
		maxVelocity(MAX_VELOCITY_DEFAULT), currentVelocity(0.0f), acceleration(ACCELERATION_DEFAULT),
		maxGravity(MAX_GRAVITY_DEFAULT), currentGravity(0.0f), gravityAcceleration(GRAVITY_ACCELERATION_DEFAULT),
		currentState(STOIC_STATE), wanderDirection(glm::vec3(0.0f)), newWanderDirectionTimer(0.0f), attackTimer(0.0f)
	{
		if (_proctor != NULL)
		{
			proctor->addComponent(this);
		}
	}

	// Constructor initialize enemy by the given type
	Enemy::Enemy(Proctor* _proctor, Proctor* _playerOneRef, Proctor* _playerTwoRef, int _islandID, std::string _enemyType) : Component(C_ENEMY, _proctor),
		playerOneRef(_playerOneRef), playerTwoRef(_playerTwoRef), islandID(_islandID), currentlyChasedPlayer(nullptr),
		currentState(STOIC_STATE), wanderDirection(glm::vec3(0.0f)), newWanderDirectionTimer(0.0f), attackTimer(0.0f)
	{
		std::cout << "ENEMY CONSTRUCTOR ISLAND ID: " << islandID << "\n";
		if(_enemyType == "locals_00" || _enemyType == "locals_01")
		{
			// Initialize normal enemy
			maxHealth = MAX_HEALTH_DEFAULT;
			currentHealth = MAX_HEALTH_DEFAULT;
			damage = DAMAGE_DEFAULT;
			sightRadius = SIGHT_RADIUS_DEFAULT;
			attackRadius = ATTACK_RADIUS_DEFAULT;
			damageRadius = DAMAGE_RADIUS_DEFAULT;
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
			damageRadius = DAMAGE_RADIUS_DEFAULT * 1.5f;
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
	Enemy::Enemy(Proctor* _proctor, Proctor* _playerOneRef, Proctor* _playerTwoRef, int _islandID, float _maxHealth, float _damage, float _sightRadius, float _attackRadius, float _damageRadius,
		float _maxVelocity, float _acceleration, float _maxGravity, float _gravityAcceleration) :
		Component(C_ENEMY, _proctor),
		playerOneRef(_playerOneRef), playerTwoRef(_playerTwoRef), islandID(_islandID), currentlyChasedPlayer(nullptr),
		maxHealth(_maxHealth), currentHealth(_maxHealth), damage(_damage), sightRadius(_sightRadius), attackRadius(_attackRadius), damageRadius(_damageRadius),
		maxVelocity(_maxVelocity), currentVelocity(0.0f), acceleration(_acceleration),
		maxGravity(_maxGravity), currentGravity(0.0f), gravityAcceleration(_gravityAcceleration),
		currentState(STOIC_STATE),
		wanderDirection(glm::vec3(0.0f)), newWanderDirectionTimer(0.0f), attackTimer(0.0f)
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
			}
		}
	}

	void Enemy::takeDamage(Proctor* playerRef, float damage)
	{
		currentHealth -= damage;
		if (currentHealth < 0.0f)
		{
			// TODO: @Ignacy or @Kuba play music for locals dying
			proctor->setScale(glm::vec3(0.0f, 0.0f, 0.0f));
			currentState = DEAD_STATE;
			currentlyChasedPlayer = nullptr;
			return;
		}
		// TODO: @Ignacy or @Kuba play music for locals taking damage
		currentlyChasedPlayer = playerRef;
		
		currentState = CHASING_STATE;
	}

	int Enemy::getIslandID()
	{
		return islandID;
	}

	void Enemy::stoicBehaviour()
	{
		/* Enemy wandering */
		/* If appropriate amount of time passed, then calculate new random wander direction */
		if (newWanderDirectionTimer > WANDER_STAY_TIME)
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
			if (newWanderDirectionTimer > WANDER_STAY_TIME/3.0f)
			{
				/* Apply rotation */
				glm::quat enemyRotation = proctor->getRotation();
				enemyRotation.y += glm::radians(10.0f * proctor->getParentHierarchy()->getDeltaTime());
				proctor->setRotation(enemyRotation);
			}
			else
			{
				glm::quat enemyRotation = proctor->getRotation();
				enemyRotation.y -= glm::radians(10.0f * proctor->getParentHierarchy()->getDeltaTime());
				proctor->setRotation(enemyRotation);
			}
		}
		else
		{
			/* Increment velocity */
			if (currentVelocity < maxVelocity)
			{
				currentVelocity += acceleration;
			}

			/* Increment gravity */
			if (currentGravity < maxGravity)
			{
				currentGravity += gravityAcceleration;
			}
			
			glm::vec3 enemyNormalDown = glm::vec3(0.0f, 0.0f, -1.0f);
			glm::vec3 enemyPosition = proctor->getPosition() -= glm::vec3(wanderDirection.x * currentVelocity * proctor->getParentHierarchy()->getDeltaTime(), currentGravity * proctor->getParentHierarchy()->getDeltaTime(), wanderDirection.z * currentVelocity * proctor->getParentHierarchy()->getDeltaTime());

			/* Wander */
			proctor->setPosition(enemyPosition);
			
			/* Calc rotation to look at the chased player */
			float dot = enemyNormalDown.x * wanderDirection.x + enemyNormalDown.z * wanderDirection.z;	// dot product between enemy starting rotation and wander direction
			float det = enemyNormalDown.x * wanderDirection.z - wanderDirection.x * enemyNormalDown.z;	// determinant
			float angle = atan2(det, dot);											// angle between enemy starting position and wander direction

			/* Apply rotation */
			glm::quat enemyStartRotation = glm::quat(1.0f, 0.0f, -angle, 0.0f);
			proctor->setRotation(enemyStartRotation);

			/* Check boundaries for island */
			if (enemyPosition.x < boundaries[islandID][0]) wanderDirection.x *= -1;
			if (enemyPosition.x > boundaries[islandID][1]) wanderDirection.x *= -1;
			if (enemyPosition.z < boundaries[islandID][2]) wanderDirection.z *= -1;
			if (enemyPosition.z > boundaries[islandID][3]) wanderDirection.z *= -1;
		}

		newWanderDirectionTimer += proctor->getParentHierarchy()->getDeltaTime();
		
		/* If first player is in sight radius then chase him */
		if(distanceToPlayer(playerOneRef) <= sightRadius)
		{
			currentlyChasedPlayer = playerOneRef;
			currentState = CHASING_STATE;
			// play music
			enemyEngine->play2D("./assets/audio/recordedSounds/Locals/Giga/GigaBlank/spotted_GW.ogg", false);
			return;
		}

		/* If second player is in sight radius then chase him */
		if (distanceToPlayer(playerTwoRef) <= sightRadius)
		{
			currentlyChasedPlayer = playerTwoRef;
			currentState = CHASING_STATE;
			// TODO: @Ignacy or @Kuba play music for chasing player
			enemyEngine->play2D("./assets/audio/recordedSounds/Locals/Giga/GigaBlank/spotted_GW.ogg", false);
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
			enemyEngine->play2D("./assets/audio/recordedSounds/Locals/Giga/GigaBlank/escape_GW.ogg", false);
			currentState = STOIC_STATE;
			return;
		}
		
		/* Increment current velocity by acceleration */
		if (currentVelocity < maxVelocity)
		{
			currentVelocity += acceleration;
		}

		/* Increment current gravity by gravity acceleration */
		if (currentGravity < maxGravity)
		{
			currentGravity += gravityAcceleration;
		}

		/* Calculate direction to player */
		glm::vec3 playerPosition = currentlyChasedPlayer->getPosition();
		glm::vec3 enemyPosition = proctor->getPosition();
		glm::vec3 direction = glm::normalize(enemyPosition - playerPosition);
		glm::vec3 enemyNormalDown = glm::vec3(0.0f, 0.0f, -1.0f);
		
		/* Chase player */
		proctor->setPosition(enemyPosition -= glm::vec3(direction.x * currentVelocity * proctor->getParentHierarchy()->getDeltaTime(), currentGravity * proctor->getParentHierarchy()->getDeltaTime(), direction.z * currentVelocity * proctor->getParentHierarchy()->getDeltaTime()));

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
			/* Change state of enemy for attack state */
			currentState = ATTACK_STATE;
			std::cout << proctor->name << " starts attack on " << currentlyChasedPlayer->name << "\n";
			enemyEngine->play2D("./assets/audio/recordedSounds/Locals/Giga/GigaBlank/attack_GW.ogg", false);
			// TODO: @Kuba @Dawid ANIMACJE ... TA TA TA. PLAY ATTACK ANIMATION HERE
		}
	}

	void Enemy::attackBehaviour()
	{
		/* Calculate direction to player */
		glm::vec3 playerPosition = currentlyChasedPlayer->getPosition();
		glm::vec3 enemyPosition = proctor->getPosition();
		glm::vec3 direction = glm::normalize(enemyPosition - playerPosition);
		glm::vec3 enemyNormalDown = glm::vec3(0.0f, 0.0f, -1.0f);

		/* Calc rotation to look at the chased player */
		float dot = enemyNormalDown.x * direction.x + enemyNormalDown.z * direction.z;	// dot product between enemy starting rotation and direction to player
		float det = enemyNormalDown.x * direction.z - direction.x * enemyNormalDown.z;	// determinant
		float angle = atan2(det, dot);											// angle between enemy starting position and direction to player in radians

		/* Apply rotation */
		glm::quat enemyStartRotation = glm::quat(1.0f, 0.0f, -angle, 0.0f);
		proctor->setRotation(enemyStartRotation);
		
		/* Check if animation attack time has ended */
		if (attackTimer > ATTACK_TIME)
		{
			/* If attack animation time has ended then check if enemy is within damage radius */
			if(distanceToPlayer(currentlyChasedPlayer) < damageRadius)
			{
				std::cout << proctor->name << " is damaging " << currentlyChasedPlayer->name << "\n";
				/* apply damage to player if he is within damage radius */
				if (currentlyChasedPlayer->name == "playerOne")
					enemyEngine->play2D("./assets/audio/recordedSounds/Players/Dawid/damage_D.ogg", false);				
				else if (currentlyChasedPlayer->name == "playerTwo")
					enemyEngine->play2D("./assets/audio/recordedSounds/Players/Kuba/damage_K.ogg", false);
				proctor->getParentHierarchy()->takeDamage();
			}

			/* Chase currently chased player again */
			currentState = CHASING_STATE;

			/* Restart attack timer */
			attackTimer = 0.0f;
		}

		/* Increment attack timer */
		attackTimer += proctor->getParentHierarchy()->getDeltaTime();
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

	void Enemy::setCurrentGravity(float _currentGravity)
	{
		currentGravity = _currentGravity;
	}
}
