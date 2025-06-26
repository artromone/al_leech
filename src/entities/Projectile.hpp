#pragma once
#include "../terrain/TerrainManager.hpp"
#include "Worm.hpp"
#include <SFML/Graphics.hpp>
#include <deque>

class Projectile {
public:
  sf::CircleShape shape;
  sf::Vector2f position;
  sf::Vector2f velocity;
  bool isActive;
  bool isLaunching;
  int damage;
  int explosionRadius;
  float launchTimer;
  float totalTime;
  float travelDistance;
  std::deque<sf::Vector2f> trail;
  int shooterTeam;

  Projectile(float x, float y, float vx, float vy, int team, int dmg = 25,
             int expRadius = 30);

  void update(float deltaTime, TerrainManager &terrain);
  void explode(TerrainManager &terrain);
  void draw(sf::RenderWindow &window);

  sf::Vector2f getPosition() const;
  bool checkWormCollision(const Worm &worm);
};
