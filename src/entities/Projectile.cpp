
#include "Projectile.hpp"
#include "../utils/GameTypes.hpp"
#include "../utils/MathUtils.hpp"
#include <cmath>
#include <random>

Projectile::Projectile(float x, float y, float vx, float vy, int team,
                       GameTypes::WeaponType type, int dmg, int expRadius) {
  position = sf::Vector2f(x, y);
  velocity = sf::Vector2f(vx, vy);
  weaponType = type;
  shooterTeam = team;
  isShrapnel = false;

  // Настройки в зависимости от типа оружия
  switch (weaponType) {
  case GameTypes::WeaponType::SNIPER_RIFLE:
    damage = 75;
    explosionRadius = 5;
    penetrationPower = 3.0f;
    shape.setRadius(2);
    shape.setFillColor(sf::Color::Blue);
    break;

  case GameTypes::WeaponType::FRAG_GRENADE:
    damage = 35;
    explosionRadius = 45;
    penetrationPower = 0.0f;
    shape.setRadius(6);
    shape.setFillColor(sf::Color::Green);
    break;

  default: // BAZOOKA
    damage = dmg;
    explosionRadius = expRadius;
    penetrationPower = 0.0f;
    shape.setRadius(GameTypes::PROJECTILE_RADIUS);
    shape.setFillColor(sf::Color::Yellow);
    break;
  }

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

  if (isLaunching) {
    launchTimer -= deltaTime;
    if (launchTimer <= 0) {
      isLaunching = false;
    }
    return;
  }

  sf::Vector2f oldPosition = position;

  // Применяем гравитацию (снайперка меньше подвержена гравитации)
  float gravityMultiplier =
      (weaponType == GameTypes::WeaponType::SNIPER_RIFLE) ? 0.3f : 1.0f;
  velocity.y += GameTypes::PROJECTILE_GRAVITY * gravityMultiplier * deltaTime;

  position += velocity * deltaTime;

  sf::Vector2f deltaPos = position - oldPosition;
  travelDistance += MathUtils::length(deltaPos);

  // След снаряда
  trail.push_back(position);
  if (trail.size() > 15) {
    trail.pop_front();
  }

  // Проверяем коллизию с местностью
  if (terrain.isColliding(static_cast<int>(position.x),
                          static_cast<int>(position.y))) {
    // Снайперская винтовка может пробивать препятствия
    if (weaponType == GameTypes::WeaponType::SNIPER_RIFLE &&
        penetrationPower > 0) {
      terrain.destroyTerrain(static_cast<int>(position.x),
                             static_cast<int>(position.y), 3);
      penetrationPower -= 1.0f;
      if (penetrationPower <= 0) {
        explode(terrain);
      }
    } else {
      explode(terrain);
    }
    return;
  }

  // Обновляем осколки для осколочной гранаты
  for (auto &shrapnel : shrapnelPieces) {
    shrapnel.update(deltaTime, terrain);
  }

  // Удаляем неактивные осколки
  shrapnelPieces.erase(
      std::remove_if(shrapnelPieces.begin(), shrapnelPieces.end(),
                     [](const Projectile &p) { return !p.isActive; }),
      shrapnelPieces.end());

  // Проверяем границы экрана
  if (position.y > GameTypes::WINDOW_HEIGHT || position.x < 0 ||
      position.x > GameTypes::WINDOW_WIDTH) {
    isActive = false;
  }
}

void Projectile::explode(TerrainManager &terrain) {
  terrain.destroyTerrain(static_cast<int>(position.x),
                         static_cast<int>(position.y), explosionRadius);

  // Создаем осколки для осколочной гранаты
  if (weaponType == GameTypes::WeaponType::FRAG_GRENADE) {
    createShrapnel();
  }

  isActive = false;
}

void Projectile::createShrapnel() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> angleDist(0.0, 2.0 * M_PI);
  std::uniform_real_distribution<> speedDist(150.0, 300.0);

  // Создаем 8 осколков
  for (int i = 0; i < 8; i++) {
    float angle = angleDist(gen);
    float speed = speedDist(gen);

    float vx = cos(angle) * speed;
    float vy = sin(angle) * speed;

    Projectile shrapnel(position.x, position.y, vx, vy, shooterTeam,
                        GameTypes::WeaponType::BAZOOKA, 15, 8);
    shrapnel.isShrapnel = true;
    shrapnel.shape.setRadius(2);
    shrapnel.shape.setFillColor(sf::Color::Red);

    shrapnelPieces.push_back(shrapnel);
  }
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

      sf::Color trailColor;
      switch (weaponType) {
      case GameTypes::WeaponType::SNIPER_RIFLE:
        trailColor = sf::Color::Blue;
        break;
      case GameTypes::WeaponType::FRAG_GRENADE:
        trailColor = sf::Color::Green;
        break;
      default:
        trailColor = sf::Color::Yellow;
        break;
      }

      trailColor.a = static_cast<sf::Uint8>(
          255 * (1.0f - static_cast<float>(i) / trail.size()));
      trailPoint.setFillColor(trailColor);
      window.draw(trailPoint);
    }
  }

  // Рисуем основной снаряд
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
    shape.setPosition(position.x - shape.getRadius(),
                      position.y - shape.getRadius());
    window.draw(shape);
  }

  // Рисуем осколки
  for (auto &shrapnel : shrapnelPieces) {
    shrapnel.draw(window);
  }
}

sf::Vector2f Projectile::getPosition() const { return position; }

bool Projectile::checkWormCollision(const Worm &worm) {
  if (!isActive || !worm.isActive || isLaunching)
    return false;

  float minDistance =
      (weaponType == GameTypes::WeaponType::SNIPER_RIFLE) ? 30.0f : 50.0f;
  if (travelDistance < minDistance)
    return false;

  float distanceLength = MathUtils::distance(position, worm.getCenter());
  bool collision = distanceLength < 20;

  // Проверяем коллизии с осколками
  for (auto &shrapnel : shrapnelPieces) {
    if (shrapnel.checkWormCollision(worm)) {
      collision = true;
      break;
    }
  }

  return collision;
}
