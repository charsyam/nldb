#if defined(WIN32) || defined(_WIN64)
#include <WinSock2.h>
#include "../thrift/lib/cpp/src/windows/GetTimeOfDay.h"
#else
# include <sys/time.h>
#endif

#include <txbase/tx_assert.h>

#include <nldb/nldb.h>

#include <iostream>
#include <stdio.h>

#include <string.h>

//#define TEST_ITERATIONS (1000L * 1000L * 300)
// Iterations per transaction
#define TEST_ITERATIONS (1L)
// Total transactions 
//#define TEST_TRANSACTIONS (3L)
//#define TEST_TRANSACTIONS (100000000L)
#define TEST_TRANSACTIONS   (10000000L)

#define KEY_SIZE   (32)
#define VALUE_SIZE (256)

typedef struct tbl_key {
	char data[ KEY_SIZE ];
} tbl_key ;
typedef struct tbl_value{
	char data[ VALUE_SIZE ];
} tbl1_value ;

nldb_uint64_t * rndNumsA;
tbl_key * rndKeysA;
tbl_value * rndValuesA;

#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

// set all duplicate values to 0 in the random value array.
// return the number of eleminated duplicate values.
size_t removeDuplicateNumbers(  size_t rnd_count, nldb_uint64_t * rnd_valus )
{
	size_t dup_count = 0;

	// remove duplicates
	for (size_t i = 0; i< TEST_ITERATIONS - 1; i++ )
	{
		for (size_t j = i+1; j< TEST_ITERATIONS; j++ )
		{
			if (rnd_valus[j] == rnd_valus[i])
			{
				rnd_valus[j] = 0;
				dup_count++;
			}
		}
	}
	return dup_count;
}

// Fill data into char array with the given random value
void fillData(char * data, size_t size, nldb_uint64_t rndNum )
{
	// The size should be multiple of 8
	tx_assert(size % sizeof(nldb_uint64_t) == 0);

	while (size > 0)
	{
		memcpy(data, &rndNum, sizeof(nldb_uint64_t));
		data += sizeof(nldb_uint64_t);
		size -= sizeof(nldb_uint64_t);
	}
}
void generateRandomNumbers()
{
	rndNumsA = new nldb_uint64_t [TEST_ITERATIONS];
	rndKeysA = new tbl_key [TEST_ITERATIONS];
	rndValuesA = new tbl_value [TEST_ITERATIONS];

	boost::random::mt19937 rng;         // produces randomness out of thin air
										// see pseudo-random number generators
	boost::random::uniform_int_distribution<nldb_uint64_t> rnd64(1,((nldb_uint64_t)1LL)<<63);
										// distribution that maps to 1..6
										// see random number distributions

	for (size_t i = 0; i< TEST_ITERATIONS; i++ )
	{
		nldb_uint64_t rndValue = rnd64(rng);
  	    rndNumsA[i] = rndValue;                   // simulate rolling a die
  	    fillData( rndKeysA[i].data, sizeof(rndKeysA[i].data), rndValue);
  	    fillData( rndValuesA[i].data, sizeof(rndValuesA[i].data), rndValue);
	}

	size_t dup_count;
	dup_count = removeDuplicateNumbers( TEST_ITERATIONS, rndNumsA);
	printf("The number of duplicates removed for random value set A : %lu\n", dup_count );

}

const int TEST_DB_ID = 1;
const int VOLATILE_TABLE_ID = 1;
const int PERSISTENT_TABLE_ID = 2;
const int REPLICATION_PORT = 9090;

static void fatal_exit(const char* error, nldb_rc_t rc) {
	fprintf(stderr, "[code=%d]%s\n",rc, error);

	exit(-1);
}
static void unexpected_result(const char* error, nldb_rc_t actual_rc, nldb_rc_t expected_rc) {
	fprintf(stderr, "%s [expected=%d, actual=%d]\n",error, expected_rc, actual_rc);
}

static void unexpected_value(const char* error, tbl_key key) {
	fprintf(stderr, "%s [key=%s]\n",error, key.data);
}


static void create_tables(nldb_db_t & db)
{
	nldb_rc_t rc;
	rc = nldb_table_create(db, VOLATILE_TABLE_ID, NLDB_TABLE_VOLATILE);
	if (rc) fatal_exit("Failed to create a table with VOLATILE_TABLE_ID", rc);

	rc = nldb_table_create(db, PERSISTENT_TABLE_ID, NLDB_TABLE_PERSISTENT);
//	rc = nldb_table_create(db, PERSISTENT_TABLE_ID, NLDB_TABLE_VOLATILE);
	if (rc) fatal_exit("Failed to create a table with NLDB_TABLE_PERSISTENT", rc);
}

