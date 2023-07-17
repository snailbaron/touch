#include "world.hpp"

#include <algorithm>
#include <iostream>

namespace {

void notifyAppear(
    thing::Entity entity, EntityType type, const LocationComponent& location)
{
    events.push(AppearEvent{
        .entity = entity,
        .position = location.position,
        .type = type,
    });
}

} // namespace

World::World()
{
    {
        _heroEntity = _em.createEntity();
        const auto& heroLocation = _em.add(_heroEntity, LocationComponent{
            .position = {2, 1},
            .velocity = {0, 0},
        });
        _heroControl = &_em.add(_heroEntity, ControlComponent{});
        notifyAppear(_heroEntity, EntityType::Hero, heroLocation);

        _cameraEntity = _em.createEntity();
        const auto& cameraLocation =
            _em.add(_cameraEntity, LocationComponent{heroLocation});
        notifyAppear(_cameraEntity, EntityType::Camera, cameraLocation);
    }

    {
        auto tree = _em.createEntity();
        const auto& location = _em.add(tree, LocationComponent{
            .position = {-1, -1},
            .velocity = {0, 0},
        });
        notifyAppear(tree, EntityType::Tree, location);
    }

    {
        auto enemy = _em.createEntity();
        const auto& location = _em.add(enemy, LocationComponent{
            .position = {-4, 3},
            .velocity = {0, 0},
        });
        _em.add(enemy, EnemyComponent{});
        notifyAppear(enemy, EntityType::Enemy, location);
    }
}

ControlComponent* World::control() const
{
    return _heroControl;
}

void World::update(double delta)
{
    const auto& heroLocation = _em.component<LocationComponent>(_heroEntity);

    for (auto e : _em.entities<ControlComponent>()) {
        const auto& control = _em.component<ControlComponent>(e);
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

    for (auto e : _em.entities<EnemyComponent>()) {
        auto& enemy = _em.component<EnemyComponent>(e);
        enemy.timeSinceShot += delta;

        auto targets = _em.entities<ControlComponent>();
        if (enemy.timeSinceShot >= enemy.timeBetweenShots && !targets.empty()) {
            enemy.timeSinceShot -= enemy.timeBetweenShots;

            const auto& target = targets.front();
            const auto& targetLocation =
                _em.component<LocationComponent>(target);
            const auto& enemyLocation = _em.component<LocationComponent>(e);
            auto bullet = _em.createEntity();
            const auto& bulletLocation = _em.add(bullet, LocationComponent{
                .position = enemyLocation.position,
                .velocity =
                    unit(targetLocation.position - enemyLocation.position) *
                        enemy.bulletSpeed,
                .radius = 0.3f});
            _em.add(bullet, ProjectileComponent{});
            notifyAppear(bullet, EntityType::Ball, bulletLocation);
        }
    }

    for (auto e : _em.entities<ProjectileComponent>()) {
        auto& projectile = _em.component<ProjectileComponent>(e);
        projectile.timeSinceFired += delta;
        if (projectile.timeSinceFired >= projectile.timeToDisappear) {
            _em.killEntity(e);
            events.push(DisappearEvent{.entity = e});
            continue;
        }

        const auto& projectileLocation = _em.component<LocationComponent>(e);
        std::cout << "distance: " << distance(projectileLocation.position, heroLocation.position) << "\n";
        if (distance(projectileLocation.position, heroLocation.position) <
                projectileLocation.radius + heroLocation.radius) {
            std::cout << "sending game over\n";
            events.push(GameOverEvent{});
        }
    }

    for (auto e : _em.entities<LocationComponent>()) {
        auto& location = _em.component<LocationComponent>(e);
        if (length(location.velocity) > 0) {
            location.position += location.velocity * delta;
            events.push(MoveEvent{
                .entity = e,
                .position = location.position,
                .velocity = location.velocity,
            });
        }
    }

    {
        // TODO: This is embarassing. Make camera hard-snap to hero location.
        const auto& heroLocation = _em.component<LocationComponent>(_heroEntity);
        auto& cameraLocation = _em.component<LocationComponent>(_cameraEntity);
        auto diff = heroLocation.position - cameraLocation.position;
        auto difflen = length(diff);
        auto cameraSpeed = difflen * difflen * 20;
        cameraLocation.velocity = unit(diff) * cameraSpeed;
    }
}
