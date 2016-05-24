#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "byf69.h"
#include "CraigUtils.h"
#include "Event.h"
#include "ObjInfo.h"
#include "Params.h"
#include "Random.h"
#include "Window.h"

#ifdef _MSC_VER
using namespace epl;
#endif
using namespace std;
using String = std::string;

Initializer<byf69> __byf69_initializer;

void byf69::initialize(void) {
    LifeForm::add_creator(byf69::create, "byf69");
}

SmartPointer<LifeForm> byf69::create(void) {
    return new byf69;
}

String byf69::player_name(void) const {
    return "byf69";
}

String byf69::species_name() const {
    return m_name;
}

void byf69::set_species_name(String new_name) {
    this->m_name = new_name;
}

Color byf69::my_color() const {
    return MAGENTA;
}

void byf69::draw(int x, int y) const {
    win.draw_rectangle(x, y, x + 4, y + 4, true);
}

void byf69::startup() {
    this->set_course(drand48() * 2.0 * M_PI);
    this->set_speed(m_standard_speed);
    SmartPointer<byf69> self = SmartPointer<byf69>(this);
    this->sniff_event = new Event(0, [self](void) { self->sniff(); });
    this->reverse_direction_event = new Event(this->reverse_direction_time(), [self]() { self->reverse_direction(); });
}

byf69::byf69() {
    SmartPointer<byf69> self = SmartPointer<byf69>(this);
    new Event(0, [self](void) { self->startup(); });
}

byf69::~byf69() {}

double byf69::average_bearing(ObjList nearby) {
    double sum = 0;
    for (ObjList::iterator i = nearby.begin(); i != nearby.end(); i++) {
        sum += i->bearing;
    }
    return sum / nearby.size();
}

/* See if lifeform is a byf69 depending on the possible speeds. */
bool byf69::is_byf69(ObjInfo info) {
    if (this->is_equal_double(info.their_speed, this->m_max_speed) ||
        this->is_equal_double(info.their_speed, this->m_chase_speed) ||
        this->is_equal_double(info.their_speed, this->m_flee_speed) ||
        this->is_equal_double(info.their_speed, this->m_standard_speed)) {
        return true;
    }
    else { return false; }
}

/* See if it is actually algae. Speed should be 0 but clever species will also set their speed to 0 so be wary */
bool byf69::is_algae(ObjInfo info) {
    if (info.species == "Algae" && this->is_equal_double(0.0, info.their_speed)) { return true; }
    else { return false; }
}

/* Compares two doubles and returns if they are logically equivalent */
bool byf69::is_equal_double(double a, double b) {
    if (a == b || std::abs(a - b) < this->DOUBLE_ERROR) { return true; }
    else { return false; }
}

ObjInfo byf69::find_closest(ObjList nearby) {
    ObjInfo closest{};
    double dist = INFINITY;
    for (ObjList::iterator i = nearby.begin(); i != nearby.end(); i++) {
        if (i->distance < dist) {
            closest = *i;
            dist = i->distance;
        }
    }
    return closest;
}

ObjInfo byf69::find_best_food(ObjList prey) {
    ObjInfo best_food{};
    /* The best eat is the best value energy_gain * eat_success_chance */
    double best_energy_gain = 0;
    for (ObjList::iterator i = prey.begin(); i != prey.end(); i++) {
        double effective_health = this->health() - ((movement_cost(this->m_chase_speed, i->distance / this->m_chase_speed)) / start_energy);
        double success_chance = eat_success_chance(effective_health * start_energy, i->health * start_energy);
        double energy_gain = (i->health * start_energy) * eat_efficiency;
        double effective_energy_gain = energy_gain * success_chance;
        if (effective_energy_gain > best_energy_gain) {
            best_food = *i;
        }
    }
    return best_food;
}

