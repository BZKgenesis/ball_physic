#include "Vector2Math.h"
#include <cmath>

namespace Vect2Maths {

    float length(const sf::Vector2<float>& vec) {
        return std::sqrt(vec.x * vec.x + vec.y * vec.y);
    }

    sf::Vector2<float> normalise(const sf::Vector2<float>& vec) {
        float len = length(vec);
        if (len == 0) {
            return sf::Vector2<float>(0, 0);
        }
        return sf::Vector2<float>(vec.x / len, vec.y / len);
    }

}