#pragma once
#pragma once
#include <stdlib.h>
// a structure to hold a particle
class Fire {
public:
	float position[3];
	float direction[3];
	float life;
	Fire* next;
	Fire() {
		position[0] = 0.5f;
		position[2] = 0.5f;
		position[1] = 0.0f;
		direction[0] = (10000 - rand() % 50000) / 10000.0f;
		direction[1] = (10000 - rand() % 30000) / 10000.0f;
		direction[2] = (10000 - rand() % 50000) / 10000.0f;
		life = rand() % 5000 / 10000.0f;
	}
};

class System {
public:
	Fire * particle_head;

	// add
	void add() {
		Fire* p = new Fire();
		p->next = particle_head;
		particle_head = p;
	}
	// update
	void update(float dt) {
		//printf("..................");
		Fire* p = particle_head;
		while (p) {
			// decrease lifespan
			p->life -= dt * 2;
			// apply gravity
			p->direction[1] += 0.5f * dt; //
			// modify position
			p->position[0] += dt * p->direction[0];
			p->position[1] += dt * p->direction[1];
			p->position[2] += dt * p->direction[2];
			// goto next particle
			p = p->next;
		}
	}
	// remove
	void remove() {
		Fire* curr = particle_head;
		Fire* prev = 0;
		while (curr) {
			if (curr->life < 0) {
				if (prev) {
					prev->next = curr->next;
				}
				else {
					particle_head = curr->next;
				}
				Fire* temp = curr;
				curr = curr->next;
				delete temp;
			}
			else {
				prev = curr;
				curr = curr->next;
			}
		}
	}
};

System lavaps;