#pragma once

#include <cmath>

template<typename T>
constexpr std::ostream& operator<<(std::ostream& stream, sf::Vector2<T> vec) {
	return stream << "(" << vec.x << ", " << vec.y << ")";
}

template<typename T>
constexpr std::ostream& operator<<(std::ostream& stream, sf::Rect<T> rect) {
	return stream << "(" << rect.left << ", " << rect.top 
		<< " x " << rect.width << ", " << rect.height << ")";
}

inline sf::View keep_ratio(const sf::Event::SizeEvent& size, const sf::Vector2u& designedsize) {
	sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);

	const float screenwidth = size.width / static_cast<float>(designedsize.x),
	            screenheight = size.height / static_cast<float>(designedsize.y);

	if (screenwidth > screenheight) {
		viewport.width = screenheight / screenwidth;
		viewport.left = (1.f - viewport.width) / 2.f;
	} else if (screenwidth < screenheight) {
		viewport.height = screenwidth / screenheight;
		viewport.top = (1.f - viewport.height) / 2.f;
	}

	sf::View view(sf::FloatRect(0, 0, designedsize.x , designedsize.y));
	view.setViewport(viewport);

	return view;
}

struct ColorHash {
	size_t operator()(const sf::Color& c) const {
		return (c.r << sizeof(uint8_t) * 3) + (c.g << sizeof(uint8_t) * 2)
			+ (c.b << sizeof(uint8_t) * 1) + c.a;
	}
};

template<typename T>
inline sf::Vector2f normalized(const sf::Vector2<T>& v) {
	const float norm = std::sqrt(v.x * v.x + v.y * v.y);
	if (norm == 0)
		return v;
	return sf::Vector2f(v.x / norm, v.y / norm);
}
