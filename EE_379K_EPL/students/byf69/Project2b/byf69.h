#pragma once

#include <memory>
#include "LifeForm.h"
#include "Init.h"

class byf69 : public LifeForm {
private:
    std::string m_name = "byf69";
    std::string m_name_eat_me = "byf69_eat_me";
    /* we use these to help us verify our identity */
    double m_signature = 0.01426595;
    double m_max_speed = max_speed - m_signature;
    double m_flee_speed = m_max_speed;
    double m_ambush_speed = m_signature + 0.2;
    double m_standard_speed = m_signature + 5.0;
    double m_chase_speed = m_standard_speed + 10.0;
    double m_friend_bearing_offset = 1 * M_PI / 3;
    double m_perceive_range_max = max_perceive_range;
    double m_perceive_range_first_quartile = m_perceive_range_max * 0.75;
    double m_perceive_range_second_quartile = m_perceive_range_max * 0.50;
    double m_perceive_range_third_quartile = m_perceive_range_max * 0.25;
    double m_perceive_range_min = min_perceive_range;
    double m_perceive_range = 50.0;
    double m_perceive_factor = 50.0;
    double m_reverse_direction_time_max = 15.0;
    double m_reverse_direction_time_min = 10.0;
    double DOUBLE_ERROR = 0.0000001;

    /*
     * If eat_chance_success > margin_eat, then we eat them.
     * If eat_chance_success is between margin_ignore and margin_eat, then we ignore them.
     * If eat_chance_success < margin_ignore, then we run away.
     */
    double m_margin_eat = 0.65;
    double m_margin_ignore = 0.35;
    double m_eat_health_factor = 2.5;
    double m_algae_food_bias = 1.25;

protected:
    static void initialize();
    void startup();
    void spawn();
    double average_bearing(ObjList);
    ObjInfo find_closest(ObjList);
    ObjInfo find_best_food(ObjList);            // either algae or prey
    ObjInfo find_best_food(ObjInfo, ObjInfo);   // algae and prey
    void set_species_name(std::string);
    bool is_byf69(ObjInfo);
    bool is_algae(ObjInfo);
    bool is_equal_double(double, double);
    void sniff();
    ObjInfo choose_perceive(double);
    Event* sniff_event;
    Event* try_to_die_event;
    void reverse_direction();
    Event* reverse_direction_event;
    double reverse_direction_time();
    double reverse_direction_bearing();
    enum byf69Action {
        CHASE,
        AMBUSH,
        FLEE,
        GET_EATEN,
        DIE
    } action;

public:
    byf69();
    virtual ~byf69();
    Color my_color() const;   // defines LifeForm::my_color
    void draw(int, int) const;
    static SmartPointer<LifeForm> create();
    virtual std::string player_name() const;
    virtual std::string species_name() const;
    virtual Action encounter(const ObjInfo&);
    friend class Initializer<byf69>;
};