#pragma once
#include <memory>
#include <engine.h>
#include <system_resources.h>
#include "../components/cmp_animation.h"
#include "../components/cmp_sprite.h"
#include "../components/cmp_player_physics.h"



class PlayerFactory {
public:
    
    static std::shared_ptr<Entity> create(Scene& scene, const sf::Vector2f& startPosition);
};