#include "Game.hpp"
#include "../utils/GameTypes.hpp"
#include "../utils/MathUtils.hpp"
#include <algorithm>

Game::Game()
    : window(sf::VideoMode(GameTypes::WINDOW_WIDTH, GameTypes::WINDOW_HEIGHT),
             "Enhanced Wormix Game"),
      terrain(GameTypes::WINDOW_WIDTH, GameTypes::WINDOW_HEIGHT),
      currentPlayer(0), aimPower(0), gameStarted(true), gameEnded(false),
      winner(-1), turnTimer(0.0f), canShoot(true) {

  // Инициализируем массив нажатых клавиш
  for (int i = 0; i < sf::Keyboard::KeyCount; i++) {
    keysPressed[i] = false;
  }

  // Создаем червяков с ID команд
  worms.push_back(Worm(150, 200, sf::Color::Green, 0));
  worms.push_back(Worm(650, 200, sf::Color::Blue, 1));

  // Размещаем червяков на земле
  for (auto &worm : worms) {
    int groundLevel =
        terrain.findGroundLevel(static_cast<int>(worm.position.x));
    worm.position.y = groundLevel - 20;
  }

  // Устанавливаем первого игрока как активного
  worms[currentPlayer].isMyTurn = true;

  window.setFramerateLimit(60);
}

void Game::calculateTrajectory() {
  trajectoryPoints.clear();

  if (!worms[currentPlayer].isMyTurn)
    return;

  sf::Vector2f wormCenter = worms[currentPlayer].getCenter();
  sf::Vector2f startPos = wormCenter + aimDirection * 25.0f;
  sf::Vector2f vel = aimDirection * (400.0f + aimPower * 3.0f);
  float dt = 0.05f;

  for (int i = 0; i < 100; i++) {
    trajectoryPoints.push_back(startPos);
    vel.y += GameTypes::PROJECTILE_GRAVITY * dt;
    startPos += vel * dt;

    if (terrain.isColliding(static_cast<int>(startPos.x),
                            static_cast<int>(startPos.y)) ||
        startPos.y > GameTypes::WINDOW_HEIGHT || startPos.x < 0 ||
        startPos.x > GameTypes::WINDOW_WIDTH) {
      break;
    }
  }
}

void Game::handleEvents() {
  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window.close();
    }

    if (event.type == sf::Event::KeyPressed) {
      keysPressed[event.key.code] = true;
      handleKeyPress(event.key.code);
    }

    if (event.type == sf::Event::KeyReleased) {
      keysPressed[event.key.code] = false;
    }

    if (event.type == sf::Event::MouseButtonPressed) {
      if (event.mouseButton.button == sf::Mouse::Left && canShoot &&
          worms[currentPlayer].isMyTurn) {
        shoot();
      }
    }

    if (event.type == sf::Event::MouseMoved) {
      updateAim();
    }
  }
}

void Game::handleKeyPress(sf::Keyboard::Key key) {
  if (!gameStarted || gameEnded) {
    if (key == sf::Keyboard::R && gameEnded) {
      restartGame();
    }
    return;
  }

  Worm &activeWorm = worms[currentPlayer];
  if (!activeWorm.isActive || !activeWorm.isMyTurn)
    return;

  switch (key) {
  case sf::Keyboard::Space:
    if (canShoot) {
      shoot();
    }
    break;
  case sf::Keyboard::W:
  case sf::Keyboard::Up: {
    sf::Vector2f jumpDir = aimDirection;
    jumpDir.y = std::min(jumpDir.y, -0.3f);
    activeWorm.jump(jumpDir);
  } break;
  }
}

void Game::handleContinuousInput() {
  if (!gameStarted || gameEnded)
    return;

  Worm &activeWorm = worms[currentPlayer];
  if (!activeWorm.isActive || !activeWorm.isMyTurn)
    return;

  if (keysPressed[sf::Keyboard::A] || keysPressed[sf::Keyboard::Left]) {
    activeWorm.move(-1.0f);
  }
  if (keysPressed[sf::Keyboard::D] || keysPressed[sf::Keyboard::Right]) {
    activeWorm.move(1.0f);
  }
}

void Game::updateAim() {
  if (!gameStarted || gameEnded || !worms[currentPlayer].isMyTurn)
    return;

  sf::Vector2i mousePos = sf::Mouse::getPosition(window);
  sf::Vector2f wormPos = worms[currentPlayer].getCenter();

  aimDirection = sf::Vector2f(mousePos.x - wormPos.x, mousePos.y - wormPos.y);
  float length = MathUtils::length(aimDirection);

  if (length > 0) {
    aimDirection = MathUtils::normalize(aimDirection);
    aimPower = std::min(length / 3.0f, 100.0f);
    calculateTrajectory();
  }
}

void Game::shoot() {
  if (!canShoot || !gameStarted || gameEnded)
    return;

  Worm &activeWorm = worms[currentPlayer];
  if (!activeWorm.isActive || !activeWorm.isMyTurn)
    return;

  float power = 400.0f + aimPower * 3.0f;
  sf::Vector2f spawnPos = activeWorm.getCenter() + aimDirection * 25.0f;

  projectiles.push_back(Projectile(spawnPos.x, spawnPos.y,
                                   aimDirection.x * power,
                                   aimDirection.y * power, activeWorm.teamId));

  canShoot = false;
  activeWorm.isMyTurn = false;
  switchToNextPlayer();
  trajectoryPoints.clear();
}

