#pragma once

#include <as.hpp>
#include <evening.hpp>
#include <thing.hpp>
#include <ve.hpp>

#include <deque>

inline evening::Channel events;

template <class T> struct XYModel {
    T x;
    T y;
};
using Position = ve::Point<XYModel, float>;
using Vector = ve::Vector<XYModel, float>;

struct LocationComponent {
    Position position;
    Vector velocity;
    float radius = 0.5f;
};

struct ControlComponent {
    Vector control;
    float acceleration = 30.f;
    float deceleration = 50.f;
    float maxSpeed = 8.f;
};

struct ProximityTurretComponent {
    double bulletSpeed = 4.0;

    size_t ammoCapacity = 3;
    double timeBetweenShots = 0.5;
    double reloadTime = 2.0;

    double proximity = 3.0;

    double timeSinceLastShot = 0.0;
    size_t ammo = ammoCapacity;
    std::deque<double> reloads;
};

struct ProjectileComponent {
    double timeToDisappear = 5.0;
    double timeSinceFired = 0.0;
};

struct SmartTowerComponent {
};

enum class EntityType {
    Hero,
    Tree,
    Ball,
    Enemy,
    Camera,
};

struct AppearEvent {
    thing::Entity entity = thing::Entity{0};
    Position position;
    EntityType type {};
};

struct DisappearEvent {
    thing::Entity entity = thing::Entity{0};
};

struct MoveEvent {
    thing::Entity entity = thing::Entity{0};
    Position position;
    Vector velocity;
};

struct GameOverEvent {};

class World {
public:
    World();

    ControlComponent* control() const;

    void update(double delta);

private:
    thing::EntityManager _em;
    ControlComponent* _heroControl = nullptr;
    as::coro::Pool _coro;

    thing::Entity _heroEntity;
    thing::Entity _cameraEntity;
};
