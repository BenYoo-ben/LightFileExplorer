#include "gtest/gtest.h"

#include "objects.hpp"

TEST (OBJECT, TEST_LOCK) {
    lock_handler lh;

    std::string testString("TESTSTRING");

    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), true);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), true);

    lh.add_lock(lh.SOFT_LOCK, testString);

    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), false);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), true);

    lh.remove_lock(lh.SOFT_LOCK, testString);

    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), true);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), true);

    lh.add_lock(lh.HARD_LOCK, testString);

    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), true);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), false);

    lh.remove_lock(lh.HARD_LOCK, testString);

    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), true);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), true);

    lh.add_lock(lh.SOFT_LOCK, testString);
    lh.add_lock(lh.HARD_LOCK, testString);

    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), false);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), false);

    lh.remove_lock(lh.SOFT_LOCK, testString);
    lh.remove_lock(lh.HARD_LOCK, testString);

    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), true);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), true);
}


