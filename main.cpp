
#include "PhysicsDebugDraw.h"
#include <chrono>
#include <vector>
#include <cstdlib>
#include <ctime>

int main() {
    std::cout << "Physics System Simulator - Box2D 3.1.0 Integration Demo\n";
    std::cout << "=======================================================\n\n";

    // Seed random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    unsigned int width = 800;
    unsigned int height = 600;

    // Create SFML window
    sf::RenderWindow window(sf::VideoMode({width, height}), "Physics System Simulator - Box2D 3.1.0");
    window.setFramerateLimit(60); //sets the game loop to run 60 times per second

    // Create Box2D world with gravity using new API
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 9.8f};
    worldDef.enableSleep = true;
    b2WorldId worldId = b2CreateWorld(&worldDef);

    // Create static ground body
    Block ground = physics::createBox(worldId,0,570,800,30,b2_staticBody,true);

    // Create left wall
    Block leftWall = physics::createBox(worldId, 0, 0, 30, 600-30, b2_staticBody,true);

    // Create right wall
    Block rightWall = physics::createBox(worldId, 800-30, 0, 30, 600-30, b2_staticBody,true);

    int net_width = 800 - 30*2;
    int net_height = 600 - 30;

    // Load texture for sprite
    sf::Texture texture;
    if (!texture.loadFromFile("character_Plane.png")) {
        std::cout << "Failed to load texture" << std::endl;
        return -1;
    }

    physics::loadAllPolygonFiles();

    // Draw UI
    static sf::Font g_font; // Make static to load only once
    static bool fontLoaded = false;

    // Load font only once
    if (!fontLoaded) {
        fontLoaded = g_font.openFromFile("C:/Windows/Fonts/Arial.ttf");
        if (!fontLoaded) {
            std::cout << "Failed to load font! Using fallback." << std::endl;
        }
    }

    // Create dynamic bodies
    const int NUM_OBJECTS = 50; //100
    //std::vector<b2BodyId> bodies;

    std::cout << "Creating " << NUM_OBJECTS << " physics objects...\n";

    auto createRandomObject = [&]() {

        //physics::createBox(worldId, 0.5*net_width, 0.25*net_height, 25, 25, b2_dynamicBody, false);

        // Create a sprite with complex collision shape from file //Block character =
        //physics::createSprite(worldId, 0.75*net_width, 0.25*net_height, "character_vertices.txt", texture, b2_dynamicBody, false, 1.0f, 0.3f, 0.6f);

        for (int i = 0; i < NUM_OBJECTS; ++i) {

            float x = 30.0f + (i % 10) / 10.0f * net_width;
            float y = 20.0f + (i % 10) / 10.0f * 0.5f * net_height;

            if (i % 3 == 0) {

                // Create a Box
                physics::createBox(worldId, x, y, 15.0f, 15.0f, b2_dynamicBody, false, 1.0f, 0.3f, 0.6f);

            } else if (i % 5 == 0) {

                // Create a circle (ball)
                physics::createCircle(worldId, x, y, 15.0f, b2_dynamicBody, false, 1.0f, 0.3f, 0.6f);

            }  else if (i % 7 == 0) {

                // Create a sprite with complex collision shape from file
                physics::createSprite(worldId, x, y, "character_vertices.txt", texture, b2_dynamicBody, false, 1.0f, 0.1f, 0.6f);

            } else {

                // Create a polygon (triangle)
                std::vector<sf::Vector2f> trianglePoints = {
                    sf::Vector2f(0.0f, -20.0f),
                    sf::Vector2f(20.0f, 20.0f),
                    sf::Vector2f(-20.0f, 20.0f)
                };
                physics::createPolygon(worldId, x, y, trianglePoints, b2_dynamicBody, false, 1.0f, 0.3f, 0.6f);

            }
        }
    };

    createRandomObject();

    std::cout << "Simulation running at 60Hz with 4 sub-steps\n";
    std::cout << "Press SPACE to add more objects\n";
    std::cout << "Press R to reset simulation\n";
    std::cout << "Press ESC to exit\n\n";

    // Main game loop
    sf::Clock clock;
    int frameCount = 0;
    double totalPhysicsTime = 0.0;

    // Main game loop
    while (window.isOpen()) {
        // Handle events
        while (const std::optional event = window.pollEvent())
        //loop that checks for events
        {
            if (event->is<sf::Event::Closed>()) //checks if window is closed
            {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();

                if (keyPressed->scancode == sf::Keyboard::Scancode::Space) {

                    float x = 30.0f + (std::rand() % 10) / 10.0f * net_width;
                    float y = 20.0f + (std::rand() % 10) / 10.0f * 0.5f * net_height;
                    //float r = 0.2f + (std::rand() % 100) * 0.008f;

                    //physics::createBox(worldId,  x, y, 15.0f, 15.0f, b2_dynamicBody, false, 1.0f, 0.3f, r);
                    physics::createBox(worldId, x, y, 15.0f, 15.0f, b2_dynamicBody, false, 1.0f, 0.3f, 0.6f);

                    std::cout << "Added object. Total: " << physics::physicsObjects.size() << "\n";
                }

                if (keyPressed->scancode == sf::Keyboard::Scancode::R) {
                    // Reset simulation

                    // Clean up existing objects
                    physics::resetObjects();

                    createRandomObject();

                    std::cout << "Simulation reset with " << physics::physicsObjects.size() << " objects\n";
                }
            }
        } //ends the event loop

        // Remainder of main loop

        // Update physics with new API
        float timeStep = 1.0f / 60.0f;
        int32_t subSteps = 4; // Recommended value from guidelines

        auto physicsStart = std::chrono::high_resolution_clock::now();
        b2World_Step(worldId, timeStep, subSteps);
        auto physicsEnd = std::chrono::high_resolution_clock::now();

        double physicsTime = std::chrono::duration_cast<std::chrono::microseconds>(physicsEnd - physicsStart).count();
        totalPhysicsTime += physicsTime;
        frameCount++;

        // Clear screen
        window.clear(sf::Color(20, 20, 40)); // Dark blue background

        // Draw UI
        if (fontLoaded) {

            sf::Text text(g_font); // a font is required to make a text object // Use the pre-loaded font

            // set the character size
            text.setCharacterSize(14); // in pixels, not points!

            // set the color
            text.setFillColor(sf::Color::White);

            // set the text style
            text.setStyle(sf::Text::Bold | sf::Text::Underlined);

            text.setPosition({50, 30});

            std::string info = "Objects: " + std::to_string(physics::physicsObjects.size()) +
                             "\nFPS: 60 (capped)" +
                             "\nPhysics Step: " + std::to_string(physicsTime / 1000.0).substr(0, 5) + " ms" +
                             "\nAvg Physics Time: " + std::to_string((totalPhysicsTime / frameCount) / 1000.0).substr(0, 5) + " ms" +
                             "\nSub-steps: " + std::to_string(subSteps) +
                             "\n\nControls:" +
                             "\nSPACE - Add object" +
                             "\nR - Reset simulation" +
                             "\nESC - Exit";

            // set the string to display
            text.setString(info);

            window.draw(text);
        }

        physics::displayWorld(worldId, window); //draws everything

        // Display everything on the video card to the monitor
        window.display();

    } //ends the game loop

    // Clean up existing objects
    physics::resetObjects();

    b2DestroyWorld(worldId);

    std::cout << "\nSimulation ended. Average physics step time: "
              << (totalPhysicsTime / frameCount) / 1000.0 << " ms\n";

    return 0;
}
