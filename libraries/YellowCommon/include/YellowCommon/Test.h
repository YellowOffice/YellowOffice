#ifndef YELLOW_COMMON_TEST_H
#define YELLOW_COMMON_TEST_H

// Tiny self-contained unit test framework for host-side and Haiku-side
// testing. Usage:
//
//   TEST(MyTest) {
//       CHECK(1 + 1 == 2);
//       CHECK_EQUAL(2, 1 + 1);
//   }
//
//   int main() { return YellowCommon::RunAllTests(); }

#include <stdio.h>
#include <string.h>

namespace YellowCommon {

typedef void (*TestFunction)();

struct TestCase {
    const char* name;
    TestFunction function;
};

const int kMaxTests = 512;

struct TestRegistry {
    TestCase tests[kMaxTests];
    int count;
    int failures;
    int currentTestFailures;

    static TestRegistry& Get() {
        static TestRegistry registry = { {}, 0, 0, 0 };
        return registry;
    }

    bool Add(const char* name, TestFunction function) {
        if (count >= kMaxTests) return false;
        tests[count].name = name;
        tests[count].function = function;
        ++count;
        return true;
    }
};

struct TestRegistrar {
    TestRegistrar(const char* name, TestFunction function) {
        TestRegistry::Get().Add(name, function);
    }
};

#define TEST(name) \
    static void name(); \
    static YellowCommon::TestRegistrar name##_registrar(#name, &name); \
    static void name()

inline void ReportFailure(const char* file, int line, const char* expression)
{
    TestRegistry& registry = TestRegistry::Get();
    ++registry.currentTestFailures;
    fprintf(stderr, "    FAIL %s:%d: %s\n", file, line, expression);
}

#define CHECK(expression) \
    do { \
        if (!(expression)) { \
            YellowCommon::ReportFailure(__FILE__, __LINE__, #expression); \
        } \
    } while (0)

#define CHECK_EQUAL(expected, actual) \
    do { \
        if (!((expected) == (actual))) { \
            YellowCommon::ReportFailure(__FILE__, __LINE__, #actual " == " #expected); \
        } \
    } while (0)

#define CHECK_STREQ(expected, actual) \
    do { \
        if (strcmp((expected), (actual)) != 0) { \
            YellowCommon::ReportFailure(__FILE__, __LINE__, #actual " == " #expected); \
        } \
    } while (0)

#define CHECK_NULL(pointer) CHECK((pointer) == nullptr)
#define CHECK_NOT_NULL(pointer) CHECK((pointer) != nullptr)

inline int RunAllTests()
{
    TestRegistry& registry = TestRegistry::Get();
    fprintf(stderr, "Running %d test(s)...\n", registry.count);
    for (int i = 0; i < registry.count; ++i) {
        registry.currentTestFailures = 0;
        fprintf(stderr, "  %s\n", registry.tests[i].name);
        registry.tests[i].function();
        if (registry.currentTestFailures > 0) {
            registry.failures += registry.currentTestFailures;
            fprintf(stderr, "  -> FAILED (%d check(s))\n", registry.currentTestFailures);
        }
    }
    if (registry.failures > 0) {
        fprintf(stderr, "\n%d failure(s) total.\n", registry.failures);
        return 1;
    }
    fprintf(stderr, "\nAll tests passed.\n");
    return 0;
}

} // namespace YellowCommon

#define YELLOW_RUN_TESTS() int main() { return YellowCommon::RunAllTests(); }

#endif // YELLOW_COMMON_TEST_H
