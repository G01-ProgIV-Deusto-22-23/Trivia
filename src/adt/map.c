#define U32_HASH_CONST  UINT32_C (0x45d9f3b)
#define U64_HASH_CONST1 UINT64_C (0xbf58476d1ce4e5b9)
#define U64_HASH_CONST2 UINT64_C (0x94d049bb133111eb)

struct __map_struct {
        size_t       cap;
        linkedlist_t data [
#ifdef __cplusplus
            0
#endif
        ];
};

typedef struct __keyval_struct {
        void      *key;
        void      *val;
        key_type_t kt;
} *keyval_t;

static uint32_t u32_hash (uint32_t x) {
    x = ((x >> 16) ^ x) * U32_HASH_CONST;
    x = ((x >> 16) ^ x) * U32_HASH_CONST;
    return (x >> 16) ^ x;
}

static uint64_t u64_hash (uint64_t x) {
    x = (x ^ (x >> 30)) * U64_HASH_CONST1;
    x = (x ^ (x >> 27)) * U64_HASH_CONST2;
    return x ^ (x >> 31);
}

static uint64_t str_hash (const char *const restrict str) {
    if (!str)
        return 0;

    uint64_t h = 0;
    for (size_t i = 0, l = strlen (str); i < l; h = ((h << 5) - h) + (uint64_t) * (str + i++))
        ;

    return h;
}

static int u32_keycmp (void *a, void *b) {
    if (((keyval_t) a)->kt != u32_key)
        return 1;

    return (int) ((uint32_t) (uintptr_t) ((keyval_t) a)->key != (uint32_t) (uintptr_t) b);
}

static int u64_keycmp (void *a, void *b) {
    if (((keyval_t) a)->kt != u64_key)
        return 1;

    return (int) ((uint64_t) ((keyval_t) a)->key != (uint64_t) b);
}

static int str_keycmp (void *a, void *b) {
    if (((keyval_t) a)->kt != str_key)
        return 1;

    if (!((keyval_t) a)->key)
        return b ? -1 : 0;

    else if (!b)
        return ((keyval_t) a)->key ? 1 : 0;

    return strcmp (((keyval_t) a)->key, b);
}

static keyval_t alloc_keyval (void *const k, void *const v, const key_type_t kt) {
    keyval_t kv;
    if (!(kv = malloc (sizeof (struct __keyval_struct))))
        return NULL;

    *kv = (struct __keyval_struct) { .key = ({
                                         void *_k = NULL;
                                         if (kt != str_key)
                                             _k = k;

                                         else if (k) {
                                             size_t l = strlen (k);
                                             if ((_k = malloc (l + 1)))
                                                 *(char *) mempcpy (_k, k, l) = '\0';
                                         }

                                         _k;
                                     }),
                                     .val = v,
                                     .kt = kt };

    if (!kv->key && kt == str_key) {
        free (kv);
        kv = NULL;
    }

    return kv;
}

static void dealloc_keyval (void *const kv) {
    if (!kv)
        return;

    if (((keyval_t) kv)->kt == str_key)
        free (((keyval_t) kv)->key);

    free (kv);
}

map_t impl_create_map (size_t capacity) {
    if (!capacity)
        capacity = DEFAULT_MAP_CAPACITY;

    map_t m;
    if (!(m = calloc (1, sizeof (struct __map_struct) + sizeof (linkedlist_t) * capacity)))
        return NULL;

    m->cap = capacity;

    return m;
}

bool impl_destroy_map (restrict map_t *const restrict m) {
    if (!(m && *m))
        return false;

    for (; (*m)->cap; destroy_linkedlist ((*m)->data + --(*m)->cap))
        ;

    free (*m);
    *m = NULL;

    return true;
}

size_t capacity_map (const restrict map_t m) {
    return m->cap;
}

