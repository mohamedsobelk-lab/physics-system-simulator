#ifndef PHYSICS_H_INCLUDED
#define PHYSICS_H_INCLUDED

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <unordered_map>

using namespace std;

const float PI = 3.14159265359;
const float deg_per_rad = 180.0f/PI;
const float pixels_per_meter = 32.0f;

typedef b2BodyId Block;

// In physics.h
namespace physics {
    struct PhysicsObject {
        //std::shared_ptr<sf::Shape> shape;
        std::shared_ptr<sf::Drawable> drawable;  // Both sf::Shape and sf::Sprite inherit from sf::Drawable
        b2BodyType bodyType;
        b2BodyId bodyId;
        bool isPersistent; // Mark objects that shouldn't be deleted on reset (like ground and walls)
        std::vector<b2BodyId> partBodies; // Store all part bodies
        std::vector<b2JointId> partJoints; // Store all joints
    };

    inline std::unordered_map<int32_t, PhysicsObject> physicsObjects; // Use body index as key

    inline Block createBox(b2WorldId worldId, float x, float y, float width, float height, b2BodyType type = b2_dynamicBody, bool isPersistent = true, float density = 1.0f, float friction = 0.4f, float restitution = 0.5f) {
        // Create body definition with defaults
        b2BodyDef bodyDef = b2DefaultBodyDef();

        // Set body properties
        bodyDef.type = type;
        bodyDef.position = (b2Vec2){(x + width/2.0f)/pixels_per_meter, (y + height/2.0f)/pixels_per_meter};
        bodyDef.linearDamping = 0.05f;

        // Create the body
        b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

        // Create polygon shape definition
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = (type == b2_staticBody) ? 0.0f : density;
        shapeDef.material.friction = friction;
        shapeDef.material.restitution = restitution; // Bounciness

        // Create polygon geometry
        b2Polygon polygon = b2MakeBox(width/pixels_per_meter/2.0f, height/pixels_per_meter/2.0f);

        // Create the shape and attach to body
        b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &polygon);

        // Create SFML shape for rendering
        auto sfmlShape = std::make_shared<sf::RectangleShape>(sf::Vector2f(width, height));
        sfmlShape->setOrigin({width/2.0f, height/2.0f});
        sfmlShape->setPosition({x + width/2.0f, y + height/2.0f});

        if (type == b2_staticBody) {
            sfmlShape->setFillColor(sf::Color::Blue);
        } else {
            sfmlShape->setFillColor(sf::Color::White);
        }

        PhysicsObject obj;
        obj.drawable = sfmlShape;
        obj.bodyType = type;
        obj.bodyId = bodyId;
        obj.isPersistent = isPersistent;

        // Store using body index as key
        physicsObjects[bodyId.index1] = obj;

