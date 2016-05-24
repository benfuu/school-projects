/* main test simulator */
#include <iostream>
#include <cmath>
#include "CraigUtils.h"
#include "Window.h"
#include "tokens.h"
#include "ObjInfo.h"
#include "QuadTree.h" 
#include "Params.h"
#include "LifeForm.h"
#include "Event.h"
#include "Random.h"

using namespace std;
using namespace epl;

template <typename T>
void bound(T& x, const T& min, const T& max) {
	assert(min < max);
	if (x > max) { x = max; }
	if (x < min) { x = min; }
}

/*
 * Calls update_position, calculates the time to next border cross, and schedules the event.
 * Checks for encounters.
 */
void LifeForm::border_cross(void) {
	if (!this->is_alive) { return; }
	this->update_position();
	this->check_encounter();
	this->compute_next_move();
}

/* Cancels any border crossing events, updates position, and schedules next movement event */
void LifeForm::region_resize(void) {
	if (!this->is_alive) { return; }
	this->border_cross_event->cancel();
	this->update_position();
	this->compute_next_move();
}

/* Call encounter on both LifeForms. */
void LifeForm::resolve_encounter(SmartPointer<LifeForm> that) {
	SmartPointer<LifeForm> self = SmartPointer<LifeForm>{ this };
	self->energy -= encounter_penalty;
	self->check_energy();
	that->energy -= encounter_penalty;
	that->check_energy();
	if (!self->is_alive || !that->is_alive) {
		return;
	}
	Action action_self = self->encounter(self->info_about_them(that));
	Action action_that = that->encounter(that->info_about_them(self));
	double eat_success_self = eat_success_chance(self->energy, that->energy);
	double eat_success_that = eat_success_chance(that->energy, self->energy);
	std::uniform_real_distribution<double> distribution(0.0, 1.0);		// generate random number between 0 and 1
    double eat_rand_self = drand48();
    double eat_rand_that = drand48();
	if (action_self == LIFEFORM_EAT && action_that == LIFEFORM_IGNORE) {	// we are trying to eat them
		if (eat_rand_self < eat_success_self) {	// successfully ate that!
			self->eat(that);
		}
	}
	else if (action_self == LIFEFORM_IGNORE && action_that == LIFEFORM_EAT) {	// they are trying to eat us
		if (eat_rand_that < eat_success_that) {	// they ate us!
			that->eat(self);
		}
	}
	else if (action_self == LIFEFORM_EAT && action_that == LIFEFORM_EAT) {	// we are trying to eat each other
		if (eat_rand_self < eat_success_self && eat_rand_that >= eat_success_that) {		// we ate them!
			self->eat(that);
		}
		else if (eat_rand_self >= eat_success_self && eat_rand_that < eat_success_that) {	// they ate us!
			that->eat(self);
		}
		else if (eat_rand_self < eat_success_self && eat_rand_that < eat_success_that) {	// tiebreaker
			double coin_flip = drand48();
			switch (encounter_strategy) {
				case EVEN_MONEY: 					// flip a coin
					if (coin_flip < 0.5) { self->eat(that); }
					else { that->eat(self); }
					break;
				case BIG_GUY_WINS:					// big guy gets first bite
					if (self->energy > that->energy) { self->eat(that); }
					else if (self->energy < that->energy) { that->eat(self); }
					else {
						if (coin_flip < 0.5) { self->eat(that); }
						else { that->eat(self); }
					}
					break;
				case UNDERDOG_IS_HERE:				// let the little guy have a chance
					if (self->energy < that->energy) { self->eat(that); }
					else if (self->energy > that->energy) { that->eat(self); }
					else {
						if (coin_flip < 0.5) { self->eat(that); }
						else { that->eat(self); }
					}
					break;
				case FASTER_GUY_WINS:				// speeding eagle gets the mouse
					if (self->speed > that->speed) { self->eat(that); }
					else if (self->speed < that->speed) { that->eat(self); }
					else {
						if (coin_flip < 0.5) { self->eat(that); }
						else { that->eat(self); }
					}
					break;
				case SLOWER_GUY_WINS:				// ambush!
					if (self->speed < that->speed) { self->eat(that); }
					else if (self->speed > that->speed) { that->eat(self); }
					else {
						if (coin_flip < 0.5) { self->eat(that); }
						else { that->eat(self); }
					}
					break;
			}
		}
		
	}
	else {	// both ignoring each other

	}
}

