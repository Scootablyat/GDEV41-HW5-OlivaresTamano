#include <raylib.h>
#include <raymath.h>
#include <iostream>

#include "entt.hpp"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float FPS = 60;
const float TIMESTEP = 1 / FPS;
const float initialCellSize = WINDOW_WIDTH;


struct PositionComponent {
    Vector2 position;
};

struct ColorComponent {
    Color color;
};

struct CircleComponent {
    float radius;
};

struct PhysicsComponent {
    Vector2 velocity;
    bool repels = false;
    bool attracts = false;
};

struct SelfDestructComponent{
    float lifeTime;
    float maxLifeTime;
};

struct DuplicateOnDeathComponent{
    int amountToSpawn;
};


float RandomDirection(){
    float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    // Make it [-1, 1]
    return x * 2.0f - 1.0f;
}

Vector2 getNormalizedMouseDirectionVector(Vector2 point){
    Vector2 mousePos = GetMousePosition();
    return Vector2Normalize(Vector2Subtract(point, mousePos));
}

void InitializeAsteroid(entt::registry &registry, float numberOfCircles){
    for (int x = 0; x < numberOfCircles; x++) {
        int fiftyFifty = GetRandomValue(1,2);
        int willItKillItself = GetRandomValue(1,100);
        int willSpawnMoreOnDeath = GetRandomValue(1,10);
        entt::entity asteroid = registry.create();
        PositionComponent& pos_comp = registry.emplace<PositionComponent>(asteroid);
        pos_comp.position = {static_cast<float>(GetRandomValue(0, WINDOW_WIDTH)), static_cast< float >(GetRandomValue(0, WINDOW_HEIGHT))};
        CircleComponent& circ_comp = registry.emplace<CircleComponent>(asteroid);
        circ_comp.radius = GetRandomValue(20, 50);
        PhysicsComponent& phys_comp = registry.emplace<PhysicsComponent>(asteroid);
        ColorComponent& color_comp = registry.emplace<ColorComponent>(asteroid);
        if(fiftyFifty == 1){
            phys_comp.velocity = {500.0f * RandomDirection(), 500.0f * RandomDirection()};
            color_comp.color = BLUE;
        }
        if(fiftyFifty == 2){
            int fiftyFifty2 = GetRandomValue(1,2);
            if(fiftyFifty2 == 1){ // repel
                phys_comp.repels = true;
                phys_comp.velocity = Vector2Scale(getNormalizedMouseDirectionVector(pos_comp.position), 300.0f);
                color_comp.color = PURPLE;
            }
            if(fiftyFifty2 == 2){ // attract
                phys_comp.attracts = true;
                phys_comp.velocity = Vector2Scale(Vector2Scale(getNormalizedMouseDirectionVector(pos_comp.position), -1), 300.0f);
                color_comp.color = YELLOW;
            }
        }
        if(willItKillItself <= 35){
            SelfDestructComponent& death_comp = registry.emplace<SelfDestructComponent>(asteroid);
            death_comp.maxLifeTime = GetRandomValue(2,5);
            std::cout << "someone is flagged for death lol" << std::endl;
        }
        if(willSpawnMoreOnDeath == 1){
            DuplicateOnDeathComponent& dupe_comp = registry.emplace<DuplicateOnDeathComponent>(asteroid);
            std::cout << "a pink horror of tzeentch has spawned" << std::endl;
        }
    }
}

float getDistanceToAsteroid(Vector2 point, Vector2 circle){
    return sqrt( pow((point.x - circle.x),2) + pow((point.y - circle.y),2) );
}

