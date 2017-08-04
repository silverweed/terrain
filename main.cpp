#include <SFML/Window.hpp>
#include "Terrain.hpp"
#include <iostream>

template<typename T>
constexpr std::ostream& operator<<(std::ostream& stream, sf::Vector2<T> vec) {
	return stream << "(" << vec.x << ", " << vec.y << ")";
}

template<typename T>
constexpr std::ostream& operator<<(std::ostream& stream, sf::Rect<T> rect) {
	return stream << "(" << rect.left << ", " << rect.top 
		<< " x " << rect.width << ", " << rect.height << ")";
}

sf::View keep_ratio(const sf::Event::SizeEvent& size, const sf::Vector2u& designedsize) {
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


int main() {
	Terrain terrain("fg.png", "bg.png");
	std::cout << "terrain size = " << terrain.getSize() << std::endl;

	sf::RenderWindow window(sf::VideoMode(terrain.getSize().x, terrain.getSize().y), "Terrain");
	window.setFramerateLimit(60);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::Resized:
				window.setView(keep_ratio(event.size, terrain.getSize()));
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Q:
					window.close();
					break;
				default:
					break;
				}
			case sf::Event::MouseButtonPressed:
				{
					const auto pos = sf::Mouse::getPosition(window);
					terrain.removeSphere(pos.x, pos.y, 40);
					break;
				}
			default:
				break;
			}
		}

		window.clear();
		window.draw(terrain);
		window.display();
	}
}
