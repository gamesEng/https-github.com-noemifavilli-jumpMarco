#include <gtest/gtest.h>
#include <engine.h>
#include "../../jump_marco/entities/player_factory.h"


// Mock Scene to use for testing
class MockScene : public Scene {
public:
    // impl for the pure virtual
    void Load() override {
        // do nothing
    }

    void UnLoad() override {
        ents.list.clear();  // clear entities 
    }

    void Update(const double& dt) override {
        // testing
    }

    void Render() override {
        //  testing
    }

    std::shared_ptr<Entity> makeEntity() override {
        return Scene::makeEntity();  // base implementation
    }
};


class PlayerFactoryTest : public ::testing::Test {
protected:
    MockScene scene;
    std::shared_ptr<Entity> player;

    virtual void SetUp() override {
        // Load the player entity using the factory
        player = PlayerFactory::create(scene, sf::Vector2f(100.f, 200.f));
    }
};

// Test: Player Entity is Created
TEST_F(PlayerFactoryTest, CreatesPlayerEntity) {
    ASSERT_NE(player, nullptr);  // Player should not be null
}

