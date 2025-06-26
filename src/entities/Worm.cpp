#include "Worm.hpp"
#include "../utils/GameTypes.hpp"
#include <algorithm>

Worm::Worm(float x, float y, sf::Color color, int team)
    : teamColor(color), teamId(team) {
  position = sf::Vector2f(x, y);
  velocity = sf::Vector2f(0, 0);
  health = 100;
  maxHealth = 100;
  shape.setRadius(GameTypes::WORM_RADIUS);
  shape.setFillColor(color);
  shape.setOutlineThickness(2);
  shape.setOutlineColor(sf::Color::Black);
  isActive = true;
  isGrounded = false;
  canJump = true;
  jumpCooldown = 0.0f;
  isMyTurn = false;

  // Настройка полоски здоровья
  healthBarBg.setSize(sf::Vector2f(30, 4));
  healthBarBg.setFillColor(sf::Color::Red);
  healthBar.setSize(sf::Vector2f(30, 4));
  healthBar.setFillColor(sf::Color::Green);
}

void Worm::update(float deltaTime, TerrainManager &terrain) {
  if (!isActive)
    return;

  // Обновляем кулдаун прыжка
  if (jumpCooldown > 0) {
    jumpCooldown -= deltaTime;
  }

  // Применяем гравитацию
  velocity.y += GameTypes::GRAVITY * deltaTime;

  // Ограничиваем скорость падения
  velocity.y = std::min(velocity.y, 500.0f);

  // Обновляем позицию по X
  sf::Vector2f newPosX =
      sf::Vector2f(position.x + velocity.x * deltaTime, position.y);
  if (!terrain.isColliding(newPosX)) {
    position.x = newPosX.x;
  } else {
    velocity.x = 0;
  }

  // Обновляем позицию по Y
  sf::Vector2f newPosY =
      sf::Vector2f(position.x, position.y + velocity.y * deltaTime);
  if (!terrain.isColliding(newPosY)) {
    position.y = newPosY.y;
    isGrounded = false;
  } else {
    if (velocity.y > 0) {
      isGrounded = true;
      canJump = true;
      jumpCooldown = 0.0f;
    }
    velocity.y = 0;
  }

  // Применяем трение только по X
  velocity.x *= 0.85f;

  // Проверяем границы экрана
  if (position.x < GameTypes::WORM_RADIUS) {
    position.x = GameTypes::WORM_RADIUS;
    velocity.x = 0;
  }
  if (position.x > GameTypes::WINDOW_WIDTH - GameTypes::WORM_RADIUS) {
    position.x = GameTypes::WINDOW_WIDTH - GameTypes::WORM_RADIUS;
    velocity.x = 0;
  }
  if (position.y > 650) {
    takeDamage(20);
    // Возвращаем червяка на поверхность
    int groundLevel = terrain.findGroundLevel(static_cast<int>(position.x));
    position.y = groundLevel - 20;
    velocity.y = 0;
  }
}

void Worm::move(float direction) {
  if (!isActive || !isMyTurn)
    return;

  velocity.x += direction * 100.0f;
  velocity.x = std::max(-150.0f, std::min(150.0f, velocity.x));
}

void Worm::jump(sf::Vector2f direction) {
  if (!isActive || !canJump || !isGrounded || jumpCooldown > 0 || !isMyTurn)
    return;

  float jumpPower = 280.0f;
  velocity.x += direction.x * jumpPower;
  velocity.y = direction.y * jumpPower;
  canJump = false;
  isGrounded = false;
  jumpCooldown = 0.5f;
}

void Worm::takeDamage(int damage) {
  health -= damage;
  if (health <= 0) {
    health = 0;
    isActive = false;
  }
}

void Worm::draw(sf::RenderWindow &window) {
  if (!isActive) {
    sf::Color deadColor = teamColor;
    deadColor.a = 100;
    shape.setFillColor(deadColor);
  } else {
    shape.setFillColor(teamColor);

    // Подсвечиваем активного игрока
    if (isMyTurn) {
      shape.setOutlineThickness(4);
      shape.setOutlineColor(sf::Color::White);
    } else {
      shape.setOutlineThickness(2);
      shape.setOutlineColor(sf::Color::Black);
    }
  }

  shape.setPosition(position.x - GameTypes::WORM_RADIUS,
                    position.y - GameTypes::WORM_RADIUS);
  window.draw(shape);

  if (isActive) {
    // Рисуем полоску здоровья
    healthBarBg.setPosition(position.x - 15, position.y - 25);
    healthBar.setPosition(position.x - 15, position.y - 25);
    float healthPercent = static_cast<float>(health) / maxHealth;
    healthBar.setSize(sf::Vector2f(30 * healthPercent, 4));

    if (healthPercent > 0.6f) {
      healthBar.setFillColor(sf::Color::Green);
    } else if (healthPercent > 0.3f) {
      healthBar.setFillColor(sf::Color::Yellow);
    } else {
      healthBar.setFillColor(sf::Color::Red);
    }

    window.draw(healthBarBg);
    window.draw(healthBar);
  }
}

sf::Vector2f Worm::getCenter() const { return position; }
