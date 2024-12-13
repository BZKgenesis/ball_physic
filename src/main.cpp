#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <algorithm>
#include "Vector2Math.h"
#include "TransformViewManager.h"

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

//Returns length of a given vector
class Particle {
public:
    bool visible = true;
    sf::Vector2f pos;
    sf::Vector2f old_pos = pos;
    float size;
    sf::Vector2f acc;
    sf::Color color = sf::Color(255, 255, 255);
    Particle(sf::Vector2f position, sf::Vector2f velocity, float size) : pos(position), acc(velocity), size(size) {};


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



sf::Color lerp(sf::Color a, sf::Color b, float t) {
    return sf::Color(a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t);
}


int main()
{
    sf::RenderWindow window(sf::VideoMode(W, H), "Physic", sf::Style::Close);
    window.setFramerateLimit(120);
    sf::Clock deltaClock;

	view = window.getDefaultView();


    int nbParticule = 0;
    int nbParticule_MAX = 1500;
    int count = 0;
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("res/sound.wav"))
        std::cout << "test" << std::endl;
    sf::Sound sound;
    sound.setBuffer(buffer);
    //sound.play();

    float mvtMax = 0;

    int fps_values[200];

    for (int i = 0; i < 200; i++) {
        fps_values[i] = 0;
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
			TransformViewManager::TransformView(moving, oldPos, zoom, view, window, event);
            switch (event.type){
                case sf::Event::Closed:
                    window.close(); //propulsion autour de la souris when press space
                    break;
                case sf::Event::KeyPressed:
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                        for (int i = 0; i < particules.size(); i++) {
                            int influence = 100;
                            sf::Vector2f force(0, 0);
                            sf::Vector2f reltaivePos(particules[i].pos - (sf::Vector2f)window.mapPixelToCoords(sf::Mouse::getPosition(window)));
                            force = Vect2Maths::normalise(reltaivePos) * (float)(influence - std::fmin(Vect2Maths::length(reltaivePos), influence)) * 10.0f;
                            particules[i].acc += force;
                        }
                    }
					break;
                case sf::Event::Resized:
                {
                    // update the view to the new size of the window
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
				    H = event.size.height;
				    W = event.size.width;
                    break;
                }
                
            }
        }
        count++;
        if (nbParticule < nbParticule_MAX && count % 2 == 0) {
            //int size = 3;
            int size = rand() % 3 + 5;
            sf::Vector2f pos(W / 2 - size, H / 2 - size);
            particules.push_back(Particle(pos, sf::Vector2f(500, 0), size));
            nbParticule++;
            pos.x = W / 2 - size;
            pos.y = H / 2 - size + 150;
            particules.push_back(Particle(pos, sf::Vector2f(500, 0), size));
            nbParticule++;
        }
        sf::Time dt = deltaClock.restart();//récup valeur du timer + redémmarer (fps)
        float deltaT = dt.asSeconds();
        float fps = 1.0f / deltaT;
        for (int i = 0; i < 199; i++) {
            fps_values[i] = fps_values[i + 1];
        }
        fps_values[199] = fps;
        float fps_moy = 0;
        for (int i = 0; i < 200; i++) {
            fps_moy += fps_values[i] / 200.0f;
        }
        window.setTitle("Physic - " + std::to_string(particules.size()) + " - FPS:" + std::to_string(round(fps_moy)));
        window.clear();
        //collisions
        for (int i = 0; i < particules.size(); i++) {
            if (particules[i].pos.x < particules[i].size + margin) {
                particules[i].pos.x = particules[i].size + margin;
            }if (particules[i].pos.x > W - (particules[i].size + margin)) {
                particules[i].pos.x = W - (particules[i].size + margin);
            }
            if (particules[i].pos.y < particules[i].size + margin) {
                particules[i].pos.y = particules[i].size + margin;
            }if (particules[i].pos.y > H - (particules[i].size + margin)) {
                particules[i].pos.y = H - (particules[i].size + margin);
            }
            particules[i].acc += sf::Vector2f(0, g);
            for (int m = 0; m < 2; m++) {
                for (int j = 0; j < nbParticule; j++) {
                    if (i != j) {
                        if (Vect2Maths::length(particules[i].pos - particules[j].pos) < (particules[i].size + particules[j].size)) {
                            sf::Vector2f mvt(Vect2Maths::normalise(particules[i].pos - particules[j].pos) * ((particules[i].size + particules[j].size) - Vect2Maths::length(particules[i].pos - particules[j].pos)) / 2.0f);
							if (Vect2Maths::length(mvt) > 2) {
                                mvt = Vect2Maths::normalise(mvt) * 2.0f;
							}
                            particules[i].pos += mvt*0.75f;
                            particules[j].pos += -mvt*0.75f;
                            float lengthMvt = Vect2Maths::length(mvt);
                            particules[i].color = lerp(colorMin, colorMax, lengthMvt);
                            particules[j].color = lerp(colorMin, colorMax, lengthMvt);
                        }
                    }
                }
            }

            particules[i].update(deltaT);

            particules[i].draw(window);
        }
        window.display();
    }
}