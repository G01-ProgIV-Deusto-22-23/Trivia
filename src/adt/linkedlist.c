struct __linkedlist_struct {
        void                       *data;
        size_t                      len;
        struct __linkedlist_struct *next;
        freefunc_t                 *free;
};

linkedlist_t create_linkedlist (void *const data) {
    linkedlist_t l;
    if (!(l = calloc (1, sizeof (struct __linkedlist_struct))))
        return NULL;

    l->len++;
    l->data = data;

    return l;
}

bool impl_destroy_linkedlist (linkedlist_t *const restrict l) {
    if (!l)
        return false;

    for (linkedlist_t temp; *l; free (temp)) {
        temp = *l;
        *l   = (*l)->next;

        if (temp->free)
            temp->free (temp->data);
    }

    return true;
}

size_t length_linkedlist (linkedlist_t l) {
    return l ? l->len : 0;
}

void freedata_linkedlist (const restrict linkedlist_t l, freefunc_t *const free) {
    if (l)
        l->free = free;
}

bool insert_linkedlist (linkedlist_t l, void *const data) {
    if (!l && l->len == SIZE_MAX - 1)
        return false;

    for (; l->next; l = l->next)
        ;

    if (!(l->next = create_linkedlist (data)))
        return false;

    return ++l->len;
}

bool impl_remove_linkedlist (linkedlist_t *const restrict l, const size_t i) {
    if (!l || i >= length_linkedlist (*l))
        return false;

    linkedlist_t temp = (*l)->next;

    if (!i) {
        size_t      len = (*l)->len;
        freefunc_t *f   = (*l)->free;

        if (f)
            f ((*l)->data);
        free (*l);
        *l = temp;

        if (*l) {
            (*l)->len  = --len;
            (*l)->free = f;
        }

        return true;
    }

    linkedlist_t prev = *l;
    for (size_t j = 1; temp; temp = (prev = temp)->next) {
        if (j++ == i) {
            prev->next = temp->next;
            if ((*l)->free)
                (*l)->free (temp->data);

            free (temp);
            (*l)->len--;

            return true;
        }
    }

    unreachable ();
    return false;
}

void *impl_find_linkedlist (linkedlist_t l, void *const data, cmpfunc_t *const cmp, size_t *const restrict i) {
    for (size_t j = 0; l; l = l->next, j++)
        if (l->data == data || (cmp && !cmp (l->data, data))) {
            if (i)
                *i = j;

            return l->data;
        }

    if (i)
        *i = (size_t) -1;

    return NULL;
}

size_t impl_indexof_linkedlist (linkedlist_t l, void *const data, cmpfunc_t *const cmp) {
    size_t i;
    impl_find_linkedlist (l, data, cmp, &i);

    return i;
}

void *impl_pop_linkedlist (linkedlist_t *const restrict l, const size_t i) {
    if (!(l && i >= length_linkedlist (*l) && !(*l)->free))
        return NULL;

    linkedlist_t temp = (*l)->next;

    if (!i) {
        size_t len  = (*l)->len;
        void  *data = (*l)->data;

        free (*l);
        *l = temp;

        if (*l)
            (*l)->len = --len;

        return data;
    }

    linkedlist_t prev = *l;
    for (size_t j = 1; temp; temp = (prev = temp)->next) {
        if (j++ == i) {
            void *data = temp->data;
            prev->next = temp->next;

            free (temp);

            return data;
        }
    }

    unreachable ();
    return NULL;
}

void *get_linkedlist (linkedlist_t l, const size_t i) {
    if (i >= length_linkedlist (l))
        return NULL;

    for (size_t j = 0; l; l = l->next)
        if (j++ == i)
            return l->data;

    unreachable ();
    return NULL;
}

void *set_linkedlist (linkedlist_t l, const size_t i, void *const data) {
    if (i >= length_linkedlist (l))
        return NULL;

    for (size_t j = 0; l; l = l->next)
        if (j++ == i)
            return l->data = data;

    unreachable ();
    return NULL;
}

void *toarray_linkedlist (linkedlist_t l, size_t *const restrict len) {
    if (!l)
        return len ? (void *) (*len = 0) : NULL;

    void **arr;
    if (!(arr = malloc (sizeof (void *) * ({
                            if (len)
                                *len = l->len;

                            l->len;
                        }))))
        return len ? (void *) (*len = 0) : NULL;

    for (size_t i = 0; l; l = l->next)
        *(arr + i++) = l->data;

    return arr;
}