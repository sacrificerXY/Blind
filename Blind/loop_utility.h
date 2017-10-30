#ifndef LOOP_UTILITY_H_INCLUDED
#define LOOP_UTILITY_H_INCLUDED

struct XYLoopCounter
{
    int x = 0;
    int y = 0;
    int max_x;
    int max_y;
    bool is_outer_loop_x;

    bool inner_loop_end = false;
    bool outer_loop_end = false;

    XYLoopCounter(int mx, int my, bool outer_is_x);
    void increment();
};

#endif // LOOP_UTILITY_H_INCLUDED
