#include "ColorUtils.h"

namespace ColorUtils
{

    sf::Color HSVtoRGB(float hue, float saturation, float value) {
        float c = value * saturation;
        float x = c * (1 - std::fabs(fmod(hue / 60.0f, 2) - 1));
        float m = value - c;

        float r = 0, g = 0, b = 0;

        if (hue >= 0 && hue < 60) {
            r = c; g = x; b = 0;
        }
        else if (hue >= 60 && hue < 120) {
            r = x; g = c; b = 0;
        }
        else if (hue >= 120 && hue < 180) {
            r = 0; g = c; b = x;
        }
        else if (hue >= 180 && hue < 240) {
            r = 0; g = x; b = c;
        }
        else if (hue >= 240 && hue < 300) {
            r = x; g = 0; b = c;
        }
        else if (hue >= 300 && hue < 360) {
            r = c; g = 0; b = x;
        }

        sf::Uint8 R = static_cast<sf::Uint8>((r + m) * 255);
        sf::Uint8 G = static_cast<sf::Uint8>((g + m) * 255);
        sf::Uint8 B = static_cast<sf::Uint8>((b + m) * 255);

        return sf::Color(R, G, B);
    }

    // Fonction pour convertir un angle en une couleur
    sf::Color angleToColor(float angle) {
        angle = fmod(angle, 360.0f); // S'assurer que l'angle est dans [0, 360)
        if (angle < 0) angle += 360.0f;

        float hue = angle;          // La teinte dépend de l'angle (0-360)
        float saturation = 1.0f;    // Saturation maximale
        float value = 1.0f;         // Valeur maximale (brillance)

        return HSVtoRGB(hue, saturation, value);
    }
    sf::Color lerp(sf::Color a, sf::Color b, float t) {
        return sf::Color(a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t);
    }
}