        return bodyId;
    }

    inline Block createCircle(b2WorldId worldId, float x, float y, float r, b2BodyType type = b2_dynamicBody, bool isPersistent = true, float density = 1.0f, float friction = 0.4f, float restitution = 0.5f) {
        // Create body definition with defaults
        b2BodyDef bodyDef = b2DefaultBodyDef();

        // Set body properties
        bodyDef.type = type;
        bodyDef.position = (b2Vec2){x/pixels_per_meter, y/pixels_per_meter};
        bodyDef.linearDamping = 0.05f;

        // Create the body
        b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

        // Create circle shape definition
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = (type == b2_staticBody) ? 0.0f : density;
        shapeDef.material.friction = friction;
        shapeDef.material.restitution = restitution; // Bounciness

        // Create circle geometry
        b2Circle circle = {(b2Vec2){0.0f, 0.0f}, r/pixels_per_meter};

        // Create the shape and attach to body
        b2ShapeId shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);

        // Create SFML circle shape for rendering
        auto sfmlShape = std::make_shared<sf::CircleShape>(sf::CircleShape(r));
        sfmlShape->setOrigin({r, r});
        sfmlShape->setPosition({x, y});

        if (type == b2_staticBody) {
            sfmlShape->setFillColor(sf::Color::Blue);
        } else {
            sfmlShape->setFillColor(sf::Color::White);
        }

        PhysicsObject obj;
        obj.drawable = sfmlShape;
        obj.bodyType = type;
        obj.bodyId = bodyId;
        obj.isPersistent = isPersistent;

        // Store using body index as key
        physicsObjects[bodyId.index1] = obj;

        return bodyId;
    }

    inline Block createPolygon(b2WorldId worldId, float x, float y, const std::vector<sf::Vector2f> &point_array, b2BodyType type = b2_dynamicBody, bool isPersistent = true, float density = 1.0f, float friction = 0.4f, float restitution = 0.5f) {

        // Create body definition with defaults
        b2BodyDef bodyDef = b2DefaultBodyDef();

        // Set body properties
        bodyDef.type = type;
        bodyDef.position = (b2Vec2){x/pixels_per_meter, y/pixels_per_meter};
        bodyDef.linearDamping = 0.05f;

        // Create the body
        b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

        // Create polygon shape definition
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = (type == b2_staticBody) ? 0.0f : density;
        shapeDef.material.friction = friction;
        shapeDef.material.restitution = restitution; // Bounciness

        // Create polygon geometry
        int n = point_array.size();
        if (n < 3 || n > B2_MAX_POLYGON_VERTICES) {
            std::cout << "Invalid number of polygon vertices: " << n << std::endl;
            return b2_nullBodyId;
        }

        // Convert points to b2Vec2 array (local coordinates)
        std::vector<b2Vec2> points(n);
        for (int i = 0; i < n; i++) {
            points[i] = (b2Vec2){point_array[i].x/pixels_per_meter, point_array[i].y/pixels_per_meter};
        }

        // Compute the convex hull from the points
        b2Hull hull = b2ComputeHull(points.data(), n);

        // Check if hull computation was successful
        if (hull.count == 0) {
            std::cout << "Failed to compute convex hull for polygon" << std::endl;
            return b2_nullBodyId;
        }

        // Create the polygon from the computed hull with 0 radius (sharp edges)
        b2Polygon polygon = b2MakePolygon(&hull, 0.0f);

        // Create the shape and attach to body
        b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &polygon);

        // Create SFML convex shape for rendering
        auto sfmlShape = std::make_shared<sf::ConvexShape>(sf::ConvexShape(n));
        for (int i = 0; i < n; i++) {
            sfmlShape->setPoint(i, sf::Vector2f(point_array[i].x, point_array[i].y));
        }
        sfmlShape->setPosition({x, y});

        if (type == b2_staticBody) {
            sfmlShape->setFillColor(sf::Color::Blue);
        } else {
            sfmlShape->setFillColor(sf::Color::White);
        }

        PhysicsObject obj;
        obj.drawable = sfmlShape;
        obj.bodyType = type;
        obj.bodyId = bodyId;
        obj.isPersistent = isPersistent;

        // Store using body index as key
        physicsObjects[bodyId.index1] = obj;

        return bodyId;
    }

    // Cache for loaded polygon data
    std::unordered_map<std::string, std::vector<b2Polygon>> polygonCache;

    // Pre-load all polygon files at game initialization
    void loadAllPolygonFiles() {
        std::vector<std::string> polygonFiles = {
            "character_vertices.txt"//,
            //"enemy_vertices.txt",
            //"obstacle_vertices.txt"
            // Add all your polygon files here
        };

        for (const auto& triangle_file : polygonFiles) {
            // This will load and cache each file
            // Read vertex data from file
            std::ifstream myfile;
            myfile.open(triangle_file);
            if (!myfile.is_open()) {
                std::cout << "No Vertex file located" << std::endl;
                continue; // Skip to next file
            }

            std::cout << "Vertex file opened successfully" << std::endl;

            size_t n;
            myfile >> n;
            std::cout << n << " triangles to process" << std::endl;

            // Process each triangle from the file (Create polygons geometry)
            std::vector<b2Polygon> newTriangles;
            for (size_t i = 0; i < n; i++) {
                b2Vec2 points[3];
                float vx, vy;

                std::cout << "Triangle " << i << ": ";

                // Convert points to b2Vec2 array (local coordinates)
                for (size_t j = 0; j < 3; j++) {
                    if (!(myfile >> vx >> vy)) { // CRITICAL: Check if read succeeds
                        std::cout << "Error reading triangle " << i << " point " << j << std::endl;
                        break;
                    }
                    points[j] = (b2Vec2){vx/pixels_per_meter, vy/pixels_per_meter};
                    std::cout << "(" << vx << "," << vy << ") "; // Debug original values
                }
                std::cout << std::endl;

                // Skip comments until end of line
                myfile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ADD THIS

                // Check if all points were read successfully
                if (myfile.fail()) {
                    std::cout << "Failed to read triangle " << i << " completely" << std::endl;
                    continue;
                }

                // Compute the convex hull from the points
                b2Hull hull = b2ComputeHull(points, 3);

                if (hull.count > 0 && b2ValidateHull(&hull)) {
                    // Create the polygon from the computed hull with 0 radius (sharp edges)
                    b2Polygon triangle = b2MakePolygon(&hull, 0.0f);
                    newTriangles.push_back(triangle);
                    std::cout << "Triangle " << i << " added via hull computation" << std::endl;
                } else {
                    std::cout << "Triangle " << i << " failed hull computation" << std::endl;

                    // Fallback: try direct triangle creation
                    b2Polygon triangle;
                    triangle.count = 3;
                    triangle.vertices[0] = points[0];
                    triangle.vertices[1] = points[1];
                    triangle.vertices[2] = points[2];
                    triangle.radius = 0.0f;

                    // Simple check: ensure points are not collinear
                    b2Vec2 ab = b2Sub(points[1], points[0]);
                    b2Vec2 ac = b2Sub(points[2], points[0]);
                    float cross = b2Cross(ab, ac);
                    if (fabsf(cross) > 0.001f) { // Not collinear
                        newTriangles.push_back(triangle);
                        std::cout << "Triangle " << i << " added via fallback" << std::endl;
                    }
                }
            }

            myfile.close();

            // Store in cache
            if (newTriangles.size() != 0){
                polygonCache[triangle_file] = newTriangles;
                std::cout << "Cached " << newTriangles.size() << " triangles from " << triangle_file << std::endl;
            }
            else{
                std::cout << "Cached 0 triangles from " << triangle_file << std::endl;
            }
        }
    }

    inline Block createSprite(b2WorldId worldId, float x, float y, std::string triangle_file,
                         const sf::Texture &t, b2BodyType type = b2_dynamicBody,
                         bool isPersistent = true, float density = 1.0f,
                         float friction = 0.4f, float restitution = 0.5f) {

        auto cacheIt = polygonCache.find(triangle_file);
        if (cacheIt == polygonCache.end()) {
            return b2_nullBodyId;
        }

        // Combine all triangles into one point cloud
        std::vector<b2Vec2> allPoints;
        for (const auto& triangle : cacheIt->second) {
            for (int i = 0; i < triangle.count; i++) {
                allPoints.push_back(triangle.vertices[i]);
            }
        }

        // Compute convex hull of all points
        b2Hull hull = b2ComputeHull(allPoints.data(), allPoints.size());
        if (hull.count == 0) {
            return b2_nullBodyId;
        }

        // Create single polygon from the hull
        b2Polygon combinedPolygon = b2MakePolygon(&hull, 0.0f);

        // Create body with single collision shape
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = type;
        bodyDef.position = (b2Vec2){x/pixels_per_meter, y/pixels_per_meter};
        bodyDef.linearDamping = 0.05f;
        b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = (type == b2_staticBody) ? 0.0f : density;
        shapeDef.material.friction = friction;
        shapeDef.material.restitution = restitution; // Bounciness

        b2CreatePolygonShape(bodyId, &shapeDef, &combinedPolygon);

        // Create SFML sprite
        auto sfmlShape = std::make_shared<sf::Sprite>(t);
        sfmlShape->setPosition({x, y});
        sfmlShape->setOrigin({t.getSize().x / 2.0f, t.getSize().y / 2.0f});

        PhysicsObject obj;
        obj.drawable = sfmlShape;
        obj.bodyType = type;
        obj.bodyId = bodyId;
        obj.isPersistent = isPersistent;

        // Store using body index as key
        physicsObjects[bodyId.index1] = obj;

        return bodyId;
    }

    inline void debugRenderCollisionShapesSimple(sf::RenderWindow& render) {
        for (const auto& pair : physicsObjects) {
            if (!b2Body_IsValid(pair.second.bodyId)) {
                continue; // Skip invalid bodies
            }

            // Get body position
            b2Vec2 position = b2Body_GetPosition(pair.second.bodyId);
            float screenX = position.x * pixels_per_meter;
            float screenY = position.y * pixels_per_meter;

            // Draw a simple rectangle at body position
            sf::RectangleShape debugRect(sf::Vector2f(10.0f, 10.0f));
            debugRect.setFillColor(sf::Color::Red);
            debugRect.setPosition({screenX - 5.0f, screenY - 5.0f});
            render.draw(debugRect);
        }
    }

    inline void displayWorld(b2WorldId worldId, sf::RenderWindow& render) {
        float timeStep = 1.0f / 60.0f;
        b2World_Step(worldId, timeStep, 4);

        // Process move events for accurate post-collision positions
        b2BodyEvents events = b2World_GetBodyEvents(worldId);
        for (int i = 0; i < events.moveCount; ++i) {
            const b2BodyMoveEvent* event = events.moveEvents + i;

            auto it = physicsObjects.find(event->bodyId.index1);
            if (it != physicsObjects.end() && it->second.bodyType == b2_dynamicBody) {

                float x = event->transform.p.x * pixels_per_meter;
                float y = event->transform.p.y * pixels_per_meter;
                float rotation = b2Rot_GetAngle(event->transform.q) * deg_per_rad;

                // Try to cast to sf::Shape
                if (auto shape = std::dynamic_pointer_cast<sf::Shape>(it->second.drawable)) {
                    shape->setPosition({x, y});
                    shape->setRotation(sf::degrees(rotation));
                }
                // Try to cast to sf::Sprite
                else if (auto sprite = std::dynamic_pointer_cast<sf::Sprite>(it->second.drawable)) {
                    sprite->setPosition({x, y});
                    sprite->setRotation(sf::degrees(rotation));
                }
            }
        }

        // Render all objects
        for (const auto& pair : physicsObjects) {
            render.draw(*pair.second.drawable);
        }

        // In your displayWorld function, call this after normal rendering:
        debugRenderCollisionShapesSimple(render);
    }

    inline void resetObjects() {
        // Temporary map for persistent objects
        std::unordered_map<int32_t, PhysicsObject> persistentObjects;

        // Separate persistent objects and destroy non-persistent ones
        for (auto it = physicsObjects.begin(); it != physicsObjects.end(); ) {
            if (it->second.isPersistent) {
                // Move persistent objects to temporary map
                persistentObjects[it->first] = std::move(it->second);
            } else {
                // Destroy non-persistent bodies
                if (b2Body_IsValid(it->second.bodyId)) {

                    // Destroy all joints first
                    for (auto jointId : it->second.partJoints) {
                        if (b2Joint_IsValid(jointId)) {
                            b2DestroyJoint(jointId);
                        }
                    }

                    // Destroy all part bodies
                    for (auto partBodyId : it->second.partBodies) {
                        if (b2Body_IsValid(partBodyId)) {
                            b2DestroyBody(partBodyId);
                        }
                    }

                    // Destroy main body
                    if (b2Body_IsValid(it->second.bodyId)) {
                        b2DestroyBody(it->second.bodyId);
                    }
                }
            }
            it = physicsObjects.erase(it);
        }

        // Clear bodies list and rebuild from persistent objects
        for (const auto& pair : persistentObjects) {
            physicsObjects[pair.first] = std::move(pair.second);
        }
    }
}

#endif // PHYSICS_H_INCLUDED
