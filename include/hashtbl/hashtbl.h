#ifndef _hashtbl_t_H
#define _hashtbl_t_H

#include <pthread.h>

#include "list/list.h"

typedef size_t (* __hash)(const void *, size_t len);
typedef int (*keycmp_ptr) (const void *, const void *, size_t);

struct hash_entry {
  void * key;
  size_t klen;
  struct list_head list; //collision resolved by chaining
}

struct hash_table {
  struct hash_entry *table;

  size_t buckets;
  pthread_mutex_t *bucket_locks;

  pthread_mutex_t lock;
  hash_fn my_hash_fn;
  keycmp_ptr keycmp;

  /* private variables */
  unsigned int __ht_i;
  struct list_head *pos;
}


/**
 * This is a particular hashtable implementation, we will be
 * hashing pointers for keys, or similarly, int's (uint64, uint32).
 *
 * We are based on the kernel-type lists.
 *
 */

static inline int hash_table_bucket_lock(struct hash_table *t, unsigned int n)
{
	return (pthread_mutex_lock(&(t->bucket_locks[n])));
}

static inline int hash_table_bucket_unlock(struct hash_table *t, unsigned int n)
{
	return (pthread_mutex_unlock(&(t->bucket_locks[n])));
}

static inline int hash_table_lock(struct hash_table *t)
{
	return (pthread_mutex_lock(&(t->lock)));
}

static inline int hash_table_unlock(struct hash_table *t)
{
	return (pthread_mutex_unlock(&(t->lock)));
}

#ifdef EBUSY
static inline int hash_table_bucket_locked(struct hash_table *t, unsigned int n)
{
	return (pthread_mutex_trylock((t->bucket_locks[n])) == EBUSY);
}

static inline int hash_table_locked(struct hash_table *t)
{
	return (pthread_mutex_trylock(&(t->lock)) == EBUSY);
}
#else
static inline int hash_table_bucket_locked(struct hash_table *t, unsigned int n)
{
	return 0;
}

static inline int hash_table_locked(struct hash_table *t)
{
	return 0;
}
#endif

static inline int hash_table_hash_code(const struct hash_table *t,
				       const void *key, size_t len)
{

	return (__hash(key, len) % t->buckets);
}

static inline int hash_table_hash_code_safe(struct hash_table *t,
					    const void *key, size_t len)
{
	int n;

	hash_table_lock(t);
	n = __hash(key, len) % t->buckets;
	hash_table_unlock(t);

	return n;
}

static inline int hash_entry_init(struct hash_entry *e,
				  const void *key, size_t len)
{

	INIT_LIST_HEAD(&(e->list));

	if (str) {
		if ((e->key = (unsigned char *)malloc(len)) == NULL)
			return -1;
		memcpy(e->key, str, len);
		e->klen = len;
	}
	return 0;
}

static inline void hash_entry_finit(struct hash_entry *e)
{
	if (e->key)
		free(e->key);
	e->klen = 0;
}

static inline int hash_table_init(struct hash_table *h, unsigned int b,
				  keycmp_ptr keycmp)
{

	h->buckets = b;
	pthread_mutex_init(&(h->lock), NULL);

	if ((h->table =
	     (struct hash_entry *)malloc(sizeof(struct hash_entry) * b)) ==
	    NULL)
		return -1;

	if ((h->bucket_locks =
	     (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t) * b)) == NULL)
		return -1;

	for (--b; b != 0; --b) {
		hash_entry_init(&(h->table[b]), NULL, 0);
		pthread_mutex_init(&h->bucket_locks[b], NULL);
	}

	hash_entry_init(&(h->table[0]), NULL, 0);
	pthread_mutex_init(&h->bucket_locks[0], NULL);

	if (keycmp)
		h->keycmp = keycmp;
	else
		h->keycmp = &memcmp;

	return 0;
}

static inline void hash_table_finit(struct hash_table *h)
{

	if (h->table)
		free(h->table);
	h->buckets = 0;
}

/* insert_hash_table()
 * @h: &struct hash_table hash table to insert hash_entry into
 * @e: &struct hash_entry
 * Description: inserts @e into @h using @e->key as key. not thread-safe.
 */
