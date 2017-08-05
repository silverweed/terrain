#pragma once

#include <vector>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include "Physics.hpp"

template<class T>
class Renderer : public sf::Drawable {
	const std::vector<T>& objects;
public:
	explicit Renderer(const std::vector<T>& objects) : objects(objects) {}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		std::for_each(objects.begin(), objects.end(), [&target, states] (const auto& obj) {
			sf::CircleShape particle(3);
			particle.setPosition(obj.position);
			particle.setFillColor(obj.color);
			target.draw(particle, states);
		});
	}
};
