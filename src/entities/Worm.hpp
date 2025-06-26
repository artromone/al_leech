#pragma once
#include "../terrain/TerrainManager.hpp"
#include <SFML/Graphics.hpp>

class Worm {
public:
  sf::CircleShape shape;
  sf::Vector2f position;
  sf::Vector2f velocity;
  int health;
  int maxHealth;
  bool isActive;
  bool isGrounded;
  bool canJump;
  sf::Color teamColor;
  sf::RectangleShape healthBar;
  sf::RectangleShape healthBarBg;
  float jumpCooldown;
  int teamId;
  bool isMyTurn;

  Worm(float x, float y, sf::Color color, int team);

  void update(float deltaTime, TerrainManager &terrain);
  void move(float direction);
  void jump(sf::Vector2f direction);
  void takeDamage(int damage);
  void draw(sf::RenderWindow &window);

  sf::Vector2f getCenter() const;
};
