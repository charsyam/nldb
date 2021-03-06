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

#ifndef _O_NLDB_TRANSACTION_LOG_BUFFER_H_
#define _O_NLDB_TRANSACTION_LOG_BUFFER_H_

#include <string.h> /* memcpy */

#include <txbase/tx_assert.h>

#include "nldb_internal.h"

#include <xs/xs.h>
#include <nldb/nldb_common.h>
#include "nldb_transaction_log_redoer.h"

#define TX_REPLICATION_MESSAGE_SIZE (65536)

// BUGBUG : Need to check if it is ok to use 64 bit integer values with this macro
#define TX_ALIGN8_UP(x) (((x) + 7 ) & ~7)

// replication message format
// ----------------------- BEGIN -------------------------
// [8 bytes] message size excluding the size field itself.
// [8 bytes] transaction id
// tranasction logs ...
// ------------------------ END --------------------------
class TxTransactionLogBuffer {
public :
	// The replication message header that comes at the begining of a replication message
	typedef struct replication_message_header_t {
		nldb_uint64_t transactionId_;
		nldb_uint64_t totalLogSize_;
	}replication_message_header_t;

    TxTransactionLogBuffer() {
		clear();
	};

	inline size_t getLogRecordCount() {
		return totalLogCount_;
	};

	inline void setTransactionId(const nldb_tx_id_t txId) {
		// Assume : We shouldn't set the transaction id twice after clear() called.
		tx_assert( transactionId_ == 0 );
		transactionId_ = txId;
	};

	inline const nldb_tx_id_t getTransactionId() {
		// Assume : We should have set the transaction id.
		tx_assert( transactionId_ != 0 );
		return transactionId_;
	};

	inline void clear() {
		// The begining address to write log records 
		// - Skip the area to write the replication message header within the replication message.
		nextLogAddr_ = (char*) replicationMessageBuffer_ + sizeof(replication_message_header_t);
		totalLogCount_ = 0;
		totalLogSize_ = 0;
		transactionId_ = 0;
		isFinalized_ = false;
	};

	inline bool isClear() {
		return totalLogCount_ == 0 && !isFinalized_;
	}

	// The log record header that comes at the begining of a log record
	typedef struct log_record_header_t {
		nldb_table_id_t                     table_id_;
		TxTransactionLogRedoer::log_type_t  log_type_;
		size_t                              key_length_;
		size_t                              value_length_;
	}log_record_header_t;

	// Append a log record into the replication message.
	inline void appendLog(const nldb_table_id_t tableId, TxTransactionLogRedoer::log_type_t logType, void * keyData, size_t keyLength, void * valueData, size_t valueLength)
	{
		size_t logSize = calcLogSize( keyLength, valueLength);

		// Make sure that we have enough space.
		// BUGBUG : Implement to move to next TransactionEvent instead of using assertion.
		tx_assert ( sizeof(replication_message_header_t) + totalLogSize_ + logSize < TX_REPLICATION_MESSAGE_SIZE );

		// prepare the log record header
		log_record_header_t logRecordHeader;
		logRecordHeader.table_id_ = tableId;
		logRecordHeader.log_type_ = logType;
		logRecordHeader.key_length_ = keyLength;
		logRecordHeader.value_length_ = valueLength;

		// Copy log record header, key, and value to replication message buffer.
		memcpy( nextLogAddr_, &logRecordHeader, sizeof(logRecordHeader));
		memcpy( getKeyAddress(nextLogAddr_), keyData, keyLength);
		if ( valueLength > 0 )
		{	
			// copy value only if the length of it is greater than 0. 'DEL' operation in a key-value store don't have any value to log.
			memcpy( getValueAddress(nextLogAddr_, keyLength), valueData, valueLength);
		}

		nextLogAddr_ =  (char*) nextLogAddr_ + logSize;
		totalLogSize_ += logSize;
		totalLogCount_++;
	};

