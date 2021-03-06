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


#ifndef _NLDB_PLUGIN_ARRAY_TREE_H_
#define _NLDB_PLUGIN_ARRAY_TREE_H_ (1)

#include <nldb/nldb_plugin.h>

class nldb_plugin_array_tree_t : public nldb_plugin_t
{
public:
	virtual ~nldb_plugin_array_tree_t() {};

	virtual nldb_rc_t table_create(const nldb_table_id_t db_id, const nldb_table_id_t table_id, nldb_plugin_table_desc_t * table_desc);

	virtual nldb_rc_t table_drop(nldb_plugin_table_desc_t & table_desc);

	virtual nldb_rc_t table_open(nldb_plugin_table_desc_t & table_desc, nldb_table_context_t * table_ctx );

	virtual nldb_rc_t table_close(nldb_table_context_t table_ctx );

	virtual nldb_rc_t table_put(nldb_table_context_t table_ctx, const nldb_key_t & key, const nldb_value_t & value);

// errors : NLDB_ERROR_KEY_NOT_FOUND
	virtual nldb_rc_t table_get(nldb_table_context_t table_ctx, const nldb_key_t & key, nldb_value_t * value, nldb_order_t * order);

// errors : NLDB_ORDER_OUT_OF_RANGE
	virtual nldb_rc_t table_get(nldb_table_context_t table_ctx, const nldb_order_t & order, nldb_key_t * key, nldb_value_t * value );

// errors : NLDB_ERROR_KEY_NOT_FOUND
	virtual nldb_rc_t table_del(nldb_table_context_t table_ctx, const nldb_key_t & key);

	virtual nldb_rc_t table_stat(nldb_table_context_t table_ctx, nldb_table_stat_t * table_stat);

	// Free allocated memory for the value if any.
	virtual nldb_rc_t value_free(nldb_value_t & value);

	// Free allocated memory for the key if any.
	virtual nldb_rc_t key_free(nldb_key_t & key);

	virtual nldb_rc_t cursor_open(nldb_table_context_t table_ctx, nldb_cursor_context_t * cursor_ctx);

	virtual nldb_rc_t cursor_seek(nldb_cursor_context_t cursor_ctx, const nldb_cursor_direction_t & direction, const nldb_key_t & key);

	// errors : NLDB_ERROR_CURSOR_NOT_OPEN
	virtual nldb_rc_t cursor_seek(nldb_cursor_context_t cursor_ctx, const nldb_cursor_direction_t & direction, const nldb_order_t & order);

	// errors : NLDB_ERROR_CURSOR_NOT_OPEN, NLDB_ERROR_END_OF_ITERATION
	virtual nldb_rc_t cursor_fetch(nldb_cursor_context_t cursor_ctx, nldb_key_t * key, nldb_value_t * value, nldb_order_t * order);

	virtual nldb_rc_t cursor_close(nldb_table_context_t table_ctx, nldb_cursor_context_t cursor_ctx);
};

#if defined(DEBUG)
extern nldb_rc_t debug_print_nodes(nldb_table_context_t table_ctx);
extern nldb_rc_t debug_check_consistency(nldb_table_context_t table_ctx);
#endif // DEBUG

#endif // _NLDB_PLUGIN_ARRAY_TREE_H_