static void drop_tables(nldb_db_t & db)
{
	nldb_rc_t rc;
	rc = nldb_table_drop(db, VOLATILE_TABLE_ID);
	if (rc) fatal_exit("Failed to drop a table with VOLATILE_TABLE_ID", rc);

	rc = nldb_table_drop(db, PERSISTENT_TABLE_ID);
	if (rc) fatal_exit("Failed to drop a table with NLDB_TABLE_PERSISTENT", rc);
}

static void put_random_records( nldb_tx_t & tx, nldb_table_t & table, size_t rnd_count, nldb_uint64_t * rnd_values, const char * data_message, nldb_rc_t expected_rc )
{
	nldb_rc_t rc;

	// put a random record
	for (size_t i=0; i<rnd_count; i++) {
		tbl_key & key = rndKeysA[i];

		tbl_value & value = rndValuesA[i];

		nldb_key_t nldb_key = { &key, sizeof(key) };
		nldb_value_t nldb_value = { &value, sizeof(value) };

		rc = nldb_table_put( tx, table, nldb_key, nldb_value);
		if (rc != expected_rc)
		{
			 unexpected_result("put : got an unexpected result", rc, expected_rc);
		}
	}
}

/*
static void get_random_records( nldb_tx_t & tx, nldb_table_t & table, size_t rnd_count, nldb_uint64_t * exected_rnd_values, const char * data_message, nldb_rc_t expected_rc )
{
	nldb_rc_t rc;

	// put a random record
	for (size_t i=0; i<rnd_count; i++) {
		nldb_uint64_t rnd = exected_rnd_values[i];

		nldb_key_t nldb_key = { &rnd, sizeof(rnd) };
		nldb_value_t actual_value;

		rc = nldb_table_get( tx, table, nldb_key, & actual_value);
		if (rc != expected_rc)
		{
			 unexpected_result("get : got an unexpected result", rc, expected_rc);
		}

		if ( rc == 0 )
		{
			tbl_value expected_value;
			make_record_value( rnd, data_message, & expected_value );

			assert( sizeof( expected_value ) == actual_value.length );

			if ( memcmp( &expected_value, actual_value.data, actual_value.length ) != 0 )
			{
				unexpected_value("get : got an unexpected value for key : %lld", rnd);
			}
		}
	}
}

static void del_random_records( nldb_tx_t & tx, nldb_table_t & table, size_t rnd_count, nldb_uint64_t * rnd_values, nldb_rc_t expected_rc )
{
	nldb_rc_t rc;

	// put a random record
	for (size_t i=0; i<rnd_count; i++) {
		nldb_uint64_t rnd = rnd_values[i];

		nldb_key_t nldb_key = { &rnd, sizeof(rnd) };

		rc = nldb_table_del( tx, table, nldb_key);
		if (rc != expected_rc)
		{
			 unexpected_result("del : got an unexpected result", rc, expected_rc);
		}
	}
}
*/