	// Parse a replication message, set output parameters listed after the replication message parameter.
	// The memory pointers set to the output parameters are valid only if the memory at the given logRecord is valid.
	inline static void parseLog(void * logRecord, nldb_table_id_t * tableId, TxTransactionLogRedoer::log_type_t * logType, void ** keyData, size_t * keyLength, void ** valueData, size_t * valueLength)
	{
		tx_assert( tableId );
		tx_assert( logType );
		tx_assert( keyData );
		tx_assert( keyLength );
		tx_assert( valueData );
		tx_assert( valueLength );

		log_record_header_t * logRecordHeader = (log_record_header_t *) logRecord;

		tx_assert( logRecordHeader->table_id_ > 0 );
		tx_assert( logRecordHeader->table_id_ <= NLDB_MAX_TABLE_COUNT_PER_DB );

		*tableId = logRecordHeader->table_id_;
		*logType = logRecordHeader->log_type_;
		*keyLength = logRecordHeader->key_length_;
		*valueLength = logRecordHeader->value_length_;

		*keyData = getKeyAddress( logRecord );

		// Copy pointer to value only if value_length_ is greater than 0
		if ( logRecordHeader->value_length_ > 0 ) {
			*valueData = getValueAddress( logRecord, logRecordHeader->key_length_ );
		}
		else {
			*valueData = NULL;
		}
	}

	// Check the replication message. Replication message publisher will call this function to check if the message to send has correct data.
	static void checkReplicationMessage(xs_msg_t * replicationMessage) {
		replication_message_header_t * message_header = (replication_message_header_t*) xs_msg_data( replicationMessage );

		tx_assert( TX_ALIGN8_UP( sizeof( replication_message_header_t) ) + message_header->totalLogSize_ <= xs_msg_size(replicationMessage) );
	}
	// Redo all log records within a replication message.
	static nldb_rc_t applyReplicationMessage(xs_msg_t * replicationMessage, TxTransactionLogRedoer & logRedoer)
	{
		nldb_rc_t rc;
		nldb_table_id_t tableId;
		TxTransactionLogRedoer::log_type_t logType;
		void * keyData;
		size_t keyLength;
		void * valueData;
		size_t valueLength;

		replication_message_header_t * message_header = (replication_message_header_t*) xs_msg_data( replicationMessage );

		size_t messageSize = xs_msg_size( replicationMessage );
		tx_assert( TX_ALIGN8_UP( sizeof( replication_message_header_t) ) + message_header->totalLogSize_ <= messageSize );

		nldb_tx_id_t masterTxId = message_header->transactionId_;
		
		// Assume : A slave process subscribes only a single master. It can't subscribe multiple masters. So, we can use static variables for keeping the previous transaction Id.
		// Assume : the transaction id starts from 0. So, the previous transaction id should be set to -1 to make the first check if the transaction id monotonously increase succeed.
		static nldb_tx_id_t prevTransactionId = INITIAL_TRANSACTION_ID - 1;

		// Make sure that the transaction id increases monotonously without any missing id, nor duplicate id.
		if ( prevTransactionId + 1 != masterTxId) 
		{
			printf("[Replication Subscriber] Transaction Id Mismatch - Expected : %lld, Actual : %lld\n", prevTransactionId + 1,  masterTxId);
			tx_assert(0);
		}
		prevTransactionId = masterTxId;
		
		// Assume : the master publishes the replication message even though the transaction aborts. This is to check if the trnsaction id monotonously grows.
		// In slave, we still need to begin a transaction to get a new tranasction id like master did. This is to have the same transaction id for the master and all slaves in the cluster.
		// Keep in mind that a slave can also become a new read-only master which publishes replication messages to other slaves. 
		// IOW, LLDB supports hierachical master-slave relationship, supporting read/write transactions in the root master node, and read-only transactions to all slaves under it.
		rc = logRedoer.beginTransaction( masterTxId );
		if (rc) return rc;

		void * logRecordStartAddr = message_header + 1;
		for(void * logRecord = logRecordStartAddr; 
		   logRecord < (char*) logRecordStartAddr + message_header->totalLogSize_; 
		   logRecord = (char*) logRecord +  calcLogSize(keyLength, valueLength) )
		{
		    parseLog(logRecord, &tableId, &logType, &keyData, &keyLength, &valueData, &valueLength);
			
			rc = logRedoer.redoLog(tableId, logType, keyData, keyLength, valueData, valueLength);
			if (rc) {
				nldb_rc_t abortRc = logRedoer.abortTransaction( masterTxId );
				tx_assert(abortRc==0);
				return rc;
			}
		}

		rc = logRedoer.commitTransaction( masterTxId );
		if (rc) return rc;
		
		return NLDB_OK;
	}