void LifeForm::eat(SmartPointer<LifeForm> that) { // this eats that
	this->energy -= eat_cost_function();
	this->check_energy();
	if (this->is_alive) {
        new Event{ 0.0, [that]() { that->die(); } };
		// that->die();
		SmartPointer<LifeForm> self = SmartPointer<LifeForm>{ this };
		new Event{ digestion_time, [self, that]() { self->gain_energy(that->energy * eat_efficiency); } };
	}
	
}

/* Simulates aging. Deducts the age penalty from a LifeForm's energy and schedules next age event. */
void LifeForm::age(void) {
	this->energy -= age_penalty;
	this->check_energy();
	if (this->is_alive) {
		SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);
		new Event{ age_frequency, [self]() { self->age(); } };		// set event to age again in age_frequencey time units
	}
}

void LifeForm::gain_energy(double amount) {
	this->energy += amount;
}


/*
 * Updates the position of a LifeForm.
 * If the time between the last call to update_position and the current call is smaller than min_delta_time, then do nothing.
 */
void LifeForm::update_position(void) {
	if (!this->is_alive) { return; }
	SimTime elapsed_time = Event::now() - this->update_time;		// calculate the elapsed time
	if (elapsed_time > min_delta_time) {	// only update position if significant amount of time has passed
		Point old_pos = Point{ this->pos };					// save the position
        Point new_pos = Point{ this->pos };
		double distance = this->speed * elapsed_time;		// distance = speed * time
		double del_x = distance * cos(this->course);		// x = d * cos(course)
		double del_y = distance * sin(this->course);		// y = d * sin(course)
		new_pos.xpos += del_x;			// update x and y coordinates
		new_pos.ypos += del_y;
		if (space.is_out_of_bounds(new_pos)) {	// fell off of the world...sucks
            SmartPointer<LifeForm> self = SmartPointer<LifeForm>{ this };
            new Event(0.0, [self]() {self->die(); });
			//this->die();
		}
		else {
            while (space.is_occupied(new_pos)) {    // fudge if position is occupied
                new_pos.xpos += Point::tolerance;
                new_pos.ypos += Point::tolerance;
            }
			this->energy -= movement_cost(this->speed, elapsed_time);	// deduct movement cost
			this->check_energy();
            if (this->is_alive) {
                this->pos = new_pos;        // first update state
                this->update_time = Event::now();		// indicate that we have executed update_position
                space.update_position(old_pos, this->pos);	// update new position in QuadTree
			}
		}
	}
}

/* Checks if there is another LifeForm within 1 unit of distance. */
void LifeForm::check_encounter(void) {
	if (!this->is_alive) { return; }
	vector<SmartPointer<LifeForm>> in_radius = space.nearby(this->pos, encounter_distance);
	for (vector<SmartPointer<LifeForm>>::iterator i = in_radius.begin(); i != in_radius.end(); i++) {
		(*i)->update_position();	// update every object's position in the radius
	}
    SmartPointer<LifeForm> closest = space.closest(this->pos);
    if (closest && closest->pos.distance(this->pos) <= encounter_distance) {
        this->resolve_encounter(closest);
    }
}

/* Calculates the next border_cross event and schedules it. */
void LifeForm::compute_next_move(void) {
	if (!this->is_alive) { return; }
    assert(this->speed >= 0);
	if (this->speed > 0.0) {
		double distance_to_edge = space.distance_to_edge(this->pos, this->course);	// calculate the distance to border
		SimTime time_to_edge = distance_to_edge / this->speed;		// time = distance/speed
		SimTime time_tolerance = Point::tolerance / this->speed;	// additional tolerance
		SmartPointer<LifeForm> self = SmartPointer<LifeForm>{ this };
		this->border_cross_event = new Event{ time_to_edge + time_tolerance, [self]() { self->border_cross(); } };	// add the tolerance to guarantee crossing the border
	}
    else {  // object is not moving, so cancel border cross event and set it to null
        this->border_cross_event->cancel();
        this->border_cross_event = nullptr;
    }
}

/* Checks energy and if below minimum required energy, then dies. */
void LifeForm::check_energy(void) {
	if (!this->is_alive) { return; }
	if (this->energy < min_energy) {
        SmartPointer<LifeForm> self = SmartPointer<LifeForm>{ this };
        new Event(0.0, [self]() {self->die(); });
		//this->die();
	}
}
	
/* Returns the information of a neighbor LifeForm. */
ObjInfo LifeForm::info_about_them(SmartPointer<LifeForm> neighbor) {
	ObjInfo info;

	info.species = neighbor->species_name();
	info.health = neighbor->health();
	info.distance = pos.distance(neighbor->position());
	info.bearing = pos.bearing(neighbor->position());
	info.their_speed = neighbor->speed;
	info.their_course = neighbor->course;
	return info;
}

