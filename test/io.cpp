#include "ce/io.h"

#include "gtest/gtest.h"

#include <iostream>

struct body_t
{
    ce::vec2<int> p;
    ce::vec2<int> steps[2];
};

struct world_t
{
    ce::uint64_t time;
    ce::list<body_t, 10> bodies;
};

CE_IO_AS(ce::vec2<int>, int[2]);
CE_IO_BIND(body_t, p, steps);
CE_IO_BIND(world_t, time, bodies);

// c-like jason example, i.e. json without quotes...

struct io_as_cjson_text : ce::io<io_as_cjson_text>
{
    char as[1024]{ };
    char const* prefix{ "" };
    char* at = as;

    void enter_class() { at = ce::to_text(at, prefix, '{'); prefix = " "; }
    void leave_class() { at = ce::to_text(at, " }"); }

    void enter_array(size_t&, bool) { at = ce::to_text(at, prefix, '['); prefix = ""; }
    void leave_array(size_t&, bool) { at = ce::to_text(at, ']'); prefix = ", "; }

    void name(char const* name) { at = ce::to_text(at, prefix, name); prefix = ": "; }
    
    template<class T> void atom(T& data) { at = ce::to_text(at, prefix, data); prefix = ", "; }

    template<class T>
    io_as_cjson_text(char const name[], T& data) { item(name, data); *at = '\0'; }
};

GTEST_TEST(io, io)
{

    int x = 42;
    EXPECT_STREQ(io_as_cjson_text("x", x).as, "x: 42");

    ce::vec2<int> v{ 1, 2 };
    EXPECT_STREQ(io_as_cjson_text("v", v).as, "v: [1, 2]");

    world_t w{ };
    EXPECT_STREQ(io_as_cjson_text("w", w).as, "w: { time: 0, bodies: [] }");

    w.bodies.size = 2;
    w.bodies[0] = { { 1, 2 }, { { 3, 4 }, { 5, 6 } } };
    w.bodies[1] = { { -1, -2 }, { { -3, -4 }, { -5, -6 } } };
    EXPECT_STREQ(io_as_cjson_text("w", w).as, "w: { time: 0, bodies: [{ p: [1, 2], steps: [[3, 4], [5, 6]] }, { p: [-1, -2], steps: [[-3, -4], [-5, -6]] }] }");

}