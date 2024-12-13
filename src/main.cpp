#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include<algorithm>

int H = 720;
int W = 1280;

//gravité
const float g = 9.807f;

const int margin = 1;

float zoom = 1.0f;
sf::Vector2f offset(0, 0);

//taux de rebond
float damping = 0.5f;
//calculates dot product of 2 vectors
sf::Color colorMax = sf::Color(255, 20, 20);
sf::Color colorMin = sf::Color(20, 20, 255);

inline float dot(const sf::Vector2f& lv, const sf::Vector2f& rv)
{
    return lv.x * rv.x + lv.y * rv.y;
}

struct element {
    element* child1 = nullptr;
    element* child2 = nullptr;
    element* child3 = nullptr;
    element* child4 = nullptr;
    sf::RectangleShape rect;
};

//Returns length of a given vector
inline float length(const sf::Vector2f& source)
{
    return std::sqrt(dot(source, source));
}
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
        if (length(vel) > 10) {
            vel = sf::Vector2f(vel.x * 10 / length(vel), vel.y * 10 / length(vel));
        }
        pos = pos + vel + acc * dt;
        acc.x = 0;
        acc.y = 0;
    }
};

std::vector<Particle> particulesOrg[10][10];
std::vector<Particle> particules;

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
    return sf::Vector2f(1 / X, 1 / Y);
}



inline sf::Vector2f normalise(sf::Vector2f source)
{
    float length = std::sqrt(dot(source, source));
    if (length != 0) source /= length;
    return source;
}

void updateTree(element* root, std::vector<Particle> particules, int nbParticule) {
    int count = 0;
    if (root->child1 == nullptr) {
        for (int i = 0; i < nbParticule; i++) {
            if (particules[i].pos.x > root->rect.getPosition().x && particules[i].pos.x < root->rect.getPosition().x + root->rect.getSize().x && particules[i].pos.y > root->rect.getPosition().y && particules[i].pos.y < root->rect.getPosition().y + root->rect.getSize().y) {
                count++;
            }
        }
        if (count > 10) {
            root->child1 = new element;
            root->child1->child1 = nullptr;
            root->child1->child2 = nullptr;
            root->child1->child3 = nullptr;
            root->child1->child4 = nullptr;
            root->child2 = new element;
            root->child2->child1 = nullptr;
            root->child2->child2 = nullptr;
            root->child2->child3 = nullptr;
            root->child2->child4 = nullptr;
            root->child3 = new element;
            root->child3->child1 = nullptr;
            root->child3->child2 = nullptr;
            root->child3->child3 = nullptr;
            root->child3->child4 = nullptr;
            root->child4 = new element;
            root->child4->child1 = nullptr;
            root->child4->child2 = nullptr;
            root->child4->child3 = nullptr;
            root->child4->child4 = nullptr;
            root->child1->rect.setSize(sf::Vector2f(root->rect.getSize().x / 2, root->rect.getSize().y / 2));
            root->child1->rect.setPosition(root->rect.getPosition());
            root->child2->rect.setSize(sf::Vector2f(root->rect.getSize().x / 2, root->rect.getSize().y / 2));
            root->child2->rect.setPosition(root->rect.getPosition().x + root->rect.getSize().x / 2, root->rect.getPosition().y);
            root->child3->rect.setSize(sf::Vector2f(root->rect.getSize().x / 2, root->rect.getSize().y / 2));
            root->child3->rect.setPosition(root->rect.getPosition().x, root->rect.getPosition().y + root->rect.getSize().y / 2);
            root->child4->rect.setSize(sf::Vector2f(root->rect.getSize().x / 2, root->rect.getSize().y / 2));
            root->child4->rect.setPosition(root->rect.getPosition().x + root->rect.getSize().x / 2, root->rect.getPosition().y + root->rect.getSize().y / 2);
            updateTree(root->child1, particules, nbParticule);
            updateTree(root->child2, particules, nbParticule);
            updateTree(root->child3, particules, nbParticule);
            updateTree(root->child4, particules, nbParticule);
        }
    }

}

void drawTree(element* root, sf::RenderWindow& win) {
    if (root->child1 == nullptr) {
        root->rect.setFillColor(sf::Color(0, 0, 0, 0));
        root->rect.setOutlineColor(sf::Color(255, 255, 255));
        root->rect.setOutlineThickness(1);
        win.draw(root->rect);
    }
    else {
        drawTree(root->child1, win);
        drawTree(root->child2, win);
        drawTree(root->child3, win);
        drawTree(root->child4, win);
    }
}

void deleteTree(element* root) {
    std::cout << "delete" << std::endl;
    if (root == nullptr) {
        return;
    }
    if (root->child1 != nullptr) {
        std::cout << "deleteChild" << std::endl;
        deleteTree(root->child1);
        deleteTree(root->child2);
        deleteTree(root->child3);
        deleteTree(root->child4);
    }
    std::cout << "deleteRoot" << std::endl;
    delete root;
}


sf::Color lerp(sf::Color a, sf::Color b, float t) {
    return sf::Color(a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t);
}


