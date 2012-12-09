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

#ifndef _NLDB_COMMON_H_
#define _NLDB_COMMON_H_ (1)

#define NLDB_OK                         (0)
#define NLDB_ERROR                      (-1)
#define NLDB_ERROR_DB_ALREADY_EXISTS    (-2)
#define NLDB_ERROR_DB_NOT_FOUND         (-3)
#define NLDB_ERROR_TABLE_ALREADY_EXISTS (-4)
#define NLDB_ERROR_TABLE_NOT_FOUND      (-5)
#define NLDB_ERROR_TX_ALREADY_BEGAN     (-6)
#define NLDB_ERROR_TX_NOT_BEGAN         (-7)
#define NLDB_ERROR_TX_NOT_ENDED         (-8)
#define NLDB_ERROR_KEY_NOT_FOUND        (-9)
#define NLDB_ERROR_CURSOR_NOT_OPEN      (-10)
#define NLDB_ERROR_END_OF_ITERATION     (-11)
#define NLDB_ERROR_INVALID_ITERATION     (-12)
#define NLDB_ERROR_TX_ALREADY_INITIALIZED (-13)
#define NLDB_ERROR_TX_ALREADY_DESTROYED (-14)
#define NLDB_ERROR_PLUGIN_NO_MORE_SLOT  (-15)
#define NLDB_ERROR_NO_MEM               (-16)
#define NLDB_ERROR_NOT_IMPLEMENTED_YET  (-17)
#define NLDB_ERROR_INIT_REPLICATION_MESSAGE (-18)
#define NLDB_ERROR_UNSUPPORTED_FEATURE  (-19)
#define NLDB_ERROR_VARIABLE_KEY_SIZE_NOT_SUPPORTED  (-20)
#define NLDB_ERROR_VARIABLE_VALUE_SIZE_NOT_SUPPORTED  (-21)

typedef int nldb_int32_t;
typedef unsigned int nldb_uint32_t;
typedef unsigned short nldb_uint16_t;
typedef long long nldb_int64_t;
typedef unsigned long long nldb_uint64_t;

typedef int nldb_rc_t;
typedef int nldb_db_id_t;
typedef int nldb_table_id_t;
typedef long long nldb_tx_id_t;

typedef unsigned short key_length_t;
typedef unsigned short value_length_t;

typedef struct nldb_key_t {
	void * data;
	key_length_t length;
} nldb_key_t;

typedef struct nldb_value_t {
	void * data;
	value_length_t length;
} nldb_value_t;


#endif //_NLDB_COMMON_H_