bool IsPointInAsteroid(entt::registry &registry, entt::entity asteroid, Vector2 point){
    CircleComponent& circle_comp = registry.get<CircleComponent>(asteroid);
    PositionComponent& position_comp = registry.get<PositionComponent>(asteroid);
    float distance = getDistanceToAsteroid(point, position_comp.position);
    if(distance < circle_comp.radius){
        return true;
    }
    return false;
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OlivaresTamano - Homework 5");
    SetTargetFPS(FPS);

    entt::registry registry;
    //std::vector<entt::entity> asteroids;
    InitializeAsteroid(registry, 5);

    float counter;
    float fixedInterval = 5.0f;
    float accumulator = 0;

    while(!WindowShouldClose()) {
        float delta_time = GetFrameTime();
        counter += delta_time;
        auto allAsteroids = registry.view<PositionComponent, CircleComponent, ColorComponent, PhysicsComponent>();
        auto allDyingAsteroids  = registry.view<SelfDestructComponent>();
        auto allDuplicatingAsteroids = registry.view<DuplicateOnDeathComponent>();

        if(counter >= fixedInterval){
            counter = 0;
            InitializeAsteroid(registry, 5);
        }

        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
            Vector2 mousePos = GetMousePosition();
            for (auto entity : allAsteroids) {
                if(IsPointInAsteroid(registry, entity, mousePos)){
                    registry.destroy(entity);
                    break;
                }
            }
            
            
        }
        //std::cout << counter << std::endl;
        accumulator += delta_time;
        while (accumulator >= TIMESTEP){ // Physics Step/*
            for (auto entity : allDyingAsteroids){
                SelfDestructComponent& deathTimer = registry.get<SelfDestructComponent>(entity);
                deathTimer.lifeTime+=delta_time;
                if(deathTimer.lifeTime >= deathTimer.maxLifeTime){
                    registry.destroy(entity);
                }
            }
            for (auto entity : allDuplicatingAsteroids){
                DuplicateOnDeathComponent& dupe_comp = registry.get<DuplicateOnDeathComponent>(entity);
                dupe_comp.amountToSpawn = GetRandomValue(2,5);
            }
            for (auto entity : allAsteroids) {
                PositionComponent& position = registry.get<PositionComponent>(entity);
                CircleComponent& circle = registry.get<CircleComponent>(entity);
                PhysicsComponent& physics = registry.get<PhysicsComponent>(entity);
                
                
                if (position.position.x - circle.radius <= 0){
                    position.position.x = circle.radius;
                    physics.velocity.x *= -1;
                }
                if (position.position.x + circle.radius >= WINDOW_WIDTH){
                    position.position.x = WINDOW_WIDTH - circle.radius;
                    physics.velocity.x *= -1;
                }
                if (position.position.y - circle.radius <= 0){
                    position.position.y = circle.radius;
                    physics.velocity.y *= -1;
                }
                if (position.position.y + circle.radius >= WINDOW_HEIGHT){
                    position.position.y = WINDOW_HEIGHT - circle.radius;
                    physics.velocity.y *= -1;
                }
                if(physics.repels == true){
                    physics.velocity = Vector2Scale(getNormalizedMouseDirectionVector(position.position), 300.0f);
                    position.position = Vector2Add(position.position, Vector2Scale(physics.velocity, TIMESTEP)); 
                }
                if(physics.attracts == true){
                    physics.velocity = Vector2Scale(Vector2Scale(getNormalizedMouseDirectionVector(position.position), -1), 300.0f);
                    position.position = Vector2Add(position.position, Vector2Scale(physics.velocity, TIMESTEP)); 
                }
                else{
                    position.position = Vector2Add(position.position, Vector2Scale(physics.velocity, TIMESTEP)); 
                }
                
            }
            
            accumulator -= TIMESTEP;
        }

        /*
        for (int i = 0; i < asteroids.size(); i++){
            if (asteroids[i] + asteroids[i].radius >= WINDOW_WIDTH || asteroids[i].position.x - asteroids[i].radius <= 0)
            {
                asteroids[i].velocity.x *= -1;
            }
            if (asteroids[i].position.y + asteroids[i].radius >= WINDOW_HEIGHT || asteroids[i].position.y - asteroids[i].radius <= 0)
            {
                asteroids[i].velocity.y *= -1;
            }
        }
        auto all_asteroids = registry.view<PositionComponent, CircleComponent, ColorComponent>();
        for (auto entity : all_asteroids) {
            PositionComponent& pos = registry.get<PositionComponent>(entity);
            CircleComponent& circle = registry.get<CircleComponent>(entity);
            ColorComponent& color = registry.get<ColorComponent>(entity);
        }
        */
        

        /*
        auto move_rect = registry.view<RectangleComponent>();
        for (auto entity: move_rect) {
            PositionComponent& position = registry.get<PositionComponent>(entity);
            RectangleComponent& rect = registry.get<RectangleComponent>(entity);

            position.position.x += 20 * GetFrameTime();
        }
        */
        BeginDrawing();
        ClearBackground(WHITE);

        /*
                auto all_squares = registry.view<PositionComponent, RectangleComponent, ColorComponent>();
        for (auto entity: all_squares) {
            RectangleComponent& rectangle = registry.get<RectangleComponent>(entity);
            PositionComponent& position = registry.get<PositionComponent>(entity);
            ColorComponent& color = registry.get<ColorComponent>(entity);

            DrawRectangleV(position.position, {rectangle.width, rectangle.height}, color.color);
        }
        */
        auto all_asteroids = registry.view<PositionComponent, CircleComponent, ColorComponent>();
        for (auto entity : all_asteroids) {
            PositionComponent& pos = registry.get<PositionComponent>(entity);
            CircleComponent& circle = registry.get<CircleComponent>(entity);
            ColorComponent& color = registry.get<ColorComponent>(entity);

            DrawCircleV(pos.position, circle.radius, color.color);
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}