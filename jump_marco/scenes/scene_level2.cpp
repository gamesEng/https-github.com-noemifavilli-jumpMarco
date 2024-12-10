#include "scene_level2.h"
#include "../components/cmp_enemy_ai.h"
#include "../components/cmp_enemy_turret.h"
#include "../components/cmp_hurt_player.h"
#include "../components/cmp_physics.h"
#include "../components/cmp_sprite.h"
#include "../components/cmp_player_physics.h"
#include "../game.h"
#include "../components/cmp_animation.h"
#include "system_resources.h"
#include <tile_level_loader.h>
#include <iostream>
#include <thread>

using namespace std;
using namespace sf;

static shared_ptr<Entity> player;

void Level2Scene::Load() {
    cout << "Scene 2 Load" << endl;

    // Load the level from the file
    ls::loadLevelFile("res/level_2.txt", 40.0f);

    // Set vertical offset for the level
    auto verticalOffset = Engine::getWindowSize().y - (ls::getHeight() * 40.f);
    ls::setOffset(Vector2f(0, verticalOffset));

    // Create Player Entity
    {
        auto startTiles = ls::findTiles(ls::START);
        if (!startTiles.empty()) {
            player = makeEntity();
            player->setPosition(ls::getTilePosition(startTiles[0]));

            // Load player textures
            auto idleTex = Resources::load<sf::Texture>("Marco/Idle.png");
            auto runTex = Resources::load<sf::Texture>("Marco/Run.png");
            auto jumpTex = Resources::load<sf::Texture>("Marco/Jump.png");

            // Add SpriteComponent
            auto spriteComp = player->addComponent<SpriteComponent>();
            spriteComp->setTexure(idleTex);
            spriteComp->getSprite().setOrigin(75.f, 80.f);

            // Add AnimationComponent
            auto animComp = player->addComponent<AnimationComponent>();

            // Idle Animation
            Animation idleAnim;
            const int frameWidth = 150, frameHeight = 150;
            for (int i = 0; i < 8; i++) {
                idleAnim.frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
            }
            idleAnim.frameTime = 0.2f;
            idleAnim.loop = true;
            animComp->addAnimation("idle", idleAnim);

            // Run Animation
            Animation runAnim;
            for (int i = 0; i < 8; i++) {
                runAnim.frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
            }
            runAnim.frameTime = 0.1f;
            runAnim.loop = true;
            animComp->addAnimation("run", runAnim, runTex);

            // Jump Animation
            Animation jumpAnim;
            for (int i = 0; i < 2; i++) {
                jumpAnim.frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
            }
            jumpAnim.frameTime = 0.15f;
            jumpAnim.loop = false;
            animComp->addAnimation("jump", jumpAnim, jumpTex);

            animComp->setAnimation("idle");

            // Add Player Physics
            player->addComponent<PlayerPhysicsComponent>(Vector2f(20.f, 30.f));
        }
        else {
            cerr << "Error: No START tile found in the level!" << endl;
        }
    }

    // Create Enemy Entity
    {
        auto enemyTiles = ls::findTiles(ls::ENEMY);
        if (!enemyTiles.empty()) {
            auto enemy = makeEntity();
            enemy->setPosition(ls::getTilePosition(enemyTiles[0]) + Vector2f(0, 24));

            // Add HurtPlayer Component
            enemy->addComponent<HurtComponent>();

            // Add AI Component
            enemy->addComponent<EnemyAIComponent>();

            // Add ShapeComponent
            auto shapeComp = enemy->addComponent<ShapeComponent>();
            shapeComp->setShape<sf::CircleShape>(16.f);
            shapeComp->getShape().setFillColor(Color::Red);
        }
        else {
            cerr << "Warning: No ENEMY tile found in the level." << endl;
        }
    }

    // Create Turret Entity
    {
        auto turretTiles = ls::findTiles('t');
        if (!turretTiles.empty()) {
            auto turret = makeEntity();
            turret->setPosition(ls::getTilePosition(turretTiles[0]) + Vector2f(20, 0));

            // Add ShapeComponent
            auto shapeComp = turret->addComponent<ShapeComponent>();
            shapeComp->setShape<sf::CircleShape>(16.f, 3);
            shapeComp->getShape().setFillColor(Color::Red);
            shapeComp->getShape().setOrigin(Vector2f(16.f, 16.f));

            // Add EnemyTurret Component
            turret->addComponent<EnemyTurretComponent>();
        }
        else {
            cerr << "Warning: No TURRET tile found in the level." << endl;
        }
    }

    // Add Physics Colliders to Wall Tiles
    {
        auto wallTiles = ls::findTiles(ls::WALL);
        if (!wallTiles.empty()) {
            for (auto w : wallTiles) {
                auto pos = ls::getTilePosition(w) + Vector2f(20.f, 20.f);
                auto wall = makeEntity();
                wall->setPosition(pos);
                wall->addComponent<PhysicsComponent>(false, Vector2f(40.f, 40.f));
            }
        }
        else {
            cerr << "Warning: No WALL tiles found in the level." << endl;
        }
    }

    // Simulate Long Loading Time
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    cout << "Scene 2 Load Done" << endl;
    setLoaded(true);
}

void Level2Scene::UnLoad() {
    cout << "Scene 2 UnLoad" << endl;
    player.reset();
    ls::unload();
    Scene::UnLoad();
}

void Level2Scene::Update(const double& dt) {
    Scene::Update(dt);

    // Player Position Check for END Tile
    const auto playerPos = player->getPosition();
    if (ls::getTileAt(playerPos) == ls::END) {
        Engine::ChangeScene((Scene*)&level3);
    }
    else if (!player->isAlive()) {
        Engine::ChangeScene((Scene*)&level2);
    }
}

void Level2Scene::Render() {
    ls::render(Engine::GetWindow());
    Scene::Render();
}
