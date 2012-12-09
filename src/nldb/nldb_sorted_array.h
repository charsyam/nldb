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

#ifndef NLDBSEARCH_H_
#define NLDBSEARCH_H_

#include <nldb/nldb_common.h>

#include <txbase/tx_assert.h>

// memmove
#include <string.h>

const int NLDB_SA_MIN_KEY_SIZE=1;

/*
 * nldb_sorted_array : a sorted array of fixed size keys. Each key maps to a void* data.
 */
template <int key_space_size> class nldb_sorted_array {
private :
	int    max_key_count_;
	int    key_count_;
	int    used_key_space_;
	key_length_t key_length_;
	char   key_space_[key_space_size];
	void * data_[key_space_size/NLDB_SA_MIN_KEY_SIZE];

	// Search the position which has the first key that is greater than or equal to the given key.
	// Set -1 to both *key_pos and *data_pos if the first key is greater than the given key.
	// If the exact key was found, set true to *key_found.
	//
	// c.f. "ge" means "greater than or equal to".
	nldb_rc_t search_first_ge_key_position(const void * key, int * key_pos, int * data_pos, bool * key_found) const
	{
		return NLDB_OK;
	}

	// Search the position which has the last key that is less than or equal to the given key.
	// Set -1 to both *key_pos and *data_pos if the last key is less than the given key.
	// If the exact key was found, set true to *key_found.
	//
	// c.f. "le" means "less than than or equal to".
	nldb_rc_t search_last_le_key_position(const void * key, int * key_pos, int * data_pos, bool * key_found) const
	{
		return NLDB_OK;
	}

	void set_data(int data_pos, const void * data)
	{
		tx_debug_assert( key_length_ > 0 );

		tx_debug_assert( data_pos >= 0 );
		tx_debug_assert( data_pos < max_key_count_ );
		tx_debug_assert( data != NULL );

		data_[data_pos] = (void*) data;
	}

	void * get_data(int data_pos) const
	{
		tx_debug_assert( key_length_ > 0 );

		tx_debug_assert( data_pos >= 0 );
		tx_debug_assert( data_pos < max_key_count_ );

		return data_[data_pos];
	}

	void * get_key(int key_pos) const
	{
		tx_debug_assert( key_length_ > 0 );

		tx_debug_assert( key_pos >= 0 );
		tx_debug_assert( key_pos + key_length_ <= key_space_size );

		return (void*)(key_space_ + key_pos);
	}

	// move keys right by the key length from the given key position.
	inline void move_keys_right_by_the_key_length_from(int key_pos)
	{
		tx_debug_assert( key_length_ > 0 );

		tx_debug_assert( key_pos >= 0 );
		tx_debug_assert( key_pos + key_length_ < key_space_size );

		char * shift_src = key_space_+key_pos;
		char * shift_dest = key_space_+key_pos+key_length_;
		int size_of_keys_to_shift = used_key_space_ - key_pos;

		tx_debug_assert( size_of_keys_to_shift > 0 );

		// Make sure that both the keys to shift are within the key_space_.
		tx_debug_assert( shift_src >= key_space_);
		tx_debug_assert( shift_dest > key_space_);
		tx_debug_assert( shift_src + size_of_keys_to_shift < key_space_ + key_space_size);
		tx_debug_assert( shift_dest + size_of_keys_to_shift <= key_space_ + key_space_size);

		// The dest and src overlaps, so use memmove instead of memcpy.
		memmove(shift_dest, shift_src, size_of_keys_to_shift);
	}

	// move keys left by the key length from the given key position.
	// The key right before the given key_pos is removed.
	inline void move_keys_left_by_the_key_length_from(int key_pos)
	{
		tx_debug_assert( key_length_ > 0 );

		tx_debug_assert( key_pos >= key_length_ );
		tx_debug_assert( key_pos < key_space_size );

		char * shift_src = key_space_+key_pos;
		char * shift_dest = key_space_+key_pos-key_length_;

		int size_of_keys_to_shift = used_key_space_ - key_pos;

		tx_debug_assert( size_of_keys_to_shift > 0 );

		// Make sure that both the keys to shift are within the key_space_.
		tx_debug_assert( shift_src > key_space_);
		tx_debug_assert( shift_dest >= key_space_);
		tx_debug_assert( shift_src + size_of_keys_to_shift <= key_space_ + key_space_size);
		tx_debug_assert( shift_dest + size_of_keys_to_shift < key_space_ + key_space_size);

		// The dest and src overlaps, so use memmove instead of memcpy.
		memmove(shift_dest, shift_src, size_of_keys_to_shift);
	}

	inline void set_key_data_at(int key_pos, int data_pos, const void * key, const void * data)
	{
		tx_debug_assert( key_length_ > 0 );

		tx_debug_assert( key_pos >= 0 );
		tx_debug_assert( data_pos >= 0 );
		tx_debug_assert( key_pos + key_length_ <= key_space_size );
		tx_debug_assert( data_pos < max_key_count_ );
		tx_debug_assert( key != NULL );
		tx_debug_assert( data != NULL );

		// copy key
		memcpy(key_space_ + key_pos, key, key_length_);

		// set value
		(void)set_data(data_pos, data);
	}

	inline void move_data_right_by_one_from(int data_pos)
	{
		tx_debug_assert( key_length_ > 0 );

		tx_debug_assert( data_pos >= 0 );
		tx_debug_assert( data_pos + 1 < max_key_count_ );

		char * shift_src = (char*) &data_[data_pos];
		char * shift_dest = (char*) &data_[data_pos + 1];
		int size_of_keys_to_shift = (char*)&data_[max_key_count_] - (char*)&data_[data_pos];

		tx_debug_assert( size_of_keys_to_shift > 0 );

		// Make sure that both the keys to shift are within the key_space_.
		tx_debug_assert( shift_src >= data_);
		tx_debug_assert( shift_dest > data_);
		tx_debug_assert( shift_src + size_of_keys_to_shift <  (char*)&data_[max_key_count_]);
		tx_debug_assert( shift_dest + size_of_keys_to_shift <= (char*)&data_[max_key_count_]);

		// The dest and src overlaps, so use memmove instead of memcpy.
		memmove(shift_dest, shift_src, size_of_keys_to_shift);
	}

	inline void move_data_left_by_one_from(int data_pos)
	{
		tx_debug_assert( key_length_ > 0 );

		tx_debug_assert( data_pos > 0 );
		tx_debug_assert( data_pos < max_key_count_ );

		char * shift_src = (char*)&data_[data_pos];
		char * shift_dest = (char*)&data_[data_pos - 1];
		int size_of_keys_to_shift = (char*)&data_[max_key_count_] - (char*)&data_[data_pos];

		tx_debug_assert( size_of_keys_to_shift > 0 );

		// Make sure that both the keys to shift are within the key_space_.
		tx_debug_assert( shift_src > data_);
		tx_debug_assert( shift_dest >= data_);
		tx_debug_assert( shift_src + size_of_keys_to_shift <= (char*)&data_[max_key_count_]);
		tx_debug_assert( shift_dest + size_of_keys_to_shift < (char*)&data_[max_key_count_]);

		// The dest and src overlaps, so use memmove instead of memcpy.
		memmove(shift_dest, shift_src, size_of_keys_to_shift);
	}


	inline void insert_key_data_at(int key_pos, int data_pos, const void * key, const void * data)
	{
		tx_debug_assert( key_length_ > 0 );

		tx_debug_assert( key_pos >= 0 );
		tx_debug_assert( data_pos >= 0 );
		tx_debug_assert( key_pos + key_length_ <= key_space_size );
		tx_debug_assert( data_pos + 1 < max_key_count_ );
		tx_debug_assert( key != NULL );
		tx_debug_assert( data != NULL );

		if (key_pos == used_key_space_) { // Append at the end of the key_space_
			// No keys to to move
		}
		else
		{
			(void)move_keys_right_by_the_key_length_from(key_pos);
			(void)move_data_right_by_one_from(data_pos);
		}

		(void)set_key_data_at(key_pos, data_pos, key, data);
	}

	inline void remove_key_data_at(int key_pos, int data_pos)
	{
		move_keys_left_by_the_key_length_from(key_pos);
		move_data_left_by_one_from(data_pos);
	}

public:
	typedef int iterator;

	nldb_sorted_array()
	{
	}

	virtual ~nldb_sorted_array()
	{
	}

	nldb_rc_t init(const key_length_t & key_length)
	{
		max_key_count_ = sizeof(key_space_)/key_length;
		key_count_ = 0;
		used_key_space_ = 0;
		key_length_ = key_length;

		return NLDB_OK;
	}

	nldb_rc_t destroy()
	{
		return NLDB_OK;
	}

	nldb_rc_t put(const void * key, const void * data)
	{
		tx_debug_assert( key_length_ > 0 );
		tx_debug_assert( key != NULL );
		tx_debug_assert( data != NULL );

		int key_pos = 0;
		int data_pos = 0;
		bool key_found = false;
		nldb_rc_t rc = search_first_ge_key_position(key, &key_pos, &data_pos, &key_found);
		if (rc) return rc;

		if ( key_found ) { // The exact key is found. Replace the value
			(void)set_data(data_pos, data);
		} else { // The exact key is not found. Insert the key at the position.
			(void)insert_key_data_at(key_pos, data_pos, key, data);
		}
		return NLDB_OK;
	}

	nldb_rc_t get(const void * key, void ** data ) const
	{
		tx_debug_assert( key_length_ > 0 );
		tx_debug_assert( key != NULL );
		tx_debug_assert( data != NULL );

		int key_pos = 0;
		int data_pos = 0;
		bool key_found = false;
		nldb_rc_t rc = search_first_ge_key_position(key, &key_pos, &data_pos, &key_found);
		if (rc) return rc;

		if (key_found)
		{
			// The key was found.
			*data = get_data(data_pos);
		}
		else
		{
			// We don't have the key.
			*data = NULL;
		}

		return NLDB_OK;
	}

	// Set the data of the first greater or equal key in key_space_ to *data.
	// Set NULL to *data in case the key_spacE_ does not have any key greater than or equal to the given key.
	//
	// c.f. "ge" means "greater than or equal to".
	nldb_rc_t find_first_ge_key(const void * key, void ** data ) const
	{
		tx_debug_assert( key_length_ > 0 );
		tx_debug_assert( key != NULL );
		tx_debug_assert( data != NULL );

		int key_pos = 0;
		int data_pos = 0;
		bool key_found = false;
		nldb_rc_t rc = search_first_ge_key_position(key, &key_pos, &data_pos, &key_found);
		if (rc) return rc;

		if (data_pos < 0) // The first key in the key_space_ is greater than the given key.
		{
			*data = NULL; // We don't have the key greater than or equal to the given key. Set NULL.
		}
		else
		{
			*data = get_data(data_pos);
		}

		return NLDB_OK;
	}

	nldb_rc_t del(const void * key, void ** data )
	{
		tx_debug_assert( key != NULL );
		tx_debug_assert( data != NULL );

		int key_pos = 0;
		int data_pos = 0;
		bool key_found = false;
		nldb_rc_t rc = search_first_ge_key_position(key, &key_pos, &data_pos, &key_found);
		if (rc) return rc;

		if (key_found)
		{
			// The key was found.
			*data = get_data(data_pos);
			(void) remove_key_data_at( key_pos, data_pos );
		}
		else
		{
			// We don't have the key.
			*data = NULL;
		}

		return NLDB_OK;
	}

	nldb_rc_t remove_max_key( void ** key, void ** data )
	{
		tx_debug_assert( key_length_ > 0 );
		tx_debug_assert( key != NULL );
		tx_debug_assert( data != NULL );

		if ( key_count_ > 0 )
		{
			tx_debug_assert( used_key_space_ >= key_length_ );

			int max_key_pos = used_key_space_ - key_length_;
			int max_data_pos = key_count_ - 1;

			*key = get_key( max_key_pos );
			*data = get_data( max_data_pos );

			(void) remove_key_data_at( max_key_pos, max_data_pos);
		}

		return NLDB_OK;
	}

	nldb_rc_t iter_forward(const void * key, iterator * iter) const
	{
		tx_debug_assert( key_length_ > 0 );
		tx_debug_assert( key != NULL );
		tx_debug_assert( iter != NULL );

		return NLDB_OK;
	}

	nldb_rc_t iter_backward(const void * key, iterator * iter) const
	{
		tx_debug_assert( key_length_ > 0 );
		tx_debug_assert( key != NULL );
		tx_debug_assert( iter != NULL );

		return NLDB_OK;
	}

	// Iterate forward for all keys within the sorted array.
	nldb_rc_t iter_forward(iterator * iter) const
	{
		tx_debug_assert( key_length_ > 0 );
		tx_debug_assert( iter != NULL );

		return NLDB_OK;
	}

	// Iterate backward for all keys within the sorted array.
	nldb_rc_t iter_backward(iterator * iter) const
	{
		tx_debug_assert( key_length_ > 0 );
		tx_debug_assert( iter != NULL );

		return NLDB_OK;
	}

	nldb_rc_t iter_next(const iterator & iter, void * key, void ** value) const
	{
		tx_debug_assert( key_length_ > 0 );
		tx_debug_assert( key != NULL );
		tx_debug_assert( value != NULL );

		return NLDB_OK;
	}

	nldb_rc_t iter_prev(const iterator & iter, void * key, void ** value) const
	{
		tx_debug_assert( key_length_ > 0 );
		tx_debug_assert( key != NULL );
		tx_debug_assert( value != NULL );

		return NLDB_OK;
	}

	nldb_rc_t merge_with(nldb_sorted_array<key_space_size> * right)
	{
		tx_debug_assert( key_length_ > 0 );
		tx_debug_assert( right != NULL );

		// Not implemented yet. Merging nodes is P2. We will implement the feature after the first release of NLDB.
		return NLDB_ERROR_UNSUPPORTED_FEATURE;
	}

	nldb_rc_t split(nldb_sorted_array<key_space_size> * right_half )
	{
		tx_debug_assert( key_length_ > 0 );
		tx_debug_assert( right_half != NULL );

		// Not implemented yet.
		return NLDB_OK;
	}

	int key_count() const {
		return key_count_;
	}

	int key_length() const {
		return key_length_;
	}

	int used_key_space() {
		return used_key_space_;
	}

	bool is_empty() const
	{
		return (used_key_space_ == 0 ) ? false : true;
	}

	bool is_full() const
	{
		return (used_key_space_ < key_space_size) ? false : true;
	}

	const void * min_key() const
	{
		return data_;
	}
};

#endif /* NLDBSEARCH_H_ */
