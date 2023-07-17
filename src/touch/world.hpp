#pragma once

#include <evening.hpp>
#include <thing.hpp>
#include <ve.hpp>

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
};

struct ControlComponent {
    Vector control;
    float acceleration = 30.f;
    float deceleration = 50.f;
    float maxSpeed = 8.f;
};

enum class EntityType {
    Hero,
    Tree,
};

struct AppearEvent {
    thing::Entity entity = thing::Entity{0};
    Position position;
    EntityType type {};
};

struct MoveEvent {
    thing::Entity entity = thing::Entity{0};
    Position position;
    Vector velocity;
};

class World {
public:
    World();

    ControlComponent* control() const;

    void update(double delta);

private:
    thing::EntityManager _em;
    ControlComponent* _heroControl = nullptr;
};
