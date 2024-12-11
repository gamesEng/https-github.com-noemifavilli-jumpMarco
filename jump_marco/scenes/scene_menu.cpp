//scene_menu.cpp
#include "scene_menu.h"
#include "../components/cmp_text.h"
#include "../game.h"
#include <SFML/Window/Keyboard.hpp>
#include <iostream>

using namespace std;
using namespace sf;



void MenuScene::Load() {
    cout << "Menu Load \n";

    // Set the starting position for the first menu option
    float yPosition = 100.0f;  // You can adjust this value as needed

    // Create the menu options
    auto option1 = makeEntity();
    option1->addComponent<TextComponent>("New Game");
    option1->get_components<TextComponent>()[0]->GetTextObject().setPosition(100.0f, yPosition);
    menuOptions.push_back(option1);


    auto option2 = makeEntity();
    option2->addComponent<TextComponent>("Load Game");
    option2->get_components<TextComponent>()[0]->GetTextObject().setPosition(100.0f, yPosition + 50.0f);  // Offset by 50 units vertically
    menuOptions.push_back(option2);

    auto option3 = makeEntity();
    option3->addComponent<TextComponent>("Exit");
    option3->get_components<TextComponent>()[0]->GetTextObject().setPosition(100.0f, yPosition + 100.0f);  // Offset by 100 units vertically
    menuOptions.push_back(option3);

    

    setLoaded(true);
}

void MenuScene::Update(const double& dt) {
    // Assuming you have a way to track the currently selected option
    static int currentSelection = 0;

    static bool downKeyPressed = false;  // To prevent multiple detections of the same press
    static bool upKeyPressed = false;

    // Handle scrolling through menu options
    if (sf::Keyboard::isKeyPressed(Keyboard::Down)) {
        if (!downKeyPressed) {  // Only move down once per key press
            currentSelection = (currentSelection + 1) % menuOptions.size();  // Loop through options
            downKeyPressed = true;  // Prevent multiple detections
        }
    }
    else {
        downKeyPressed = false;  // Reset when key is released
    }

    if (sf::Keyboard::isKeyPressed(Keyboard::Up)) {
        if (!upKeyPressed) {  // Only move up once per key press
            currentSelection = (currentSelection - 1 + menuOptions.size()) % menuOptions.size();
            upKeyPressed = true;  // Prevent multiple detections
        }
    }
    else {
        upKeyPressed = false;  // Reset when key is released
    }

    // Update the text color based on the selection
    for (size_t i = 0; i < menuOptions.size(); ++i) {
        auto& optionText = menuOptions[i]->get_components<TextComponent>();
        if (!optionText.empty()) {  // Ensure the vector is not empty
            if (i == currentSelection) {
                optionText[0]->GetTextObject().setFillColor(sf::Color::Yellow);  // Highlight selected option
            }
            else {
                optionText[0]->GetTextObject().setFillColor(sf::Color::White);   // Unselected options
            }
        }
    }


    // When the user presses Enter
    if (sf::Keyboard::isKeyPressed(Keyboard::Enter)) {
        if (currentSelection == 0) {
            Engine::ChangeScene(&level1);  // Start the game
        }
        else if (currentSelection == 1) {
            Engine::ChangeScene(&level1);  // Restart the game
        }
        else if (currentSelection == 2) {
            Engine::Quit();  // Quit the game
        }
        
    }

    Scene::Update(dt);
}
