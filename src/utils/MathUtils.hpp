#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

namespace MathUtils {
inline float distance(const sf::Vector2f &a, const sf::Vector2f &b) {
  sf::Vector2f diff = a - b;
  return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

inline sf::Vector2f normalize(const sf::Vector2f &vector) {
  float length = std::sqrt(vector.x * vector.x + vector.y * vector.y);
  if (length == 0)
    return sf::Vector2f(0, 0);
  return vector / length;
}

inline float length(const sf::Vector2f &vector) {
  return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}
} // namespace MathUtils
