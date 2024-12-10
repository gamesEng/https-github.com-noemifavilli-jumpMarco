#include "scene_level1.h"
#include "../components/cmp_player_physics.h"
#include "../components/cmp_sprite.h"
#include "../components/cmp_animation.h"
#include "../game.h"
#include "system_resources.h"
#include <tile_level_loader.h>
#include <iostream>
#include <thread>

using namespace std;
using namespace sf;
sf::Music backgroundMusic;

static shared_ptr<Entity> player;

void Level1Scene::Load() {
  cout << " Scene 1 Load" << endl;
  ls::loadLevelFile("res/level_1.txt", 40.0f);

  auto ho = Engine::getWindowSize().y - (ls::getHeight() * 40.f);
  ls::setOffset(Vector2f(0, ho));


  //// Create player
  {
      player = makeEntity();
      player->setPosition(ls::getTilePosition(ls::findTiles(ls::START)[0]));


      // Load player textures
      auto idleTex = Resources::load<sf::Texture>("Marco/Idle.png");
      auto runTex = Resources::load<sf::Texture>("Marco/Run.png");
      auto jumpTex = Resources::load<sf::Texture>("Marco/Jump.png");

      // Add SpriteComponent
      auto spriteComp = player->addComponent<SpriteComponent>();
      // Start with idle 
      spriteComp->setTexure(idleTex);
      //spriteComp->getSprite().setColor(sf::Color::White);
      spriteComp->getSprite().setOrigin(75.f, 80.f);
     
      // Add AnimationComponent
      auto animComp = player->addComponent<AnimationComponent>();

      Animation idleAnim;

      const int frameWidth = 150;
      const int frameHeight = 150;
      // 8 frame pic
      for (int i = 0; i < 8; i++) {
          idleAnim.frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
      }

      idleAnim.frameTime = 0.2f; // Half a second per frame
      idleAnim.loop = true;

      animComp->addAnimation("idle", idleAnim);

      Animation runAnim;
      for (int i = 0; i < 8; i++) {
          runAnim.frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
      }
      runAnim.frameTime = 0.1f; // faster than idle
      runAnim.loop = true;
      //
     
      animComp->addAnimation("run", runAnim, runTex);

      
      // Jump - 2 frame pic
      Animation jumpAnim;
      for (int i = 0; i < 2; i++) {
          jumpAnim.frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
      }
      jumpAnim.frameTime = 0.15f;
      jumpAnim.loop = false;
      animComp->addAnimation("jump", jumpAnim, jumpTex);


      animComp->setAnimation("idle");

      // Add player physics component
      player->addComponent<PlayerPhysicsComponent>(Vector2f(20.f, 30.f));

  }

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
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
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
}

void Level1Scene::Render() {
  ls::render(Engine::GetWindow());
  Scene::Render();
}
