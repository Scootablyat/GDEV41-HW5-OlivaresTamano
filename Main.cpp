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
    Vector2 position;
    float mass;
    float inverse_mass;

};

float RandomDirection(){
    float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    // Make it [-1, 1]
    return x * 2.0f - 1.0f;
}

void InitializeAsteroid(entt::registry &registry, float numberOfCircles){
    for (int x = 0; x < numberOfCircles; x++) {
        entt::entity asteroid = registry.create();
        PositionComponent& pos_comp = registry.emplace<PositionComponent>(asteroid);
        pos_comp.position = {static_cast<float>(GetRandomValue(0, WINDOW_WIDTH)), static_cast< float >(GetRandomValue(0, WINDOW_HEIGHT))};
        CircleComponent& circ_comp = registry.emplace<CircleComponent>(asteroid);
        circ_comp.radius = GetRandomValue(20, 50);
        ColorComponent& color_comp = registry.emplace<ColorComponent>(asteroid);
        color_comp.color = BLUE;
        PhysicsComponent& phys_comp = registry.emplace<PhysicsComponent>(asteroid);
        phys_comp.velocity = {500.0f * RandomDirection(), 500.0f * RandomDirection()};
    }
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

        if(counter >= fixedInterval){
            counter = 0;
            InitializeAsteroid(registry, 5);
        }
        std::cout << counter << std::endl;

        auto move_asteroids = registry.view<CircleComponent>();
        accumulator += delta_time;
        while (accumulator >= TIMESTEP){ // Physics Step
            for (auto entity : move_asteroids) {
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
                position.position = Vector2Add(position.position, Vector2Scale(physics.velocity, TIMESTEP));
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