ObjInfo byf69::find_best_food(ObjInfo algae, ObjInfo prey) {
    double algae_effective_health = this->health() - ((movement_cost(this->m_chase_speed, algae.distance / this->m_chase_speed)) / start_energy);
    double algae_success_chance = eat_success_chance(algae_effective_health * start_energy, algae.health * start_energy);
    double algae_energy_gain = (algae.health * start_energy) * eat_efficiency;
    double algae_effective_energy_gain = algae_energy_gain * algae_success_chance;

    double prey_effective_health = this->health() - ((movement_cost(this->m_chase_speed, prey.distance / this->m_chase_speed)) / start_energy);
    double prey_success_chance = eat_success_chance(algae_effective_health * start_energy, prey.health * start_energy);
    double prey_energy_gain = (prey.health * start_energy) * eat_efficiency;
    double prey_effective_energy_gain = prey_energy_gain * prey_success_chance;

    return algae_effective_energy_gain * this->m_algae_food_bias > prey_effective_energy_gain ? algae : prey;
}

Action byf69::encounter(const ObjInfo& info) {
    if (this->is_byf69(info) && info.species != this->m_name_eat_me) {
        return LIFEFORM_IGNORE;
    }
    else {
        if (info.species == this->m_name_eat_me) {
            if ((this->m_eat_health_factor - 1.0) * this->health() > info.health) {
                return LIFEFORM_IGNORE; // false encounter with eat_me lifeform
            }
        }
        this->sniff_event->cancel();
        this->reverse_direction_event->cancel();
        SmartPointer<byf69> self = SmartPointer<byf69>(this);
        this->sniff_event = new Event(0.0, [self](void) { self->sniff(); });
        this->reverse_direction_event = new Event(0.0, [self]() { self->reverse_direction(); });
        return LIFEFORM_EAT;
    }
}

