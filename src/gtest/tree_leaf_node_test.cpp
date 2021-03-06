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

#include <gtest/gtest.h>

// for memcmp
#include <string.h>

// for htonl
#include <arpa/inet.h>

#include <nldb/nldb_common.h>
#include <nldb/nldb.h>

#include "tree_test_common.h"

class TreeLeafNodeTest : public testing::Test  {
  protected:
    virtual void SetUp() {

    }

    virtual void TearDown() {

    }
};

TEST_F(TreeLeafNodeTest, is_empty) {
	tree_leaf_node_t leaf(TREE_KEY_LEN);
	EXPECT_TRUE( leaf.is_empty() );

	ASSERT_TRUE( leaf.put(KEY_01, VALUE_01) == NLDB_OK );

	EXPECT_TRUE( ! leaf.is_empty() );

	void * value = NULL;

	ASSERT_TRUE( leaf.del(KEY_01, &value) == NLDB_OK );

	EXPECT_TRUE( leaf.is_empty() );
}

TEST_F(TreeLeafNodeTest, is_full) {
	tree_leaf_node_t leaf(TREE_KEY_LEN);

	ASSERT_TRUE( TREE_KEYS_PER_NODE == 3 );

	EXPECT_TRUE( ! leaf.is_full() );
	ASSERT_TRUE( leaf.put(KEY_01, VALUE_01) == NLDB_OK );

	EXPECT_TRUE( ! leaf.is_full() );
	ASSERT_TRUE( leaf.put(KEY_02, VALUE_02) == NLDB_OK );

	EXPECT_TRUE( ! leaf.is_full() );
	ASSERT_TRUE( leaf.put(KEY_03, VALUE_03) == NLDB_OK );

	EXPECT_TRUE( leaf.is_full() );


	void * value = NULL;

	ASSERT_TRUE( leaf.del(KEY_01, &value) == NLDB_OK );
	EXPECT_TRUE( ! leaf.is_full() );

	ASSERT_TRUE( leaf.del(KEY_02, &value) == NLDB_OK );
	EXPECT_TRUE( ! leaf.is_full() );

	ASSERT_TRUE( leaf.del(KEY_03, &value) == NLDB_OK );
	EXPECT_TRUE( ! leaf.is_full() );
}

TEST_F(TreeLeafNodeTest, min_key) {
	tree_leaf_node_t leaf(TREE_KEY_LEN);

	ASSERT_TRUE( TREE_KEYS_PER_NODE == 3 );

	EXPECT_TRUE( leaf.min_key() == NULL );

	ASSERT_TRUE( leaf.put(KEY_02, VALUE_02) == NLDB_OK );
	EXPECT_TRUE( KEY_02_MATCHES( leaf.min_key() )  );

	ASSERT_TRUE( leaf.put(KEY_01, VALUE_01) == NLDB_OK );
	EXPECT_TRUE( KEY_01_MATCHES( leaf.min_key() )  );

	ASSERT_TRUE( leaf.put(KEY_03, VALUE_03) == NLDB_OK );
	EXPECT_TRUE( KEY_01_MATCHES( leaf.min_key() )  );

	void * value = NULL;

	ASSERT_TRUE( leaf.del(KEY_01, &value) == NLDB_OK );
	EXPECT_TRUE( KEY_02_MATCHES( leaf.min_key() )  );

	ASSERT_TRUE( leaf.del(KEY_02, &value) == NLDB_OK );
	EXPECT_TRUE( KEY_03_MATCHES( leaf.min_key() )  );

	ASSERT_TRUE( leaf.del(KEY_03, &value) == NLDB_OK );
	EXPECT_TRUE( leaf.min_key() == NULL );
}


TEST_F(TreeLeafNodeTest, keys_with_values) {
	tree_leaf_node_t leaf(TREE_KEY_LEN);

	ASSERT_TRUE( TREE_KEYS_PER_NODE == 3 );

	EXPECT_TRUE( leaf.keys_with_values().key_count() == 0 );

	ASSERT_TRUE( leaf.put(KEY_02, VALUE_02) == NLDB_OK );
	EXPECT_TRUE( leaf.keys_with_values().key_count() == 1 );
	EXPECT_TRUE( KEY_MATCHES( leaf.keys_with_values().min_key(), KEY_02 ) );

	ASSERT_TRUE( leaf.put(KEY_01, VALUE_01) == NLDB_OK );
	EXPECT_TRUE( leaf.keys_with_values().key_count() == 2 );
	EXPECT_TRUE( KEY_MATCHES( leaf.keys_with_values().min_key(), KEY_01 KEY_02 ) );

	ASSERT_TRUE( leaf.put(KEY_03, VALUE_03) == NLDB_OK );
	EXPECT_TRUE( leaf.keys_with_values().key_count() == 3 );
	EXPECT_TRUE( KEY_MATCHES( leaf.keys_with_values().min_key(), KEY_01 KEY_02 KEY_03 ) );

	void * value = NULL;

	ASSERT_TRUE( leaf.del(KEY_01, &value) == NLDB_OK );
	EXPECT_TRUE( leaf.keys_with_values().key_count() == 2 );
	EXPECT_TRUE( KEY_MATCHES( leaf.keys_with_values().min_key(), KEY_02 KEY_03 ) );

	ASSERT_TRUE( leaf.del(KEY_02, &value) == NLDB_OK );
	EXPECT_TRUE( leaf.keys_with_values().key_count() == 1 );
	EXPECT_TRUE( KEY_MATCHES( leaf.keys_with_values().min_key(), KEY_03 ) );

	ASSERT_TRUE( leaf.del(KEY_03, &value) == NLDB_OK );
	EXPECT_TRUE( leaf.keys_with_values().key_count() == 0 );
	EXPECT_TRUE( leaf.keys_with_values().min_key() == NULL );
}

