#include "player_factory.h"

std::shared_ptr<Entity> PlayerFactory::create(Scene& scene, const sf::Vector2f& startPosition) {
    // Use Scene's makeEntity method to create the player
    auto player = scene.makeEntity();
    player->addTag("player");
    player->setPosition(startPosition);

    // Load textures
    auto idleTex = Resources::load<sf::Texture>("Marco/Idle.png");
    auto runTex = Resources::load<sf::Texture>("Marco/Run.png");
    auto jumpTex = Resources::load<sf::Texture>("Marco/Jump.png");

    // Add SpriteComponent
    auto spriteComp = player->addComponent<SpriteComponent>();
    spriteComp->setTexure(idleTex);
    spriteComp->getSprite().setOrigin(75.f, 80.f);

    // Add AnimationComponent
    auto animComp = player->addComponent<AnimationComponent>();

    // Add Idle Animation
    Animation idleAnim;
    const int frameWidth = 150;
    const int frameHeight = 150;
    for (int i = 0; i < 8; i++) {
        idleAnim.frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }
    idleAnim.frameTime = 0.2f;
    idleAnim.loop = true;
    animComp->addAnimation("idle", idleAnim);

    // Add Run Animation
    Animation runAnim;
    for (int i = 0; i < 8; i++) {
        runAnim.frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }
    runAnim.frameTime = 0.1f;
    runAnim.loop = true;
    animComp->addAnimation("run", runAnim, runTex);

    // Add Jump Animation
    Animation jumpAnim;
    for (int i = 0; i < 2; i++) {
        jumpAnim.frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }
    jumpAnim.frameTime = 0.15f;
    jumpAnim.loop = false;
    animComp->addAnimation("jump", jumpAnim, jumpTex);

    // Set the default animation
    animComp->setAnimation("idle");

    // Add PlayerPhysicsComponent
    player->addComponent<PlayerPhysicsComponent>(sf::Vector2f(20.f, 30.f));

    return player;
}
