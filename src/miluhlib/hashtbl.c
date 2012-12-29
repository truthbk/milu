#include <stdlib.h>

#include "hashtbl/hashtbl.h"

static int hash_table_resize(struct hash_table *h);

static inline void hash_table_insert(struct hash_table *h,
		       struct hash_entry *e )
{
	unsigned int n;

	n = hash_table_hash_code(h, e->key, e->klen);
	list_add(&(e->list), &(h->table[n].list));
}

/* insert_hash_table(_i)
 * @h: &struct hash_table hash table to insert hash_entry into
 * @e: &struct hash_entry
 * Description: inserts @e into @h using @e->key as key. not thread-safe.
 */
void hash_table_insert_i(struct hash_table *h,
                         struct hash_entry *e,
                         const uintptr_t key, size_t len)
{
    hash_entry_init(e, (const void *)key, len, 0);
    hash_table_insert(h, e);
}

/* insert_hash_table_s()
 * @h: &struct hash_table hash table to insert hash_entry into
 * @e: &struct hash_entry
 * Description: inserts @e into @h using @e->key as key. not thread-safe.
 */
void hash_table_insert_s(struct hash_table *h,
                         struct hash_entry *e,
                         const char * key, size_t len)
{
    hash_entry_init(e, (const void *)key, len, 1);
    hash_table_insert(h, e);
}

/* insert_hash_table_safe()
 * @h: &struct hash_table hash table to insert hash_entry into
 * @e: &struct hash_entry
 * @key: use key to insert the hash_entry
 * @len: length of the key
 * Description: inserts @e into @h using @e->key as key. thread-safe.
 */
static inline void hash_table_insert_safe(struct hash_table *h,
        struct hash_entry *e)
{
    unsigned int n;
    short resized = 0;

    n = hash_table_hash_code_safe(h, e->key, e->klen);

    if(h->_used_buckets >= h->_resize_threshold)
    {
        if(!hash_table_resize(h))
            resized = 1;
    }

    if(resized)
    {
        n = hash_table_hash_code_safe(h, e->key, e->klen);
    }
    hash_table_bucket_lock(h, n);
    if(list_empty(&h->table[n].list))
        h->_used_buckets++;
    list_add(&(e->list), &(h->table[n].list));
    hash_table_bucket_unlock(h, n);
}

void hash_table_insert_safe_i(struct hash_table *h,
        struct hash_entry *e,
        const uintptr_t key, size_t len)
{
    hash_entry_init(e, (const void *)key, len, 0);
    hash_table_insert_safe(h, e);
}

void hash_table_insert_safe_s(struct hash_table *h,
        struct hash_entry *e,
        const char * key, size_t len)
{
    hash_entry_init(e, (const void *)key, len, 1);
    hash_table_insert_safe(h, e);
}

/* hash_table_lookup_key()
 * @h: hash table to look into
 * @key: the key to look for
 * @len: length of the key
 * Description: looks up the hash table for the presence of key. 
 * Returns: returns a pointer to the hash_entry that matches the key. otherise returns NULL.
 * Notes: in the presence of duplicate keys the function returns the first hash_entry found.
 * 		  function is not safe from delections. 
 * 		  function is not thread safe. 
 */
