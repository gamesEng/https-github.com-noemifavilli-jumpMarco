#include "cmp_player_physics.h"
#include "system_physics.h"
#include <tile_level_loader.h>
#include <SFML/Window/Keyboard.hpp>
#include "cmp_animation.h"
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;
using namespace Physics;



bool PlayerPhysicsComponent::isGrounded() const {
  auto touch = getTouching();
  const auto& pos = _body->GetPosition();
  const float halfPlrHeigt = _size.y * .5f;
  const float halfPlrWidth = _size.x * .52f;
  b2WorldManifold manifold;
  for (const auto& contact : touch) {
    contact->GetWorldManifold(&manifold);
    const int numPoints = contact->GetManifold()->pointCount;
    bool onTop = numPoints > 0;
    // If all contacts are below the player.
    for (int j = 0; j < numPoints; j++) {
      onTop &= (manifold.points[j].y < pos.y - halfPlrHeigt);
    }
    if (onTop) {
      return true;
    }
  }

  return false;
}

void PlayerPhysicsComponent::update(double dt) {

  const auto pos = _parent->getPosition();

  //Teleport to start if we fall off map.
  if (pos.y > ls::getHeight() * ls::getTileSize()) {
    teleport(ls::getTilePosition(ls::findTiles(ls::START)[0]));
  }

  // added running flag
  bool movingHorizontally = false; 
  if (Keyboard::isKeyPressed(Keyboard::Left) ||
      Keyboard::isKeyPressed(Keyboard::Right)) {
      movingHorizontally = true; 
      // Moving Either Left or Right
      if (Keyboard::isKeyPressed(Keyboard::Right)) {
          if (getVelocity().x < _maxVelocity.x)
              impulse({ (float)(dt * _groundspeed), 0 });
      }
      else {
          if (getVelocity().x > -_maxVelocity.x)
              impulse({ -(float)(dt * _groundspeed), 0 });
      }
  }
  else {
      // Dampen X axis movement
      dampen({ 0.9f, 1.0f });
  }


  // Retrieve the AnimationComponent to change animations:
  auto animComp = _parent->GetCompatibleComponent<AnimationComponent>(); 
  std::shared_ptr<AnimationComponent> anim = nullptr;
  if (!animComp.empty()) {
      anim = animComp[0];
  }



  // Handle Jump
  if (Keyboard::isKeyPressed(Keyboard::Up)) {
    _grounded = isGrounded();
    if (_grounded) {
      setVelocity(Vector2f(getVelocity().x, 0.f));
      teleport(Vector2f(pos.x, pos.y - 2.0f));
      impulse(Vector2f(0, -6.f));
      _jumpSound.play();
      // Change to jump animation when initiating jump
      if (anim) { 
          anim->setAnimation("jump");
      }
    }
  }

  //Are we in air?
  bool wasGrounded = _grounded; // track previous grounded state
  if (!_grounded) {
      // Check to see if we have landed yet
      _grounded = isGrounded();
      // disable friction while jumping
      setFriction(0.f);
      if (_grounded) {
          // Just landed
          setFriction(0.1f);

          // If we just landed, decide if run or idle:
          if (anim) {
              if (movingHorizontally) {
                  anim->setAnimation("run");
              }
              else {
                  anim->setAnimation("idle");
              }
          }
      }
  }
  else {
      // Grounded and not jumping
      setFriction(0.1f);
      // Decide if run or idle based on horizontal movement
      if (anim) { 
          if (movingHorizontally) {
              anim->setAnimation("run");
          }
          else {
              anim->setAnimation("idle");
          }
      }
  }

  // Clamp velocity.
  auto v = getVelocity();
  v.x = copysign(min(abs(v.x), _maxVelocity.x), v.x);
  v.y = copysign(min(abs(v.y), _maxVelocity.y), v.y);
  setVelocity(v);

  PhysicsComponent::update(dt);
}

PlayerPhysicsComponent::PlayerPhysicsComponent(Entity* p,
    const Vector2f& size)
    : PhysicsComponent(p, true, size) {
    _size = sv2_to_bv2(size, true);
    _maxVelocity = Vector2f(200.f, 400.f);
    _groundspeed = 30.f;
    _grounded = false;
    _body->SetSleepingAllowed(false);
    _body->SetFixedRotation(true);
    //Bullet items have higher-res collision detection
    _body->SetBullet(true);

    // Load jump sound
    if (!_jumpSoundBuffer.loadFromFile("res/sounds/Jump_sound.flac")) {
        cerr << "Failed to load jump sound!" << endl;

    }
    _jumpSound.setBuffer(_jumpSoundBuffer);
    _jumpSound.setVolume(30.f);
}
