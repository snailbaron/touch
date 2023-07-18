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
            .position = {0, 0},
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
        _em.add(enemy, ProximityTurretComponent{});
        notifyAppear(enemy, EntityType::Enemy, location);
    }

    {
        auto t1 = _em.createEntity();
        const auto& l1 = _em.add(t1, LocationComponent{
            .position = {-1, 8},
            .velocity = {0, 0},
        });
        notifyAppear(t1, EntityType::Enemy, l1);
        _em.add(t1, ProximityTurretComponent{
            .bulletSpeed = 15.0,
            .ammoCapacity = 5,
            .timeBetweenShots = 0.1,
            .reloadTime = 1.0,
            .proximity = 2.0
        });

        auto t2 = _em.createEntity();
        const auto& l2 = _em.add(t2, LocationComponent{
            .position = {1, 8},
            .velocity = {0, 0},
        });
        notifyAppear(t2, EntityType::Enemy, l2);
        _em.add(t2, ProximityTurretComponent{
            .bulletSpeed = 15.0,
            .ammoCapacity = 5,
            .timeBetweenShots = 0.1,
            .reloadTime = 1.0,
            .proximity = 2.0
        });
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

    for (auto e : _em.entities<ProximityTurretComponent>()) {
        auto& enemy = _em.component<ProximityTurretComponent>(e);

        std::cerr.setf(std::ios::fixed, std::ios::floatfield);
        std::cerr.precision(6);

        std::cerr << "reloads:";
        for (auto r : enemy.reloads) {
            std::cerr << " " << r;
        }
        std::cerr << "\n";

        enemy.timeSinceLastShot += delta;

        while (!enemy.reloads.empty() && enemy.reloads.front() <= delta) {
            enemy.reloads.pop_front();
            enemy.ammo++;
        }
        for (auto& reload : enemy.reloads) {
            reload -= delta;
        }

        if (enemy.ammo == 0) {
            continue;
        }

        if (enemy.timeSinceLastShot < enemy.timeBetweenShots) {
            continue;
        }

        const auto& enemyLocation = _em.component<LocationComponent>(e);
        if (distance(enemyLocation.position, heroLocation.position) >
                enemy.proximity) {
            std::cerr << "too far: " <<
                distance(enemyLocation.position, heroLocation.position) << "\n";
            continue;
        }

        enemy.ammo--;
        enemy.timeSinceLastShot = 0.0;
        enemy.reloads.push_back(enemy.reloadTime);

        auto bullet = _em.createEntity();
        const auto& bulletLocation = _em.add(bullet, LocationComponent{
            .position = enemyLocation.position,
            .velocity =
                unit(heroLocation.position - enemyLocation.position) *
                    enemy.bulletSpeed,
            .radius = 0.3f});
        _em.add(bullet, ProjectileComponent{});
        notifyAppear(bullet, EntityType::Ball, bulletLocation);
    }

    auto projectilesToKill = std::vector<thing::Entity>{};
    for (auto e : _em.entities<ProjectileComponent>()) {
        auto& projectile = _em.component<ProjectileComponent>(e);
        projectile.timeSinceFired += delta;
        if (projectile.timeSinceFired >= projectile.timeToDisappear) {
            projectilesToKill.push_back(e);
            continue;
        }

        //const auto& projectileLocation = _em.component<LocationComponent>(e);
        //if (distance(projectileLocation.position, heroLocation.position) <
        //        projectileLocation.radius + heroLocation.radius) {
        //    events.push(GameOverEvent{});
        //}
    }

    for (auto e : projectilesToKill) {
        _em.killEntity(e);
        events.push(DisappearEvent{.entity = e});
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
