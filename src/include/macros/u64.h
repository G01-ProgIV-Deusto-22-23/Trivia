#ifndef TRIVIA_MACRO_U64_H
#define TRIVIA_MACRO_U64_H

#define u64decomp(x)  (uint32_t) ((uint64_t) (x) >> 32), (uint32_t) (x)
#define u64comp(w, h) (((uint64_t) (w) << 32) | (uint32_t) (h))

#endif