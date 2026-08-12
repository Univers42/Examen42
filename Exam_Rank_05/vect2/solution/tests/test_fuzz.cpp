#include "../vect2.hpp"
#include "vtest.hpp"
#include <climits>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sstream>
#include <string>

// ---------------------------------------------------------------------------
// Differential fuzzing.
//
// Every vect2 in play is shadowed by a `Model` holding the same two numbers in
// `long` (64-bit here), which is wide enough that the reference arithmetic
// never wraps. Before an operation is applied we compute the answer in the
// model; if that answer would not fit in an int we skip the operation, because
// signed overflow is undefined behaviour and vect2 cannot be blamed for it.
// Otherwise we apply it for real and compare the entire state, read back both
// through operator[] and through operator<<.
// ---------------------------------------------------------------------------

namespace {

const int NOBJ = 6;

struct Model {
    long x;
    long y;
};

bool fits(long v) { return (v >= (long)INT_MIN && v <= (long)INT_MAX); }
bool fits2(long a, long b) { return (fits(a) && fits(b)); }

// std::rand() is only guaranteed to reach 32767, so stack a few draws to cover
// the whole int range.
unsigned long rawRandom() {
    unsigned long r = 0;
    for (int i = 0; i < 3; ++i)
        r = (r << 11) ^ (unsigned long)std::rand();
    return (r);
}

long rndRange(long lo, long hi) {
    unsigned long span = (unsigned long)(hi - lo) + 1UL;
    return (lo + (long)(rawRandom() % span));
}

int rndIndex(int n) { return ((int)(rawRandom() % (unsigned long)n)); }

// Component values: mostly small (so operations actually land instead of being
// skipped for overflow), with a tail of huge and extreme values.
int rndValue() {
    int bucket = rndIndex(100);
    if (bucket < 55)
        return ((int)rndRange(-1000, 1000));
    if (bucket < 80)
        return ((int)rndRange(-1000000, 1000000));
    if (bucket < 92)
        return ((int)rndRange((long)INT_MIN, (long)INT_MAX));
    static const int extreme[] = {
        0, 1, -1, 2, -2, INT_MAX, INT_MIN, INT_MAX - 1, INT_MIN + 1,
        INT_MAX / 2, INT_MIN / 2
    };
    return (extreme[rndIndex((int)(sizeof(extreme) / sizeof(extreme[0])))]);
}

int rndScalar() {
    int bucket = rndIndex(100);
    if (bucket < 65)
        return ((int)rndRange(-10, 10));
    if (bucket < 88)
        return ((int)rndRange(-100000, 100000));
    static const int extreme[] = { 0, 1, -1, 2, -2, INT_MAX, INT_MIN };
    return (extreme[rndIndex((int)(sizeof(extreme) / sizeof(extreme[0])))]);
}

std::string modelText(const Model& m) {
    std::ostringstream os;
    os << "{" << m.x << ", " << m.y << "}";
    return (os.str());
}

std::string vectText(const vect2& v) {
    std::ostringstream os;
    os << v;
    return (os.str());
}

// Reads the whole state back two independent ways and compares it to the model.
bool stateMatches(const vect2 v[], const Model m[], std::string& why) {
    for (int i = 0; i < NOBJ; ++i) {
        std::ostringstream os;
        if ((long)v[i][0] != m[i].x || (long)v[i][1] != m[i].y) {
            os << "v[" << i << "] via operator[]: expected " << modelText(m[i])
               << ", got {" << v[i][0] << ", " << v[i][1] << "}";
            why = os.str();
            return (false);
        }
        if (vectText(v[i]) != modelText(m[i])) {
            os << "v[" << i << "] via operator<<: expected " << modelText(m[i])
               << ", got " << vectText(v[i]);
            why = os.str();
            return (false);
        }
    }
    return (true);
}

const int OP_COUNT = 25;

// Returns 1 when the operation ran, 0 when it was skipped to avoid signed
// overflow. `what` describes the operation for the failure report.
int applyOp(int op, vect2 v[], Model m[], std::string& what) {
    int a = rndIndex(NOBJ);
    int b = rndIndex(NOBJ);
    int c = rndIndex(NOBJ);
    int n = rndScalar();
    std::ostringstream tag;

    switch (op) {
    case 0:
        tag << "v[" << a << "] = v[" << b << "]";
        what = tag.str();
        v[a] = v[b];
        m[a] = m[b];
        return (1);

    case 1:
        tag << "v[" << a << "] = v[" << a << "]  (self-assign)";
        what = tag.str();
        v[a] = v[a];
        return (1);

    case 2: {
        long nx = m[a].x + m[b].x, ny = m[a].y + m[b].y;
        tag << "v[" << a << "] += v[" << b << "]";
        what = tag.str();
        if (!fits2(nx, ny))
            return (0);
        vect2& r = (v[a] += v[b]);
        if (&r != &v[a]) { what += "  [+= did not return *this]"; return (-1); }
        m[a].x = nx; m[a].y = ny;
        return (1);
    }

    case 3: {
        long nx = m[a].x - m[b].x, ny = m[a].y - m[b].y;
        tag << "v[" << a << "] -= v[" << b << "]";
        what = tag.str();
        if (!fits2(nx, ny))
            return (0);
        vect2& r = (v[a] -= v[b]);
        if (&r != &v[a]) { what += "  [-= did not return *this]"; return (-1); }
        m[a].x = nx; m[a].y = ny;
        return (1);
    }

    case 4: {
        long nx = m[a].x * (long)n, ny = m[a].y * (long)n;
        tag << "v[" << a << "] *= " << n;
        what = tag.str();
        if (!fits2(nx, ny))
            return (0);
        vect2& r = (v[a] *= n);
        if (&r != &v[a]) { what += "  [*= did not return *this]"; return (-1); }
        m[a].x = nx; m[a].y = ny;
        return (1);
    }

    case 5: {
        long nx = m[a].x + m[b].x, ny = m[a].y + m[b].y;
        tag << "v[" << c << "] = v[" << a << "] + v[" << b << "]";
        what = tag.str();
        if (!fits2(nx, ny))
            return (0);
        v[c] = v[a] + v[b];
        m[c].x = nx; m[c].y = ny;
        return (1);
    }

    case 6: {
        long nx = m[a].x - m[b].x, ny = m[a].y - m[b].y;
        tag << "v[" << c << "] = v[" << a << "] - v[" << b << "]";
        what = tag.str();
        if (!fits2(nx, ny))
            return (0);
        v[c] = v[a] - v[b];
        m[c].x = nx; m[c].y = ny;
        return (1);
    }

    case 7: {
        long nx = m[a].x * (long)n, ny = m[a].y * (long)n;
        tag << "v[" << c << "] = v[" << a << "] * " << n;
        what = tag.str();
        if (!fits2(nx, ny))
            return (0);
        v[c] = v[a] * n;
        m[c].x = nx; m[c].y = ny;
        return (1);
    }

    case 8: {
        long nx = (long)n * m[a].x, ny = (long)n * m[a].y;
        tag << "v[" << c << "] = " << n << " * v[" << a << "]";
        what = tag.str();
        if (!fits2(nx, ny))
            return (0);
        v[c] = n * v[a];
        m[c].x = nx; m[c].y = ny;
        return (1);
    }

    case 9: {
        long nx = -m[a].x, ny = -m[a].y;
        tag << "v[" << c << "] = -v[" << a << "]";
        what = tag.str();
        if (!fits2(nx, ny))          // -INT_MIN does not fit
            return (0);
        v[c] = -v[a];
        m[c].x = nx; m[c].y = ny;
        return (1);
    }

    case 10: {
        long nx = m[a].x + 1, ny = m[a].y + 1;
        tag << "++v[" << a << "]";
        what = tag.str();
        if (!fits2(nx, ny))
            return (0);
        vect2& r = ++v[a];
        if (&r != &v[a]) { what += "  [++v did not return *this]"; return (-1); }
        m[a].x = nx; m[a].y = ny;
        return (1);
    }

    case 11: {
        long ox = m[a].x, oy = m[a].y;
        long nx = ox + 1, ny = oy + 1;
        tag << "v[" << a << "]++";
        what = tag.str();
        if (!fits2(nx, ny))
            return (0);
        vect2 old = v[a]++;
        if ((long)old[0] != ox || (long)old[1] != oy) {
            what += "  [v++ did not yield the old value]";
            return (-1);
        }
        m[a].x = nx; m[a].y = ny;
        return (1);
    }

    case 12: {
        long nx = m[a].x - 1, ny = m[a].y - 1;
        tag << "--v[" << a << "]";
        what = tag.str();
        if (!fits2(nx, ny))
            return (0);
        vect2& r = --v[a];
        if (&r != &v[a]) { what += "  [--v did not return *this]"; return (-1); }
        m[a].x = nx; m[a].y = ny;
        return (1);
    }

    case 13: {
        long ox = m[a].x, oy = m[a].y;
        long nx = ox - 1, ny = oy - 1;
        tag << "v[" << a << "]--";
        what = tag.str();
        if (!fits2(nx, ny))
            return (0);
        vect2 old = v[a]--;
        if ((long)old[0] != ox || (long)old[1] != oy) {
            what += "  [v-- did not yield the old value]";
            return (-1);
        }
        m[a].x = nx; m[a].y = ny;
        return (1);
    }

    case 14: {
        int idx = rndIndex(2);
        int val = rndValue();
        tag << "v[" << a << "][" << idx << "] = " << val;
        what = tag.str();
        v[a][idx] = val;
        if (idx == 0) m[a].x = val; else m[a].y = val;
        return (1);
    }

    case 15: {
        long nx = m[a].x * 2, ny = m[a].y * 2;
        tag << "v[" << a << "] += v[" << a << "]  (aliased)";
        what = tag.str();
        if (!fits2(nx, ny))
            return (0);
        v[a] += v[a];
        m[a].x = nx; m[a].y = ny;
        return (1);
    }

    case 16:
        tag << "v[" << a << "] -= v[" << a << "]  (aliased, zeroes)";
        what = tag.str();
        v[a] -= v[a];
        m[a].x = 0; m[a].y = 0;
        return (1);

    case 17: {
        // v[a] += v[a] += v[b] : the inner call runs first and returns a
        // reference to v[a], so the outer one always doubles the new value.
        long ix = m[a].x + m[b].x, iy = m[a].y + m[b].y;
        long fx = ix * 2, fy = iy * 2;
        tag << "v[" << a << "] += v[" << a << "] += v[" << b << "]";
        what = tag.str();
        if (!fits2(ix, iy) || !fits2(fx, fy))
            return (0);
        v[a] += v[a] += v[b];
        m[a].x = fx; m[a].y = fy;
        return (1);
    }

    case 18: {
        // (v[a] += v[b]) += v[c] : v[c] is read *after* the first step, which
        // matters when c aliases a.
        long ax = m[a].x + m[b].x, ay = m[a].y + m[b].y;
        long cx = (c == a) ? ax : m[c].x;
        long cy = (c == a) ? ay : m[c].y;
        long fx = ax + cx, fy = ay + cy;
        tag << "(v[" << a << "] += v[" << b << "]) += v[" << c << "]";
        what = tag.str();
        if (!fits2(ax, ay) || !fits2(fx, fy))
            return (0);
        (v[a] += v[b]) += v[c];
        m[a].x = fx; m[a].y = fy;
        return (1);
    }

    case 19:
        tag << "v[" << a << "] = v[" << b << "] = v[" << c << "]";
        what = tag.str();
        v[a] = v[b] = v[c];
        m[b] = m[c];
        m[a] = m[b];
        return (1);

    case 20:
        // Algebraic identity: whatever v[a] is, this must leave it alone, and
        // no intermediate can overflow (a - a is always 0).
        tag << "v[" << a << "] = v[" << a << "] - v[" << a << "] + v[" << a << "]";
        what = tag.str();
        v[a] = v[a] - v[a] + v[a];
        return (1);

    case 21: {
        // Copy ctor round trip: a copy must be equal to, and independent of,
        // its source.
        tag << "vect2 t(v[" << a << "]) ; v[" << c << "] = t";
        what = tag.str();
        vect2 t(v[a]);
        if (!(t == v[a])) { what += "  [copy != source]"; return (-1); }
        v[c] = t;
        m[c] = m[a];
        return (1);
    }

    case 22: {
        // Compound and binary forms must agree.
        long nx = m[a].x * (long)n, ny = m[a].y * (long)n;
        tag << "compound vs binary  * " << n << " on v[" << a << "]";
        what = tag.str();
        if (!fits2(nx, ny))
            return (0);
        vect2 lhs(v[a]);
        lhs *= n;
        if (!(lhs == v[a] * n)) { what += "  [*= and * disagree]"; return (-1); }
        v[c] = lhs;
        m[c].x = nx; m[c].y = ny;
        return (1);
    }

    case 23: {
        // == / != against the model, and != as the exact negation of ==.
        bool modelEq = (m[a].x == m[b].x && m[a].y == m[b].y);
        bool got = (v[a] == v[b]);
        bool gotNe = (v[a] != v[b]);
        tag << "v[" << a << "] == v[" << b << "]";
        what = tag.str();
        if (got != modelEq) { what += "  [== disagrees with the model]"; return (-1); }
        if (gotNe == got)   { what += "  [!= is not the negation of ==]"; return (-1); }
        return (1);
    }

    case 24: {
        // (a + b) - b == a
        long sx = m[a].x + m[b].x, sy = m[a].y + m[b].y;
        tag << "v[" << c << "] = (v[" << a << "] + v[" << b << "]) - v[" << b << "]";
        what = tag.str();
        if (!fits2(sx, sy))
            return (0);
        vect2 got = (v[a] + v[b]) - v[b];
        if (!(got == v[a])) { what += "  [(a+b)-b != a]"; return (-1); }
        v[c] = got;
        m[c] = m[a];
        return (1);
    }
    }
    return (0);
}

// ---------------------------------------------------------------------------
// Random expression trees: builds arbitrarily nested expressions such as
// -(3 * (a + b) - c) * -2, evaluates each one with vect2 and with the model,
// and compares. This reaches operator combinations a flat op list never does.
// ---------------------------------------------------------------------------

struct Node {
    int   kind;                 // 0 leaf, 1 add, 2 sub, 3 neg, 4 v*n, 5 n*v
    int   lx, ly;               // leaf components
    int   n;                    // scalar for kinds 4 and 5
    Node* a;
    Node* b;
};

Node* buildTree(int depth) {
    Node* node = new Node;
    node->a = 0;
    node->b = 0;
    node->n = 0;
    node->lx = 0;
    node->ly = 0;

    if (depth <= 0 || rndIndex(4) == 0) {
        // Mostly small leaves, so that deep trees still land inside int and
        // actually get evaluated -- but with a tail drawn from the full range
        // (INT_MAX, INT_MIN and friends), so nested expressions are pushed to
        // the edges too. Trees that would overflow are dropped by evalModel.
        node->kind = 0;
        if (rndIndex(100) < 82) {
            node->lx = (int)rndRange(-1000, 1000);
            node->ly = (int)rndRange(-1000, 1000);
        } else {
            node->lx = rndValue();
            node->ly = rndValue();
        }
        return (node);
    }
    node->kind = 1 + rndIndex(5);
    node->a = buildTree(depth - 1);
    if (node->kind == 1 || node->kind == 2)
        node->b = buildTree(depth - 1);
    if (node->kind == 4 || node->kind == 5)
        node->n = (int)rndRange(-8, 8);
    return (node);
}

void freeTree(Node* node) {
    if (!node)
        return;
    freeTree(node->a);
    freeTree(node->b);
    delete node;
}

Model evalModel(const Node* node, bool& ok) {
    Model r;
    r.x = 0;
    r.y = 0;
    if (!ok)
        return (r);

    switch (node->kind) {
    case 0:
        r.x = node->lx;
        r.y = node->ly;
        break;
    case 1: {
        Model l = evalModel(node->a, ok), s = evalModel(node->b, ok);
        r.x = l.x + s.x; r.y = l.y + s.y;
        break;
    }
    case 2: {
        Model l = evalModel(node->a, ok), s = evalModel(node->b, ok);
        r.x = l.x - s.x; r.y = l.y - s.y;
        break;
    }
    case 3: {
        Model l = evalModel(node->a, ok);
        r.x = -l.x; r.y = -l.y;
        break;
    }
    default: {
        Model l = evalModel(node->a, ok);
        r.x = l.x * (long)node->n; r.y = l.y * (long)node->n;
        break;
    }
    }
    if (!fits2(r.x, r.y))
        ok = false;
    return (r);
}

vect2 evalVect(const Node* node) {
    switch (node->kind) {
    case 0:  return (vect2(node->lx, node->ly));
    case 1:  return (evalVect(node->a) + evalVect(node->b));
    case 2:  return (evalVect(node->a) - evalVect(node->b));
    case 3:  return (-evalVect(node->a));
    case 4:  return (evalVect(node->a) * node->n);
    default: return (node->n * evalVect(node->a));
    }
}

std::string treeText(const Node* node) {
    std::ostringstream os;
    switch (node->kind) {
    case 0:  os << "{" << node->lx << ", " << node->ly << "}"; break;
    case 1:  os << "(" << treeText(node->a) << " + " << treeText(node->b) << ")"; break;
    case 2:  os << "(" << treeText(node->a) << " - " << treeText(node->b) << ")"; break;
    case 3:  os << "-(" << treeText(node->a) << ")"; break;
    case 4:  os << "(" << treeText(node->a) << " * " << node->n << ")"; break;
    default: os << "(" << node->n << " * " << treeText(node->a) << ")"; break;
    }
    return (os.str());
}

// ---------------------------------------------------------------------------

int statefulCampaign(long steps, unsigned seed) {
    vect2 v[NOBJ];
    Model m[NOBJ];

    for (int i = 0; i < NOBJ; ++i) {
        int x = rndValue(), y = rndValue();
        v[i] = vect2(x, y);
        m[i].x = x;
        m[i].y = y;
    }

    std::string why;
    if (!stateMatches(v, m, why)) {
        std::cout << "  [KO] seeding the state already diverged: " << why << "\n";
        return (1);
    }

    long applied = 0, skipped = 0;
    int  failures = 0;

    for (long step = 0; step < steps; ++step) {
        std::string what;
        int result = applyOp(rndIndex(OP_COUNT), v, m, what);

        if (result == 0) {
            skipped++;
            continue;
        }
        applied++;
        if (result < 0) {
            std::cout << "  [KO] seed " << seed << " step " << step
                      << ": " << what << "\n";
            if (++failures >= 5)
                return (failures);
            continue;
        }
        if (!stateMatches(v, m, why)) {
            std::cout << "  [KO] seed " << seed << " step " << step
                      << " after  " << what << "\n         " << why << "\n";
            if (++failures >= 5)
                return (failures);
            // Resynchronise so one bug does not drown the report.
            for (int i = 0; i < NOBJ; ++i) {
                m[i].x = v[i][0];
                m[i].y = v[i][1];
            }
        }
    }

    std::cout << "  " << applied << " operations applied, " << skipped
              << " skipped (would overflow int)\n";
    vtest::check(0, failures, "stateful differential fuzz");
    return (failures);
}

int expressionCampaign(long trees, unsigned seed) {
    long evaluated = 0, skipped = 0;
    int  failures = 0;

    for (long i = 0; i < trees; ++i) {
        Node* root = buildTree(1 + rndIndex(6));
        bool  ok = true;
        Model expected = evalModel(root, ok);

        if (!ok) {
            skipped++;
            freeTree(root);
            continue;
        }
        evaluated++;
        vect2 got = evalVect(root);
        if ((long)got[0] != expected.x || (long)got[1] != expected.y) {
            std::cout << "  [KO] seed " << seed << " tree " << i << ": "
                      << treeText(root) << "\n         expected "
                      << modelText(expected) << ", got " << vectText(got) << "\n";
            if (++failures >= 5) {
                freeTree(root);
                break;
            }
        }
        freeTree(root);
    }

    std::cout << "  " << evaluated << " expression trees evaluated, " << skipped
              << " skipped (would overflow int)\n";
    vtest::check(0, failures, "random expression trees");
    return (failures);
}

}  // namespace

int main(int argc, char** argv) {
    unsigned seed  = 42;
    long     steps = 200000;
    long     trees = 20000;

    for (int i = 1; i < argc; ++i) {
        if (!std::strcmp(argv[i], "--seed") && i + 1 < argc)
            seed = (unsigned)std::strtoul(argv[++i], 0, 10);
        else if (!std::strcmp(argv[i], "--steps") && i + 1 < argc)
            steps = std::strtol(argv[++i], 0, 10);
        else if (!std::strcmp(argv[i], "--trees") && i + 1 < argc)
            trees = std::strtol(argv[++i], 0, 10);
        else if (!std::strcmp(argv[i], "--random"))
            seed = (unsigned)std::time(0);
    }

    if (sizeof(long) < 8)
        std::cout << "note: `long` is " << (int)sizeof(long)
                  << " bytes here, so the reference model cannot outrange int;"
                     " overflow skipping will be conservative.\n";

    std::cout << "seed = " << seed << "\n";
    std::srand(seed);

    vtest::section("Stateful differential fuzz");
    statefulCampaign(steps, seed);

    vtest::section("Random expression trees");
    expressionCampaign(trees, seed);

    return (vtest::summary("fuzz campaigns"));
}
