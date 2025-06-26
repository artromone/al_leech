#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class TerrainManager {
private:
  std::vector<std::vector<bool>> terrain;
  int width, height;
  sf::Image terrainImage;
  sf::Texture terrainTexture;
  sf::Sprite terrainSprite;

public:
  TerrainManager(int w, int h);

  void generateTerrain();
  void destroyTerrain(int centerX, int centerY, int radius);
  void updateTexture();

  bool isColliding(int x, int y) const;
  bool isColliding(sf::Vector2f pos, int radius = 15) const;
  int findGroundLevel(int x) const;

  void draw(sf::RenderWindow &window);
};