void updateParticules(std::vector<Particle> (& particulesOrg)[10][10], std::vector<Particle>& particules, int nbParticule) {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			particulesOrg[i][j].clear();
		}
	}
	for (int i = 0; i < particules.size(); i++) {
		int x = particules[i].pos.x / (W / 10);
		int y = particules[i].pos.y / (H / 10);
		//std::cout << x << " " << y << std::endl;
		particulesOrg[x][y].push_back(particules[i]);
	}

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
			for (int k = 0; k < particulesOrg[i][j].size(); k++) {
				particulesOrg[i][j][k].acc += sf::Vector2f(0, g);
                if (particulesOrg[i][j][k].pos.x < particulesOrg[i][j][k].size + margin) {
                    particulesOrg[i][j][k].pos.x = particulesOrg[i][j][k].size + margin;
                }if (particulesOrg[i][j][k].pos.x > W - (particulesOrg[i][j][k].size + margin)) {
                    particulesOrg[i][j][k].pos.x = W - (particulesOrg[i][j][k].size + margin);
                }
                if (particulesOrg[i][j][k].pos.y < particulesOrg[i][j][k].size + margin) {
                    particulesOrg[i][j][k].pos.y = particulesOrg[i][j][k].size + margin;
                }if (particulesOrg[i][j][k].pos.y > H - (particulesOrg[i][j][k].size + margin)) {
                    particulesOrg[i][j][k].pos.y = H - (particulesOrg[i][j][k].size + margin);
                }

			}


			for (int k = i - 1; k < i + 2; k++) {
                for (int l = j - 1; l < j + 2; l++) {
                    if (k >= 0 && k < 10 && l >= 0 && l < 10) {
                        for (int m = 0; m < particulesOrg[k][l].size(); m++) {
                            for (int n = 0; n < particulesOrg[i][j].size(); n++) {
                                if (particulesOrg[i][j][n].pos != particulesOrg[k][l][m].pos) {
                                    if (length(particulesOrg[i][j][n].pos - particulesOrg[k][l][m].pos) < (particulesOrg[i][j][n].size + particulesOrg[k][l][m].size)) {
										sf::Vector2f mvt(normalise(particulesOrg[i][j][n].pos - particulesOrg[k][l][m].pos) * ((particulesOrg[i][j][n].size + particulesOrg[k][l][m].size) - length(particulesOrg[i][j][n].pos - particulesOrg[k][l][m].pos)) / 2.0f);
										particulesOrg[i][j][n].pos += mvt;
										particulesOrg[k][l][m].pos += -mvt;
										float lengthMvt = length(mvt);
										//particulesOrg[i][j][n].color = lerp(colorMin, colorMax, lengthMvt);
										//particulesOrg[k][l][m].color = lerp(colorMin, colorMax, lengthMvt);
										particulesOrg[i][j][n].color = sf::Color(i*25, j*25, 0);
										particulesOrg[k][l][m].color = sf::Color(k*25, l*25, 0);
									}
								}
							}
						}
					}
				}
			}   
        }
    }

	particules.clear();
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			for (int k = 0; k < particulesOrg[i][j].size(); k++) {
				particules.push_back(particulesOrg[i][j][k]);
			}
		}
	}
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(W, H), "Physic", sf::Style::Close);
    window.setFramerateLimit(120);
    sf::Clock deltaClock;
    sf::Vector2f oldPos;
    bool moving = false;

    float zoom = 1;

    // Retrieve the window's default view
    sf::View view = window.getDefaultView();

    //std::vector<Particle> particules;


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
                            force = normalise(reltaivePos) * (float)(influence - std::fmin(length(reltaivePos), influence)) * 10.0f;
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
                        zoom = std::min(2.f, zoom + .1f);
                    else if (event.mouseWheelScroll.delta >= 1)
                        zoom = std::max(.5f, zoom - .1f);

                    // Update our view
                    view.setSize(window.getDefaultView().getSize()); // Reset the size
                    view.zoom(zoom); // Apply the zoom level (this transforms the view)
                    window.setView(view);
                    break;
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
            /*if ((particules[i].pos.y > H - particules[i].size * 2 && particules[i].vel.y > 0) || (particules[i].pos.y < particules[i].size * 2 && particules[i].vel.y < 0)) {
                particules[i].vel.y = -particules[i].vel.y * damping;
                particules[i].vel.x *= damping;
            }
            if ((particules[i].pos.x > W - particules[i].size * 2 && particules[i].vel.x > 0) || (particules[i].pos.x < particules[i].size * 2 && particules[i].vel.x < 0)) {
                particules[i].vel.x = -particules[i].vel.x * damping;
            }*/
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
                        if (length(particules[i].pos - particules[j].pos) < (particules[i].size + particules[j].size)) {
                            sf::Vector2f mvt(normalise(particules[i].pos - particules[j].pos) * ((particules[i].size + particules[j].size) - length(particules[i].pos - particules[j].pos)) / 2.0f);
							if (length(mvt) > 2) {
                                mvt = normalise(mvt) * 2.0f;
							}
                            particules[i].pos += mvt*0.75f;
                            particules[j].pos += -mvt*0.75f;
                            float lengthMvt = length(mvt);
                            particules[i].color = lerp(colorMin, colorMax, lengthMvt);
                            particules[j].color = lerp(colorMin, colorMax, lengthMvt);
                        }
                    }
                }
            }

			//updateParticules(particulesOrg, particules, nbParticule);


            particules[i].update(deltaT);

            particules[i].draw(window);
        }
        /*element root;
        root.rect.setSize(sf::Vector2f(W, H));
        root.rect.setPosition(0, 0);
        root.child1 = nullptr;
        root.child2 = nullptr;
        root.child3 = nullptr;
        root.child4 = nullptr;
        root.rect.setSize(sf::Vector2f(W, H));
        root.rect.setPosition(0, 0);

        updateTree(&root, particules, nbParticule);
        drawTree(&root, window);
        deleteTree(root.child1);
        deleteTree(root.child2);
        deleteTree(root.child3);
        deleteTree(root.child4);*/
        window.display();
    }
}