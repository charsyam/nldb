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


using namespace std;

#include "nldb_cursor_test.h"

#include "nldb_data.h"

TEST_F(NLDBCursorTest, find_existing_fetch_forward) {

	ASSERT_TRUE( nldb_cursor_seek( cursor, NLDB_CURSOR_FORWARD, KEY(r6) ) == 0);

	nldb_key_t key;
	nldb_value_t value;

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r6, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r6, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r7, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r7, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r8, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r8, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == NLDB_ERROR_END_OF_ITERATION );

}


TEST_F(NLDBCursorTest, find_existing_fetch_backward) {

	ASSERT_TRUE( nldb_cursor_seek( cursor, NLDB_CURSOR_BACKWARD, KEY(r3) ) == 0);

	nldb_key_t key;
	nldb_value_t value;

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r3, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r3, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r2, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r2, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r1, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r1, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == NLDB_ERROR_END_OF_ITERATION );
}


TEST_F(NLDBCursorTest, find_not_existing_fetch_forward) {

	ASSERT_TRUE( nldb_cursor_seek( cursor, NLDB_CURSOR_FORWARD, KEY(r_5_6) ) == 0);

	nldb_key_t key;
	nldb_value_t value;

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r6, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r6, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r7, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r7, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r8, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r8, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == NLDB_ERROR_END_OF_ITERATION );

}

TEST_F(NLDBCursorTest, find_not_existing_fetch_backward) {

	ASSERT_TRUE( nldb_cursor_seek( cursor, NLDB_CURSOR_BACKWARD, KEY(r_3_4) ) == 0);

	nldb_key_t key;
	nldb_value_t value;

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r3, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r3, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r2, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r2, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r1, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r1, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == NLDB_ERROR_END_OF_ITERATION );
}


TEST_F(NLDBCursorTest, find_min_fetch_forward) {

	ASSERT_TRUE( nldb_cursor_seek( cursor, NLDB_CURSOR_FORWARD, KEY(r1) ) == 0);

	nldb_key_t key;
	nldb_value_t value;

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r1, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r1, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r2, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r2, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r3, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r3, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r4, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r4, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r5, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r5, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r6, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r6, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r7, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r7, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r8, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r8, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == NLDB_ERROR_END_OF_ITERATION );

}

TEST_F(NLDBCursorTest, find_min_fetch_backward) {

	ASSERT_TRUE( nldb_cursor_seek( cursor, NLDB_CURSOR_BACKWARD, KEY(r1) ) == 0);

	nldb_key_t key;
	nldb_value_t value;

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r1, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r1, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == NLDB_ERROR_END_OF_ITERATION );
}


TEST_F(NLDBCursorTest, find_max_fetch_forward) {
	ASSERT_TRUE( nldb_cursor_seek( cursor, NLDB_CURSOR_FORWARD, KEY(r8) ) == 0);

	nldb_key_t key;
	nldb_value_t value;

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r8, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r8, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == NLDB_ERROR_END_OF_ITERATION );

}

TEST_F(NLDBCursorTest, find_max_fetch_backward) {
	ASSERT_TRUE( nldb_cursor_seek( cursor, NLDB_CURSOR_BACKWARD, KEY(r8) ) == 0);

	nldb_key_t key;
	nldb_value_t value;

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r8, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r8, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r7, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r7, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r6, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r6, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r5, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r5, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r4, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r4, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r3, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r3, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r2, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r2, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r1, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r1, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == NLDB_ERROR_END_OF_ITERATION );
}

TEST_F(NLDBCursorTest, find_before_min_fetch_forward) {

	ASSERT_TRUE( nldb_cursor_seek( cursor, NLDB_CURSOR_FORWARD, KEY(r_bmin) ) == 0);

	nldb_key_t key;
	nldb_value_t value;

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r1, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r1, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r2, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r2, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r3, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r3, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r4, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r4, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r5, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r5, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r6, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r6, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r7, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r7, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r8, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r8, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == NLDB_ERROR_END_OF_ITERATION );

}

TEST_F(NLDBCursorTest, find_before_min_fetch_backward) {

	ASSERT_TRUE( nldb_cursor_seek( cursor, NLDB_CURSOR_BACKWARD, KEY(r_bmin) ) == 0);

	nldb_key_t key;
	nldb_value_t value;

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == NLDB_ERROR_END_OF_ITERATION );
}


TEST_F(NLDBCursorTest, find_after_max_fetch_forward) {
	ASSERT_TRUE( nldb_cursor_seek( cursor, NLDB_CURSOR_FORWARD, KEY(r_amax) ) == 0);

	nldb_key_t key;
	nldb_value_t value;

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == NLDB_ERROR_END_OF_ITERATION );
}

TEST_F(NLDBCursorTest, find_after_max_fetch_backward) {
	ASSERT_TRUE( nldb_cursor_seek( cursor, NLDB_CURSOR_BACKWARD, KEY(r_amax) ) == 0);

	nldb_key_t key;
	nldb_value_t value;

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r8, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r8, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r7, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r7, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r6, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r6, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r5, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r5, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r4, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r4, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r3, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r3, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r2, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r2, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == 0 );
	EXPECT_TRUE( IS_KEY_EQUAL( r1, key ) );
	EXPECT_TRUE( IS_VALUE_EQUAL( r1, value ) );

	ASSERT_TRUE( nldb_cursor_fetch( cursor,  &key,  &value ) == NLDB_ERROR_END_OF_ITERATION );
}

// TODO : Add cursor tests by calling nldb_cursor_seek with a key order
