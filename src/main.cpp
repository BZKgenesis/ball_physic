#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include "Vector2Math.h"
#include "TransformViewManager.h"
#include "ColorUtils.h"

int H = 720;
int W = 1280;

//gravité
const float g = 9.807f;

const int margin = 1;

// zoom et déplacement
float zoom = 1.0f;
sf::Vector2f offset(0, 0);
sf::Vector2f oldPos;
bool moving = false;
sf::View view;


//taux de rebond
float damping = 0.5f;
//calculates dot product of 2 vectors
sf::Color colorMax = sf::Color(255, 20, 20);
sf::Color colorMin = sf::Color(20, 20, 255);


struct element {
    element* child1 = nullptr;
    element* child2 = nullptr;
    element* child3 = nullptr;
    element* child4 = nullptr;
    sf::RectangleShape rect;
};

std::string to_string_custom(double value) {
    std::ostringstream oss;
    oss.precision(16);  // Définir la précision souhaitée (par exemple 16 chiffres significatifs)
    oss << std::noshowpoint << value;  // Supprime l'affichage des zéros inutiles
    return oss.str();
}



//Returns length of a given vector
class Particle {
public:
    bool visible = true;
    sf::Vector2f pos;
    sf::Vector2f old_pos = pos;
    float size;
    sf::Vector2f acc;
    sf::Color color;
    Particle(sf::Vector2f position, sf::Vector2f velocity, float size, sf::Color color) : pos(position), acc(velocity), size(size), color(color) {};


    void draw(sf::RenderWindow& win) {
        sf::CircleShape circle = sf::CircleShape(size);
        circle.setPosition(pos - sf::Vector2f(size, size));
        circle.setFillColor(color);
        circle.setOutlineColor(color);
        win.draw(circle);
    }

    void update(float dt) {
        sf::Vector2f vel(pos - old_pos);
        old_pos = pos;
        if (dt > 0.1f) {
            dt = 0.1f;
        }
        if (Vect2Maths::length(vel) > 10) {
            vel = sf::Vector2f(vel.x * 10 / Vect2Maths::length(vel), vel.y * 10 / Vect2Maths::length(vel));
        }
        pos = pos + vel + acc * dt;
        acc.x = 0;
        acc.y = 0;
    }
};

std::vector<Particle> particules;


void processEvtCollisionWindow(Particle& part) {
    if (part.pos.x < part.size + margin) {
        part.pos.x = part.size + margin;
    }if (part.pos.x > W - (part.size + margin)) {
        part.pos.x = W - (part.size + margin);
    }
    if (part.pos.y < part.size + margin) {
        part.pos.y = part.size + margin;
    }if (part.pos.y > H - (part.size + margin)) {
        part.pos.y = H - (part.size + margin);
    }
}

void processEvtCollisionCirlce(Particle& part, float radius) {
    if ( Vect2Maths::length(part.pos - sf::Vector2f(W / 2, H / 2)) > (radius-part.size)) {
        part.pos -= Vect2Maths::normalise((part.pos) - sf::Vector2f(W / 2, H / 2)) * (Vect2Maths::length((part.pos) - sf::Vector2f(W / 2, H / 2))- (radius - part.size));
    }
}