void byf69::sniff() {
    this->reverse_direction_event->cancel();
    ObjList nearby{};
    /* High energy lifeforms can perceive further. */
    /*
    if (this->health() > this->m_perceive_factor * perceive_cost(this->m_perceive_range_max) / start_energy) {
        nearby = this->perceive(this->m_perceive_range_max);
    }
    else if (this->health() > this->m_perceive_factor * perceive_cost(this->m_perceive_range_first_quartile) / start_energy) {
        nearby = this->perceive(this->m_perceive_range_first_quartile);
    }
    else if (this->health() > this->m_perceive_factor * perceive_cost(this->m_perceive_range_second_quartile) / start_energy) {
        nearby = this->perceive(this->m_perceive_range_second_quartile);
    }
    else if (this->health() > this->m_perceive_factor * perceive_cost(this->m_perceive_range_third_quartile) / start_energy) {
        nearby = this->perceive(this->m_perceive_range_third_quartile);
    }
    else if (this->health() > this->m_perceive_factor * perceive_cost(this->m_perceive_range_min) / start_energy) {
        nearby = this->perceive(this->m_perceive_range_max);
    }
    else {
        this->set_species_name(this->m_name_eat_me);
        return;
    }
    */
    if (this->health() < 1.0) {
        nearby = this->perceive(this->m_perceive_range / 2);
    }
    else {
        nearby = this->perceive(this->m_perceive_range);
    }
    ObjList algae{};
    ObjList prey{};
    ObjList predators{};
    ObjList friends{};
    ObjList neutral{};
    for (ObjList::iterator i = nearby.begin(); i != nearby.end(); i++) {
        if (this->is_equal_double(0.0, i->health)) {
            continue;
        }
        if (this->is_byf69(*i)) {   // if our species, then is our friend
            friends.push_back(*i);
            continue;
        }
        double effective_health = this->health() - ((movement_cost(this->get_speed(), i->distance / this->get_speed())) / start_energy);
        if (effective_health <= 0.0) {    // try to get eaten by our own
            this->action = GET_EATEN;
        }
        double success_chance = eat_success_chance(effective_health * start_energy, i->health * start_energy);
        if (this->is_algae(*i)) {   // check if algae
            algae.push_back(*i);
            continue;
        }
        if (success_chance > this->m_margin_eat) {    // they are our prey
            prey.push_back(*i);
        }
        else if (success_chance < this->m_margin_ignore) {  // they are hunting us
            predators.push_back(*i);
        }
        else {  // we might both want to eat each other, so let's consider the encounter strategy
            neutral.push_back(*i);
        }
        /*
        
            */
    }
    if (this->action == GET_EATEN) {
        if (friends.size() > 0) {   // let our friends eat us
            this->set_species_name(this->m_name_eat_me);
            ObjInfo closest = this->find_closest(friends);
            this->set_course(closest.bearing);
            this->set_speed(this->m_max_speed);
        }
        else {
            this->action = FLEE;
        }
    }
    if (algae.size() > 0 || prey.size() > 0) {
        ObjInfo best_food{};
        if (algae.size() > 0 && prey.size() == 0) {         // only algae, find closest
            best_food = this->find_closest(algae);
        }
        else if (algae.size() == 0 && prey.size() > 0) {    // only prey, find best energy gain
            best_food = this->find_best_food(prey);
        }
        else {  // there are both algae and prey to eat
            ObjInfo best_algae = this->find_closest(algae);
            ObjInfo best_prey = this->find_best_food(prey);
            best_food = this->find_best_food(best_algae, best_prey);
        }
        this->set_species_name(best_food.species);
        this->set_course(best_food.bearing);
        this->set_speed(m_chase_speed);
    }

    else if (friends.size() > 0) {  // let's find a friend to follow
        ObjInfo closest_friend = this->find_closest(friends);
        if (this->m_eat_health_factor * this->health() < closest_friend.health) {
            this->set_species_name(this->m_name_eat_me);
            this->set_course(closest_friend.bearing);
            this->set_speed(m_max_speed);
        }
        else {
            this->set_course(closest_friend.bearing + this->m_friend_bearing_offset);
            this->set_speed(m_standard_speed);
        }
    }

    else if (neutral.size() > 0) {
        ObjInfo closest = this->find_closest(neutral);
        double effective_health = this->health() - ((movement_cost(this->m_chase_speed, closest.distance / this->m_chase_speed)) / start_energy);
        switch (encounter_strategy) {
        case EVEN_MONEY: 					// take your chance and make them prey
            this->set_course(closest.bearing);
            this->set_speed(m_standard_speed);
            break;
        case BIG_GUY_WINS:
            if (effective_health > closest.health) {   // if we are bigger, they are prey
                this->set_course(closest.bearing);
                this->set_speed(this->m_chase_speed);
            }
            else {  // RUNNNNNNNN
                this->set_course(closest.bearing + this->reverse_direction_bearing());
                this->set_speed(this->m_standard_speed);
            }
            break;
        case UNDERDOG_IS_HERE:				// let the little guy have a chance
            if (effective_health < closest.health) {   // if we are smaller, they are prey
                this->set_course(closest.bearing);
                this->set_speed(this->m_chase_speed);
            }
            else {  // RUNNNNNNNN
                this->set_course(closest.bearing + this->reverse_direction_bearing());
                this->set_speed(this->m_standard_speed);
            }
            break;
        case FASTER_GUY_WINS:				// speeding eagle gets the mouse
            this->set_course(closest.bearing);
            this->set_speed(this->m_max_speed);
            break;
        case SLOWER_GUY_WINS:				// ambush!
            this->set_course(closest.bearing);
            this->set_speed(this->m_ambush_speed);
            break;
        }
    }

    else if (predators.size() > 0) {    // run away?
        double avg_bearing = this->average_bearing(predators);
        this->set_course(avg_bearing + M_PI);
        this->set_speed(m_flee_speed);
    }

    else {
        this->set_course(this->get_course() + M_PI);
        this->set_speed(this->m_standard_speed);
    }

    SmartPointer<byf69> self = SmartPointer<byf69>{ this };
    this->sniff_event = new Event{ 5.0, [self]() { self->sniff(); } };
    this->reverse_direction_event = new Event{ this->reverse_direction_time(), [self]() {self->reverse_direction(); } };
    if (this->health() > 2.0 + 4 * (1.0 - reproduce_cost)) { spawn(); }
}

void byf69::reverse_direction() {
    this->reverse_direction_event->cancel();
    this->set_course(this->get_course() + this->reverse_direction_bearing());
    SmartPointer<byf69> self = SmartPointer<byf69>{ this };
    this->reverse_direction_event = new Event{ this->reverse_direction_time(), [self]() { self->reverse_direction(); } };
}

double byf69::reverse_direction_time() {
    return epl::drand48() * (this->m_reverse_direction_time_max - this->m_reverse_direction_time_min) + this->m_reverse_direction_time_min;
}

double byf69::reverse_direction_bearing() {
    return epl::drand48() * 2 * M_PI;
}

void byf69::spawn() {
    SmartPointer<byf69> child = new byf69{};
    this->reproduce(child);
}