TEST_F(TreeLeafNodeTest, put_get_del) {

	tree_leaf_node_t leaf(TREE_KEY_LEN);

	ASSERT_TRUE( TREE_KEYS_PER_NODE == 3 );

	// get : the key does not exist
	void * value = NULL;

	ASSERT_TRUE( leaf.get(KEY_01, &value, NULL) == NLDB_OK );
	EXPECT_TRUE( value == NULL );

	// put : put a new key
	ASSERT_TRUE( leaf.put(KEY_01, VALUE_01) == NLDB_OK );
	ASSERT_TRUE( leaf.get(KEY_01, &value, NULL) == NLDB_OK );
	EXPECT_TRUE( VALUE_01_MATCHES(value) );

	// put : put a new key
	ASSERT_TRUE( leaf.put(KEY_02, VALUE_02) == NLDB_OK );
	ASSERT_TRUE( leaf.get(KEY_02, &value, NULL) == NLDB_OK );
	EXPECT_TRUE( VALUE_02_MATCHES(value) );

	// put : replace an existing key
	ASSERT_TRUE( leaf.put(KEY_02, "new_value_02") == NLDB_OK );
	ASSERT_TRUE( leaf.get(KEY_02, &value, NULL) == NLDB_OK );
	EXPECT_TRUE( VALUE_MATCHES(value, "new_value_02") );

	// del : del a non-existing key
	ASSERT_TRUE( leaf.del(KEY_03, &value) == NLDB_OK );
	EXPECT_TRUE( value == NULL );

	// del : del a existing key.
	ASSERT_TRUE( leaf.del(KEY_02, &value) == NLDB_OK );
	EXPECT_TRUE( VALUE_MATCHES(value, "new_value_02") );

	// del : get a deleted key
	ASSERT_TRUE( leaf.get(KEY_02, &value, NULL) == NLDB_OK );
	EXPECT_TRUE( value == NULL );
}

TEST_F(TreeLeafNodeTest, merge_with) {
	tree_leaf_node_t leaf1(TREE_KEY_LEN);
	tree_leaf_node_t leaf2(TREE_KEY_LEN);

	// merge_with is not supported yet.
	ASSERT_TRUE( leaf1.merge_with(&leaf2) == NLDB_ERROR_UNSUPPORTED_FEATURE );
}

TEST_F(TreeLeafNodeTest, split_with_two_keys) {
	tree_leaf_node_t leaf(TREE_KEY_LEN);

	// put two keys
	ASSERT_TRUE( leaf.put(KEY_02, VALUE_02) == NLDB_OK );
	ASSERT_TRUE( leaf.put(KEY_01, VALUE_01) == NLDB_OK );

	// the original leaf should have KEY_01, KEY_02.
	EXPECT_TRUE( leaf.keys_with_values().key_count() == 2 );
	EXPECT_TRUE( KEY_MATCHES( leaf.keys_with_values().min_key(), KEY_01 KEY_02) );

	tree_leaf_node_t * new_right_leaf;
	ASSERT_TRUE( leaf.split( &new_right_leaf ) == NLDB_OK );

	// the original leaf should have KEY_01.
	EXPECT_TRUE( leaf.keys_with_values().key_count() == 1 );
	EXPECT_TRUE( KEY_MATCHES( leaf.keys_with_values().min_key(), KEY_01 ) );

	// the new right leaf node should have KEY_02.
	EXPECT_TRUE( new_right_leaf->keys_with_values().key_count() == 1 );
	EXPECT_TRUE( KEY_MATCHES( new_right_leaf->keys_with_values().min_key(), KEY_02 ) );

	// Check the prev/next links between leaf nodes.
	EXPECT_TRUE( leaf.prev_ == NULL );
	EXPECT_TRUE( leaf.next_ == new_right_leaf );

	EXPECT_TRUE( new_right_leaf->prev_ == & leaf );
	EXPECT_TRUE( new_right_leaf->next_ == NULL );
}

TEST_F(TreeLeafNodeTest, unlink) {
}

