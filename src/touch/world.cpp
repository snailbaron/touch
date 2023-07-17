#include "world.hpp"

#include <algorithm>
#include <iostream>

World::World()
{
    {
        auto hero = _em.createEntity();
        _em.add(hero, LocationComponent{
            .position = {2, 1},
            .velocity = {0, 0},
        });
        _heroControl = &_em.add(hero, ControlComponent{});

        events.push(AppearEvent{
            .entity = hero,
            .position = {2, 1},
            .type = EntityType::Hero,
        });
    }

    {
        auto tree = _em.createEntity();
        _em.add(tree, LocationComponent{
            .position = {-1, -1},
            .velocity = {0, 0},
        });
        events.push(AppearEvent{
            .entity = tree,
            .position = {-1, -1},
            .type = EntityType::Tree,
        });
    }
}

ControlComponent* World::control() const
{
    return _heroControl;
}

void World::update(double delta)
{
    for (auto e : _em.entities<ControlComponent>()) {
        auto& control = _em.component<ControlComponent>(e);
        auto& location = _em.component<LocationComponent>(e);

        location.velocity += control.control * delta *
            (control.acceleration + control.deceleration);
        if (auto speed = length(location.velocity); speed > 0) {
            auto newSpeed = std::clamp(
                speed - (float)delta * control.deceleration,
                0.f,
                control.maxSpeed);
            location.velocity *= newSpeed / speed;
        }
    }

    for (auto e : _em.entities<LocationComponent>()) {
        auto& location = _em.component<LocationComponent>(e);
        if (length(location.velocity) > 0) {
            location.position += location.velocity * delta;
            std::cout << "sending move event to " << location.position << "\n";
            events.push(MoveEvent{
                .entity = e,
                .position = location.position,
                .velocity = location.velocity,
            });
        }
    }
}
