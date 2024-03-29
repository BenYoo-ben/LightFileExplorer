#include "gtest/gtest.h"
#include <unistd.h>
#include "objects.hpp"

TEST (OBJECTS, FILE_OBJECT) {
    file_object fObj("NAME", "TIME", "AUTH", "1000", "1");
    fObj.set_type("TYPE");

    ASSERT_EQ(fObj.get_name(), "NAME");
    ASSERT_EQ(fObj.get_time(), "TIME");
    ASSERT_EQ(fObj.get_type(), "TYPE");
    ASSERT_EQ(fObj.get_auth(), "AUTH");

    ASSERT_EQ(fObj.get_size(), 1000);
    ASSERT_EQ(fObj.get_is_dir(), true);
}

TEST (JSON_HANDLER, DIRECTORY_TO_FILE_OBJECT_VECTOR) {
    json_handler jh;
    std::vector<file_object> fObjs;

    // Test Success
    ASSERT_EQ(jh.directory_to_file_object_vector("./unittest/testDirectory", &fObjs) >= 0, true);

    ASSERT_EQ(fObjs.size() == 4, true);

    // Test Failure(no dir)
    fObjs.clear();
    ASSERT_EQ(jh.directory_to_file_object_vector("./unittest/emptyDirectory", &fObjs) < 0, true);

    // Test Failure(yes dir, loop file))
    fObjs.clear();
    ASSERT_EQ(jh.directory_to_file_object_vector("./unittest/testLoopDirectory", &fObjs) < 0, true);
}

TEST (JSON_HANDLER, MAKE_JSON_OBJECT) {
    json_handler jh;
    Json::Value jv;

    ASSERT_EQ(jh.make_json_object("./unittest/testDirectory", &jv) >= 0, true);

    bool is1Found = false;
    for (auto itr : jv) {
        if (itr["name"] == "1") {
            if (itr["size"] != "512") {
                printf("size fail! ::: %s\n", itr["size"].asString().c_str());
                break;
            }

            if (itr["is_dir"] != "0") {
                printf("is_dir fail! ::: %s\n", itr["is_dir"].asString().c_str());
                break;
            }

            is1Found = true;
        }
    }

    ASSERT_EQ(is1Found, true);


    // Test Failure(no dir)
    ASSERT_EQ(jh.make_json_object("./unittest/emptyDirectory", &jv) < 0, true);
}

TEST (OBJECTS, TEST_LOCK) {
    lock_handler lh;

    std::string testString("TESTSTRING");

    // Initial State
    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), true);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), true);

    lh.add_lock(lh.SOFT_LOCK, testString);

    // S(1) H(0)
    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), false);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), true);

    lh.remove_lock(lh.SOFT_LOCK, testString);

    // S(0) H(0)
    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), true);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), true);

    lh.add_lock(lh.HARD_LOCK, testString);

    // S(0) H(1)
    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), true);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), false);

    lh.remove_lock(lh.HARD_LOCK, testString);

    // S(0) H(0)
    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), true);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), true);

    lh.add_lock(lh.SOFT_LOCK, testString);
    lh.add_lock(lh.HARD_LOCK, testString);

    // S(1) H(1)
    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), false);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), false);

    lh.remove_lock(lh.SOFT_LOCK, testString);
    lh.remove_lock(lh.HARD_LOCK, testString);

    // S(0) H(0)
    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), true);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), true);

    lh.add_lock(lh.SOFT_LOCK, testString);
    lh.add_lock(lh.SOFT_LOCK, testString);

    lh.add_lock(lh.HARD_LOCK, testString);
    lh.add_lock(lh.HARD_LOCK, testString);

    // S(2) H(2)
    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), false);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), false);

    lh.remove_lock(lh.SOFT_LOCK, testString);
    lh.remove_lock(lh.HARD_LOCK, testString);

    // S(1) H(1)
    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), false);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), false);

    lh.remove_lock(lh.SOFT_LOCK, testString);
    lh.remove_lock(lh.HARD_LOCK, testString);

    // S(0) H(0)
    ASSERT_EQ(lh.check_lock(lh.SOFT_LOCK, testString), true);
    ASSERT_EQ(lh.check_lock(lh.HARD_LOCK, testString), true);
}




