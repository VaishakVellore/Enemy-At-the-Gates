#pragma once

// a structure to hold a particle
class Particle {
public:
	float position[3];
	float direction[3];
	float life;
	Particle* next;
	Particle() {
		position[0] = 0.5f;
		position[2] = 0.5f;
		position[1] = 0.0f;
		direction[0] = (10000 - rand() % 50000) / 10000.0f;
		direction[1] = (10000 - rand() % 30000) / 10000.0f;
		direction[2] = (10000 - rand() % 50000) / 10000.0f;
		life = rand() % 5000 / 10000.0f;
	}
};

class ParticleSystem {
public:
	Particle * particle_head;

	// add
	void add() {
		Particle* p = new Particle();
		p->next = particle_head;
		particle_head = p;
	}
	// update
	void update(float dt,int a) {
		Particle* p = particle_head;
		while (p) {
			switch (a) {
			case 0: 
				// decrease lifespan
				p->life -= dt;
				// apply gravity
				p->direction[1] += 20.0f * dt; // orange
				// modify position
				p->position[0] += dt * p->direction[0];
				p->position[1] += dt * p->direction[1];
				p->position[2] += dt * p->direction[2];
				// goto next particle
				p = p->next;
				break;
			case 1:
				// decrease lifespan
				p->life -= dt;
				// apply gravity
				p->direction[1] += 30.0f * dt; // red
				// modify position
				p->position[0] += dt * p->direction[0];
				p->position[1] += dt * p->direction[1];
				p->position[2] += dt * p->direction[2];
				// goto next particle
				p = p->next;
				break;
			case 2:
				// decrease lifespan
				p->life -= dt;
				// apply gravity
				p->direction[1] += 10.0f * dt; // yellow
				// modify position
				p->position[0] += dt * p->direction[0];
				p->position[1] += dt * p->direction[1];
				p->position[2] += dt * p->direction[2];
				// goto next particle
				p = p->next;
				break;
			case 3:
				// decrease lifespan
				p->life -= dt;
				// apply gravity
				p->direction[1] += 10.0f * dt; // yellow
				// modify position
				p->position[0] += dt * p->direction[0];
				p->position[1] += dt * p->direction[1];
				p->position[2] += dt * p->direction[2];
				// goto next particle
				p = p->next;
				break;
			}
			// decrease lifespan
			/*p->life -= dt;
			// apply gravity
			p->direction[1] += 30.0f * dt; //
			// modify position
			p->position[0] += dt * p->direction[0];
			p->position[1] += dt * p->direction[1];
			p->position[2] += dt * p->direction[2];
			// goto next particle
			p = p->next;*/
			
		}
	}
	// remove
	void remove() {
		Particle* curr = particle_head;
		Particle* prev = 0;
		while (curr) {
			if (curr->life < 0) {
				if (prev) {
					prev->next = curr->next;
				}
				else {
					particle_head = curr->next;
				}
				Particle* temp = curr;
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

ParticleSystem ps,ps1,ps2,ps3;