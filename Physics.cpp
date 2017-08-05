#include "Physics.hpp"
#include <algorithm>

Physics::Physics(const Terrain& terr) : terrain(terr) {}

void Physics::add(const PhysObj& obj) {
	objects.emplace_back(obj);
}

void Physics::step(float delta) {
	std::for_each(objects.begin(), objects.end(), [this, delta] (auto& obj) {
		obj.position += obj.velocity * delta;
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
		}
		obj.velocity += obj.acceleration * delta;
	});
}