static struct hash_entry *hash_table_lookup_key(const struct hash_table *h,
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

struct hash_entry *hash_table_lookup_key_i(const struct hash_table *h,
					 const uintptr_t key,
					 size_t len)
{
    return hash_table_lookup_key(h, (const void *)key, len);
}

struct hash_entry *hash_table_lookup_key_s(const struct hash_table *h,
					 const char * key,
					 size_t len)
{
    return hash_table_lookup_key(h, (const void *)key, len);
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
static struct hash_entry *hash_table_lookup_key_safe(struct hash_table *h,
					      const void *key,
					      size_t len)
{

	unsigned int hcode = hash_table_hash_code_safe(h, key, len);
	struct hash_entry *tmp;
	struct list_head *pos;

	hash_table_bucket_lock(h, hcode);

	list_for_each(pos, &(h->table[hcode].list)) {
		tmp = list_entry(pos, struct hash_entry, list);

		if (h->keycmp(tmp->key, key, tmp->klen) == 0) {
			hash_table_bucket_unlock(h, hcode);
			return tmp;
		}
	}

	hash_table_bucket_unlock(h, hcode);
	return NULL;
}

struct hash_entry *hash_table_lookup_key_safe_i(struct hash_table *h,
					 const uintptr_t key,
					 size_t len)
{
    if(len > sizeof(uintptr_t))
        return NULL;
    return hash_table_lookup_key_safe(h, (const void *)key, len);
}

struct hash_entry *hash_table_lookup_key_safe_s(struct hash_table *h,
					 const char * key,
					 size_t len)
{
    return hash_table_lookup_key_safe(h, (const void *)key, len);
}

static struct hash_entry *hash_table_del_key(struct hash_table *h, 
                                      const void *key,
				      size_t len)
{
	struct hash_entry *e;

	if ((e = hash_table_lookup_key(h, key, len)) == NULL)
		return NULL;

	list_del_init(&(e->list));
	return e;
}

struct hash_entry *hash_table_del_key_i(struct hash_table *h, 
                                      const uintptr_t key,
				      size_t len)
{
    if(len > sizeof(uintptr_t))
        return NULL;

    return hash_table_del_key(h, (const void *)key, len);
}

struct hash_entry *hash_table_del_key_s(struct hash_table *h, 
                                      const char *key,
				      size_t len)
{
    return hash_table_del_key(h, (const void *)key, len);
}

static struct hash_entry *hash_table_del_key_safe(struct hash_table *h,
					   const void *key, 
                                           size_t len)
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

struct hash_entry *hash_table_del_key_safe_i(struct hash_table *h, 
                                      const uintptr_t key,
				      size_t len)
{
    if(len > sizeof(uintptr_t))
        return NULL;

    return hash_table_del_key_safe(h, (const void *)key, len);
}

struct hash_entry *hash_table_del_key_safe_s(struct hash_table *h, 
                                      const char *key,
				      size_t len)
{
    return hash_table_del_key_safe(h, (const void *)key, len);
}

static int hash_table_resize(struct hash_table *h)
{
    int ret;
    unsigned int i;
    //need these to iterate hashtable
    struct hash_table aux_htbl;
    struct hash_entry * hentry, * hentry_aux;
    struct list_head  * lh = NULL;
    struct list_head  * laux = NULL;
    unsigned int hti;

    hash_table_lock(h);
    ret = hash_table_init( &aux_htbl,
            h->_resize_threshold*2,
            h->keycmp,
            h->my_hash_fn);

    if(ret)
    {
        hash_table_unlock(h);
        return ret;
    }

    hash_table_for_each_safe( hentry, h, lh, laux, hti )
    {
        hentry_aux = hash_table_del_hash_entry_safe( h, hentry );

        if(hentry_aux)
        {
            hash_table_insert( &aux_htbl, hentry_aux); 
        }
    }

    //we've reused hentries so no need to free those!
    free(h->table);
    for( i = 0 ; i < h->buckets ; i++ )
    {
        pthread_mutex_destroy(&h->bucket_locks[i]);
    }
    free(h->bucket_locks);

    h->bucket_locks = aux_htbl.bucket_locks;
    h->table = aux_htbl.table;
    h->buckets = aux_htbl.buckets;
    h->_used_buckets = aux_htbl._used_buckets;
    h->_resize_threshold = aux_htbl._resize_threshold;
    h->_factor = aux_htbl._factor;

    hash_table_unlock(h);

    /*
     * we intentionally don't call hash_table_finit on aux_htbl.
     * Those resources are then used by the resized table.
     */
    return 0;
}
