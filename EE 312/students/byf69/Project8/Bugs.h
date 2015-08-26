#ifndef _Bugs_h
#define _Bugs_h

// The list of all bugs is implemented with a C++ vector
#include <vector>
using std::vector;


struct Bug {
	/* the bug's current position is defined by three variables
	 * x and y give the x and y coordinates of the bug's position
	 * dir gives the bugs current facing (the direction it is headed)
	 * directions are numbered clockwise beginning at straight up (zero)
	 * There are eight directions (the four "manhattan" directions plus 
	 * the four diagonals) numbered from 0 to 7
	 */
	int x;
	int y;
	int dir;
	
	/* Each time step, the bug loses some health (energy expended to move)
	 * each time the bug eats some food, it gains health, etc.
	 * if health gets to zero the bug dies.  If health gets high enough
	 * the bug will reproduce
	 */
	int health;
	
	/* As bugs reproduce they create the next generation.
	 * Each child produced by this bug will have generation one greater
	 * than it does.  So, if I'm generation 5, my children will be
	 * generation 6, my grandchildren generation 7 and so on
	 */
	int generation;
	
	/* 
	 * age is the number of timeSteps for which this bug has been alive
	 */
	int age; 
	
	/* each bug has eight genes:
	 * The genes correspond to the probability of changing the direction
	 * in which the bug will move
	 * genes[0] is the relative probability of going straight
	 * genes[1] would be turning one click to the right and so on
	 * clockwise until we get to genes[7] which is the probability of
	 * turning one click to the left.
	 *
	 * to keep genetic mutation simple, the genes are not the actual
	 * probability.  Each gene is a number between zero and GENE_TOTAL
	 * The probability of going straight is then genes[0] / GENE_TOTAL
	 * and so on.
	 * 
	 * to mutate a set of genes, the value in the genes can be changed
	 * in any way as long as the total of all the genes is unchanged.
	 */
	int genes[8]; 
};

/* Parameters Desribing Bugs */
static const int GENE_TOTAL = 24; // sum of genes[0] .. genes[7]
static const int START_HEALTH = 100; // value for health when simulation begins
static const int REPRODUCE_HEALTH = 200; // health required to reproduce
static const int EAT_HEALTH = 20; // value gained from eating one food
static const int MOVE_HEALTH = 1; // value lost each time step
static const bool DISTRIBUTED_GENES = 1; // starting bugs have uniformly distributed genes
							// if DISTRIBUTED_GENES is false, then starting bugs go straight

/* Parameters and Global Variables describing the world */
static const int WORLD_SIZE = 200; // world is WORD_SIZE square
extern int world[WORLD_SIZE][WORLD_SIZE]; // one int per position
/* objects in the world are encoded as ints
 * non-negative ints indicate a bug.
 * If the encoded value is 13, for example, then bug #13 is in that
 * position of the world.  NOTE: there is a bug #0 
 */
static const int EMPTY = -1; // empty squares in the world 
static const int FOOD = -2; // a square with food in it
static const int INITIAL_FOOD = WORLD_SIZE * WORLD_SIZE / 10; // number of initial food in world
static const int INITIAL_BUGS = 100; // number of initial bugs in world
static const int NUM_FOOD = 5; // number of food created each time step

static const int SPEED_UP = 1; // a quick and dirty way to make the simulation run faster
// if speed_up > 1, then we'll run in high speed mode until we reach time_step = SLOW_TIME
// then we'll slow down.
static const int SLOW_TIME = 10000000;
//static const int SLOW_TIME = 0x7fffffff; // max int 

/* all bugs are kept in a vector (expandable array)
 * the position in the vector is the "bug #" (see world)
 */
extern vector<Bug> bug_list;

/* simulation statistics */
extern int time_step;
extern int average_age;
extern int average_generation;
extern int percent_straight;
extern int percent_right;
extern int percent_left;
extern int percent_back;



/* Project 10 functions to write */
extern void createWorld();
extern void timeStep();
#endif /* _Bugs_h */
