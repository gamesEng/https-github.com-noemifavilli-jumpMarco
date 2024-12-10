#pragma once

#include "engine.h"
#include <vector>
#include "../components/cmp_text.h"

class MenuScene : public Scene {
public:
	MenuScene() = default;
	~MenuScene() override = default;

	void Load() override;
	void Update(const double& dt) override;

private:

	std::vector<std::shared_ptr<Entity>> menuOptions; // Store menu options

};