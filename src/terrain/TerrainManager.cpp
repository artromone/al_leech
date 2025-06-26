#include "TerrainManager.hpp"
#include "../utils/GameTypes.hpp"
#include <cmath>
#include <random>

TerrainManager::TerrainManager(int w, int h) : width(w), height(h) {
  terrain.resize(height, std::vector<bool>(width, false));
  terrainImage.create(width, height, sf::Color::Transparent);
  generateTerrain();
  updateTexture();
}

void TerrainManager::generateTerrain() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.0, 1.0);

  // Создаем базовый ландшафт
  for (int x = 0; x < width; x++) {
    int groundHeight = height - 120 + static_cast<int>(40 * sin(x * 0.008));
    for (int y = groundHeight; y < height; y++) {
      terrain[y][x] = true;
      terrainImage.setPixel(x, y, sf::Color(139, 69, 19));
    }
  }

  // Добавляем холмы и препятствия
  for (int i = 0; i < 20; i++) {
    int centerX = static_cast<int>(dis(gen) * width);
    int centerY = static_cast<int>(dis(gen) * (height - 250)) + 100;
    int radius = 15 + static_cast<int>(dis(gen) * 35);

    for (int x = centerX - radius; x <= centerX + radius; x++) {
      for (int y = centerY - radius; y <= centerY + radius; y++) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
          float distance = sqrt((x - centerX) * (x - centerX) +
                                (y - centerY) * (y - centerY));
          if (distance <= radius) {
            terrain[y][x] = true;
            terrainImage.setPixel(x, y, sf::Color(139, 69, 19));
          }
        }
      }
    }
  }
}

void TerrainManager::destroyTerrain(int centerX, int centerY, int radius) {
  for (int x = centerX - radius; x <= centerX + radius; x++) {
    for (int y = centerY - radius; y <= centerY + radius; y++) {
      if (x >= 0 && x < width && y >= 0 && y < height) {
        float distance =
            sqrt((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY));
        if (distance <= radius) {
          terrain[y][x] = false;
          terrainImage.setPixel(x, y, sf::Color::Transparent);
        }
      }
    }
  }
  updateTexture();
}

void TerrainManager::updateTexture() {
  terrainTexture.loadFromImage(terrainImage);
  terrainSprite.setTexture(terrainTexture);
}

bool TerrainManager::isColliding(int x, int y) const {
  if (x < 0 || x >= width || y < 0 || y >= height)
    return true;
  return terrain[y][x];
}

bool TerrainManager::isColliding(sf::Vector2f pos, int radius) const {
  for (int dx = -radius; dx <= radius; dx++) {
    for (int dy = -radius; dy <= radius; dy++) {
      if (dx * dx + dy * dy <= radius * radius) {
        if (isColliding(static_cast<int>(pos.x) + dx,
                        static_cast<int>(pos.y) + dy)) {
          return true;
        }
      }
    }
  }
  return false;
}

int TerrainManager::findGroundLevel(int x) const {
  for (int y = 0; y < height; y++) {
    if (terrain[y][x])
      return y;
  }
  return height;
}

void TerrainManager::draw(sf::RenderWindow &window) {
  window.draw(terrainSprite);
}
