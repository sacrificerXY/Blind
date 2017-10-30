#include "stdafx.h"
#include "loop_utility.h"

#include <iostream>

using namespace std;

XYLoopCounter::XYLoopCounter(int mx, int my, bool outer_is_x)
    : max_x(mx), max_y(my), is_outer_loop_x(outer_is_x)
{
}

void XYLoopCounter::increment()
{
    if (is_outer_loop_x)
    {
        ++y;
//        cout << "x inner++\n";
        if (y >= max_y)
        {
            y = 0;
            inner_loop_end = true;
            ++x;
//            cout << "x outer++\n";
            if (x >= max_x)
            {
//                cout << "x out end++\n";
                outer_loop_end = true;
            }
        }
        else inner_loop_end = false;
    }
    else
    {
        ++x;
//        cout << "y inner++\n";
        if (x >= max_x)
        {
            x = 0;
            inner_loop_end = true;
            ++y;
//            cout << "y outer++\n";
            if (y >= max_y)
            {
//                cout << "y out end++\n";
                outer_loop_end = true;
            }
        }
        else inner_loop_end = false;
    }
}
