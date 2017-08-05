#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include "utils.hpp"

class Terrain : public sf::Drawable {
	/** Number of pixels per side */
	sf::Vector2u size;
	sf::Image bg, fg;
	sf::Texture bgtex, fgtex;
	/** Whether each pixel is solid or not */
	std::vector<bool> solidity;
	/** The vector containing normals per-pixel. Only surface pixels have valid normals. */
	std::vector<sf::Vector2f> normals;

	size_t _idx(int x, int y) const;
	bool _isSolidPixel(const sf::Color& pixel) const;
	void _buildSolidityMap();
	void _updateNormals();
	sf::Vector2f _normal(int x, int y) const;

public:
	explicit Terrain(const std::string& fgfile, const std::string& bgfile);

	sf::Vector2u getSize() const { return size; }
	bool isSolid(int x, int y) const;
	bool isSurface(int x, int y) const;
	sf::Vector2f getNormal(int x, int y) const;

	/** Removes a circle centered in (cx, cy) with radius `radius` from terrain, and returns a
	 *  map (color => perc), where `perc` is the percentage of pixels of that color that
	 *  were removed.
	 */
	std::unordered_map<sf::Color, float, ColorHash> removeCircle(int cx, int cy, float radius);

	void drawNormals(sf::RenderTarget& target) const;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
