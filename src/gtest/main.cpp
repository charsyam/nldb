/*
 * (C) Copyright 2012 ThankyouSoft (http://ThankyouSoft.com/) and Nanolat(http://Nanolat.com).
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the NOSL(Nanolat Open Source License) v1.0.
 * 
 * Simple Description :
 * - If you are in USA, Canada, or Germany, it is *likely* that you get free LGPL v2.1.
 * - In other countries, you are *likely* under GPL v3, but you can by LGPL v2.1 from Nanolat Co., Ltd.
 * - For free software, you *definitely* get free LGPL v2.1 whereever you are.
 * 
 * Detailed Description :
 * - In the hope of seeing increasing open source contributions in countries 
 *   that do not have enough open source contributors, Nanolat Open Source License 
 *   provides free LGPL v2.1 only to companies and people of countries 
 *   that have at least three KudoRank 10 contributors in http://www.ohloh.net/people. 
 *   The list of countries are updated by the beginning of a year. 
 *   In 2013, these countries are USA, Canada, and Germany.
 *
 * - For companies and people of other countries, you are under GPL v3.0, 
 *   but you can buy LGPL v2.1 license for a product from Nanolat Co., Ltd.
 *
 * - However, for non-commercial usage, you get free LGPL v2.1 whereever you are.
 *
 * - A company is of a country if the head of office is in the country.
 *
 * - A person is of a country if the nationality of the person is the country.
 *
 * - A country has a KudoRank 10 contributor 
 *   if the contributor's location is the country in http://www.ohloh.net/.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *
 * Contributors:
 *     Kangmo Kim
 */


#include <iostream>

#include "gtest/gtest.h"

/*
TEST(sample_test_case, sample_test) 
{ 
    EXPECT_EQ(1, 1); 
} 

void Foo(float value) {
	printf("value=>%f",value);
}


TEST(sample_test_case, tbb) 
{ 
	float v[] = {0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f};
    // BUGBUG write some test code
} 
*/

/*
 * List of tests to add.
 * 1) Get by Order.
 *    (LevelDB, TC should fail, ArrayTree should succeed)
 * 2) Get by Order. Open Cursor with a Key, move forward until it reaches to the end.
 *    (LevelDB, TC should fail, ArrayTree should succeed)
 * 3) Get by Key with Order Statistics.
 *    (LevelDB, TC should fail, ArrayTree should succeed)
 * 4) Get table statistics
 *    (LevelDB, TC should fail, ArrayTree should succeed)
 */
int main(int argc, char** argv)  
{  
    testing::InitGoogleTest(&argc, argv);  
    return RUN_ALL_TESTS();
} 


