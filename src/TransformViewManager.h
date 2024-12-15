#pragma once
#include <SFML/Graphics.hpp>

namespace TransformViewManager {

    void TransformView(bool& moving, sf::Vector2f& oldPos, float& zoom, sf::View& view, sf::RenderWindow& window, sf::Event event, int W, int H);

}