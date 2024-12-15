#pragma once
#include <SFML/Graphics/Color.hpp>
#include <cmath>

namespace ColorUtils
{
	sf::Color HSVtoRGB(float hue, float saturation, float value);

	sf::Color angleToColor(float angle);

	sf::Color lerp(sf::Color a, sf::Color b, float t);
}