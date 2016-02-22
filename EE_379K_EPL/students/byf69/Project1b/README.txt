The only file that must be committed is Vector.h. You may add or modify
additional files at your own discretion. You can modify
Vector_PhaseA_unittests.cpp however you see fit.

HOWEVER: when we grade your project, we will use our own test files
and we will #include "Vector.h". You must ensure that any code (template
definitions) we need are included by just the one line #include "Vector.h"

Good Luck!

*****************************************************************************
FOR LINUX USERS:
[testing with gtest]
type "make -f Makefile_gtest" to build the test executable with gtest
type "make -f Makefile_gtest test" to build & execute the test executable

While understanding makefiles is not necessary for this course,
it is often very important in Unix environments.  If you wish to
understand how Makefile works, see Makefile_With_Comments.

[testing without gtest]
Note: If you do not want to use gtest, feel free to create your own main
function. Rename Vector_PhaseA_unittests.cpp to something without a *.cpp
suffix, or you can remove it entirely. Then:

type "make" to build the test executable
type "make test" to build & execute the test executable
