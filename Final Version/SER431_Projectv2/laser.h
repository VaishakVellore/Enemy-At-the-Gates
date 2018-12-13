#include<stdlib.h>

// a structure to hold a particle
class Laser {
public:
	float position[3];
	float direction[3];
	float life;
	Laser* next;
	Laser() {
		position[0] = 0.5f;
		position[2] = 0.5f;
		position[1] = 0.0f;
		direction[0] = (10000 - rand() % 20000) / 50000.0f;
		direction[1] = (10000 - rand() % 20000) / 50000.0f;
		direction[2] = (10000 - rand() % 20000) / 50000.0f;
		life = rand() % 10000 / 10000.0f;
	}
};

class LaserSystem {
public:
	Laser * particle_head;
	// add
	void add() {
		Laser* p = new Laser();
		p->next = particle_head;
		particle_head = p;
	}
	// update
	void update(float dt) {
		Laser* p = particle_head;
		while (p) {
			// decrease lifespan
			p->life -= dt;
			// apply gravity
			p->direction[2] -= 100.0f * dt;
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
		Laser* curr = particle_head;
		Laser* prev = 0;
		while (curr) {
			if (curr->life < 0) {
				if (prev) {
					prev->next = curr->next;
				}
				else {
					particle_head = curr->next;
				}
				Laser* temp = curr;
				curr = curr->next;
				delete temp;
			}
			else {
				prev = curr;
				curr = curr->next;
			}
		}
	}

	// remove
	void removeLaser() {
		Laser* curr = particle_head;
		Laser* prev = 0;
		while (curr) {
				if (prev) {
					prev->next = curr->next;
				}
				else {
					particle_head = curr->next;
				}
				Laser* temp = curr;
				curr = curr->next;
				delete temp;
		}
	}
};

LaserSystem laserps;