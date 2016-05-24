#include <cstdint>
#include <stdexcept>
#include <future>
#include <chrono>
#include "gtest/gtest.h"
#include "Vector.h"

#define PHASE_B

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
  };

  uint64_t Foo::constructions = 0;
  uint64_t Foo::destructions = 0;
  uint64_t Foo::copies = 0;
  uint64_t Foo::moves = 0;
} //namespace

/*****************************************************************************************/
// Phase B Tests
/*****************************************************************************************/
#if defined(PHASE_B0) | defined(PHASE_B)
TEST(PhaseB, Movector){
  vector<Foo> x;
  for(unsigned int i=0; i<10; ++i)
    x.push_back(Foo());

  vector<Foo> y(x);
  Foo::reset();
  vector<Foo> z(std::move(x));

  EXPECT_EQ(Foo::moves, 0);
  EXPECT_EQ(Foo::copies, 0);
  EXPECT_EQ(Foo::constructions, 0);
  EXPECT_EQ(y.size(), z.size());
}
#endif

#if defined(PHASE_B1) | defined(PHASE_B)
TEST(PhaseB, PushBackMove){
  Foo::reset();
  {
    vector<Foo> x(10); // 10 default-constructed Foo objects
    for (int k = 0; k < 11; ++k) { 
      x.push_back(Foo());
    }
  } //ensures x is destroyed

  EXPECT_EQ(21, Foo::constructions);
  EXPECT_EQ(21, Foo::destructions);
  EXPECT_EQ(0, Foo::copies);
}
#endif

#if defined(PHASE_B2) | defined(PHASE_B)
TEST(PhaseB, ReallocCopy){
  Foo::reset();
  {
    vector<vector<Foo>> x(3);
    x[0].push_back(Foo()); //1 alive Foo
    x.push_back(x[0]); //1 copy, 2 alive Foo
  } //ensures x is destroyed

  EXPECT_EQ(1, Foo::constructions);
  EXPECT_EQ(2, Foo::destructions);
  EXPECT_EQ(1, Foo::copies);
}
#endif
