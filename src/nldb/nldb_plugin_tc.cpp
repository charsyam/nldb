/*
 * (C) Copyright 2012 ThankyouSoft (http://ThankyouSoft.com/) and others.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * Contributors:
 *     Kangmo Kim
 *     Saryong Kang
 */

#include "nldb_plugin_tc.h"

#include <tcutil.h>

nldb_rc_t nldb_plugin_tc_t::table_create(const nldb_table_id_t & db_id, const nldb_table_id_t & table_id, nldb_plugin_table_desc_t * table_desc)
{
	// no table descriptor to store.
	table_desc->length = 0;
	return NLDB_OK;
}

nldb_rc_t nldb_plugin_tc_t::table_drop(nldb_plugin_table_desc_t & table_desc)
{
	// nothing to remove from disk
	return NLDB_OK;
}

nldb_rc_t nldb_plugin_tc_t::table_open(nldb_plugin_table_desc_t & table_desc, nldb_table_context_t * table_ctx)
{
	TCNDB * ndb = tcndbnew();
	*table_ctx = ndb;
	return NLDB_OK;
}

nldb_rc_t nldb_plugin_tc_t::table_close(nldb_table_context_t table_ctx)
{
	TCNDB * ndb = (TCNDB*) table_ctx;
	tcndbdel(ndb);

	return NLDB_OK;
}

// errors : NLDB_ERROR_KEY_ALREADY_EXISTS
nldb_rc_t nldb_plugin_tc_t::table_put(nldb_table_context_t table_ctx, const nldb_key_t & key, const nldb_value_t & value)
{
	TCNDB * ndb = (TCNDB*) table_ctx;

	tcndbput(ndb, key.data, key.length, value.data, value.length);

	return NLDB_OK;
}


// errors : NLDB_ERROR_KEY_NOT_FOUND
nldb_rc_t nldb_plugin_tc_t::table_get(nldb_table_context_t table_ctx, const nldb_key_t & key, nldb_value_t * value)
{
	TCNDB * ndb = (TCNDB*) table_ctx;

	int value_size;
	void * value_ptr = tcndbget(ndb, key.data, key.length, &value_size);
	if (!value_ptr) // the value is not found
		return NLDB_ERROR_KEY_NOT_FOUND;

	value->data = value_ptr;
	value->length = value_size;

	return NLDB_OK;
}

// errors : NLDB_ERROR_KEY_NOT_FOUND
nldb_rc_t nldb_plugin_tc_t::table_del(nldb_table_context_t table_ctx, const nldb_key_t & key)
{
	TCNDB * ndb = (TCNDB*) table_ctx;

	bool deleted = tcndbout(ndb, key.data, key.length);

	if (! deleted)
		return NLDB_ERROR_KEY_NOT_FOUND;

	return NLDB_OK;
}

nldb_rc_t nldb_plugin_tc_t::value_free(nldb_value_t & value) {
	tcfree(value.data);
	value.length = 0;
	return NLDB_OK;
}

nldb_rc_t nldb_plugin_tc_t::key_free(nldb_key_t & value) {
	return NLDB_ERROR_UNSUPPORTED_FEATURE;
}

nldb_rc_t nldb_plugin_tc_t::cursor_open(nldb_table_context_t table_ctx, nldb_cursor_context_t * cursor_ctx)
{
	return NLDB_ERROR_UNSUPPORTED_FEATURE;
}

nldb_rc_t nldb_plugin_tc_t::cursor_seek_forward (nldb_cursor_context_t cursor_ctx, const nldb_key_t & key)
{
	return NLDB_ERROR_UNSUPPORTED_FEATURE;
}

nldb_rc_t nldb_plugin_tc_t::cursor_seek_backward(nldb_cursor_context_t cursor_ctx, const nldb_key_t & key)
{
	return NLDB_ERROR_UNSUPPORTED_FEATURE;
}

nldb_rc_t nldb_plugin_tc_t::cursor_move_forward (nldb_cursor_context_t cursor_ctx, nldb_key_t * key, nldb_value_t * value)
{
	return NLDB_ERROR_UNSUPPORTED_FEATURE;
}

nldb_rc_t nldb_plugin_tc_t::cursor_move_backward(nldb_cursor_context_t cursor_ctx, nldb_key_t * key, nldb_value_t * value)
{
	return NLDB_ERROR_UNSUPPORTED_FEATURE;
}

nldb_rc_t nldb_plugin_tc_t::cursor_close(nldb_table_context_t table_ctx, nldb_cursor_context_t cursor_ctx)
{
	return NLDB_ERROR_UNSUPPORTED_FEATURE;
}