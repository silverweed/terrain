#include "Physics.hpp"
#include <cstdio>
#include <iostream>

Physics::Physics(const Terrain& terr, std::mutex& mtx)
	: terrain(terr)
	, mtx(mtx)
{}

void Physics::add(const PhysObj& obj)
{
	objects.emplace_back(obj);
	activeObjIdx.emplace_back(objects.size() - 1);
}

static bool _shouldBeMadeInactive(const PhysObj& obj)
{
	return sqrlength(obj.velocity) < 1 || obj.position.y < 0 || obj.position.y > 10000;
}

void Physics::step(float delta)
{
	mtx.lock();
	std::cerr << "objects: " << objects.size() << " (active: " << activeObjIdx.size() << ")\n";
	auto write = activeObjIdx.begin();
	for (auto read = write; read != activeObjIdx.end(); ++read) {
		auto& obj = objects[*read];

		if (_shouldBeMadeInactive(obj)) {
			continue;
		}

		if (read != write)
			*write = std::move(*read);
		++write;

		obj.position += obj.velocity * delta + 0.5f * obj.acceleration * delta * delta;

		// Check collision
		int ix = std::max(0, std::min(int(terrain.getSize().x), int(obj.position.x))),
		    iy = std::max(0, std::min(int(terrain.getSize().y), int(obj.position.y)));
		if (terrain.isSolid(ix, iy)) {
			if (terrain.isSurface(ix, iy)) {
				const auto normal = terrain.getNormal(ix, iy);
				// Mirror velocity vs normal
				auto projection = obj.velocity.x * normal.x + obj.velocity.y * normal.y;
				obj.velocity -= normal * projection * 2.f * obj.bounciness;
			} else {
				// Unstuck the particle (FIXME: this is stupid and works bad)
				obj.velocity *= -1.f;
				float step = 1;
				do {
					obj.position += obj.velocity * delta * step;
					ix = std::max(0, std::min(int(terrain.getSize().x), int(obj.position.x)));
					iy = std::max(0, std::min(int(terrain.getSize().y), int(obj.position.y)));
					step *= 1.05;
				} while (terrain.isSolid(ix, iy));
			}
		} else {
			obj.velocity += obj.acceleration * delta;
		}
	}
	mtx.unlock();

	activeObjIdx.erase(write, activeObjIdx.end());
}
