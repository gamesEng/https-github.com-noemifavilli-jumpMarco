#pragma once
#include "ecm.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>

struct Animation {
	std::vector<sf::IntRect> frames;
	float frameTime;  // time per frame
	bool loop;
};

class AnimationComponent : public Component {
protected:
	sf::Sprite* _sprite;
	std::map<std::string, Animation> _animations;
	std::map<std::string, std::shared_ptr<sf::Texture>> _animTextures;
	std::string _currentAnim;
	size_t _currentFrame;
	float _timer;

public:
	AnimationComponent() = delete;
	explicit AnimationComponent(Entity* p);

	void addAnimation(const std::string& name, const Animation& anim, std::shared_ptr<sf::Texture> tex = nullptr);
	void setAnimation(const std::string& name);
	void resetAnimation();

	void update(double dt) override;
	void render() override;
};