map_t impl_resize_map (map_t *const restrict m, size_t capacity) {
    if (!m)
        return NULL;

    if (!*m)
        return *m = create_map (capacity);

    if (!capacity)
        capacity = DEFAULT_MAP_CAPACITY;

    size_t oldcap = (*m)->cap;

    if (oldcap == capacity)
        return *m;

    size_t    nkv = 0;
    keyval_t *kvs;
    {
        char *_kvs;
        if (!(_kvs = malloc ((sizeof (size_t) + sizeof (keyval_t *)) * oldcap)))
            return impl_destroy_map (m), *m;

        for (size_t i = 0; i < oldcap; nkv += *((size_t *) (void *) _kvs + i++)) {
            if (*((*m)->data + i)) {
                if (!(*((keyval_t **) (void *) (_kvs + sizeof (size_t) * oldcap) + i) =
                          toarray_linkedlist (*((*m)->data + i), (size_t *) (void *) _kvs + i))) {
                    i--;
                    for (size_t j, c; i != (size_t) -1;
                         free (*((keyval_t **) (void *) (_kvs + sizeof (size_t) * oldcap) + i--)))
                        for (j = 0, c = *(size_t *) (void *) _kvs + i; j < c;
                             dealloc_keyval (*(*((keyval_t **) (void *) (_kvs + sizeof (size_t) * oldcap) + i) + j++)))
                            ;
                    free (_kvs);

                    return impl_destroy_map (m), *m;
                }

                freedata_linkedlist (*((*m)->data + i), NULL);
                destroy_linkedlist ((*m)->data + i);
            }
        }

        if (!(kvs = malloc (sizeof (keyval_t) * nkv))) {
            for (size_t i = 0, j, c; i < oldcap; i++) {
                for (j = 0, c = *(size_t *) (void *) _kvs + i; j < c;
                     dealloc_keyval (*(*((keyval_t **) (void *) (_kvs + sizeof (size_t) * oldcap) + i) + j++)))
                    ;

                free (*((keyval_t **) (void *) (_kvs + sizeof (size_t) * oldcap) + i));
            }
            free (_kvs);

            return impl_destroy_map (m), *m;
        }

        keyval_t *pos = kvs;
        for (size_t i = 0; i < oldcap; free (*((keyval_t **) (void *) (_kvs + sizeof (size_t) * oldcap) + i++)))
            if (*((keyval_t **) (void *) (_kvs + sizeof (size_t) * oldcap) + i))
                pos = mempcpy (
                    pos, *((keyval_t **) (void *) (_kvs + sizeof (size_t) * oldcap) + i),
                    *((size_t *) (void *) _kvs + i) * sizeof (keyval_t)
                );
        free (_kvs);
    }

    if (!(*m = realloc (*m, sizeof (struct __map_struct) + sizeof (linkedlist_t) * capacity))) {
        for (size_t i = 0; i < nkv; dealloc_keyval (*(kvs + i++)))
            ;
        free (kvs);

        return *m;
    }

    if (capacity > oldcap)
        memset ((*m)->data + oldcap, 0, (capacity - oldcap) * sizeof (linkedlist_t));
    (*m)->cap = capacity;

    for (size_t i = 0; i < nkv; dealloc_keyval (*(kvs + i++)))
        if ((*(kvs + i))->key && !impl_put_map (*m, (*(kvs + i))->key, (*(kvs + i))->kt, (*(kvs + i))->val))
            warning ("could not reinsert key-value pair in the map.");
    free (kvs);

    return *m;
}

void *impl_get_map (const restrict map_t m, void *const k, const key_type_t kt) {
    return m ? ({
        keyval_t kv = find_linkedlist (
            *(m->data + ((size_t
                        ) (kt == u32_key   ? u32_hash ((uint32_t) (uintptr_t) k)
                           : kt == u64_key ? u64_hash ((uint64_t) (uintptr_t) k)
                                           : str_hash ((const char *) k))) %
                            m->cap),
            k, *(((cmpfunc_t *[]) { u32_keycmp, u64_keycmp, str_keycmp }) + kt)
        );
        kv ? kv->val : NULL;
    })
             : NULL;
}

bool impl_set_map (const restrict map_t m, void *const k, const key_type_t kt, void *const v) {
    return m ? ({
        keyval_t kv = find_linkedlist (
            *(m->data + ((size_t
                        ) (kt == u32_key   ? u32_hash ((uint32_t) (uintptr_t) k)
                           : kt == u64_key ? u64_hash ((uint64_t) (uintptr_t) k)
                                           : str_hash ((const char *) k))) %
                            m->cap),
            k, *(((cmpfunc_t *[]) { u32_keycmp, u64_keycmp, str_keycmp }) + kt)
        );
        kv ? (kv->val = v, true) : false;
    })
             : false;
}

bool impl_put_map (const restrict map_t m, void *const k, const key_type_t kt, void *const v) {
    if (!m)
        return false;

    if (impl_get_map (m, k, kt))
        return impl_set_map (m, k, kt, v);

    keyval_t kv;
    if (!(kv = alloc_keyval (k, v, kt)))
        return false;

    size_t i = ((size_t
               ) (kt == u32_key   ? u32_hash ((uint32_t) (uintptr_t) k)
                  : kt == u64_key ? u64_hash ((uint64_t) (uintptr_t) k)
                                  : str_hash ((const char *) k))) %
               m->cap;

    if (!*(m->data + i)) {
        if (!(*(m->data + i) = create_linkedlist (kv))) {
            free (kv);

            return false;
        }

        freedata_linkedlist (*(m->data + i), dealloc_keyval);
    }

    else if (!insert_linkedlist (*(m->data + i), kv)) {
        free (kv);

        return false;
    }

    return true;
}

bool impl_remove_map (const restrict map_t m, void *const k, const key_type_t kt) {
    return m ? ({
        const size_t i = indexof_linkedlist (
            *(m->data + ((size_t
                        ) (kt == u32_key   ? u32_hash ((uint32_t) (uintptr_t) k)
                           : kt == u64_key ? u64_hash ((uint64_t) (uintptr_t) k)
                                           : str_hash ((const char *) k))) %
                            m->cap),
            k, *(((cmpfunc_t *[]) { u32_keycmp, u64_keycmp, str_keycmp }) + kt)
        );

        if (i != (size_t) -1)
            remove_linkedlist (
                *(m->data + ((size_t
                            ) (kt == u32_key   ? u32_hash ((uint32_t) (uintptr_t) k)
                               : kt == u64_key ? u64_hash ((uint64_t) (uintptr_t) k)
                                               : str_hash ((const char *) k))) %
                                m->cap),
                i
            );

        i != (size_t) -1;
    })
             : false;
}