
#pragma once
#include "../terrain/TerrainManager.hpp"
#include "../utils/GameTypes.hpp"
#include "Worm.hpp"
#include <SFML/Graphics.hpp>
#include <deque>
#include <vector>

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

  // Новые поля для разных типов оружия
  GameTypes::WeaponType weaponType;
  bool isShrapnel;
  std::vector<Projectile> shrapnelPieces;
  float penetrationPower;

  Projectile(float x, float y, float vx, float vy, int team,
             GameTypes::WeaponType type = GameTypes::WeaponType::BAZOOKA,
             int dmg = 25, int expRadius = 30);

  void update(float deltaTime, TerrainManager &terrain);
  void explode(TerrainManager &terrain);
  void createShrapnel();
  void draw(sf::RenderWindow &window);

  sf::Vector2f getPosition() const;
  bool checkWormCollision(const Worm &worm);
};
