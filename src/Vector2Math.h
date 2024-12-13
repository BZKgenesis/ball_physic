#pragma once

#include <SFML/System/Vector2.hpp>
#include <cmath>

namespace Vect2Maths {

    float length(const sf::Vector2<float>& vec);
    sf::Vector2<float> normalise(const sf::Vector2<float>& vec);

}