int main()
{
    sf::RenderWindow window(sf::VideoMode(W, H), "Physic");
    window.setFramerateLimit(120);
    sf::Clock deltaClock;

	view = window.getDefaultView();

	bool fullscreen = false;

	double time = 0;
	double lastSpawn = 0;
	float spawnRate = 10.0f;
	float speed = 1.0f;

	int substep = 1;


    int nbParticule = 0;
    int nbParticule_MAX = 1000;
    int count = 0;
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("res/sound.wav"))
        std::cout << "test" << std::endl;
    sf::Sound sound;
    sound.setBuffer(buffer);
    //sound.play();

    sf::Font font;
    if (!font.loadFromFile("res/arial.ttf"))
    {
        std::cout << "error" << std::endl;
    }

	sf::Text text = sf::Text("test", font, 24);
	text.setFillColor(sf::Color::White);

    float mvtMax = 0;

    int fps_values[200]{};

	float spawnVelocity = 750.f;
	float spawnAngle = 3.1415/2.0f;
	float spawnAngleSpeed = 0.01f;
	float spawnAngleWidth = 1.f;

	bool spawning = true;

    while (window.isOpen())
    {
        sf::Time true_dt = deltaClock.restart();//récup valeur du timer + redémmarer (fps)
        sf::Time dt = true_dt * speed;
        float deltaT = dt.asSeconds();
        float fps = 1.0f / true_dt.asSeconds();
        for (int i = 0; i < 199; i++) {
            fps_values[i] = fps_values[i + 1];
        }
        fps_values[199] = fps;
        float fps_moy = 0;
        for (int i = 0; i < 200; i++) {
            fps_moy += fps_values[i] / 200.0f;
        }
		time += deltaT;
        sf::Event event;
        while (window.pollEvent(event))
        {
			TransformViewManager::TransformView(moving, oldPos, zoom, view, window, event,W,H);
            switch (event.type){
                case sf::Event::Closed:
                    window.close(); //propulsion autour de la souris when press space
                    break;
                case sf::Event::KeyPressed:
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                        for (int i = 0; i < particules.size(); i++) {
                            int influence = 200;
                            sf::Vector2f force(0, 0);
                            sf::Vector2f reltaivePos(particules[i].pos - (sf::Vector2f)window.mapPixelToCoords(sf::Mouse::getPosition(window)));
                            force = Vect2Maths::normalise(reltaivePos) * (float)(influence - std::fmin(Vect2Maths::length(reltaivePos), influence)) * 10.0f;
                            particules[i].acc += force;
                        }
                    }
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
						particules.clear();
						nbParticule = 0;
					}
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F11)) {
						if (fullscreen) {
							fullscreen = false;
							W = 1280;
							H = 720;
							window.create(sf::VideoMode(W, H), "Physic");
                            window.setFramerateLimit(120);
                            view.setSize(W, H);
                            view.zoom(zoom);
                            window.setView(view);
						}
						else {
							fullscreen = true;
							W = sf::VideoMode::getFullscreenModes()[0].width;
							H = sf::VideoMode::getFullscreenModes()[0].height;
						    window.create(sf::VideoMode::getFullscreenModes()[0], "Physic", sf::Style::Fullscreen);
                            window.setFramerateLimit(120);
                            view.setSize(W, H);
                            view.zoom(zoom);
                            window.setView(view);
						}
                    }
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
						window.close();
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add)) {
						speed *= 1.1f;
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract)) {
						speed *= 0.9f;
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad0)) {
						speed = 1.f;
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Multiply)) {
                        if (spawning) {
							spawning = false;
						}
                        else {
							spawning = true;
						}
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1)) {
                        substep = std::min(substep+1,16);
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2)) {
						substep = std::max(substep - 1, 1);
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
						for (int i = 0; i < particules.size(); i++) {
							sf::Vector2i pixelPos = window.mapCoordsToPixel(particules[i].pos);
							particules[i].color = sf::Color( (pixelPos.x * 255) / W, (pixelPos.y * 255) / H, 0);
						}
					}
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
                        for (int i = 0; i < particules.size(); i++) {
                            particules[i].color = ColorUtils::angleToColor(((float)i / (float)nbParticule_MAX) * 360.f);
                        }
                    }
					break;
                case sf::Event::Resized:
                {
                    // update the view to the new size of the window
					view.setSize(event.size.width, event.size.height);
					view.zoom(zoom);
                    //sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(view);
				    H = event.size.height;
				    W = event.size.width;
                    break;
                }
                
            }
        }
        count++;
        if (spawning && nbParticule < nbParticule_MAX && time - lastSpawn > (1/spawnRate)) {
            //int size = 3;
			lastSpawn = time;
            int size = rand() % 10 + 7;
            sf::Vector2f pos(W / 2 - size, H / 2 - size);
            particules.push_back(Particle(pos, sf::Vector2f(cos(cos(count * spawnAngleSpeed)* spawnAngleWidth + spawnAngle)* spawnVelocity, sin(cos(count * spawnAngleSpeed)* spawnAngleWidth + spawnAngle)* spawnVelocity), size, ColorUtils::angleToColor(((float)nbParticule /(float)nbParticule_MAX)*360.f)));
            nbParticule++;
        }
        window.setTitle("Physic - " + std::to_string(particules.size()) + " - FPS:" + std::to_string(round(fps_moy)));
        window.clear();
        const float radius = 500;
        sf::CircleShape circle = sf::CircleShape(radius+5, 64);
        circle.setPosition(sf::Vector2f(W / 2, H / 2) - sf::Vector2f(radius+5, radius+5));
        circle.setFillColor(sf::Color(50,50,50));
        window.draw(circle);
        //collisions
        for (int i = 0; i < particules.size(); i++) {
            processEvtCollisionCirlce(particules[i], radius);
            particules[i].acc += sf::Vector2f(0, g);
            for (int m = 0; m < substep; m++) {
                for (int j = 0; j < nbParticule; j++) {
                    if (i != j) {
                        if (Vect2Maths::length(particules[i].pos - particules[j].pos) < (particules[i].size + particules[j].size)) {
                            sf::Vector2f mvt(Vect2Maths::normalise(particules[i].pos - particules[j].pos) * ((particules[i].size + particules[j].size) - Vect2Maths::length(particules[i].pos - particules[j].pos)) / 2.0f);
							if (Vect2Maths::length(mvt) > 2) {
                                mvt = Vect2Maths::normalise(mvt) * 2.0f;
							}
                            particules[i].pos += mvt*0.75f;
                            particules[j].pos += -mvt*0.75f;
                        }
                    }
                }
            }

            particules[i].update(deltaT);

            particules[i].draw(window);
        }
        text.setString("Particules: " + std::to_string(nbParticule) + "/" + std::to_string(nbParticule_MAX) + "\nFPS: " + to_string_custom(std::round(fps_moy)) + "\nSpeed: " + std::to_string(speed));
		text.setPosition(-100, -100);
		window.draw(text);
        window.display();
    }
}