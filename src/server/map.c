#define U32_HASH_CONST   UINT32_C (0x45d9f3b)
#define U32_UNHASH_CONST UINT32_C (0x119de1f3)

/* typedef struct {
} *hashmap_t; */

uint32_t u32_hash (uint32_t x) {
    x = ((x >> 16) ^ x) * U32_HASH_CONST;
    x = ((x >> 16) ^ x) * U32_HASH_CONST;
    return (x >> 16) ^ x;
}

uint32_t u32_unhash (uint32_t x) {
    x = ((x >> 16) ^ x) * U32_UNHASH_CONST;
    x = ((x >> 16) ^ x) * U32_UNHASH_CONST;
    return ((x >> 16) ^ x);
}

uint64_t u64_hash (uint64_t x) {
    x = (x ^ (x >> 30)) * UINT64_C (0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)) * UINT64_C (0x94d049bb133111eb);
    return x ^ (x >> 31);
}

uint64_t u64_unhash (uint64_t x) {
    x = (x ^ (x >> 31) ^ (x >> 62)) * UINT64_C (0x319642b2d24d8ec3);
    x = (x ^ (x >> 27) ^ (x >> 54)) * UINT64_C (0x96de1b173f119089);
    return x ^ (x >> 30) ^ (x >> 60);
}