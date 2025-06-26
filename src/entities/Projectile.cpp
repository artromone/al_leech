#include "Projectile.hpp"
#include "../utils/GameTypes.hpp"
#include "../utils/MathUtils.hpp"
#include <cmath>

Projectile::Projectile(float x, float y, float vx, float vy, int team, int dmg,
                       int expRadius) {
  position = sf::Vector2f(x, y);
  velocity = sf::Vector2f(vx, vy);
  damage = dmg;
  explosionRadius = expRadius;
  shooterTeam = team;
  shape.setRadius(GameTypes::PROJECTILE_RADIUS);
  shape.setFillColor(sf::Color::Yellow);
  shape.setOutlineThickness(1);
  shape.setOutlineColor(sf::Color::Red);
  isActive = true;
  isLaunching = true;
  launchTimer = 0.2f;
  totalTime = 0.0f;
  travelDistance = 0.0f;
}

void Projectile::update(float deltaTime, TerrainManager &terrain) {
  if (!isActive)
    return;

  totalTime += deltaTime;

  // Анимация пуска
  if (isLaunching) {
    launchTimer -= deltaTime;
    if (launchTimer <= 0) {
      isLaunching = false;
    }
    return;
  }

  // Сохраняем предыдущую позицию для расчета расстояния
  sf::Vector2f oldPosition = position;

  // Применяем гравитацию
  velocity.y += GameTypes::PROJECTILE_GRAVITY * deltaTime;

  // Обновляем позицию
  position += velocity * deltaTime;

  // Обновляем пройденное расстояние
  sf::Vector2f deltaPos = position - oldPosition;
  travelDistance += MathUtils::length(deltaPos);

  // Добавляем точку в след
  trail.push_back(position);
  if (trail.size() > 15) {
    trail.pop_front();
  }

  // Проверяем коллизию с местностью
  if (terrain.isColliding(static_cast<int>(position.x),
                          static_cast<int>(position.y))) {
    explode(terrain);
    return;
  }

  // Проверяем границы экрана
  if (position.y > GameTypes::WINDOW_HEIGHT || position.x < 0 ||
      position.x > GameTypes::WINDOW_WIDTH) {
    isActive = false;
  }
}

void Projectile::explode(TerrainManager &terrain) {
  terrain.destroyTerrain(static_cast<int>(position.x),
                         static_cast<int>(position.y), explosionRadius);
  isActive = false;
}

void Projectile::draw(sf::RenderWindow &window) {
  if (!isActive)
    return;

  // Рисуем след снаряда
  if (!isLaunching && trail.size() > 1) {
    for (size_t i = 1; i < trail.size(); i++) {
      sf::CircleShape trailPoint(1.5f - (i * 0.1f));
      trailPoint.setPosition(trail[i].x - trailPoint.getRadius(),
                             trail[i].y - trailPoint.getRadius());
      sf::Color trailColor = sf::Color::Yellow;
      trailColor.a = static_cast<sf::Uint8>(
          255 * (1.0f - static_cast<float>(i) / trail.size()));
      trailPoint.setFillColor(trailColor);
      window.draw(trailPoint);
    }
  }

  if (isLaunching) {
    float scale = 1.0f + 0.8f * sin(totalTime * 30);
    sf::CircleShape launchShape = shape;
    launchShape.setRadius(shape.getRadius() * scale);
    launchShape.setPosition(position.x - launchShape.getRadius(),
                            position.y - launchShape.getRadius());
    sf::Color launchColor = sf::Color::White;
    launchColor.a = static_cast<sf::Uint8>(255 * (launchTimer / 0.2f));
    launchShape.setFillColor(launchColor);
    window.draw(launchShape);
  } else {
    shape.setPosition(position.x - GameTypes::PROJECTILE_RADIUS,
                      position.y - GameTypes::PROJECTILE_RADIUS);
    window.draw(shape);
  }
}

sf::Vector2f Projectile::getPosition() const { return position; }

bool Projectile::checkWormCollision(const Worm &worm) {
  if (!isActive || !worm.isActive || isLaunching)
    return false;

  // Не наносим урон, если снаряд не пролетел минимальное расстояние
  if (travelDistance < 50.0f)
    return false;

  float distanceLength = MathUtils::distance(position, worm.getCenter());
  return distanceLength < 20;
}
