#pragma once
#include "../entities/Projectile.hpp"
#include "../entities/Worm.hpp"
#include "../terrain/TerrainManager.hpp"
#include "../utils/GameTypes.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class Game {
private:
  sf::RenderWindow window;
  TerrainManager terrain;
  std::vector<Worm> worms;
  std::vector<Projectile> projectiles;
  int currentPlayer;
  sf::Clock clock;
  sf::Vector2f aimDirection;
  float aimPower;
  bool gameStarted;
  bool gameEnded;
  int winner;
  GameTypes::WeaponType currentWeapon;
  int weaponIndex;
  std::vector<sf::Vector2f> trajectoryPoints;
  bool keysPressed[sf::Keyboard::KeyCount];
  float turnTimer;
  bool canShoot;

public:
  Game();

  void run();

private:
  void calculateTrajectory();
  void handleEvents();
  void handleKeyPress(sf::Keyboard::Key key);
  void handleContinuousInput();
  void updateAim();
  void shoot();
  void switchToNextPlayer();
  int getActiveWormsCount();
  void restartGame();
  void update();
  void render();
};
