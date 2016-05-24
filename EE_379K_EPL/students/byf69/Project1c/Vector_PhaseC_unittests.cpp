#include <cstdint>
#include <stdexcept>
#include "gtest/gtest.h"
#include "Vector.h"

//#define PHASE_C

using epl::vector;
using std::begin;
using std::end;

#ifdef PHASE_C

TEST(PhaseC, direct_init_list){
	vector<int> w{ 1, 2, 3 };
	EXPECT_EQ(3, w.size());
}

#define ARRAY_SIZE(X) (sizeof(X)/sizeof(*X))
TEST(PhaseC, copy_init_list){
  vector<int> x = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 42, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  int ans[] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 42, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_EQ(ARRAY_SIZE(ans), x.size());
  for(unsigned int i=0; i<ARRAY_SIZE(ans); ++i)
    EXPECT_EQ(x[i], ans[i]);
}

TEST(PhaseC, range_for_loop){
	vector<int32_t> x(10);

	int32_t k = 0;
	for (auto& v : x) {
		v = k++;
	}

	const vector<int32_t>& y = x;
	int32_t s = 0;
	for (const auto& v : y) {
		s += v;
	}

	EXPECT_EQ(45, s);
}

TEST(PhaseC, ItrExceptMild) {
    vector<int> x(1);
    auto itr = begin(x);
    x.pop_back();
    try {
        int a;
        a = *itr;
        FAIL();
        *itr = a;
    }
    catch (epl::invalid_iterator ii) {
        EXPECT_EQ(ii.level, epl::invalid_iterator::MILD);
    }
}

TEST(PhaseC, ItrExceptModerate){
	vector<int>  x(3), y{1,2,3};
	auto xi = begin(x);
	
	x = y;
	try{
		*xi = 5;
    FAIL();
	}
	catch(epl::invalid_iterator ex){
    EXPECT_EQ(ex.level, epl::invalid_iterator::MODERATE);
	}
}

TEST(PhaseC, ItrExceptMild1){
  vector<int> x(3);
  auto itr = begin(x);
  x.pop_back();
  try{
    volatile int a;
    a = *itr;
    FAIL();
    *itr = a;
  } catch (epl::invalid_iterator ii){
    EXPECT_EQ(ii.level, epl::invalid_iterator::MILD);
  }
}

#endif
