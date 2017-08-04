#pragma once

#include <SFML/Graphics.hpp>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

class Terrain : public sf::Drawable {
	/** Number of pixels per side */
	sf::Vector2u size;
	sf::Image bg, fg;
	sf::Texture bgtex, fgtex;
	/** Whether each pixel is solid or not */
	std::vector<bool> solidity;
	/** The vector containing normals per-pixel. Only surface pixels have valid normals. */
	std::vector<sf::Vector2f> normals;

#ifndef NDEBUG
#define CHECK_VALID_IDX(x, y) \
	if (unsigned(x) > size.x || unsigned(y) > size.y) { \
		throw; \
	}
#else
#define CHECK_VALID_IDX(x, y)
#endif

	bool _isSolidPixel(const sf::Color& pixel) const {
		return pixel.a != 0;
	}

	size_t _idx(int x, int y) const {
		return y * size.x + x;
	}

	void _buildSolidityMap() {
		for (unsigned x = 0; x < size.x; ++x)
			for (unsigned y = 0; y < size.y; ++y)
				solidity[_idx(x, y)] = _isSolidPixel(fg.getPixel(x, y));
	}

	void _updateNormals() {
		int i = 0;
		for (unsigned x = 0; x < size.x; ++x)
			for (unsigned y = 0; y < size.y; ++y)
				if (isSurface(x, y)) {
					normals[_idx(x, y)] = normal(x, y);
					++i;
				}
		std::cout << "found " << i << " surface points.\n";
	}

public:
	explicit Terrain(const std::string& fgfile, const std::string& bgfile) {
		if (!fg.loadFromFile(fgfile))
			throw;
		if (!bg.loadFromFile(bgfile))
			throw;
		fgtex.loadFromImage(fg);
		bgtex.loadFromImage(bg);
		size = fg.getSize();
		solidity.resize(size.x * size.y);
		normals.resize(size.x * size.y);
		assert(solidity.size() == size.x * size.y);
		assert(std::all_of(solidity.begin(), solidity.end(), [] (auto x) { return !x; }));
		_buildSolidityMap();
		_updateNormals();
	}

	bool solid(int x, int y) const {
		CHECK_VALID_IDX(x, y);
		return solidity[_idx(x, y)];
	}

	bool isSurface(int x, int y) const {
		if (!solid(x, y)) return false;
		bool at_least_1_solid = false, at_least_1_not_solid = false;
		for (int i = std::max(0, x - 1); i < std::min(int(size.x) - 1, x + 2); ++i)
			for (int j = std::max(0, y - 1); j < std::min(int(size.y) - 1, y + 2); ++j) {
				if (i == x && j == y) continue;
				if (solid(i, j))
					at_least_1_solid = true;
				else
					at_least_1_not_solid = true;

				if (at_least_1_solid && at_least_1_not_solid)
					return true;
			}
		return false;
	}

	// https://gamedevelopment.tutsplus.com/tutorials/coding-destructible-pixel-terrain-how-to-make-everything-explode--gamedev-45
	sf::Vector2f normal(int x, int y) const {
		CHECK_VALID_IDX(x, y);

		static constexpr int STEP = 6;
		sf::Vector2f avg;
		for (int i = -STEP; i <= STEP; ++i) {
			if (x + i < 0)
				continue;
			if (x + i >= int(size.x))
				break;
			for (int j = -STEP; j < STEP; ++j) {
				if (y + j < 0)
					continue;
				if (y + j >= int(size.y))
					break;
				if (solid(x + i, y + j)) {
					avg.x -= i;
					avg.y -= j;
				}
			}
		}
		return avg / std::sqrt(avg.x * avg.x + avg.y * avg.y);
	}

	void removeSphere(int cx, int cy, float radius) {
		const auto contained = [=] (int x, int y) {
			return (x - cx) * (x - cx) + (y - cy) * (y - cy) < radius * radius;
		};
		for (int i = std::max(0, int(cx - radius)); i < std::min(int(size.x), int(cx + radius)); ++i) {
			for (int j = std::max(0, int(cy - radius)); j < std::min(int(size.y), int(cy + radius)); ++j) {
				if (!contained(i, j)) continue;
				fg.setPixel(i, j, sf::Color(0, 0, 0, 0));
			}
		}
		fgtex.update(fg);
		_buildSolidityMap();
		_updateNormals();
		std::cout << "normals updated\n";
	}

	void drawNormals(sf::RenderTarget& target) const {
		static constexpr float LEN = 20;
		for (unsigned x = 0; x < size.x; x += 10) {
			for (unsigned y = 0; y < size.y; ++y) {
				// Ensure this is a surface point
				if (!isSurface(x, y))
					continue;
				if (solid(x, y)) {
					sf::Vertex line[2];
					line[0].position = sf::Vector2f(x, y);
					line[0].color = sf::Color::Black;
					line[1].position = sf::Vector2f(x, y) + normals[_idx(x, y)] * LEN;
					line[1].color = sf::Color::Black;
					target.draw(line, 2, sf::Lines);
				}
			}
		}
	}

	sf::Vector2u getSize() const { return size; }

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		sf::Sprite bgsprite(bgtex);
		target.draw(bgsprite, states);
		sf::Sprite fgsprite(fgtex);
		target.draw(fgsprite, states);
		drawNormals(target);
	}
};
