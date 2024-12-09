#include "cmp_animation.h"
#include "cmp_sprite.h"
#include <stdexcept>

AnimationComponent::AnimationComponent(Entity* p)
    : Component(p), _currentFrame(0), _timer(0.f) {

    // Retrieve the sprite component from the parent.
    // If using get_components:
    // auto sc = _parent->get_components<SpriteComponent>();
    // If using GetCompatibleComponent:
    auto sc = _parent->GetCompatibleComponent<SpriteComponent>();

    if (sc.empty()) {
        throw std::runtime_error("AnimationComponent requires a SpriteComponent!");
    }

    _sprite = &sc[0]->getSprite(); // Store pointer to the sprite
}

void AnimationComponent::addAnimation(const std::string& name, const Animation& anim, std::shared_ptr<sf::Texture> tex) {
    _animations[name] = anim;
    if (tex) {
        _animTextures[name] = tex;
    }

    // If no current animation is set, choose this one
    if (_currentAnim.empty()) {
        setAnimation(name);
    }
}

void AnimationComponent::setAnimation(const std::string& name) {
    auto it = _animations.find(name);
    if (it == _animations.end()) {
        throw std::runtime_error("Animation not found: " + name);
    }

    _currentAnim = name;
    resetAnimation();

    // If this animation has its own texture, switch to it
    auto texIt = _animTextures.find(name);
    if (texIt != _animTextures.end() && texIt->second) {
        _sprite->setTexture(*texIt->second);
    }
}

void AnimationComponent::resetAnimation() {
    _currentFrame = 0;
    _timer = 0.f;
    const auto& anim = _animations[_currentAnim];
    if (!anim.frames.empty()) {
        _sprite->setTextureRect(anim.frames[_currentFrame]);
    }
}

void AnimationComponent::update(double dt) {
    if (_currentAnim.empty()) return;

    const auto& anim = _animations[_currentAnim];
    _timer += static_cast<float>(dt);

    // If time exceeds frameTime, move to the next frame
    if (_timer >= anim.frameTime && !anim.frames.empty()) {
        _timer = 0.f;
        _currentFrame++;
        if (_currentFrame >= anim.frames.size()) {
            // If animation loops, wrap around
            if (anim.loop) {
                _currentFrame = 0;
            }
            else {
                // Stay on the last frame if not looping
                _currentFrame = anim.frames.size() - 1;
            }
        }
        _sprite->setTextureRect(anim.frames[_currentFrame]);
    }
}

void AnimationComponent::render() {
    // Nothing special to do here since SpriteComponent handles rendering.
    // The AnimationComponent just changes the sprite's textureRect.
}
