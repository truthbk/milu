#include <stdlib.h>

#include "hashtbl/hashtbl.h"

void hash_table_resize(struct hash_table *h)
{
    //TODO
    return;
}

/* insert_hash_table()
 * @h: &struct hash_table hash table to insert hash_entry into
 * @e: &struct hash_entry
 * Description: inserts @e into @h using @e->key as key. not thread-safe.
 */
void hash_table_insert(struct hash_table *h,
		       struct hash_entry *e,
		       const void *key, size_t len)
{
	unsigned int n;

	hash_entry_init(e, key, len);
	n = hash_table_hash_code(h, key, len);
	list_add(&(e->list), &(h->table[n].list));
}

/* insert_hash_table_safe()
 * @h: &struct hash_table hash table to insert hash_entry into
 * @e: &struct hash_entry
 * @key: use key to insert the hash_entry
 * @len: length of the key
 * Description: inserts @e into @h using @e->key as key. thread-safe.
 */
void hash_table_insert_safe(struct hash_table *h,
			    struct hash_entry *e,
			    const void *key, size_t len)
{
	unsigned int n;

	hash_entry_init(e, key, len);
	n = hash_table_hash_code_safe(h, key, len);

	hash_table_bucket_lock(h, n);
        if(h->_used_bucket >= h->_resize_threshold)
            hash_table_resize(h);
        if(list_empty(h->table[n].list))
                h->_used_buckets++;
	list_add(&(e->list), &(h->table[n].list));
	hash_table_bucket_unlock(h, n);
}

/* hash_table_lookup_key()
 * @h: hash table to look into
 * @str: the key to look for
 * @len: length of the key
 * Description: looks up the hash table for the presence of key. 
 * Returns: returns a pointer to the hash_entry that matches the key. otherise returns NULL.
 * Notes: in the presence of duplicate keys the function returns the first hash_entry found.
 * 		  function is not safe from delections. 
 * 		  function is not thread safe. 
 */
struct hash_entry *hash_table_lookup_key(const struct hash_table *h,
					 const void *key,
					 size_t len)
{
	unsigned int hcode = hash_table_hash_code(h, key, len);
	struct hash_entry *tmp;
	struct list_head *pos;

	list_for_each(pos, &(h->table[hcode].list))
	{
		tmp = list_entry(pos, struct hash_entry, list);

		if ((tmp->klen == len)
		    && (h->keycmp(tmp->key, key, tmp->klen) == 0))
			return tmp;
	}
	return NULL;
}

/* hash_table_lookup_key_safe()
 * @h: hash table to look into
 * @str: the key to look for
 * @len: length of the key
 * Description: looks up the hash table for the presence of key. 
 * Returns: returns a pointer to the hash_entry that matches the key. otherise returns NULL.
 * Notes: in the presence of duplicate keys the function returns the first hash_entry found.
 * 		  function is not safe from delections. 
 */
struct hash_entry *hash_table_lookup_key_safe(struct hash_table *h,
					      const void *key,
					      size_t len)
{

	unsigned int hcode = hash_table_hash_code_safe(h, key, len);
	struct hash_entry *tmp;
	struct list_head *pos;

	hash_table_bucket_lock(h, hcode);

	list_for_each(pos, &(h->table[hcode].list)) {
		tmp = list_entry(pos, struct hash_entry, list);

		if (memcmp(tmp->key, key, tmp->klen) == 0) {
			hash_table_bucket_unlock(h, hcode);
			return tmp;
		}
	}

	hash_table_bucket_unlock(h, hcode);
	return NULL;
}

struct hash_entry *hash_table_del_key(struct hash_table *h, const void *key,
				      size_t len)
{
	struct hash_entry *e;

	if ((e = hash_table_lookup_key(h, key, len)) == NULL)
		return NULL;

	list_del_init(&(e->list));
	return e;
}

struct hash_entry *hash_table_del_key_safe(struct hash_table *h,
					   const void *key, size_t len)
{
	struct hash_entry *e;
	unsigned int n = hash_table_hash_code(h, key, len);

	hash_table_bucket_lock(h, n);
	if ((e = hash_table_lookup_key(h, key, len)) != NULL) {
		list_del_init(&(e->list));
		hash_table_bucket_unlock(h, n);
		return e;
	}

	hash_table_bucket_unlock(h, n);
	return NULL;
}

