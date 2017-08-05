#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System.hpp>
#include <vector>
#include "Terrain.hpp"

struct PhysObj {
	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::Vector2f acceleration;
	sf::Color color;
	float bounciness;
};

class Physics {
	const Terrain& terrain;
	std::vector<PhysObj> objects;

public:
	explicit Physics(const Terrain& terrain);

	const std::vector<PhysObj>& getObjects() const { return objects; }

	void add(const PhysObj& obj);

	void step(float delta);
};
