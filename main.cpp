#include "FPSCounter.hpp"
#include "Physics.hpp"
#include "Renderer.hpp"
#include "Terrain.hpp"
#include "utils.hpp"
#include <SFML/Window.hpp>
#include <X11/Xlib.h>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <pthread.h>
#include <random>
#include <thread>

std::mutex physMtx;
bool quit = false;

void physicsThread(Physics& physics)
{
	const auto dt = std::chrono::milliseconds(10);
	FPSCounter fps("Physics");
	fps.start();
	std::cerr << "Started physics thread\n";
	while (!quit) {
		physics.step(10 / 1000.0);
		fps.addFrame();
		fps.report();
		std::this_thread::sleep_for(dt);
	}
}

void renderThread(sf::RenderWindow& window, const Terrain& terrain, const Renderer<PhysObj>& renderer)
{
	window.setActive(true);
	// const auto dt = std::chrono::milliseconds(long(1000 / 60.0));
	FPSCounter fps("Render");
	fps.start();
	std::cerr << "Started render thread\n";
	while (!quit && window.isOpen()) {
		window.clear();
		window.draw(terrain);
		window.draw(renderer);
		window.display();
		fps.addFrame();
		fps.report();
		// std::this_thread::sleep_for(dt);
	}
}

void _addParticles(const sf::Vector2i& pos,
	float radius,
	Physics& physics,
	const std::unordered_map<sf::Color, float, ColorHash>& percs)
{
	static constexpr auto N_PIXELS = 50;
	static std::random_device rng;
	std::uniform_real_distribution<float> dist(-radius * 0.8, radius * 0.8), bounceDist(0.1, 0.9);
	physMtx.lock();
	for (const auto& pair : percs) {
		for (int i = 0; i < int(N_PIXELS * pair.second); ++i) {
			// Establish position
			const sf::Vector2f mpos(pos.x + dist(rng), pos.y + dist(rng));
			physics.add(PhysObj{ // pos
				mpos,
				// vel
				10.f * (mpos - sf::Vector2f(pos)),
				// acceleration
				sf::Vector2f(0, 98),
				// color
				pair.first,
				// bounciness
				bounceDist(rng) });
		}
	}
	physMtx.unlock();
}

int main()
{
	XInitThreads();

	Terrain terrain("fg.png", "bg.png");
	Physics physics(terrain, physMtx);
	Renderer<PhysObj> renderer(physics.getObjects(), physMtx);
	sf::RenderWindow window(sf::VideoMode(terrain.getSize().x, terrain.getSize().y), "Terrain");
	window.setActive(false);

	std::thread phys(physicsThread, std::ref(physics));
	std::thread rend(renderThread, std::ref(window), std::cref(terrain), std::cref(renderer));
	if (pthread_setname_np(phys.native_handle(), "terrain_Physics") != 0) {
		perror("pthread_setname_np(physics)");
		return 1;
	}
	if (pthread_setname_np(rend.native_handle(), "terrain_Render") != 0) {
		perror("pthread_setname_np(render)");
		return 1;
	}

	std::cout << "terrain size = " << terrain.getSize() << std::endl;

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
			case sf::Event::MouseButtonPressed: {
				const auto pos = sf::Mouse::getPosition(window);
				const auto radius = 40;
				const auto percs = terrain.removeCircle(pos.x, pos.y, radius);
				_addParticles(pos, radius, physics, percs);
				break;
			}
			default:
				break;
			}
		}

		// std::cout << "physics particles: " << physics.getObjects().size() << std::endl;
		// physics.step(clock.restart().asSeconds());

		// window.clear();
		// window.draw(terrain);
		// window.draw(renderer);
		// window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(long(1000 / 60)));
	}

	quit = true;
	phys.join();
	rend.join();
}
