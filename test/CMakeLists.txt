include(FetchContent)


FetchContent_Declare(googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(googletest)

add_executable(TestParser ParserTest.cpp)
add_executable(TestCode CodeTest.cpp)

target_link_libraries(TestCode PRIVATE Code GTest::gtest_main)
target_link_libraries(TestParser PRIVATE Parser GTest::gtest_main)

include(GoogleTest)

gtest_discover_tests(TestParser)
gtest_discover_tests(TestCode)