	// Finish the log buffer, do not allow appending more logs, set all fields within the log buffer with valid values so that it can be sent
	inline void finalize() {
		tx_assert( transactionId_ > 0 );

		// ASSUME : Master/Slave nodes are all using the same endian. No marshalling required.
		replication_message_header_t * replicationHeader = (replication_message_header_t*) replicationMessageBuffer_;
		replicationHeader->transactionId_ = transactionId_;
		replicationHeader->totalLogSize_ = totalLogSize_;

		isFinalized_ = true;
	}

	inline void buildReplicationMessage(xs_msg_t * replicationMessage) {
		tx_assert( nextLogAddr_ > replicationMessageBuffer_ );
		size_t messageSize = (char*) nextLogAddr_ -  replicationMessageBuffer_;
		tx_assert( messageSize <= TX_REPLICATION_MESSAGE_SIZE );

		// BUGBUG need to catch error_t that is thrown by rebuild function.
		int rc = xs_msg_init_data(replicationMessage, replicationMessageBuffer_, messageSize, NULL, NULL );
		tx_assert(rc == 0);
	};
	inline void destroyReplicationMessage(xs_msg_t * replicationMessage) {
		int rc = xs_msg_close(replicationMessage);
		tx_assert(rc == 0);
	}

	inline bool isFinalized() {
		return isFinalized_;
	}

private :
	static inline  size_t calcLogSize(size_t keyLength, size_t valueLength) 
	{
		// Align up by 8 bytes for the starting address of log record header, key, and data 
		// so that slaves can simply typecast values within the replication message buffer.
		// the starting address of key and value are also aligned by 8 bytes.
		//
		// Key and Value can have a struct within it, so that the slaves don't need to copy the data to translate. 
		// They can simply type cast key and value within the replication buffer.

		return TX_ALIGN8_UP( sizeof(log_record_header_t) ) + 
			   TX_ALIGN8_UP( keyLength ) + 
			   TX_ALIGN8_UP( valueLength );
	};

	// Return the address of key within a log record
	static inline void *getKeyAddress(void * logAddress)
	{
		return (char*) logAddress + TX_ALIGN8_UP( sizeof(log_record_header_t) );
	}
	// Return the address of value within a log record
	static inline void *getValueAddress(void * logAddress, size_t keyLength)
	{
		return (char*) logAddress + TX_ALIGN8_UP( sizeof(log_record_header_t) ) + TX_ALIGN8_UP( keyLength );
	}

    // The transaction log buffer to replicate to masters.
    char replicationMessageBuffer_[TX_REPLICATION_MESSAGE_SIZE];

	// The address to append the log within the replication message.
	void * nextLogAddr_;

	// The sum of sizes of all log records within the replication message.
	size_t totalLogSize_;

	// The transaction id
	nldb_tx_id_t transactionId_;

	// The number of log records within the replication message.
	size_t totalLogCount_;

	// Is this replication buffer finalized? IOW, can we send this replication buffer to masters?
	bool isFinalized_;
	
};


#endif /* _O_NLDB_TRANSACTION_LOG_BUFFER_H_ */
