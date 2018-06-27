#pragma once

#include "Physics.hpp"
#include <SFML/Graphics.hpp>
#include <mutex>
#include <vector>

template <class T>
class Renderer : public sf::Drawable {
	const std::vector<T>& objects;
	std::mutex& mtx;

public:
	float pointWidth = 5;

	explicit Renderer(const std::vector<T>& objects, std::mutex& mtx)
		: objects(objects)
		, mtx(mtx)
	{}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		int i = 0;
		mtx.lock();
		sf::VertexArray arr(sf::PrimitiveType::Triangles, 3 * objects.size());
		for (const auto& obj : objects) {
			sf::Vertex v{};
			v.color = obj.color;

			v.position = obj.position - sf::Vector2f(pointWidth * 0.5, pointWidth * 0.5);
			arr[i] = v;
			++i;

			v.position = obj.position + sf::Vector2f(pointWidth * 0.5, -pointWidth * 0.5);
			arr[i] = v;
			++i;

			v.position = obj.position + sf::Vector2f(0, pointWidth * 0.5);
			arr[i] = v;
			++i;
		}
		mtx.unlock();
		target.draw(arr, states);
	}
};
