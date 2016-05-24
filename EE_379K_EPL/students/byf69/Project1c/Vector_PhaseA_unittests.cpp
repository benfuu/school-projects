#include <cstdint>
#include <stdexcept>
#include <future>
#include <chrono>
#include "gtest/gtest.h"
#include "Vector.h"

#define PHASE_A

#ifdef _MSC_VER
#define noexcept
#endif

using epl::vector;

/*****************************************************************************************/
// Class Instrumentation
/*****************************************************************************************/
namespace{
  //Class Instrumentation
  class Foo {
  public:
    bool alive;
    
    static uint64_t constructions;
    static uint64_t destructions;
    static uint64_t copies;
    static uint64_t moves;
    static void reset(){ moves=copies=destructions=constructions=0; }

    Foo(void) { alive = true; ++constructions; }
    ~Foo(void) { destructions += alive;}
    Foo(const Foo&) noexcept { alive = true; ++copies; }
    Foo(Foo&& that) noexcept { that.alive = false; this->alive = true; ++moves;}
    //Foo& operator=(const Foo&) noexcept { alive = true; ++copies; return *this; }
    //Foo& operator=(Foo&& that) noexcept { that.alive = false; this->alive = true; ++moves; return *this; }
  };

  uint64_t Foo::constructions = 0;
  uint64_t Foo::destructions = 0;
  uint64_t Foo::copies = 0;
  uint64_t Foo::moves = 0;

} //namespace

bool newCalled = false;
void* operator new(size_t size){
  newCalled = true;
  return malloc(size);
}

/*****************************************************************************************/
// Phase A Tests
/*****************************************************************************************/
//TEST SUITE 1
#if defined(PHASE_A0) | defined(PHASE_A)
TEST(PhaseA, basic_functionality) {
	vector<int> x;
	EXPECT_EQ(0, x.size());

	x.push_back(42);
	EXPECT_EQ(1, x.size());
	EXPECT_EQ(42, x[0]);

	x[0] = 10;

	EXPECT_EQ(10, x[0]);

}
#endif

#if defined(PHASE_A1) | defined(PHASE_A)
TEST(PhaseA, constructors) {
	vector<int> x;
	EXPECT_EQ(0, x.size());
	x.push_back(42);
	vector<int> y{ x };
	EXPECT_EQ(1, y.size());
	EXPECT_EQ(42, y[0]);
	y[0] = 10;
	EXPECT_NE(10, x[0]);

	vector<int> z(10); // must use () to avoid ambiguity over initializer list
	EXPECT_EQ(10, z.size());
}
#endif

#if defined(PHASE_A2) | defined(PHASE_A)
TEST(PhaseA, index_range_check) {
	vector<int> x(10);
	EXPECT_NO_THROW(
	for (int k = 0; k < 10; k += 1) {
		x[k] = k;
	});

	EXPECT_THROW(x[10] = 42, std::out_of_range);
}
#endif

#if defined(PHASE_A3) | defined(PHASE_A)
TEST(PhaseA, PushBackAmort) {
  Foo::reset();
  int maxWait = 1; //seconds
  unsigned int smallPush_cnt=0, bigPush_cnt=0;
  bool die = false;
 
  std::future<void> build = std::async (
    std::launch::async,
    [&smallPush_cnt, &bigPush_cnt, &die] (void) {
      Foo obj;
      vector<Foo> x;
      int size = 1000;
      
      for(int i=0; i<size; ++i){
        x.push_back(obj); //push back some
        if(die) return;
      }

      newCalled = false;
      while(newCalled == false){
        x.push_back(obj);
        if(die) return;
      }

      newCalled = false;
      while(newCalled == false){
        x.push_back(obj);
        if(die) return;
        smallPush_cnt +=1;
      }

      newCalled = false;
      while(newCalled == false){
        x.push_back(obj);
        if(die) return;
        bigPush_cnt +=1;
      }
    }
  );

  std::chrono::seconds timeout(maxWait);
  auto status = build.wait_for(timeout);
  EXPECT_EQ(std::future_status::ready, status);
  die = true;
 
  //Check for at least amoritized doubling.
  //Anything more will also pass
  EXPECT_LT(1.9, float(bigPush_cnt)/float(smallPush_cnt));
}
#endif


//Ghetto C-style way to find the size of an array
//Only used in next test
#define ARRAY_SIZE(X) (sizeof(X)/sizeof(*X))
#if defined(PHASE_A4) | defined(PHASE_A)
TEST(PhaseA, PushBackFront){
  vector<int> x; // creates an empty vector
  EXPECT_EQ(0, x.size());
  x.push_back(42); 
  EXPECT_EQ(1, x.size());
  for (int k = 0; k < 10; k += 1) {
    x.push_back(k);
    x.push_front(k);
  }
  int ans[] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 42, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_EQ(ARRAY_SIZE(ans), x.size());
  for(unsigned int i=0; i<ARRAY_SIZE(ans); ++i)
    EXPECT_EQ(x[i], ans[i]);
}
#endif

#if defined(PHASE_A5) | defined(PHASE_A)
TEST(PhaseA, CopyCtor){
  vector<int> x;
  x.push_back(42);
  vector<int> y(x); // copy constructed
  x.push_back(0);
  
  EXPECT_EQ(1, y.size());
  EXPECT_EQ(42, y[0]);
  
  y.pop_back();
  EXPECT_EQ(0, y.size());
  EXPECT_EQ(2, x.size());
  EXPECT_EQ(42, x[0]);
  EXPECT_EQ(0, x[1]);
}
#endif

#if defined(PHASE_A6) | defined(PHASE_A)
TEST(PhaseA, PushBackCopy){
  Foo::reset();
  {
    vector<Foo> x(10); // 10 default-constructed Foo objects
    for (int k = 0; k < 11; ++k) { 
      Foo lval;
      x.push_back(lval);
    }
  } //ensures x is destroyed

  EXPECT_EQ(21, Foo::constructions);
  EXPECT_EQ(32, Foo::destructions);
  EXPECT_EQ(11, Foo::copies);
}
#endif

//this is the main entry point for the program.  Other
//tests can be in other cpp files, as long as there is
//only one of these main functions.
int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	auto out = RUN_ALL_TESTS();

#ifdef _MSC_VER
	system("pause");
#endif

	return out;
}
