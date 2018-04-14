#pragma once

#include <list>
#include <memory>
#include <random>
#include <sstream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

struct Particle
{
	float speed;
	float angle;
	sf::Time lifetime;
	RectangleShape rect;
	float size = 1.01;
};

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
public:
	std::vector<Particle> m_particles;
	sf::Time m_lifetime;
	sf::Vector2f m_emitter;
	int emitterAngle;
	int emitterSpeed;
	int emitterLifetime;
	Texture texture;
	float min_size = 1;
	float max_size = 5;
	float max_speed = 20;
	float min_speed = 5;

	ParticleSystem(unsigned int count, int angle, int speed, int lifetime,Texture t):
		m_particles(count),
		m_lifetime(sf::seconds(lifetime)),
		m_emitter(0, 0),
		emitterAngle(angle),
		emitterSpeed(speed)
	{
		
		texture = t;
	}

	void setEmitter(sf::Vector2f position)
	{
		m_emitter = position;
	}

	void update(sf::Time elapsed)
	{
		for (std::size_t i = 0; i < m_particles.size(); ++i)
		{
			
			

			// update the particle lifetime
			Particle& p = m_particles[i];
			p.lifetime -= elapsed;
		

			// if the particle is dead, respawn it
			if (p.lifetime <= sf::Time::Zero)
				resetParticle(i);


			//change size
			p.size = p.lifetime.asSeconds() / (0 - m_lifetime.asSeconds()) * (max_size - min_size) + min_size;

			p.rect.setScale(Vector2f(p.size, p.size));
		

			// update the position of the corresponding vertex
			p.speed += 0.1f;
			Vector2f velocity(p.speed * cos(p.angle), p.speed * sin(p.angle));
			
			p.rect.move(velocity * elapsed.asSeconds());

			// update the alpha (transparency) of the particle according to its lifetime
			float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
			Color c = p.rect.getFillColor();
			c.a = static_cast<sf::Uint8>(ratio * 255);
			p.rect.setFillColor(c);
		}
	}

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		

		// apply the transform
		states.transform *= getTransform();

		// our particles don't use a texture
		states.texture = NULL;

		// draw the vertex array
		for (int i = 0; i < m_particles.size(); i++)
		{
			target.draw(m_particles[i].rect);
		}

	}

private:


	void resetParticle(std::size_t index)
	{
		// give a random velocity and lifetime to the particle
		float angle = (emitterAngle / 2 - std::rand() % emitterAngle + 90) * 3.14f / -180.f;
		float speed = (std::rand() % emitterSpeed) + emitterSpeed;
		m_particles[index].angle = angle;
		m_particles[index].speed= speed;
		m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);

		// reset the position of the corresponding vertex
		m_particles[index].rect.setSize(Vector2f(50, 50));
		m_particles[index].rect.setPosition(m_emitter);
		m_particles[index].rect.setTexture(&texture);
	}


};


int main()
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(1080, 720), "Particles");

	Texture t;
	t.loadFromFile("smoke.png");
	// create a clock to track the elapsed time
	sf::Clock clock;

	Texture u;
	u.loadFromFile("fire.png");

	Font font;
	font.loadFromFile("BAUHS93.ttf");

	Text text;
	text.setFont(font);
	text.setPosition(100, 100);
	

	// create the particle system
	ParticleSystem particles(1000, 120, 100, 3, t);
	ParticleSystem particles2(1000, 50, 50, 4, u);


	// Particle system position
	particles.setEmitter(sf::Vector2f(800, 700));
	particles2.setEmitter(sf::Vector2f(300, 700));


	// run the main loop
	while (window.isOpen())
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Dash))
			{
				if (particles.m_particles.size() > 200)
				{
					particles.m_particles.erase(particles.m_particles.end() - 200, particles.m_particles.end());
				}

				if (particles2.m_particles.size() > 200)
				{
					particles2.m_particles.erase(particles2.m_particles.end() - 200, particles2.m_particles.end());
				}
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))
			{
				vector<Particle> newparticles = vector<Particle>(200);
				particles.m_particles.insert(particles.m_particles.end(), newparticles.begin(), newparticles.end());

				vector<Particle> newparticles2 = vector<Particle>(200);
				particles2.m_particles.insert(particles2.m_particles.end(), newparticles2.begin(), newparticles2.end());
			}
		}

		


		// update it
		sf::Time elapsed = clock.restart();
		particles.update(elapsed);
		particles2.update(elapsed);


		text.setString(to_string(particles.m_particles.size()));
		// draw it
		window.clear();
		window.draw(particles);
		window.draw(particles2);
		window.draw(text);
		window.display();
	}

	return 0;
}