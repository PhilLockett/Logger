/**
 * @file    unittest.cpp
 * @author  Phil Lockett <phillockett65@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * Statistics Implementation.
 */

#include <iostream>

#include "unittest.h"

std::string UnitTest_c::testCase = "UNDEFINED";
std::string UnitTest_c::description = "UNDEFINED";
std::string UnitTest_c::condition = "UNDEFINED";
bool UnitTest_c::verbose = true;
int UnitTest_c::errors = 0;



/**
 * Send the current name-value pairs to the output stream.
 *
 * @param  os - Output stream.
 */
void UnitTest_c::display(std::ostream &os) const
{
     os << "\tTest Case:\t" << testCase << "()\n";
     os << "\tDescription:\t" << description << "()\n";
     os << "\tVerbose:\t" << verbose << '\n';
}

void UnitTest_c::progress(const std::string & test, const std::string & desc)
{
    testCase = test;
    description = desc;

    if (verbose)
        std::cout << '\t' << testCase << " - " << description << '\n';
}

void UnitTest_c::failure(const std::string & cond)
{
    errors++;
    condition = cond;
    std::cerr << '\n';
    std::cerr << "While running " << testCase << " (" << description << "):\n";
    std::cerr << "\tRequirement (" << condition << ") failed\n";
    std::cerr << '\n';
}

