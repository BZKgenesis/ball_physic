#include "TransformViewManager.h"

namespace TransformViewManager
{
    void TransformView(bool& moving, sf::Vector2f& oldPos, float& zoom, sf::View& view, sf::RenderWindow& window, sf::Event event, int W, int H) {
        switch (event.type) {
            case sf::Event::MouseButtonPressed:
            {
                // Mouse button is pressed, get the position and set moving as active
                if (event.mouseButton.button == 2) {
                    moving = true;
                    oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                }
                break;
            }
            case  sf::Event::MouseButtonReleased:
            {
                // Mouse button is released, no longer move
                if (event.mouseButton.button == 2) {
                    moving = false;
                }
                break;
            }
            case sf::Event::MouseMoved:
            {
                // Ignore mouse movement unless a button is pressed (see above)
                if (!moving)
                    break;
                // Determine the new position in world coordinates
                const sf::Vector2f newPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                // Determine how the cursor has moved
                // Swap these to invert the movement direction
                const sf::Vector2f deltaPos = oldPos - newPos;

                // Move our view accordingly and update the window
                view.setCenter(view.getCenter() + deltaPos);
                window.setView(view);

                // Save the new position as the old one
                // We're recalculating this, since we've changed the view
                oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                break;
            }
            case sf::Event::MouseWheelScrolled:
                // Ignore the mouse wheel unless we're not moving
                if (moving)
                    break;

                // Determine the scroll direction and adjust the zoom level
                // Again, you can swap these to invert the direction
                if (event.mouseWheelScroll.delta <= -1)
                    zoom = std::min(3.f, zoom + .1f);
                else if (event.mouseWheelScroll.delta >= 1)
                    zoom = std::max(.25f, zoom - .1f);

                // Update our view
                view.setSize(W,H); // Reset the size
                view.zoom(zoom); // Apply the zoom level (this transforms the view)
                window.setView(view);
                break;
        }
    }
}