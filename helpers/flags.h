//
// Created by Manuel Blanco Valentin on 1/11/21.
//

#ifndef DEFINE_ENUM_FLAG_OPERATORS
#define DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) \
extern "C++" { \
inline ENUMTYPE operator | (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((int)a) | ((int)b)); } \
inline ENUMTYPE &operator |= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((int &)a) |= ((int)b)); } \
inline ENUMTYPE operator & (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((int)a) & ((int)b)); } \
inline ENUMTYPE &operator &= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((int &)a) &= ((int)b)); } \
inline ENUMTYPE operator ~ (ENUMTYPE a) { return ENUMTYPE(~((int)a)); } \
inline ENUMTYPE operator ^ (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((int)a) ^ ((int)b)); } \
inline ENUMTYPE &operator ^= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((int &)a) ^= ((int)b)); } \
}
#else
#define DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) // NOP, C allows these operators.
#endif

#ifndef HLSPY_FLAGS_H
#define HLSPY_FLAGS_H

// Definition of verilog Parser flags
// Powers of two so we ensure that each variable has one single bit
// and so we can compare them and operate with them individually
// (such as xor, and, or, etc).
typedef enum _FLAGS__ {
    // Decimal            // Binary
    NONE          = 0,    // 000000
    AUTO_INCLUDE  = 1,    // 000001
    RELOAD = 2,           // 000010
    MONITOR = 4,          // 000100
    SEE = 8
} FLAGS;

DEFINE_ENUM_FLAG_OPERATORS(FLAGS);


#endif //HLSPY_FLAGS_H