int llcep_nldb_master(int arc, char** argv) {
	
	generateRandomNumbers();

	nldb_rc_t rc;
	rc = nldb_init();
	if (rc) fatal_exit("Failed to initialize LLDB system", rc);

	// BUGBUG : The master hangs if it runs in the same db path with a slave which is running. 
	// Fix : Need to show an error message "An instance is already up and running on this db path"
	rc = nldb_db_create( TEST_DB_ID );
	if (rc) fatal_exit("Failed to create LLDB", rc);

	nldb_replication_master_op_t master_op = { "*", REPLICATION_PORT };

	nldb_db_t db;
	rc = nldb_db_open( TEST_DB_ID, & master_op, NULL/*slave*/, &db );
	if (rc) fatal_exit("Failed to open LLDB", rc);

	create_tables( db );

	nldb_table_t vol_table;
	rc = nldb_table_open(db, VOLATILE_TABLE_ID, &vol_table);
	if (rc) fatal_exit("Failed to open table, VOLATILE_TABLE_ID", rc);

	nldb_table_t pers_table;
//	rc = nldb_table_open(db, VOLATILE_TABLE_ID, &pers_table);
	rc = nldb_table_open(db, PERSISTENT_TABLE_ID, &pers_table);
	if (rc) fatal_exit("Failed to open table, PERSISTENT_TABLE_ID", rc);

	nldb_tx_t tx;
	rc = nldb_tx_init( db, &tx );
	if (rc) fatal_exit("Failed to initialize a transaction", rc);

    struct timeval start_time, end_time;
	gettimeofday(&start_time, NULL);

	for (int loop = 0 ; loop < TEST_TRANSACTIONS; loop ++ )
	{
		rc = nldb_tx_begin(tx);
		if (rc) fatal_exit("Failed to begin a transaction", rc);

//		for (int i=0; i<2; i++)
		int i=0;
		{
			nldb_table_t table = (i==0)? vol_table : pers_table;

//			del_random_records( tx, table, TEST_ITERATIONS, rnd_values, NLDB_ERROR_KEY_NOT_FOUND);
//			get_random_records( tx, table, TEST_ITERATIONS, rnd_values, "ThankyouSoft", NLDB_ERROR_KEY_NOT_FOUND);
			put_random_records( tx, table, TEST_ITERATIONS, rndNumsA, "ThankyouSoft", NLDB_OK);
////			put_random_records( tx, table, TEST_ITERATIONS, rnd_values, "ThankyouSoft", NLDB_ERROR_KEY_ALREADY_EXISTS);
//			get_random_records( tx, table, TEST_ITERATIONS, rnd_values, "ThankyouSoft", NLDB_OK);
//			del_random_records( tx, table, TEST_ITERATIONS, rnd_values, NLDB_OK);
		}

		rc = nldb_tx_commit(tx);
		if (rc) fatal_exit("Failed to commit a transaction", rc);
	}

	rc = nldb_db_wait_for_replication_publishers( db );
	if (rc) fatal_exit("Failed to wait for replication publishers", rc);

	gettimeofday(&end_time, NULL);

    double start, end;
    start = start_time.tv_sec + ((double) start_time.tv_usec / 1000000);
    end = end_time.tv_sec + ((double) end_time.tv_usec / 1000000);

    std::cout.precision(15);
    std::cout << "LLDB performance: ";
    std::cout << (TEST_TRANSACTIONS * 1.0) / (end - start)
              << " transactions/secs" << std::endl;

	rc = nldb_tx_destroy( tx );
	if (rc) fatal_exit("Failed to destroy a transaction", rc);

	printf("Press Enter to start closing/dropping tables, closing DB, dropping DB\n");
	(void)getchar();

	rc = nldb_table_close(vol_table);
	if (rc) fatal_exit("Failed to close table, VOLATILE_TABLE_ID", rc);

	rc = nldb_table_close(pers_table);
	if (rc) fatal_exit("Failed to close table, VOLATILE_TABLE_ID", rc);

	drop_tables( db );

	rc = nldb_db_close( db );
	if (rc) fatal_exit("Failed to close LLDB", rc);

	rc = nldb_db_drop( TEST_DB_ID );
	if (rc) fatal_exit("Failed to drop LLDB", rc);

	rc = nldb_destroy();
	if (rc) fatal_exit("Failed to finalize LLDB instance", rc);

    return EXIT_SUCCESS;
}

nldb_rc_t replication_trigger_handler(const nldb_trigger_type_t trigger_type, const nldb_table_id_t & table_id, const nldb_key_t & key, const nldb_value_t & value)
{
	printf("Type:%s, Table Id:%d, Key length:%ld, Value length:%ld\n", (trigger_type==TT_PUT)?"PUT":(trigger_type==TT_DEL)?"DEL":"UNKNOWN", table_id, key.length, value.length);
	return NLDB_OK;
}

int llcep_nldb_slave(int arc, char** argv) {

	nldb_rc_t rc;
	rc = nldb_init();
	if (rc) fatal_exit("Failed to initialize LLDB system", rc);

	rc = nldb_db_create( TEST_DB_ID );
	if (rc) fatal_exit("Failed to create LLDB", rc);

	nldb_replication_slave_op_t slave_op = { {"localhost", REPLICATION_PORT},  };
	slave_op.trigger_handler = NULL;
//	slave_op.trigger_handler = & replication_trigger_handler;

	nldb_db_t db;
	rc = nldb_db_open( TEST_DB_ID, NULL /*master*/, & slave_op/*slave*/, & db );
	if (rc) fatal_exit("Failed to open LLDB", rc);

	create_tables( db );

	printf("Press Enter to start dropping tables, closing DB, dropping DB\n");
	(void)getchar();

	drop_tables( db );

	rc = nldb_db_close( db );
	if (rc) fatal_exit("Failed to close LLDB", rc);

	rc = nldb_db_drop( TEST_DB_ID );
	if (rc) fatal_exit("Failed to drop LLDB", rc);

    return EXIT_SUCCESS;
}

