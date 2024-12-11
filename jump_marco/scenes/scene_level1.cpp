#include "scene_level1.h"
#include "../components/cmp_player_physics.h"
#include "../components/cmp_sprite.h"
#include "../components/cmp_animation.h"
#include "../game.h"
#include "system_resources.h"
#include <tile_level_loader.h>
#include <iostream>
#include <thread>
#include "../components/cmp_enemy_ai.h"
#include "../components/cmp_hurt_player.h"
#include "../components/cmp_enemy_turret.h"
#include "../entities/player_factory.h"

using namespace std;
using namespace sf;
sf::Music backgroundMusic;
sf::View view; // View for the camera

static shared_ptr<Entity> player;

void Level1Scene::Load() {
  cout << " Scene 1 Load" << endl;
  ls::loadLevelFile("res/level_1.txt", 40.0f);

  auto ho = Engine::getWindowSize().y - (ls::getHeight() * 40.f);
  ls::setOffset(Vector2f(0, ho));

  // Create Player at Start(s)
  auto startPosition = ls::getTilePosition(ls::findTiles(ls::START)[0]);
  player = PlayerFactory::create(*this, startPosition);
 

  {
      auto enemyTiles = ls::findTiles(ls::ENEMY);
      if (!enemyTiles.empty()) {
          auto enemy = makeEntity();
          enemy->setPosition(ls::getTilePosition(enemyTiles[0]) + Vector2f(0, 24));

          // Add HurtPlayer Component
          enemy->addComponent<HurtComponent>();

          // Add AI Component
          enemy->addComponent<EnemyAIComponent>();
          enemy->addComponent<EnemyTurretComponent>();

          // Add ShapeComponent
          auto shapeComp = enemy->addComponent<ShapeComponent>();
          shapeComp->setShape<sf::CircleShape>(16.f);
          shapeComp->getShape().setFillColor(Color::Red);
      }
      else {
          cerr << "Warning: No ENEMY tile found in the level." << endl;
      }
  }

  // Camera
  // Set up the view to match the window size
  view.setSize(Engine::GetWindow().getSize().x, Engine::GetWindow().getSize().y);
  // Center the view on the player's initial position
  view.setCenter(player->getPosition());


  // Add physics colliders to level tiles.
  {

    auto wallTex = Resources::load<sf::Texture>("stonewall.png");
    auto walls = ls::findTiles(ls::WALL);

    for (auto w : walls) {
      auto pos = ls::getTilePosition(w);
      pos += Vector2f(20.f, 20.f); //offset to center
      auto e = makeEntity();
      e->setPosition(pos);

      // Add PhysicsComponent
      e->addComponent<PhysicsComponent>(false, Vector2f(40.f, 40.f));

      // Add SpriteComponent
      auto spriteComp = e->addComponent<SpriteComponent>();
      spriteComp->setTexure(wallTex); // Set wall texture
      spriteComp->getSprite().setOrigin(20.f, 20.f); // Center sprite

    }
  }

  //Simulate long loading times
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  cout << " Scene 1 Load Done" << endl;

  // Play background music
  if (!backgroundMusic.openFromFile("res/sounds/Background_music.mp3")) {
      cerr << "Failed to background music!" << endl;
  }
  else {
      backgroundMusic.setLoop(true);
      backgroundMusic.setVolume(20.f);
      backgroundMusic.play();
  }

  setLoaded(true);
}


void Level1Scene::UnLoad() {
  cout << "Scene 1 Unload" << endl;

  // Reset the view to default
  Engine::GetWindow().setView(Engine::GetWindow().getDefaultView());

  // Stop the background music
  if (backgroundMusic.getStatus() == sf::Music::Playing) {
      backgroundMusic.stop();
  }

  player.reset();
  ls::unload();
  Scene::UnLoad();
}

void Level1Scene::Update(const double& dt) {

  if (ls::getTileAt(player->getPosition()) == ls::END) {
    Engine::ChangeScene((Scene*)&level2);
  }
  Scene::Update(dt);

  // Get the player's current position
  sf::Vector2f playerPos = player->getPosition();

  // Define the bounds based on map size and view size
  // I tried clamping, didn't work
  float halfWidth = view.getSize().x / 2.f;
  float halfHeight = view.getSize().y / 2.f;
  float leftBound = halfWidth;
  float rightBound = ls::getWidth() * ls::getTileSize() - halfWidth;
  float topBound = halfHeight;
  float bottomBound = ls::getHeight() * ls::getTileSize() - halfHeight;

  // Constrain the camera position within bounds
  sf::Vector2f cameraPos = playerPos;

  if (cameraPos.x < leftBound) {
      cameraPos.x = leftBound;
  }
  else if (cameraPos.x > rightBound) {
      cameraPos.x = rightBound;
  }

  if (cameraPos.y < topBound) {
      cameraPos.y = topBound;
  }
  else if (cameraPos.y > bottomBound) {
      cameraPos.y = bottomBound;
  }

  // Center the view on the constrained position
  view.setCenter(cameraPos);

  // Change the scene if the player reaches the end tile
  if (ls::getTileAt(player->getPosition()) == ls::END) {
      Engine::ChangeScene((Scene*)&menu);
  }

}

void Level1Scene::Render() {

  // Set the view for the current frame
  Engine::GetWindow().setView(view);

  ls::render(Engine::GetWindow());

  Scene::Render();
}
