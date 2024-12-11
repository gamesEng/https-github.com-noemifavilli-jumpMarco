#include "cmp_enemy_ai.h"

using namespace std;
using namespace sf;

void EnemyAIComponent::update(double dt) {
    // Calculate movement
    auto mov = _direction * (float)(dt * _speed);

    // Check if the enemy can move; reverse direction if not
    if (!validMove(_parent->getPosition() + mov)) {
        _direction *= -1.f; // Reverse direction
    }

    // Move the enemy
    move(_direction * (float)(dt * _speed));

    // Call parent update for any additional logic
    ActorMovementComponent::update(dt);
}

EnemyAIComponent::EnemyAIComponent(Entity* p) : ActorMovementComponent(p) {
    _direction = Vector2f(1.0f, 0); // Start moving right
    _speed = 50.0f;                // Movement speed
}