void Game::switchToNextPlayer() {
  int nextPlayer = currentPlayer;
  do {
    nextPlayer = (nextPlayer + 1) % worms.size();
  } while (!worms[nextPlayer].isActive && getActiveWormsCount() > 1);

  currentPlayer = nextPlayer;
  worms[currentPlayer].isMyTurn = true;
  canShoot = true;
  turnTimer = 0.0f;
}

int Game::getActiveWormsCount() {
  int count = 0;
  for (const auto &worm : worms) {
    if (worm.isActive)
      count++;
  }
  return count;
}

void Game::restartGame() {
  worms.clear();
  projectiles.clear();
  terrain = TerrainManager(GameTypes::WINDOW_WIDTH, GameTypes::WINDOW_HEIGHT);

  worms.push_back(Worm(150, 200, sf::Color::Green, 0));
  worms.push_back(Worm(650, 200, sf::Color::Blue, 1));

  for (auto &worm : worms) {
    int groundLevel =
        terrain.findGroundLevel(static_cast<int>(worm.position.x));
    worm.position.y = groundLevel - 20;
  }

  currentPlayer = 0;
  worms[currentPlayer].isMyTurn = true;
  gameStarted = true;
  gameEnded = false;
  winner = -1;
  canShoot = true;
  turnTimer = 0.0f;
}

void Game::update() {
  if (!gameStarted || gameEnded)
    return;

  float deltaTime = clock.restart().asSeconds();
  turnTimer += deltaTime;

  handleContinuousInput();

  for (auto &worm : worms) {
    worm.update(deltaTime, terrain);
  }

  for (auto &projectile : projectiles) {
    projectile.update(deltaTime, terrain);

    for (auto &worm : worms) {
      if (projectile.checkWormCollision(worm)) {
        sf::Vector2f explosionPos = projectile.getPosition();
        for (auto &targetWorm : worms) {
          float distanceLength =
              MathUtils::distance(explosionPos, targetWorm.getCenter());

          if (distanceLength < projectile.explosionRadius) {
            int damage = static_cast<int>(
                projectile.damage *
                (1.0f - distanceLength / projectile.explosionRadius));
            if (targetWorm.teamId == projectile.shooterTeam) {
              damage = damage / 3;
            }

            targetWorm.takeDamage(damage);

            if (distanceLength > 0) {
              sf::Vector2f knockback = (targetWorm.getCenter() - explosionPos);
              knockback = MathUtils::normalize(knockback);
              targetWorm.velocity += knockback * 150.0f;
            }
          }
        }

        projectile.explode(terrain);
        break;
      }
    }
  }

  projectiles.erase(
      std::remove_if(projectiles.begin(), projectiles.end(),
                     [](const Projectile &p) { return !p.isActive; }),
      projectiles.end());

  int activeCount = getActiveWormsCount();
  if (activeCount <= 1) {
    gameEnded = true;
    for (size_t i = 0; i < worms.size(); i++) {
      if (worms[i].isActive) {
        winner = i;
        break;
      }
    }
  }
}

void Game::render() {
  window.clear(sf::Color(135, 206, 235));

  terrain.draw(window);

  for (auto &worm : worms) {
    worm.draw(window);
  }

  for (auto &projectile : projectiles) {
    projectile.draw(window);
  }

  if (worms[currentPlayer].isMyTurn && !trajectoryPoints.empty()) {
    for (size_t i = 1; i < trajectoryPoints.size(); i++) {
      sf::CircleShape point(2);
      point.setPosition(trajectoryPoints[i].x - 2, trajectoryPoints[i].y - 2);
      sf::Color pointColor = sf::Color::White;
      pointColor.a = static_cast<sf::Uint8>(
          255 * (1.0f - static_cast<float>(i) / trajectoryPoints.size()));
      point.setFillColor(pointColor);
      window.draw(point);
    }
  }

  if (gameStarted && !gameEnded && worms[currentPlayer].isMyTurn) {
    sf::Vector2f wormCenter = worms[currentPlayer].getCenter();
    sf::Vector2f aimEnd = wormCenter + aimDirection * (50.0f + aimPower);

    sf::Vertex line[] = {sf::Vertex(wormCenter, sf::Color::White),
                         sf::Vertex(aimEnd, sf::Color::Red)};
    window.draw(line, 2, sf::Lines);

    sf::CircleShape powerIndicator(3 + aimPower / 10);
    powerIndicator.setPosition(aimEnd.x - powerIndicator.getRadius(),
                               aimEnd.y - powerIndicator.getRadius());
    powerIndicator.setFillColor(
        sf::Color(255, 255 - static_cast<int>(aimPower * 2.55f), 0));
    window.draw(powerIndicator);
  }

  if (gameEnded) {
    sf::RectangleShape overlay(
        sf::Vector2f(GameTypes::WINDOW_WIDTH, GameTypes::WINDOW_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);
  }

  window.display();
}

void Game::run() {
  while (window.isOpen()) {
    handleEvents();
    update();
    render();
  }
}
