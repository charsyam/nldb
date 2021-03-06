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


#ifndef _O_NLDB_TRANSCTION_LOG_REDOER_H_
#define _O_NLDB_TRANSCTION_LOG_REDOER_H_ (1)

#include <txbase/tx_assert.h>

#include <nldb/nldb.h>
#include "nldb_transaction_log_buffer.h"

// Redo transaction logs
// For each replication message :
//    begin a transaction, open tables if not open, redo log records, commit a transaction
//    assumption : open tables still remain open even though the transaction commits.
// 
// (P2) Shutdown : Need decision - do we need shutdown? how about simply kill the process and do restart recovery always ?
//    close all open tables
class TxTransactionLogRedoer {
public :
	typedef enum log_type_t {
		LT_PUT = 1,
		LT_DEL
	}log_type_t;

	typedef struct table_t {
		nldb_table_t table;
		// indicates if this table is open
		bool is_open_;
	} table_t ;
	
	TxTransactionLogRedoer(nldb_db_t & db, const nldb_replication_trigger_hanlder_t & trigger_handler) : db_(db) {
		for (unsigned int i=0; i < sizeof(tables_)/sizeof(tables_[0]); i++ ) {
			tables_[i].is_open_ = false;
		}

		trigger_handler_ = trigger_handler;
	}

	nldb_rc_t init() {
		nldb_rc_t
		rc = nldb_tx_init( db_, &redoingTx );
		return rc;
	}

	// Not used for now.
	/*
	nldb_rc_t destroy() {
		nldb_rc_t
		rc = nldb_tx_destroy( redoingTx );
		return rc;
	}
	*/

	// @param masterTxId : the transaciotn Id at master. Not used for now.
	nldb_rc_t beginTransaction(const nldb_tx_id_t & masterTxId)
	{
		nldb_rc_t rc = nldb_tx_begin(redoingTx);
		return rc;
	}

	nldb_rc_t redoLog(const nldb_table_id_t & tableId, const log_type_t & logType, void * keyData, key_length_t keyLength, void * valueData, value_length_t valueLength)
	{
		nldb_table_t * table = NULL;
		nldb_rc_t rc = getOpenTable( tableId, & table );

		nldb_key_t nldb_key = { keyData, keyLength };

		switch (logType) {
			case LT_PUT :
			{
				nldb_value_t nldb_value = { valueData, sizeof(valueLength) };

				rc = nldb_table_put( redoingTx, *table, nldb_key, nldb_value);
				if (rc) return rc;

				if (trigger_handler_)
					trigger_handler_(TT_PUT, tableId, nldb_key, nldb_value);

			}
			break;
			case LT_DEL :
			{
				rc = nldb_table_del( redoingTx, *table, nldb_key);
				if (rc) return rc;
				if (trigger_handler_)
				{
					nldb_value_t nldb_value = { NULL, 0 };
					trigger_handler_(TT_DEL, tableId, nldb_key, nldb_value);
				}
			}
			break;
			default :
			{
				// Above log types are the only ones that we know.
				tx_assert(0);
			}
		}
		return NLDB_OK;
	}

	// @param masterTxId : the transaciotn Id at master. Not used for now.
	nldb_rc_t commitTransaction(const nldb_tx_id_t & masterTxId)
	{
		nldb_rc_t rc = nldb_tx_commit(redoingTx);
		return rc;
	}

	// @param masterTxId : the transaciotn Id at master. Not used for now.
	nldb_rc_t abortTransaction(const nldb_tx_id_t & masterTxId)
	{
		nldb_rc_t rc = nldb_tx_abort(redoingTx);
		return rc;
	}

private :
	// Set the output parameter table to the handle of open table. If the table is not open yet, this function opens it first.
	nldb_rc_t getOpenTable(const nldb_table_id_t & tableId, nldb_table_t ** table )
	{
		tx_assert( tableId <= NLDB_MAX_TABLE_COUNT_PER_DB );
		
		nldb_table_t * theTable;

		theTable = & tables_[tableId].table;

		if ( ! tables_[tableId].is_open_ ) {
			nldb_rc_t rc = nldb_table_open(db_, tableId, theTable );
			if (rc) return rc;
			tables_[tableId].is_open_ = true;
		}

		*table = theTable;
		return NLDB_OK;
	};

	// The database where we redo log records.
	nldb_db_t & db_;

	// The tranasction id assigned by the time slave begins a transaction.
	nldb_tx_t redoingTx;

	// use table id as an index to this array. table id starts from 1.
	table_t tables_[NLDB_MAX_TABLE_COUNT_PER_DB + 1];

	// The handler function that is called whenever PUT/DEL operation is replicated.
	nldb_replication_trigger_hanlder_t trigger_handler_;
};

#endif /* _O_NLDB_TRANSCTION_LOG_REDOER_H_ */
