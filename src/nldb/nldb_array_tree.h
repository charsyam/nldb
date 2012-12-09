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

#ifndef NLDB_ARRAY_TREE_H_
#define NLDB_ARRAY_TREE_H_ (1)

#include "nldb_sorted_array.h"

// for memcmp
#include <string.h>

#include <txbase/tx_assert.h>

const int NLDB_TREE_MIN_KEY_SIZE = 1;
/*
 * nldb_sorted_array : a sorted array of fixed size keys. Each key maps to a void* data.
 */

template<int key_space_size> class nldb_array_tree {
protected:

	class node_t {
	public :
		// The magic value to check if the node memory is not corrupt
		typedef enum NODE_MAGIC_VALUE {
			INTERNAL_NODE_MAGIC=0xcafecafe,
			LEAF_NODE_MAGIC=0xcafebebe
		}NODE_MAGIC_VALUE;
	protected :
		// The parent node.
		node_t * parent_;
		// Magic value.
		NODE_MAGIC_VALUE magic_;
	public :
		inline bool is_internal_node()
		{
			return (magic_ == INTERNAL_NODE_MAGIC) ? true : false;
		}

		inline bool is_leaf_node()
		{
			return (magic_ == LEAF_NODE_MAGIC) ? true : false;
		}

		node_t(NODE_MAGIC_VALUE magic) {
			tx_debug_assert(magic == INTERNAL_NODE_MAGIC || magic == LEAF_NODE_MAGIC);

			parent_ = NULL;
			magic_ = magic;
		}
		virtual ~node_t() {}

		inline node_t * parent() {
			return parent_;
		}

		inline void set_parent( node_t * parent ) {
			parent_ = parent;
		}
	};

	class leaf_node_t : public node_t
	{
	private:
		// The sorted keys in an array. Each key has a mapping from a key to pointer of the value.
		nldb_sorted_array<key_space_size> keys_with_values_;

	public:
		// The next sibling node
		leaf_node_t * next_;
		// The previous sibling
		leaf_node_t * prev_;

		leaf_node_t(const key_length_t & key_length) : node_t( node_t::LEAF_NODE_MAGIC )
		{
			tx_debug_assert( key_length > 0 );

			keys_with_values_.init(key_length);
		}
		virtual ~leaf_node_t()
		{
		}

		bool is_empty() const
		{
			return keys_with_values_.is_empty();
		}

		bool is_full() const
		{
			return keys_with_values_.is_full();
		}

		const void * min_key() const
		{
			return keys_with_values_.min_key();
		}

		nldb_sorted_array<key_space_size> & keys_with_values()
		{
			return keys_with_values_;
		}

		nldb_rc_t put (const void * key, const void * value)
		{
			tx_debug_assert( key != NULL );
			tx_debug_assert( value != NULL );

			nldb_rc_t rc = keys_with_values_.put(key, value);
			if (rc) return rc;

			return NLDB_OK;
		}
		nldb_rc_t get (const void * key, void ** value ) const
		{
			tx_debug_assert( key != NULL );
			tx_debug_assert( value != NULL );

			nldb_rc_t rc = keys_with_values_.get(key, value);
			if (rc) return rc;

			return NLDB_OK;
		}
		nldb_rc_t del (const void * key, void ** value)
		{
			tx_debug_assert( key != NULL );
			tx_debug_assert( value != NULL );

			nldb_rc_t rc = keys_with_values_.del(key, value);
			if (rc) return rc;

			return NLDB_OK;
		}

		nldb_rc_t merge_with(leaf_node_t * node)
		{
			tx_debug_assert( node != NULL );

			// Not implemented yet. Merging nodes is P2. We will implement the feature after the first release of NLDB.
			return NLDB_ERROR_UNSUPPORTED_FEATURE;
		}

		nldb_rc_t split(leaf_node_t ** right_node )
		{
			tx_debug_assert( right_node != NULL );

			leaf_node_t * node = node_factory::new_leaf_node(keys_with_values_.key_length());

			// Move half of keys in this node to "node".
			keys_with_values_.split( & node->keys_with_values() );
			// Not implemented yet.

			// Maintain the double linked list on the leaf nodes.
			node->prev_ = this;
			node->next_ = this->next_;
			this->next_ = node;

			return NLDB_OK;
		}

	};

	class internal_node_t : public node_t
	{
	private:
		// The sorted keys in an array. Each key has a mapping from a key to the right children node.
		nldb_sorted_array<key_space_size> keys_with_right_children_;

		// The child node which has keys less than than the first key.
		node_t * left_child_;
	public :
		internal_node_t(const key_length_t & key_length) : node_t( node_t::INTERNAL_NODE_MAGIC )
		{
			tx_debug_assert( key_length > 0 );

			keys_with_right_children_.init(key_length);
		}
		virtual ~internal_node_t()
		{
		}

		void set_left_child(node_t * left_child)
		{
			tx_debug_assert( left_child != NULL );

			left_child_ = left_child;

			left_child_->set_parent(this);
		}

		node_t * left_child() const
		{
			return left_child_;
		}

		bool is_empty() const
		{
			return keys_with_right_children_.is_empty();
		}

		bool is_full() const
		{
			return keys_with_right_children_.is_full();
		}

		const void * min_key() const
		{
			return keys_with_right_children_.min_key();
		}

		nldb_sorted_array<key_space_size> & keys_with_right_children()
		{
			return keys_with_right_children_;
		}

		nldb_rc_t put (const void * key, node_t * node )
		{
			tx_debug_assert( key != NULL );
			tx_debug_assert( node != NULL );

			nldb_rc_t rc = keys_with_right_children_.put(key, node);
			if (rc) return rc;

			node->set_parent(this);

			return NLDB_OK;
		}

		// Return the subtree which should serve the given key.
		// Note that the key might not exist in the subtree.
		// Set the root of the subtree to *node.
		nldb_rc_t find_serving_node (const void * key, node_t ** node ) const
		{
			tx_debug_assert( key != NULL );
			tx_debug_assert( node != NULL );

			// find the node that is mapped with the first greater than or equal to the given key.
			nldb_rc_t rc = keys_with_right_children_.find_first_ge_key(key, (void**)node);
			if (rc) return rc;

			// keys_with_right_children_ does not have the serving node for the key.
			// IOW, all keys in keys_with_right_children_ are greater than the given key.
			// In this case, the left child of this node should serve the key in this case.
			if ( *node == NULL )
				*node = left_child_;

			return NLDB_OK;
		}

		nldb_rc_t del (const void * key, node_t ** deleted_node )
		{
			tx_debug_assert( key != NULL );
			tx_debug_assert( deleted_node != NULL );

			node_t * node;

			nldb_rc_t rc = keys_with_right_children_.get(key, (void**)&node);
			if (rc) return rc;

			node->set_parent(NULL);

			*deleted_node = node;

			return NLDB_OK;
		}

		nldb_rc_t merge_with(internal_node_t * node)
		{
			tx_debug_assert( node != NULL );

			// Not implemented yet. Merging nodes is P2. We will implement the feature after the first release of NLDB.
			return NLDB_ERROR_UNSUPPORTED_FEATURE;
		}

		nldb_rc_t split(internal_node_t ** right_node, void ** mid_key )
		{
			tx_debug_assert( right_node != NULL );
			tx_debug_assert( mid_key != NULL );

			internal_node_t * node = node_factory::new_internal_node(keys_with_right_children_.key_length());

			// Move half of keys in this node to "node".
			keys_with_right_children_.split( & node->keys_with_right_children() );
			// Not implemented yet.

			// The key in the middle between the two split nodes.
			void * mid_key_node = NULL;
			keys_with_right_children_.remove_max_key(mid_key, & mid_key_node);

			// Even after removing the maximum key, the node should have at least a key.
			tx_assert(keys_with_right_children_.key_count() > 0 );

			// Set the node attached with the mid key to the left child of the new split node.
			node->set_left_child((node_t*)mid_key_node);

			return NLDB_OK;
		}
	};

	class node_factory {
    public :
		static leaf_node_t * new_leaf_node(const key_length_t & key_length)
		{
			tx_debug_assert( key_length > 0 );

			leaf_node_t * p = new leaf_node_t(key_length);
			return p;
		}
		static void return_leaf_node(leaf_node_t * leaf_node)
		{
			tx_debug_assert( leaf_node != NULL );

			delete leaf_node;
		}
		static internal_node_t * new_internal_node(const key_length_t & key_length)
		{
			tx_debug_assert( key_length > 0 );

			internal_node_t * p = new internal_node_t(key_length);
			return p;
		}
		static void return_internal_node(internal_node_t * internal_node)
		{
			tx_debug_assert( internal_node != NULL );

			delete internal_node;
		}
	};

	inline int compare_keys(void * key1, void * key2) const
	{
		tx_debug_assert( is_initialized() );
		tx_debug_assert( key_length_ > 0 );

		tx_debug_assert( key1 != NULL );
		tx_debug_assert( key2 != NULL );

		return memcmp(key1, key2, key_length_);
	}

	inline int compare_keys(const void * key1, void * key2) const
	{
		tx_debug_assert( is_initialized() );
		tx_debug_assert( key_length_ > 0 );

		tx_debug_assert( key1 != NULL );
		tx_debug_assert( key2 != NULL );

		return memcmp(key1, key2, key_length_);
	}

	inline int compare_keys(void * key1, const void * key2) const
	{
		tx_debug_assert( is_initialized() );
		tx_debug_assert( key_length_ > 0 );

		tx_debug_assert( key1 != NULL );
		tx_debug_assert( key2 != NULL );

		return memcmp(key1, key2, key_length_);
	}

	inline int compare_keys(const void * key1, const void * key2) const
	{
		tx_debug_assert( is_initialized() );
		tx_debug_assert( key_length_ > 0 );

		tx_debug_assert( key1 != NULL );
		tx_debug_assert( key2 != NULL );

		return memcmp(key1, key2, key_length_);
	}

	nldb_rc_t find_leaf_node(const void * key, leaf_node_t ** node) const {
		tx_debug_assert( is_initialized() );

		tx_debug_assert( key != NULL );
		tx_debug_assert( node != NULL );

		node_t * n;
		for (n = root_node_;
			 n->is_internal_node();
			 )
		{
			internal_node_t * internal_node = (internal_node_t*) n ;

			nldb_rc_t rc = internal_node->find_serving_node(key, &n);
			if (rc) return rc;
		}

		tx_debug_assert( n->is_leaf_node() );

		*node = (leaf_node_t*) n;

		return NLDB_OK;
	}

	nldb_rc_t put_to_internal_node(internal_node_t * node, const void * key, node_t * key_node)
	{
		tx_debug_assert( is_initialized() );

		tx_debug_assert( node != NULL );
		tx_debug_assert( key != NULL );
		tx_debug_assert( key_node != NULL );

		if ( node->is_full() )
		{
			internal_node_t * right_node = NULL;

			// The key in the middle between the original node and the right node.
			// The key was removed from the original node.
			// right_node node not keep the key, but its left_child_ points to the node of the removed key.
			void * mid_key = NULL;

			nldb_rc_t rc = node->split( &right_node, & mid_key );
			if (rc) return rc;

			int cmp = compare_keys(key, mid_key);
			if ( cmp < 0 )
			{
				rc = node->put(key, key_node);
				if (rc) return rc;
			}
			else if (cmp > 0 )
			{
				rc = right_node->put(key, key_node);
				if (rc) return rc;
			}
			else // cmp == 0
			{
				// The mid_key is the key in the middle in the node to be splitted.
				// The key and key_node is from the child of this node, which is splitted before this function was called.
				// It does not make sense the key in the middle of the splitted child node exists in the parent(this) node.
				tx_assert(0);
			}

			if ( node->parent() ) // not a root node
			{
				rc = put_to_internal_node( (internal_node_t*) node->parent(), mid_key, right_node);
				if (rc) return rc;
			}
			else // root node
			{
				// create a new root node
				internal_node_t * new_root_node = node_factory::new_internal_node(key_length_);
				// Set the old root node as the left child of the new root node
				new_root_node->set_left_child(node);
				// Set the right node as the 2nd child of the new root node.
				new_root_node->put(mid_key, right_node);
				// Yes! the new root node is elected!
				root_node_ = new_root_node;
			}
		}
		else
		{
			nldb_rc_t rc = node->put(key, key_node);
			if (rc) return rc;
		}

		return NLDB_OK;
	}

	nldb_rc_t put_to_leaf_node(leaf_node_t * node, const void * key, const void * value)
	{
		tx_debug_assert( is_initialized() );

		tx_debug_assert( node != NULL );
		tx_debug_assert( key != NULL );
		tx_debug_assert( value != NULL );

		if ( node->is_full() )
		{
			leaf_node_t * right_node = NULL;

			nldb_rc_t rc = node->split( &right_node );
			if (rc) return rc;

			const void * mid_key = right_node->min_key();

			if ( compare_keys(mid_key, key) < 0 )
			{
				rc = node->put(key, value);
				if (rc) return rc;
			}
			else
			{
				rc = right_node->put(key,value);
				if (rc) return rc;
			}

			rc = put_to_internal_node( (internal_node_t*) node->parent(), mid_key, right_node);
			if (rc) return rc;
		}
		else
		{
			nldb_rc_t rc = node->put(key, value);
			if (rc) return rc;
		}
		return NLDB_OK;
	}

	nldb_rc_t del_from_internal_node(internal_node_t * node, const void * key, node_t ** deleted_node)
	{
		tx_debug_assert( is_initialized() );

		tx_debug_assert( node != NULL );
		tx_debug_assert( key != NULL );
		tx_debug_assert( deleted_node != NULL );

		const void * old_min_key = node->min_key();

		nldb_rc_t rc = node->del(key, deleted_node);
		if (rc) return rc;

		if ( compare_keys( old_min_key, key) == 0 ) // Was the deleted key the minimum key?
		{
			internal_node_t * parent = (internal_node_t*) node->parent();

			if ( parent )
			{
				if ( node == parent->left_child() )
				{
					// Do nothing. We have nothing to change in the parent.
				}
				else
				{
					const void * new_min_key = node->min_key();

					node_t * deleted_node = NULL;

					// TODO : find a way to update an existing key in the node.

					// delete the old minimum key in the parent.
					rc = del_from_internal_node(parent, old_min_key, & deleted_node);
					if (rc) return rc;

					tx_assert(deleted_node == node);

					// put the new minimum key in the parent.
					rc = put_to_internal_node(parent, new_min_key, node);
					if (rc) return rc;
				}
			}
			else
			{
				// parent == NULL
				// The node is root node, we have no parent node to update.
			}
		}

		return NLDB_OK;
	}


	nldb_rc_t del_from_leaf_node(leaf_node_t * node, const void * key, void ** deleted_value)
	{
		tx_debug_assert( is_initialized() );

		tx_debug_assert( node != NULL );
		tx_debug_assert( key != NULL );
		tx_debug_assert( deleted_value != NULL );

		const void * old_min_key = node->min_key();

		nldb_rc_t rc = node->del(key, deleted_value);
		if (rc) return rc;

		if ( compare_keys( old_min_key, key) == 0 ) // Was the deleted key the minimum key?
		{
			internal_node_t * parent = (internal_node_t*) node->parent();

			if ( node == parent->left_child() )
			{
				// Do nothing. We have nothing to change in the parent.
			}
			else
			{
				const void * new_min_key = node->min_key();

				node_t * deleted_node = NULL;

				// TODO : find a way to update an existing key in the node.

				// delete the old minimum key in the parent.
				rc = del_from_internal_node(parent, old_min_key, & deleted_node);
				if (rc) return rc;

				tx_assert(deleted_node == node);

				// put the new minimum key in the parent.
				rc = put_to_internal_node(parent, new_min_key, node);
				if (rc) return rc;
			}
		}

		return NLDB_OK;
	}


	bool initialized_;
	int  key_length_;

	internal_node_t * root_node_;

public:
	typedef struct iterator
	{
		// The node that has the key to iterate.
		leaf_node_t * current_node_;
		// The iterator within the sorted key array in the current_node_.
		typename nldb_sorted_array<key_space_size>::iterator key_iter_;
	} iterator;

	int key_length() {
		tx_debug_assert( key_length_ > 0 );

		return key_length_;
	}
	nldb_array_tree() {
		initialized_ = false;
	}
	virtual ~nldb_array_tree() {}

	bool is_initialized() {
		return initialized_;
	}

	nldb_rc_t init(const key_length_t & key_length)
	{
		// the array tree object should not be initialized twice.
		tx_debug_assert( ! is_initialized() );

		initialized_ = true;
		key_length_ = key_length;

		root_node_ = node_factory::new_internal_node( key_length_ );

		// Not implemented yet.
		return NLDB_OK;
	}

	nldb_rc_t destroy()
	{
		initialized_ = false;
		// Not implemented yet.
		return NLDB_OK;
	}

	nldb_rc_t put (const void * key, const void * value)
	{
		tx_debug_assert( is_initialized() );
		tx_debug_assert( key != NULL );
		tx_debug_assert( value != NULL );

		if (root_node_->is_empty()) // No key exists at all.
		{
			leaf_node_t * leaf = node_factory::new_leaf_node( key_length_ );

			// Put the value on the leaf node.
			nldb_rc_t rc = leaf->put(key, value);
			if (rc) return rc;

			// Set the leaf node as the left child of the root node.
			(void)root_node_->set_left_child(leaf);
		}

		leaf_node_t * leaf = NULL;

		nldb_rc_t rc = find_leaf_node(key, &leaf);
		if (rc) return rc;

		rc = put_to_leaf_node(leaf, key, value);
		if (rc) return rc;

		return NLDB_OK;
	}

	nldb_rc_t get (const void * key, void ** value ) const
	{
		tx_debug_assert( is_initialized() );
		tx_debug_assert( key != NULL );
		tx_debug_assert( value != NULL );

		leaf_node_t * leaf_node = NULL;

		nldb_rc_t rc = find_leaf_node(key, & leaf_node);
		if (rc) return rc;

		tx_assert(leaf_node);

		rc = leaf_node->get(key, value);
		if (rc) return rc;

		return NLDB_OK;
	}

	nldb_rc_t del (const void * key, void ** deleted_value)
	{
		tx_debug_assert( is_initialized() );
		tx_debug_assert( key != NULL );
		tx_debug_assert( deleted_value != NULL );

		leaf_node_t * leaf_node = NULL;

		nldb_rc_t rc = find_leaf_node(key, & leaf_node);
		if (rc) return rc;

		tx_assert(leaf_node);

		rc = del_from_leaf_node(leaf_node, key, deleted_value);
		if (rc) return rc;

		return NLDB_OK;
	}

	nldb_rc_t seek_forward(const void * key, iterator * iter) const
	{
		tx_debug_assert( is_initialized() );
		tx_debug_assert( key != NULL );
		tx_debug_assert( iter != NULL );

		leaf_node_t * leaf_node = NULL;

		nldb_rc_t rc = find_leaf_node(key, & leaf_node);
		if (rc) return rc;

		// Even when the key does not exist, find_leaf_node returns a node where the key "SHOULD" be inserted.
		// leaf_node should not be NULL in any case.
		tx_assert( leaf_node );

		iter->current_node_ = leaf_node;

		nldb_sorted_array<key_space_size> & keys_with_values = leaf_node->keys_with_values();

		rc = keys_with_values.iter_forward(key, &iter->key_iter_);
		if (rc) return rc;

		return NLDB_OK;
	}

	nldb_rc_t seek_backward(const void * key, iterator * iter) const
	{
		tx_debug_assert( is_initialized() );
		tx_debug_assert( key != NULL );
		tx_debug_assert( iter != NULL );

		leaf_node_t * leaf_node = NULL;

		nldb_rc_t rc = find_leaf_node(key, & leaf_node);
		if (rc) return rc;

		// Even when the key does not exist, find_leaf_node returns a node where the key "SHOULD" be inserted.
		// leaf_node should not be NULL in any case.
		tx_assert( leaf_node );

		iter->current_node_ = leaf_node;

		nldb_sorted_array<key_space_size> & keys_with_values = leaf_node->keys_with_values();

		rc = keys_with_values.iter_backward(key, &iter->key_iter_);
		if (rc) return rc;

		return NLDB_OK;
	}

	nldb_rc_t move_forward(iterator & iter, void ** key, void ** value, bool * end_of_iter) const
	{
		tx_debug_assert( is_initialized() );
		tx_debug_assert( key != NULL );
		tx_debug_assert( value != NULL );
		tx_debug_assert( end_of_iter != NULL );

		if ( iter.current_node_ == NULL )
		{
			return NLDB_ERROR_END_OF_ITERATION;
		}

		nldb_sorted_array<key_space_size> & keys_with_values = iter.current_node_->keys_with_values();

		nldb_rc_t rc = keys_with_values.iter_next(iter.key_iter_, key, value);
		if (rc) return rc;

		if (*key == NULL) { // No more keys to iterate

			// Move to next leaf node.
			iter.current_node_ = iter.current_node_->next_;

			if ( iter.current_node_ )
			{
				iter.current_node_->keys_with_values().iter_forward(&iter.key_iter_);
			}
		}

		return NLDB_OK;
	}

	nldb_rc_t move_backward(iterator & iter, void ** key, void ** value, bool * end_of_iter) const
	{
		tx_debug_assert( is_initialized() );
		tx_debug_assert( key != NULL );
		tx_debug_assert( value != NULL );
		tx_debug_assert( end_of_iter != NULL );

		if ( iter.current_node_ == NULL )
		{
			return NLDB_ERROR_END_OF_ITERATION;
		}

		nldb_sorted_array<key_space_size> & keys_with_values = iter.current_node_->keys_with_values();

		nldb_rc_t rc = keys_with_values.iter_prev(iter.key_iter_, key, value);
		if (rc) return rc;

		if (*key == NULL) { // No more keys to iterate

			// Move to previous leaf node.
			iter.current_node_ = iter.current_node_->prev_;

			if ( iter.current_node_ )
			{
				iter.current_node_->keys_with_values().iter_backward(&iter.key_iter_);
			}
		}

		return NLDB_OK;
	}
};

#endif /* NLDB_ARRAY_TREE_H_ */