/* The functions set_course and set_speed cancel any current border cross event, update position, and schedule the next border_cross event. */
void LifeForm::set_course(double course) {
	if (!this->is_alive) { return; }
	this->border_cross_event->cancel();
	this->update_position();
	this->course = course;			// set course
	this->compute_next_move();
}
void LifeForm::set_speed(double speed) {
	if (!this->is_alive) { return; }
	this->border_cross_event->cancel();
	this->update_position();
	this->speed = speed >= max_speed ? max_speed : speed;			// set speed
	this->compute_next_move();
}

/*
 * Simulates asexual reproduction. The child receives half (minus reproduce_cost) of its parent's energy, and it is placed halfway between
 * the encounter distance and the maximum reproduction distance at a random bearing.
 */
void LifeForm::reproduce(SmartPointer<LifeForm> child) {
	if (!this->is_alive || (this->energy / 2) * (1.0 - reproduce_cost) < min_energy) {	// dead LifeForms cannot have children...that's gross.
		return;
	}
    this->update_position();
    if (!this->is_alive) { return; }
	SimTime elapsed_time = Event::now() - reproduce_time;
	if (elapsed_time > min_reproduce_time) {	// reproduce must not be called more frequently than min_reproduce_time
		Point test_pos{ this->pos };
		int placement_attempts = 5;
		bool found_space = true;
        SmartPointer<LifeForm> nearest;
		do {
			double reproduce_bearing = drand48() * 2.0 * M_PI;
            double reproduce_distance = drand48()*(reproduce_dist - encounter_distance) + encounter_distance;
			//double reproduce_distance = distribution(random_generator);	// random distance between encounter distance and max reproduce_dist
			double dist_x = reproduce_distance * cos(reproduce_bearing);
			double dist_y = reproduce_distance * sin(reproduce_bearing);
			test_pos.xpos = this->pos.xpos + dist_x;
			test_pos.ypos = this->pos.ypos + dist_y;
            nearest = space.closest(test_pos);
			placement_attempts -= 1;	// count number of times we've tried to place
			if (placement_attempts == 0) {
				found_space = false;
				break;
			}
		} while (space.is_out_of_bounds(test_pos) || (nearest && nearest->pos.distance(test_pos) <= encounter_distance));
		child->pos = test_pos;
		child->start_point = child->pos;
        child->energy = this->energy = (this->energy / 2) * (1.0 - reproduce_cost);	// child energy = parent energy = (parent energy/2)*(1.0-reproduce_cost)
		space.insert(child, child->pos, [child]() { child->region_resize(); });
		(void) new Event(age_frequency, [child]() { child->age(); });
		child->is_alive = true;
		if (!found_space) {	// if couldn't find a spot for child, resolve encounter with closest LifeForm
			SmartPointer<LifeForm> closest = space.closest(child->pos);
			child->resolve_encounter(closest);
		}
		this->reproduce_time = Event::now();	// indicate that we have executed reproduce
	}
	else {	// trying to call reproduce too frequently, so delete child and do not apply any energy penalties
		
	}
	
}

/* Finds all LifeForms that are within the requested radius. */
ObjList LifeForm::perceive(double radius) {
	if (!this->is_alive) { return ObjList{}; }	// must be alive
    this->update_position();
    if (!this->is_alive) { return ObjList{}; }
	/* Check boundaries and set radius to min/max if necessary. */
	if (radius < min_perceive_range) { radius = min_perceive_range; }
	else if (radius > max_perceive_range) { radius = max_perceive_range; }
	/* Create ObjList that contains the ObjInfo for every LifeForm within the radius. */
    this->energy -= perceive_cost(radius);		// deduct the cost to perceive
    this->check_energy();
    if (this->is_alive) {
        vector<SmartPointer<LifeForm>> neighbors = space.nearby(this->pos, radius);
        /* Get up-to-date information on every LifeForm in radius */
        for (vector<SmartPointer<LifeForm>>::iterator i = neighbors.begin(); i != neighbors.end(); i++) {
            (*i)->update_position();
        }
        neighbors = space.nearby(this->pos, radius);
        ObjList neighbors_info{};
        for (vector<SmartPointer<LifeForm>>::iterator i = neighbors.begin(); i != neighbors.end(); i++) {
            ObjInfo neighbor_info = this->info_about_them(*i);
            neighbors_info.push_back(neighbor_info);
        }
        if (neighbors_info.size() > 0) { return neighbors_info; }
        else { return ObjList{}; }
    }
    else { return ObjList{}; }
}
