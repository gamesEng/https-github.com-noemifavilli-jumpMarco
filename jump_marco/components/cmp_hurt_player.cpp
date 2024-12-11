#include "cmp_hurt_player.h"
#include <engine.h>


using namespace std;
using namespace sf;

void HurtComponent::update(double dt) {
    if (auto pl = _player.lock()) { // Check if player is valid
        if (length(pl->getPosition() - _parent->getPosition()) < 25.0) {
            // Instead of deleting, just log "Ouch!"
            std::cout << "Ouch!" << std::endl;
        }
    }
}

HurtComponent::HurtComponent(Entity* p)
    : Component(p), _player(_parent->scene->ents.find("player")[0]) {
    if (!p) {
        std::cerr << "Error: Could not find entity with tag 'player'!" << std::endl;
    }
}