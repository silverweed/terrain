#pragma once

#include "Terrain.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System.hpp>
#include <mutex>
#include <vector>

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
	std::vector<int> activeObjIdx;
	std::mutex& mtx;

public:
	explicit Physics(const Terrain& terrain, std::mutex& mtx);

	const std::vector<PhysObj>& getObjects() const { return objects; }

	void add(const PhysObj& obj);

	void step(float delta);
};