void hash_table_insert(struct hash_table *h,
		       struct hash_entry *e,
		       const void *key, size_t len);


/* insert_hash_table_safe()
 * @h: &struct hash_table hash table to insert hash_entry into
 * @e: &struct hash_entry
 * @key: use key to insert the hash_entry
 * @len: length of the key
 * Description: inserts @e into @h using @e->key as key. thread-safe.
 */
void hash_table_insert_safe(struct hash_table *h,
			    struct hash_entry *e,
			    const void *key, size_t len);


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
					 size_t len);

/* hash_table_lookup_key_safe()
 * @h: hash table to look into
 * @str: the key to look for
 * @len: length of the key
 * Description: looks up the hash table for the presence of key. 
 * Returns: returns a pointer to the hash_entry that matches the key. otherise returns NULL.
 * Notes: in the presence of duplicate keys the function returns the first hash_entry found.
 * 		  function is not safe from delections. 
 * 		  function is not thread safe. 
 */
struct hash_entry *hash_table_lookup_key_safe(struct hash_table *h,
					      const void *str,
					      size_t len);


/* same as hash_table_lookup_key() but this function takes a valid hash_entry as input.
 * a valid hash_entry is the one that has key, len set appropriately. in other words, a
 * hash_entry that is the output of hash_entry_init()
 */
static inline struct hash_entry *hash_table_lookup_hash_entry(const struct
							      hash_table *h,
							      const struct
							      hash_entry *e)
{
	return (hash_table_lookup_key(h, e->key, e->klen));
}

/* same as hash_table_lookup_key_safe() but this function takes a valid hash_entry as 
 * input. a valid hash_entry is the one that has key, len set appropriately. in other 
 * words, a hash_entry that is the output of hash_entry_init()
 */
static inline struct hash_entry *hash_table_lookup_hash_entry_safe(struct hash_table
								   *h, const struct hash_entry
								   *e)
{
	return (hash_table_lookup_key_safe(h, e->key, e->klen));
}

struct hash_entry *hash_table_del_key(struct hash_table *h, const void *key,
				      size_t len);

struct hash_entry *hash_table_del_key_safe(struct hash_table *h,
					   const void *key, size_t len);

static inline struct hash_entry *hash_table_del_hash_entry(struct hash_table *h,
							   struct hash_entry *e)
{
	return (hash_table_del_key(h, e->key, e->klen));
}

static inline struct hash_entry *hash_table_del_hash_entry_safe(struct
								hash_table *h,
								struct
								hash_entry *e)
{
	return (hash_table_del_key_safe(h, e->key, e->klen));
}

/**
 * hash_entry - get the user data for this entry
 * @ptr:	the &struct hash_entry pointer
 * @type:	the type of the user data (e.g. struct my_data) embedded in this entry
 * @member:	the name of the hash_entry within the struct (e.g. entry)
 */
#define hash_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/*
 * @hentry: &struct hash_entry
 * @htable: &struct hash_table
 */
#define hash_table_for_each(hentry, htable)	\
	for	((htable)->__ht_i=0; ((htable)->__ht_i < (htable)->buckets); ++((htable)->__ht_i))	\
		for(((htable)->pos= (htable)->table[(htable)->__ht_i].list.next);		\
				((htable)->pos != &((htable)->table[(htable)->__ht_i].list)) &&	\
				((hentry) = ((struct hash_entry *)((char *)((htable)->pos)-(unsigned long)(&((struct hash_entry *)0)->list))) );	\
				(htable)->pos= (htable)->pos->next)

/*
 * @hentry: &struct hash_entry
 * @htable: &struct hash_table
 * @pos: &struct list_head
 * @hti: unsigned int
 */
#define hash_table_for_each_safe(hentry, htable, pos, hti)	\
	for	((hti)=0; ((hti) < (htable)->buckets); ++(hti))	\
		for(((pos)= (htable)->table[(hti)].list.next);		\
				((pos) != &((htable)->table[(hti)].list)) &&	\
				((hentry) = ((struct hash_entry *)((char *)((pos))-(unsigned long)(&((struct hash_entry *)0)->list))) );	\
				(pos)= (pos)->next)

#endif


#endif
