//-----------------------------------------------------------------------------
// Copyright (c) 2014 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "testing/unitTesting.h"

// This unit test will blow up without thread static support
#include "core/threadStatic.h"
#ifdef TORQUE_ENABLE_THREAD_STATICS

// Declare a test thread static
DITTS(U32, gUnitTestFoo, 42);
DITTS(F32, gUnitTestF32, 1.0);

TEST(ThreadStaticTest, BasicAPI)
{
   // ThreadStatic list should be initialized right now, so lets see if it has
   // any entries.
   EXPECT_FALSE(_TorqueThreadStaticReg::getStaticList().empty())
      << "Self-registration has failed, or no statics declared";

   // Spawn a new copy.
   TorqueThreadStaticListHandle testInstance = _TorqueThreadStaticReg::spawnThreadStaticsInstance();

   // Test the copy
   ASSERT_EQ(_TorqueThreadStaticReg::getStaticList(0).size(), testInstance->size())
      << "Spawned static list has a different size from master copy.";

   // Traverse the list and compare it to the initial value copy (index 0)
   for(S32 i = 0; i < _TorqueThreadStaticReg::getStaticList().size(); i++)
   {
      _TorqueThreadStatic *master = _TorqueThreadStaticReg::getStaticList()[i];
      _TorqueThreadStatic *cpy = (*testInstance)[i];

      // Make sure it is not the same memory
      EXPECT_NE(master, cpy)
         << "Copy not spawned properly.";

      // Make sure the sizes are the same
      ASSERT_EQ(master->getMemInstSize(), cpy->getMemInstSize())
         << "Size mismatch between master and copy";

      // Make sure the initialization occurred properly
      EXPECT_EQ(0, dMemcmp(master->getMemInstPtr(), cpy->getMemInstPtr(), master->getMemInstSize()))
         << "Initial value for spawned list is not correct";
   }

   // Test metrics if enabled
#ifdef TORQUE_ENABLE_THREAD_STATIC_METRICS
   U32 fooHitCount = (*testInstance)[_gUnitTestFooTorqueThreadStatic::getListIndex()]->getHitCount();
#endif

   // Set/get some values (If we test static metrics, this is hit 1)
   ATTS_(gUnitTestFoo, 1) = 55;

   // Test to see that the master copy and the spawned copy differ
   // (If we test metrics, this is hit 2, for the instance, and hit 1 for the master)
   EXPECT_NE(ATTS_(gUnitTestFoo, 0), ATTS_(gUnitTestFoo, 1))
      << "Assignment for spawned instanced memory failed";

#ifdef TORQUE_ENABLE_THREAD_STATIC_METRICS
   U32 fooHitCount2 = (*testInstance)[_gUnitTestFooTorqueThreadStatic::getListIndex()]->getHitCount();
   EXPECT_EQ(fooHitCount2, (fooHitCount + 2))
      << "Thread static metric hit count failed";
#endif

   // Destroy instances
   _TorqueThreadStaticReg::destroyInstance(testInstance);
}

#ifdef TORQUE_ENABLE_PROFILER
#include "math/mRandom.h"
#include "platform/profiler.h"

// Declare a test thread static
DITTS(U32, gInstancedStaticFoo, 42);
static U32 gTrueStaticFoo = 42;

TEST(ThreadStaticTest, StressThreadStatic)
{
   ASSERT_FALSE(gProfiler->isEnabled())
      << "Profiler is currently enabled, test cannot continue";

   // Spawn an instance
   TorqueThreadStaticListHandle testInstance = _TorqueThreadStaticReg::spawnThreadStaticsInstance();

   static const dsize_t TEST_SIZE = 100000;

   // What we are going to do in this test is to test some U32 static
   // performance. The test will be run TEST_SIZE times, and so first create
   // an array of values to standardize the tests on.
   U32 testValue[TEST_SIZE];

   for(S32 i = 0; i < TEST_SIZE; i++)
      testValue[i] = gRandGen.randI();

   // Reset the profiler, tell it to dump to console when done
   gProfiler->dumpToConsole();
   gProfiler->enable(true);

   // Value array is initialized, so lets put the foo's through the paces
   PROFILE_START(ThreadStaticPerf_TrueStaticAssign);
   for(int i = 0; i < TEST_SIZE; i++)
      gTrueStaticFoo = testValue[i];
   PROFILE_END();

   PROFILE_START(ThreadStaticPerf_InstanceStaticAssign);
   for(S32 i = 0; i < TEST_SIZE; i++)
      ATTS_(gInstancedStaticFoo, 1) = testValue[i];
   PROFILE_END();

   gProfiler->enable(false);

   // Clean up instance
   _TorqueThreadStaticReg::destroyInstance(testInstance);
}
#endif

#endif
