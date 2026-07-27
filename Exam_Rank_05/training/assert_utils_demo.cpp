#include "assert_utils.hpp"
#include "../vect2/vect2.hpp"

int main() {
    TestSuite suite;

    vect2 a(2, 2);
    vect2 b(1, 3);

    vect2 sum = a + b;
    suite.assertEqual(3, sum.getX(), "a + b -> x");
    suite.assertEqual(5, sum.getY(), "a + b -> y");

    vect2 diff = a - b;
    suite.assertEqual(1, diff.getX(), "a - b -> x");
    suite.assertEqual(-1, diff.getY(), "a - b -> y");

    // a deliberately wrong expectation, so you can see a [FAIL] line too
    suite.assertEqual(99, sum.getX(), "a + b -> x (should fail on purpose)");

    vect2 c(5, 5);
    vect2 old = c++;
    suite.assertEqual(5, old.getX(), "post-increment returns OLD value");
    suite.assertEqual(6, c.getX(), "post-increment mutates original");

    suite.summary();
}
