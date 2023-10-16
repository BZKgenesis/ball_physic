#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include<algorithm>

const int H = 720;
const int W = 1280;

//gravité
const float g = 9.807f;

//taux de rebond
float damping = 0.5f;

class Particle {
public:
    bool visible = true;
    sf::Vector2f pos;
    sf::Vector2f old_pos = pos;
    float size;
    sf::Vector2f acc;
    sf::Color color = sf::Color(255,255,255);
    Particle(sf::Vector2f position, sf::Vector2f velocity, float size) : pos(position),acc(velocity), size(size) {};


    void draw(sf::RenderWindow& win) {
        sf::CircleShape circle = sf::CircleShape(size);
        circle.setPosition(pos - sf::Vector2f(size, size));
        circle.setFillColor(color);
        circle.setOutlineColor(color);
        win.draw(circle);
    }

    void update(float dt) {
        const sf::Vector2f vel(pos - old_pos);
        old_pos = pos;
        if (dt > 0.1f) {
            dt = 0.1f;
        }
        pos = pos + vel + acc * dt;
        acc.x = 0;
        acc.y = 0;
    }
};

inline sf::Vector2f invert(const sf::Vector2f& source)
{
    float X = source.x;
    float Y = source.y;
    if (X < 0.001f && X > 0) {
        X = 0.001f;
    }
    else if (X > -0.001f && X < 0) {
        X = -0.001f;
    }
    if (Y < 0.001f && Y > 0) {
        Y = 0.001f;
    }
    else if (Y > -0.001f && Y < 0) {
        Y = -0.001f;
    }
    return sf::Vector2f(1/X, 1/Y);
}

//calculates dot product of 2 vectors
inline float dot(const sf::Vector2f& lv, const sf::Vector2f& rv)
{
    return lv.x * rv.x + lv.y * rv.y;
}

//Returns length of a given vector
inline float length(const sf::Vector2f& source)
{
    return std::sqrt(dot(source, source));
}

inline sf::Vector2f normalise(sf::Vector2f source)
{
    float length = std::sqrt(dot(source, source));
    if (length != 0) source /= length;
    return source;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(W, H), "Physic");
    window.setFramerateLimit(60);
    sf::Clock deltaClock;

    std::vector<Particle> particules;

    int nbParticule = 0;
    int nbParticule_MAX = 1500;
    int count = 0;
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("sound.wav"))
        std::cout << "test" << std::endl;
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();

    


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            //propulsion autour de la souris when press space
            if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                for (int i = 0; i < nbParticule; i++) {
                    int influence = 100;
                    sf::Vector2f force(0, 0);
                    sf::Vector2f reltaivePos(particules[i].pos - (sf::Vector2f)sf::Mouse::getPosition(window));
                    force = normalise(reltaivePos) * (float)(influence - std::fmin(length(reltaivePos), influence))*10.0f;
                    particules[i].acc += force;

                }
            }

        }
        count++;
        if (nbParticule < nbParticule_MAX && count % 1 ==0) {
            int size = 2;
            sf::Vector2f pos(W/2 - size, H/2 - size);
            particules.push_back(Particle(pos, sf::Vector2f(500, 0), size));
            nbParticule++;
            //std::cout << nbParticule << " / " << nbParticule_MAX << std::endl;
            pos.x = W / 2 - size;
            pos.y = H / 2 - size - 10;
            particules.push_back(Particle(pos, sf::Vector2f(500, 0), size));
            nbParticule++;
            //std::cout << nbParticule << " / " << nbParticule_MAX << std::endl;
        }
        sf::Time dt = deltaClock.restart();//récup valeur du timer + redémmarer (fps)
        float deltaT = dt.asSeconds();
        window.clear();
        //collisions
        for (int i = 0; i < nbParticule; i++) {
            /*if ((particules[i].pos.y > H - particules[i].size * 2 && particules[i].vel.y > 0) || (particules[i].pos.y < particules[i].size * 2 && particules[i].vel.y < 0)) {
                particules[i].vel.y = -particules[i].vel.y * damping;
                particules[i].vel.x *= damping;
            }
            if ((particules[i].pos.x > W - particules[i].size * 2 && particules[i].vel.x > 0) || (particules[i].pos.x < particules[i].size * 2 && particules[i].vel.x < 0)) {
                particules[i].vel.x = -particules[i].vel.x * damping;
            }*/
            if (particules[i].pos.x < particules[i].size) {
                particules[i].pos.x = particules[i].size;
            }if (particules[i].pos.x > W - particules[i].size) {
                particules[i].pos.x = W - particules[i].size;
            }
            if (particules[i].pos.y < particules[i].size) {
                particules[i].pos.y = particules[i].size;
            }if (particules[i].pos.y > H - particules[i].size) {
                particules[i].pos.y = H - particules[i].size;
            }
            particules[i].acc += sf::Vector2f(0, g);
            for (int m = 0; m < 8; m++) {
                for (int j = 0; j < nbParticule; j++) {
                    if (i != j) {
                        if (length(particules[i].pos - particules[j].pos) < (particules[i].size + particules[j].size)) {
                            sf::Vector2f mvt(normalise(particules[i].pos - particules[j].pos) * (-(length(particules[i].pos - particules[j].pos) - (particules[i].size + particules[j].size))) / 2.0f);
                            particules[i].pos += mvt;
                            particules[j].pos += -mvt;
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