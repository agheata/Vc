/*
    Copyright (C) 2009 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) version 3.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef SSE_VECTOR_H
#define SSE_VECTOR_H

#include "intrinsics.h"

#ifndef _M128
# define _M128 __m128
#endif

#ifndef _M128I
# define _M128I __m128i
#endif

#ifndef _M128D
# define _M128D __m128d
#endif

#ifndef ALIGN
# ifdef __GNUC__
#  define ALIGN(n) __attribute__((aligned(n)))
# else
#  define ALIGN(n) __declspec(align(n))
# endif
#endif

namespace SSE
{
    enum { VectorAlignment = 16 };
    template<typename T> class Vector;
    class Mask;

    ALIGN(16) static const int _OneMaskData[4]  = { 0x00000001, 0x00000001, 0x00000001, 0x00000001 };
    ALIGN(16) static const int _FullMaskData[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
#define _FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF *reinterpret_cast<const _M128I *const>(_FullMaskData)

#define PARENT_DATA (static_cast<Parent *>(this)->data)
#define PARENT_DATA_CONST (static_cast<const Parent *>(this)->data)
        template<typename T, typename Parent>
        struct VectorBase
        {
            typedef _M128 IntrinType;
            operator _M128() { return PARENT_DATA; }
            operator const _M128() const { return PARENT_DATA_CONST; }
        };
        template<typename Parent>
        struct VectorBase<float, Parent>
        {
            typedef _M128 IntrinType;
            operator _M128() { return PARENT_DATA; }
            operator const _M128() const { return PARENT_DATA_CONST; }
        };
        template<typename Parent>
        struct VectorBase<double, Parent>
        {
            typedef _M128D IntrinType;
            operator _M128D() { return PARENT_DATA; }
            operator const _M128D() const { return PARENT_DATA_CONST; }
        };
#define OP_DECL(symbol, fun) \
            inline Vector<T> &operator symbol##=(const Vector<T> &x); \
            inline Vector<T> &operator symbol##=(const T &x); \
            inline Vector<T> operator symbol(const Vector<T> &x) const; \
            inline Vector<T> operator symbol(const T &x) const;
        template<typename Parent>
        struct VectorBase<int, Parent>
        {
            typedef _M128I IntrinType;
            operator _M128I() { return PARENT_DATA; }
            operator const _M128I() const { return PARENT_DATA_CONST; }
#define T int
            OP_DECL(|, or_)
            OP_DECL(&, and_)
            OP_DECL(^, xor_)
#undef T
            ALIGN(16) static const int IndexesFromZero[4];
        };
        template<typename Parent>
        struct VectorBase<unsigned int, Parent>
        {
            typedef _M128I IntrinType;
            operator _M128I() { return PARENT_DATA; }
            operator const _M128I() const { return PARENT_DATA_CONST; }
#define T unsigned int
            OP_DECL(|, or_)
            OP_DECL(&, and_)
            OP_DECL(^, xor_)
#undef T
            ALIGN(16) static const unsigned int IndexesFromZero[4];
        };
        template<typename Parent>
        struct VectorBase<short, Parent>
        {
            typedef _M128I IntrinType;
            operator _M128I() { return PARENT_DATA; }
            operator const _M128I() const { return PARENT_DATA_CONST; }
#define T short
            OP_DECL(|, or_)
            OP_DECL(&, and_)
            OP_DECL(^, xor_)
#undef T
            ALIGN(16) static const short IndexesFromZero[8];
        };
        template<typename Parent>
        struct VectorBase<unsigned short, Parent>
        {
            typedef _M128I IntrinType;
            operator _M128I() { return PARENT_DATA; }
            operator const _M128I() const { return PARENT_DATA_CONST; }
#define T unsigned short
            OP_DECL(|, or_)
            OP_DECL(&, and_)
            OP_DECL(^, xor_)
#undef T
            ALIGN(16) static const unsigned short IndexesFromZero[8];
        };
#undef OP_DECL
#undef PARENT_DATA
#undef PARENT_DATA_CONST

        template<typename From, typename To> struct StaticCastHelper {};
        template<> struct StaticCastHelper<float       , int         > { static _M128I cast(const _M128  &v) { return _mm_cvtps_epi32(v); } };
        template<> struct StaticCastHelper<double      , int         > { static _M128I cast(const _M128D &v) { return _mm_cvtpd_epi32(v); } };
        template<> struct StaticCastHelper<int         , int         > { static _M128I cast(const _M128I &v) { return v; } };
        template<> struct StaticCastHelper<unsigned int, int         > { static _M128I cast(const _M128I &v) { return v; } };
        template<> struct StaticCastHelper<float       , unsigned int> { static _M128I cast(const _M128  &v) { return _mm_cvtps_epi32(v); } };
        template<> struct StaticCastHelper<double      , unsigned int> { static _M128I cast(const _M128D &v) { return _mm_cvtpd_epi32(v); } };
        template<> struct StaticCastHelper<int         , unsigned int> { static _M128I cast(const _M128I &v) { return v; } };
        template<> struct StaticCastHelper<unsigned int, unsigned int> { static _M128I cast(const _M128I &v) { return v; } };
        template<> struct StaticCastHelper<float       , float       > { static _M128  cast(const _M128  &v) { return v; } };
        template<> struct StaticCastHelper<double      , float       > { static _M128  cast(const _M128D &v) { return _mm_cvtpd_ps(v); } };
        template<> struct StaticCastHelper<int         , float       > { static _M128  cast(const _M128I &v) { return _mm_cvtepi32_ps(v); } };
        template<> struct StaticCastHelper<unsigned int, float       > { static _M128  cast(const _M128I &v) { return _mm_cvtepi32_ps(v); } };
        template<> struct StaticCastHelper<float       , double      > { static _M128D cast(const _M128  &v) { return _mm_cvtps_pd(v); } };
        template<> struct StaticCastHelper<double      , double      > { static _M128D cast(const _M128D &v) { return v; } };
        template<> struct StaticCastHelper<int         , double      > { static _M128D cast(const _M128I &v) { return _mm_cvtepi32_pd(v); } };
        template<> struct StaticCastHelper<unsigned int, double      > { static _M128D cast(const _M128I &v) { return _mm_cvtepi32_pd(v); } };

        template<> struct StaticCastHelper<unsigned short, short         > { static _M128I cast(const _M128I &v) { return v; } };
        template<> struct StaticCastHelper<unsigned short, unsigned short> { static _M128I cast(const _M128I &v) { return v; } };
        template<> struct StaticCastHelper<short         , unsigned short> { static _M128I cast(const _M128I &v) { return v; } };
        template<> struct StaticCastHelper<short         , short         > { static _M128I cast(const _M128I &v) { return v; } };

        template<typename From, typename To> struct ReinterpretCastHelper {};
        template<> struct ReinterpretCastHelper<float       , int         > { static _M128I cast(const _M128  &v) { return _mm_castps_si128(v); } };
        template<> struct ReinterpretCastHelper<double      , int         > { static _M128I cast(const _M128D &v) { return _mm_castpd_si128(v); } };
        template<> struct ReinterpretCastHelper<int         , int         > { static _M128I cast(const _M128I &v) { return v; } };
        template<> struct ReinterpretCastHelper<unsigned int, int         > { static _M128I cast(const _M128I &v) { return v; } };
        template<> struct ReinterpretCastHelper<float       , unsigned int> { static _M128I cast(const _M128  &v) { return _mm_castps_si128(v); } };
        template<> struct ReinterpretCastHelper<double      , unsigned int> { static _M128I cast(const _M128D &v) { return _mm_castpd_si128(v); } };
        template<> struct ReinterpretCastHelper<int         , unsigned int> { static _M128I cast(const _M128I &v) { return v; } };
        template<> struct ReinterpretCastHelper<unsigned int, unsigned int> { static _M128I cast(const _M128I &v) { return v; } };
        template<> struct ReinterpretCastHelper<float       , float       > { static _M128  cast(const _M128  &v) { return v; } };
        template<> struct ReinterpretCastHelper<double      , float       > { static _M128  cast(const _M128D &v) { return _mm_castpd_ps(v); } };
        template<> struct ReinterpretCastHelper<int         , float       > { static _M128  cast(const _M128I &v) { return _mm_castsi128_ps(v); } };
        template<> struct ReinterpretCastHelper<unsigned int, float       > { static _M128  cast(const _M128I &v) { return _mm_castsi128_ps(v); } };
        template<> struct ReinterpretCastHelper<float       , double      > { static _M128D cast(const _M128  &v) { return _mm_castps_pd(v); } };
        template<> struct ReinterpretCastHelper<double      , double      > { static _M128D cast(const _M128D &v) { return v; } };
        template<> struct ReinterpretCastHelper<int         , double      > { static _M128D cast(const _M128I &v) { return _mm_castsi128_pd(v); } };
        template<> struct ReinterpretCastHelper<unsigned int, double      > { static _M128D cast(const _M128I &v) { return _mm_castsi128_pd(v); } };

        template<> struct ReinterpretCastHelper<unsigned short, short         > { static _M128I cast(const _M128I &v) { return v; } };
        template<> struct ReinterpretCastHelper<unsigned short, unsigned short> { static _M128I cast(const _M128I &v) { return v; } };
        template<> struct ReinterpretCastHelper<short         , unsigned short> { static _M128I cast(const _M128I &v) { return v; } };
        template<> struct ReinterpretCastHelper<short         , short         > { static _M128I cast(const _M128I &v) { return v; } };

        template<typename T> struct VectorHelper {};

#define CAT_HELPER2(a, b) a##b
#define CAT_HELPER(a, b) CAT_HELPER2(a, b)
#define OP1(op) \
        static inline TYPE op(const TYPE &a) { return CAT_HELPER(_mm_##op##_, SUFFIX)(a); }
#define OP(op) \
        static inline TYPE op(const TYPE &a, const TYPE &b) { return CAT_HELPER(_mm_##op##_ , SUFFIX)(a, b); }
#define OP_(op) \
        static inline TYPE op(const TYPE &a, const TYPE &b) { return CAT_HELPER(_mm_##op    , SUFFIX)(a, b); }
#define OPx(op, op2) \
        static inline TYPE op(const TYPE &a, const TYPE &b) { return CAT_HELPER(_mm_##op2##_, SUFFIX)(a, b); }
#define OPcmp(op) \
        static inline TYPE cmp##op(const TYPE &a, const TYPE &b) { return CAT_HELPER(_mm_cmp##op##_, SUFFIX)(a, b); }
#define OP_CAST_(op) \
        static inline TYPE op(const TYPE &a, const TYPE &b) { return CAT_HELPER(_mm_castps_, SUFFIX)( \
            _mm_##op##ps(CAT_HELPER(CAT_HELPER(_mm_cast, SUFFIX), _ps)(a), \
              CAT_HELPER(CAT_HELPER(_mm_cast, SUFFIX), _ps)(b))); \
        }
#define MINMAX \
        static inline TYPE min(TYPE &a, TYPE &b) { return CAT_HELPER(_mm_min_, SUFFIX)(a, b); } \
        static inline TYPE max(TYPE &a, TYPE &b) { return CAT_HELPER(_mm_max_, SUFFIX)(a, b); }
#define LOAD(T) \
        static inline TYPE load1(const T *x) { return CAT_HELPER(_mm_load1_, SUFFIX)(x); } \
        static inline TYPE load (const T *x) { return CAT_HELPER(_mm_load_ , SUFFIX)(x); }
#define LOAD_CAST(T) \
        static inline TYPE load1(const T *x) { return CAT_HELPER(_mm_castps_, SUFFIX)(_mm_load1_ps(reinterpret_cast<const float *>(x))); } \
        static inline TYPE load (const T *x) { return CAT_HELPER(_mm_castps_, SUFFIX)(_mm_load_ps (reinterpret_cast<const float *>(x))); }
#define STORE(T) \
            static inline void store1(T *mem, TYPE x) { return CAT_HELPER(_mm_store1_, SUFFIX)(mem, x); } \
            static inline void store (T *mem, TYPE x) { return CAT_HELPER(_mm_store_ , SUFFIX)(mem, x); } \
            static inline void storeStreaming(T *mem, TYPE x) { return CAT_HELPER(_mm_stream_ , SUFFIX)(mem, x); }
#define STORE_CAST(T) \
            static inline void store1(T *mem, TYPE x) { return _mm_store1_ps(reinterpret_cast<float *>(mem), CAT_HELPER(CAT_HELPER(_mm_cast, SUFFIX), _ps)(x)); } \
            static inline void store (T *mem, TYPE x) { return _mm_store_ps (reinterpret_cast<float *>(mem), CAT_HELPER(CAT_HELPER(_mm_cast, SUFFIX), _ps)(x)); } \
            static inline void storeStreaming(T *mem, TYPE x) { return _mm_stream_ps(reinterpret_cast<float *>(mem), CAT_HELPER(CAT_HELPER(_mm_cast, SUFFIX), _ps)(x)); }
#define GATHER_SCATTER(T) \
            static inline void gather(TYPE &v, const _M128I &indexes, const T *baseAddr) { \
                const unsigned int *const i = reinterpret_cast<const unsigned int *>(&indexes); \
                v = CAT_HELPER(_mm_setr_, SUFFIX)(baseAddr[i[0]], baseAddr[i[1]], baseAddr[i[2]], baseAddr[i[3]]); \
            } \
            template<typename S> \
            static inline void gather(TYPE &v, const _M128I &indexes, const S *baseAddr, const T S::* member1) { \
                const unsigned int *const i = reinterpret_cast<const unsigned int *>(&indexes); \
                v = CAT_HELPER(_mm_setr_, SUFFIX)(baseAddr[i[0]].*(member1), baseAddr[i[1]].*(member1), baseAddr[i[2]].*(member1), baseAddr[i[3]].*(member1)); \
            } \
            template<typename S1, typename S2> \
            static inline void gather(TYPE &v, const _M128I &indexes, const S1 *baseAddr, const S2 S1::* member1, const T S2::* member2) { \
                const unsigned int *const i = reinterpret_cast<const unsigned int *>(&indexes); \
                v = CAT_HELPER(_mm_setr_, SUFFIX)(baseAddr[i[0]].*(member1).*(member2), baseAddr[i[1]].*(member1).*(member2), baseAddr[i[2]].*(member1).*(member2), baseAddr[i[3]].*(member1).*(member2)); \
            } \
            static inline void scatter(const TYPE &v, const _M128I &indexes, T *baseAddr) { \
                const unsigned int *const i = reinterpret_cast<const unsigned int *>(&indexes); \
                const T *const vv = reinterpret_cast<const T *>(&v); \
                baseAddr[i[0]] = vv[0]; \
                baseAddr[i[1]] = vv[1]; \
                baseAddr[i[2]] = vv[2]; \
                baseAddr[i[3]] = vv[3]; \
            } \
            template<typename S> \
            static inline void scatter(const TYPE &v, const _M128I &indexes, S *baseAddr, T S::* member1) { \
                const unsigned int *const i = reinterpret_cast<const unsigned int *>(&indexes); \
                const T *const vv = reinterpret_cast<const T *>(&v); \
                baseAddr[i[0]].*(member1) = vv[0]; \
                baseAddr[i[1]].*(member1) = vv[1]; \
                baseAddr[i[2]].*(member1) = vv[2]; \
                baseAddr[i[3]].*(member1) = vv[3]; \
            } \
            template<typename S1, typename S2> \
            static inline void scatter(const TYPE &v, const _M128I &indexes, S1 *baseAddr, S2 S1::* member1, T S2::* member2) { \
                const unsigned int *const i = reinterpret_cast<const unsigned int *>(&indexes); \
                const T *const vv = reinterpret_cast<const T *>(&v); \
                baseAddr[i[0]].*(member1).*(member2) = vv[0]; \
                baseAddr[i[1]].*(member1).*(member2) = vv[1]; \
                baseAddr[i[2]].*(member1).*(member2) = vv[2]; \
                baseAddr[i[3]].*(member1).*(member2) = vv[3]; \
            }
#define GATHER_SCATTER_16(T) \
            static inline void gather(TYPE &v, const _M128I &indexes, const T *baseAddr) { \
                const unsigned short *const i = reinterpret_cast<const unsigned short *>(&indexes); \
                v = CAT_HELPER(_mm_setr_, SUFFIX)(baseAddr[i[0]], baseAddr[i[1]], baseAddr[i[2]], baseAddr[i[3]], baseAddr[i[4]], baseAddr[i[5]], baseAddr[i[6]], baseAddr[i[7]]); \
            } \
            template<typename S> \
            static inline void gather(TYPE &v, const _M128I &indexes, const S *baseAddr, const T S::* member1) { \
                const unsigned short *const i = reinterpret_cast<const unsigned short *>(&indexes); \
                v = CAT_HELPER(_mm_setr_, SUFFIX)(baseAddr[i[0]].*(member1), baseAddr[i[1]].*(member1), \
                        baseAddr[i[2]].*(member1), baseAddr[i[3]].*(member1), baseAddr[i[4]].*(member1), \
                        baseAddr[i[5]].*(member1), baseAddr[i[6]].*(member1), baseAddr[i[7]].*(member1)); \
            } \
            template<typename S1, typename S2> \
            static inline void gather(TYPE &v, const _M128I &indexes, const S1 *baseAddr, const S2 S1::* member1, const T S2::* member2) { \
                const unsigned short *const i = reinterpret_cast<const unsigned short *>(&indexes); \
                v = CAT_HELPER(_mm_setr_, SUFFIX)(baseAddr[i[0]].*(member1).*(member2), \
                        baseAddr[i[1]].*(member1).*(member2), baseAddr[i[2]].*(member1).*(member2), \
                        baseAddr[i[3]].*(member1).*(member2), baseAddr[i[4]].*(member1).*(member2), \
                        baseAddr[i[5]].*(member1).*(member2), baseAddr[i[6]].*(member1).*(member2), \
                        baseAddr[i[7]].*(member1).*(member2)); \
            } \
            static inline void scatter(const TYPE &v, const _M128I &indexes, T *baseAddr) { \
                const unsigned short *const i = reinterpret_cast<const unsigned short *>(&indexes); \
                const T *const vv = reinterpret_cast<const T *>(&v); \
                baseAddr[i[0]] = vv[0]; \
                baseAddr[i[1]] = vv[1]; \
                baseAddr[i[2]] = vv[2]; \
                baseAddr[i[3]] = vv[3]; \
                baseAddr[i[4]] = vv[4]; \
                baseAddr[i[5]] = vv[5]; \
                baseAddr[i[6]] = vv[6]; \
                baseAddr[i[7]] = vv[7]; \
            } \
            template<typename S> \
            static inline void scatter(const TYPE &v, const _M128I &indexes, S *baseAddr, T S::* member1) { \
                const unsigned short *const i = reinterpret_cast<const unsigned short *>(&indexes); \
                const T *const vv = reinterpret_cast<const T *>(&v); \
                baseAddr[i[0]].*(member1) = vv[0]; \
                baseAddr[i[1]].*(member1) = vv[1]; \
                baseAddr[i[2]].*(member1) = vv[2]; \
                baseAddr[i[3]].*(member1) = vv[3]; \
                baseAddr[i[4]].*(member1) = vv[4]; \
                baseAddr[i[5]].*(member1) = vv[5]; \
                baseAddr[i[6]].*(member1) = vv[6]; \
                baseAddr[i[7]].*(member1) = vv[7]; \
            } \
            template<typename S1, typename S2> \
            static inline void scatter(const TYPE &v, const _M128I &indexes, S1 *baseAddr, S2 S1::* member1, T S2::* member2) { \
                const unsigned short *const i = reinterpret_cast<const unsigned short *>(&indexes); \
                const T *const vv = reinterpret_cast<const T *>(&v); \
                baseAddr[i[0]].*(member1).*(member2) = vv[0]; \
                baseAddr[i[1]].*(member1).*(member2) = vv[1]; \
                baseAddr[i[2]].*(member1).*(member2) = vv[2]; \
                baseAddr[i[3]].*(member1).*(member2) = vv[3]; \
                baseAddr[i[4]].*(member1).*(member2) = vv[4]; \
                baseAddr[i[5]].*(member1).*(member2) = vv[5]; \
                baseAddr[i[6]].*(member1).*(member2) = vv[6]; \
                baseAddr[i[7]].*(member1).*(member2) = vv[7]; \
            }

        template<> struct VectorHelper<double> {
#define TYPE _M128D
#define SUFFIX pd
            LOAD(double)
            STORE(double)
            static inline void gather(TYPE &v, const _M128I &indexes, const double *baseAddr) {
                const unsigned int *const i = reinterpret_cast<const unsigned int *>(&indexes);
                v = _mm_setr_pd(baseAddr[i[0]], baseAddr[i[1]]);
            }
            template<typename S>
            static inline void gather(TYPE &v, const _M128I &indexes, const S *baseAddr, const double S::* member1) {
                const unsigned int *const i = reinterpret_cast<const unsigned int *>(&indexes);
                v = CAT_HELPER(_mm_setr_, SUFFIX)(baseAddr[i[0]].*(member1), baseAddr[i[1]].*(member1));
            }
            template<typename S1, typename S2>
            static inline void gather(TYPE &v, const _M128I &indexes, const S1 *baseAddr, const S2 S1::* member1, const double S2::* member2) {
                const unsigned int *const i = reinterpret_cast<const unsigned int *>(&indexes);
                v = CAT_HELPER(_mm_setr_, SUFFIX)(baseAddr[i[0]].*(member1).*(member2), baseAddr[i[1]].*(member1).*(member2));
            }
            static inline void scatter(const TYPE &v, const _M128I &indexes, double *baseAddr) {
                const unsigned int *const i = reinterpret_cast<const unsigned int *>(&indexes);
                _mm_storel_pd(&baseAddr[i[0]], v);
                _mm_storeh_pd(&baseAddr[i[1]], v);
            }
            template<typename S>
            static inline void scatter(const TYPE &v, const _M128I &indexes, S *baseAddr, double S::* member1) {
                const unsigned int *const i = reinterpret_cast<const unsigned int *>(&indexes);
                _mm_storel_pd(&(baseAddr[i[0]].*(member1)), v);
                _mm_storeh_pd(&(baseAddr[i[1]].*(member1)), v);
            }
            template<typename S1, typename S2>
            static inline void scatter(const TYPE &v, const _M128I &indexes, S1 *baseAddr, S2 S1::* member1, double S2::* member2) {
                const unsigned int *const i = reinterpret_cast<const unsigned int *>(&indexes);
                _mm_storel_pd(&(baseAddr[i[0]].*(member1).*(member2)), v);
                _mm_storeh_pd(&(baseAddr[i[1]].*(member1).*(member2)), v);
            }


            static inline TYPE set(const double a) { return CAT_HELPER(_mm_set1_, SUFFIX)(a); }
            static inline TYPE set(const double a, const double b) { return CAT_HELPER(_mm_set_, SUFFIX)(a, b); }
            static inline void setZero(TYPE &v) { v = CAT_HELPER(_mm_setzero_, SUFFIX)(); }

            static inline void multiplyAndAdd(TYPE &v1, TYPE v2, TYPE v3) { v1 = add(mul(v1, v2), v3); }

            OP(add) OP(sub) OP(mul) OP(div)
            OPcmp(eq) OPcmp(neq)
            OPcmp(lt) OPcmp(nlt)
            OPcmp(le) OPcmp(nle)

            OP1(sqrt)
            static inline TYPE abs(const TYPE a) {
              static const TYPE mask = { 0x7fffffffffffffff, 0x7fffffffffffffff };
              return CAT_HELPER(_mm_and_, SUFFIX)(a, mask);
            }

            MINMAX
#undef TYPE
#undef SUFFIX
        };

        template<> struct VectorHelper<float> {
#define TYPE _M128
#define SUFFIX ps
            LOAD(float)
            STORE(float)
            GATHER_SCATTER(float)

            static inline TYPE set(const float a) { return CAT_HELPER(_mm_set1_, SUFFIX)(a); }
            static inline TYPE set(const float a, const float b, const float c, const float d) { return CAT_HELPER(_mm_set_, SUFFIX)(a, b, c, d); }
            static inline void setZero(TYPE &v) { v = CAT_HELPER(_mm_setzero_, SUFFIX)(); }

            static void pack(TYPE &v1, _M128I &_m1, TYPE &v2, _M128I &_m2) {
                // there are 256 different m1.m2 combinations
                TYPE &m1 = reinterpret_cast<TYPE &>(_m1);
                TYPE &m2 = reinterpret_cast<TYPE &>(_m2);
                const int m1Mask = _mm_movemask_ps(m1);
                switch (m1Mask) {
                case 15: // v1 is full, nothing to do
                    return true;
                // 240 left
                case 0:  // v1 is empty, take v2
                    m1 = m2;
                    v1 = v2;
                    setZero(m2);
                    return _mm_movemask_ps(m1) == 15;
                // 224 left
                default:
                    {
                        TYPE tmp;
                        const int m2Mask = _mm_movemask_ps(m2);
                        switch (m2Mask) {
                        case 15: // v2 is full, just swap
                            tmp = v1;
                            v1 = v2;
                            v2 = tmp;
                            tmp = m1;
                            m1 = m2;
                            m2 = tmp;
                            return true;
                // 210 left
                        case 0: // v2 is empty, nothing to be gained from packing
                            return false;
                // 196 left
                        case 0: // v2 is empty, nothing to be gained from packing
                        }
                        // m1 and m2 are neither full nor empty
                        tmp = _mm_or_ps(m1, m2);
                        const int m3Mask = _mm_movemask_ps(tmp);
                        // m3Mask tells use where both vectors have no entries
                        const int m4Mask = _mm_movemask_ps(_mm_and_ps(m1, m2));
                        // m3Mask tells use where both vectors have entries
                        if (m4Mask == 0 || m3Mask == 15) {
                            // m4Mask == 0: No overlap, simply move all we can from v2 into v1.
                            //              Empties v2.
                            // m3Mask == 15: Simply merge the parts from v2 into v1 where v1 is
                            //               empty.
                            const TYPE m2Move = _mm_andnot_ps(m1, m2); // the part to be moved into v1
                            v1 = _mm_add_ps(
                                    _mm_and_ps(v1, m1),
                                    _mm_and_ps(v2, m2Move)
                                    );
                            m1 = tmp;
                            m2 = _mm_andnot_ps(m2Move, m2);
                            return m3Mask == 15;
                // 
                        }
                        if ((m4Mask & 3) == 3) {
                            // the high values are available
                            tmp = _mm_unpackhi_ps(v1, v2);
                            v2  = _mm_unpacklo_ps(v1, v2);
                            v1  = tmp;
                            tmp = _mm_unpackhi_ps(m1, m2);
                            m2  = _mm_unpacklo_ps(m1, m2);
                            m1  = tmp;
                            return true;
                        }
                        if ((m4Mask & 12) == 12) {
                            // the low values are available
                            tmp = _mm_unpacklo_ps(v1, v2);
                            v2  = _mm_unpackhi_ps(v1, v2);
                            v1  = tmp;
                            tmp = _mm_unpacklo_ps(m1, m2);
                            m2  = _mm_unpackhi_ps(m1, m2);
                            m1  = tmp;
                            return true;
                        }
                        if ((m4Mask & 5) == 5) {
                            tmp = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(0, 2, 0, 2));
                            v2  = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(1, 3, 1, 3));
                            v1  = tmp;
                            tmp = _mm_shuffle_ps(m1, m2, _MM_SHUFFLE(0, 2, 0, 2));
                            m2  = _mm_shuffle_ps(m1, m2, _MM_SHUFFLE(1, 3, 1, 3));
                            m1  = tmp;
                            return true;
                        }
                        if ((m4Mask & 6) == 6) {
                            tmp = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(1, 2, 1, 2));
                            v2  = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(0, 3, 0, 3));
                            v1  = tmp;
                            tmp = _mm_shuffle_ps(m1, m2, _MM_SHUFFLE(1, 2, 1, 2));
                            m2  = _mm_shuffle_ps(m1, m2, _MM_SHUFFLE(0, 3, 0, 3));
                            m1  = tmp;
                            return true;
                        }
                        if ((m4Mask & 9) == 9) {
                            tmp = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(0, 3, 0, 3));
                            v2  = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(1, 2, 1, 2));
                            v1  = tmp;
                            tmp = _mm_shuffle_ps(m1, m2, _MM_SHUFFLE(0, 3, 0, 3));
                            m2  = _mm_shuffle_ps(m1, m2, _MM_SHUFFLE(1, 2, 1, 2));
                            m1  = tmp;
                            return true;
                        }
                        if ((m4Mask & 10) == 10) {
                            tmp = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(1, 3, 1, 3));
                            v2  = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(0, 2, 0, 2));
                            v1  = tmp;
                            tmp = _mm_shuffle_ps(m1, m2, _MM_SHUFFLE(1, 3, 1, 3));
                            m2  = _mm_shuffle_ps(m1, m2, _MM_SHUFFLE(0, 2, 0, 2));
                            m1  = tmp;
                            return true;
                        }
                        float *const vv1 = reinterpret_cast<float *>(&v1);
                        float *const vv2 = reinterpret_cast<float *>(&v2);
                        unsigned int *const mm1 = reinterpret_cast<unsigned int *>(&_m1);
                        unsigned int *const mm2 = reinterpret_cast<unsigned int *>(&_m2);
                        int j = 0;
                        for (int i = 0; i < 4; ++i) {
                            if (!(m1Mask & (1 << i))) { // v1 entry not set, take the first from v2
                                while (j < 4 && !(m2Mask & (1 << j))) {
                                    ++j;
                                }
                                if (j < 4) {
                                    vv1[i] = vv2[j];
                                    mm1[i] = 0xffffffff;
                                    mm2[j] = 0;
                                    ++j;
                                }
                            }
                        }
                        return _mm_movemask_ps(m1) == 15;
//X                         // m4Mask has exactly one bit set
//X                         switch (m4Mask) {
//X                         case 1:
//X                             // x___    xx__    xx__    xx__    xxx_    x_x_    x_x_    x_xx    x__x  + mirrored horizontally
//X                             // x___    x___    x_x_    x__x    x___    x___    x__x    x___    x___
//X                             break;
//X                         case 2:
//X                             break;
//X                         case 4:
//X                             break;
//X                         case 8:
//X                             break;
//X                         }
                    }
                }
            }

            static inline void multiplyAndAdd(TYPE &v1, TYPE v2, TYPE v3) { v1 = add(mul(v1, v2), v3); }

            OP(add) OP(sub) OP(mul) OP(div)
            OPcmp(eq) OPcmp(neq)
            OPcmp(lt) OPcmp(nlt)
            OPcmp(le) OPcmp(nle)

            OP1(sqrt)
            static inline TYPE abs(const TYPE a) {
              static const TYPE mask = { 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff };
              return CAT_HELPER(_mm_and_, SUFFIX)(a, mask);
            }

            MINMAX
#undef TYPE
#undef SUFFIX
        };

        template<> struct VectorHelper<int> {
#define TYPE _M128I
#define SUFFIX si128
            LOAD_CAST(int)
            STORE_CAST(int)

            OP_(or_) OP_(and_) OP_(xor_)
            static inline void setZero(TYPE &v) { v = CAT_HELPER(_mm_setzero_, SUFFIX)(); }
#undef SUFFIX
#define SUFFIX epi32
            GATHER_SCATTER(int)

            static inline TYPE set(const int a) { return CAT_HELPER(_mm_set1_, SUFFIX)(a); }
            static inline TYPE set(const int a, const int b, const int c, const int d) { return CAT_HELPER(_mm_set_, SUFFIX)(a, b, c, d); }

            static inline void multiplyAndAdd(TYPE &v1, TYPE v2, TYPE v3) { v1 = add(mul(v1, v2), v3); }

#ifdef __SSSE3__
            OP1(abs)
#endif

#ifdef __SSE4_1__
            static inline TYPE mul(const TYPE a, const TYPE b) { return _mm_mullo_epi32(a, b); }
            MINMAX
#else
            static inline TYPE mul(const TYPE a, const TYPE b) {
                const int *const _a = reinterpret_cast<const int *>(&a);
                const int *const _b = reinterpret_cast<const int *>(&b);
                union {
                    int i[4];
                    TYPE v;
                } x = { {
                    _a[0] * _b[0],
                    _a[1] * _b[1],
                    _a[2] * _b[2],
                    _a[3] * _b[3]
                } };
                return x.v;
//X                 TYPE hi = _mm_mulhi_epi16(a, b);
//X                 hi = _mm_slli_epi32(hi, 16);
//X                 TYPE lo = _mm_mullo_epi16(a, b);
//X                 return or_(hi, lo);
            }
#endif

            static inline TYPE div(const TYPE a, const TYPE b) {
                const int *const _a = reinterpret_cast<const int *>(&a);
                const int *const _b = reinterpret_cast<const int *>(&b);
                union {
                    int i[4];
                    TYPE v;
                } x = { {
                    _a[0] / _b[0],
                    _a[1] / _b[1],
                    _a[2] / _b[2],
                    _a[3] / _b[3]
                } };
                return x.v;
            }

            OP(add) OP(sub)
            OPcmp(eq)
            OPcmp(lt)
            OPcmp(gt)
            static inline TYPE cmpneq(const TYPE &a, const TYPE &b) { _M128I x = cmpeq(a, b); return _mm_xor_si128(x, _FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF); }
            static inline TYPE cmpnlt(const TYPE &a, const TYPE &b) { _M128I x = cmplt(a, b); return _mm_xor_si128(x, _FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF); }
            static inline TYPE cmple (const TYPE &a, const TYPE &b) { _M128I x = cmpgt(a, b); return _mm_xor_si128(x, _FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF); }
            static inline TYPE cmpnle(const TYPE &a, const TYPE &b) { return cmpgt(a, b); }
#undef TYPE
#undef SUFFIX
        };

        template<> struct VectorHelper<unsigned int> {
#define TYPE _M128I
#define SUFFIX si128
            LOAD_CAST(unsigned int)
            STORE_CAST(unsigned int)
            OP_CAST_(or_) OP_CAST_(and_) OP_CAST_(xor_)
            static inline void setZero(TYPE &v) { v = CAT_HELPER(_mm_setzero_, SUFFIX)(); }

#undef SUFFIX
#define SUFFIX epu32

#ifdef __SSE4_1__
            MINMAX
#endif

            static inline TYPE mul(const TYPE a, const TYPE b) {
                TYPE hi = _mm_mulhi_epu16(a, b);
                hi = _mm_slli_epi32(hi, 16);
                TYPE lo = _mm_mullo_epi16(a, b);
                return or_(hi, lo);
            }
            static inline TYPE div(const TYPE a, const TYPE b) {
                const unsigned int *const _a = reinterpret_cast<const unsigned int *>(&a);
                const unsigned int *const _b = reinterpret_cast<const unsigned int *>(&b);
                union {
                    unsigned int i[4];
                    TYPE v;
                } x = { {
                    _a[0] / _b[0],
                    _a[1] / _b[1],
                    _a[2] / _b[2],
                    _a[3] / _b[3]
                } };
                return x.v;
            }

#undef SUFFIX
#define SUFFIX epi32
            GATHER_SCATTER(unsigned int)
            static inline TYPE set(const unsigned int a) { return CAT_HELPER(_mm_set1_, SUFFIX)(a); }
            static inline TYPE set(const unsigned int a, const unsigned int b, const unsigned int c, const unsigned int d) { return CAT_HELPER(_mm_set_, SUFFIX)(a, b, c, d); }

            OP(add) OP(sub)
            OPcmp(eq)
            OPcmp(lt)
            OPcmp(gt)
            static inline TYPE cmpneq(const TYPE &a, const TYPE &b) { return _mm_xor_si128(cmpeq(a, b), _FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF); }
            static inline TYPE cmpnlt(const TYPE &a, const TYPE &b) { return _mm_xor_si128(cmplt(a, b), _FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF); }
            static inline TYPE cmple (const TYPE &a, const TYPE &b) { return _mm_xor_si128(cmpgt(a, b), _FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF); }
            static inline TYPE cmpnle(const TYPE &a, const TYPE &b) { return cmpgt(a, b); }
#undef TYPE
#undef SUFFIX
        };

        template<> struct VectorHelper<signed short> {
#define TYPE _M128I
#define SUFFIX si128
            LOAD_CAST(short)
            STORE_CAST(short)

            OP_(or_) OP_(and_) OP_(xor_)
            static inline void setZero(TYPE &v) { v = CAT_HELPER(_mm_setzero_, SUFFIX)(); }
#undef SUFFIX
#define SUFFIX epi16
            GATHER_SCATTER_16(signed short)

            static inline TYPE set(const short a) { return CAT_HELPER(_mm_set1_, SUFFIX)(a); }
            static inline TYPE set(const short a, const short b, const short c, const short d,
                    const short e, const short f, const short g, const short h) {
                return CAT_HELPER(_mm_set_, SUFFIX)(a, b, c, d, e, f, g, h);
            }

            static inline void multiplyAndAdd(TYPE &v1, TYPE v2, TYPE v3) {
                v1 = add(mul(v1, v2), v3); }

#ifdef __SSSE3__
            OP1(abs)
#endif

            OPx(mul, mullo)
            OP(min) OP(max)

            static inline TYPE div(const TYPE a, const TYPE b) {
                const short *const _a = reinterpret_cast<const short *>(&a);
                const short *const _b = reinterpret_cast<const short *>(&b);
                union {
                    short i[8];
                    TYPE v;
                } x = { {
                    _a[0] / _b[0],
                    _a[1] / _b[1],
                    _a[2] / _b[2],
                    _a[3] / _b[3],
                    _a[4] / _b[4],
                    _a[5] / _b[5],
                    _a[6] / _b[6],
                    _a[7] / _b[7]
                } };
                return x.v;
            }

            OP(add) OP(sub)
            OPcmp(eq)
            OPcmp(lt)
            OPcmp(gt)
            static inline TYPE cmpneq(const TYPE &a, const TYPE &b) { _M128I x = cmpeq(a, b); return _mm_xor_si128(x, _FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF); }
            static inline TYPE cmpnlt(const TYPE &a, const TYPE &b) { _M128I x = cmplt(a, b); return _mm_xor_si128(x, _FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF); }
            static inline TYPE cmple (const TYPE &a, const TYPE &b) { _M128I x = cmpgt(a, b); return _mm_xor_si128(x, _FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF); }
            static inline TYPE cmpnle(const TYPE &a, const TYPE &b) { return cmpgt(a, b); }
#undef TYPE
#undef SUFFIX
        };

        template<> struct VectorHelper<unsigned short> {
#define TYPE _M128I
#define SUFFIX si128
            LOAD_CAST(unsigned short)
            STORE_CAST(unsigned short)
            OP_CAST_(or_) OP_CAST_(and_) OP_CAST_(xor_)
            static inline void setZero(TYPE &v) { v = CAT_HELPER(_mm_setzero_, SUFFIX)(); }

#undef SUFFIX
#define SUFFIX epu16
            static inline TYPE div(const TYPE a, const TYPE b) {
                const unsigned short *const _a = reinterpret_cast<const unsigned short *>(&a);
                const unsigned short *const _b = reinterpret_cast<const unsigned short *>(&b);
                union {
                    unsigned short i[8];
                    TYPE v;
                } x = { {
                    _a[0] / _b[0],
                    _a[1] / _b[1],
                    _a[2] / _b[2],
                    _a[3] / _b[3],
                    _a[4] / _b[4],
                    _a[5] / _b[5],
                    _a[6] / _b[6],
                    _a[7] / _b[7]
                } };
                return x.v;
            }

#undef SUFFIX
#define SUFFIX epi16
            OPx(mul, mullo) // should work correctly for all values
            OP(min) OP(max) // XXX breaks for values with MSB set
            GATHER_SCATTER_16(unsigned short)
            static inline TYPE set(const unsigned short a) { return CAT_HELPER(_mm_set1_, SUFFIX)(a); }
            static inline TYPE set(const unsigned short a, const unsigned short b, const unsigned short c,
                    const unsigned short d, const unsigned short e, const unsigned short f,
                    const unsigned short g, const unsigned short h) {
                return CAT_HELPER(_mm_set_, SUFFIX)(a, b, c, d, e, f, g, h);
            }

            OP(add) OP(sub)
            OPcmp(eq)
            OPcmp(lt)
            OPcmp(gt)
            static inline TYPE cmpneq(const TYPE &a, const TYPE &b) { return _mm_xor_si128(cmpeq(a, b), _FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF); }
            static inline TYPE cmpnlt(const TYPE &a, const TYPE &b) { return _mm_xor_si128(cmplt(a, b), _FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF); }
            static inline TYPE cmple (const TYPE &a, const TYPE &b) { return _mm_xor_si128(cmpgt(a, b), _FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF); }
            static inline TYPE cmpnle(const TYPE &a, const TYPE &b) { return cmpgt(a, b); }
#undef TYPE
#undef SUFFIX
        };
#undef GATHER_SCATTER_16
#undef GATHER_SCATTER
#undef STORE
#undef LOAD
#undef OP1
#undef OP
#undef OPx
#undef OPcmp
#undef CAT_HELPER
#undef CAT_HELPER2

namespace VectorSpecialInitializerZero { enum Enum { Zero }; }
namespace VectorSpecialInitializerRandom { enum Enum { Random }; }

bool cmpeq32_64(const Mask &, const Mask &);
class Mask : public VectorBase<unsigned int, Mask>
{
    friend struct VectorBase<unsigned int, Mask>;
    friend inline bool cmpeq32_64(const Mask &m1, const Mask &m2) {
        // ps gives 4 bits (MSB from 4 32bit values)
        // pd gives 2 bits (MSB from 2 64bit values)
        return (_mm_movemask_ps(_mm_castsi128_ps(m1.data)) & 3) == _mm_movemask_pd(_mm_castsi128_pd(m2.data));
    }
    protected:
        _M128I data;
    public:
        enum { Size = 16 / sizeof(unsigned int) };
        inline Mask() : data(*reinterpret_cast<const _M128I *>(_FullMaskData)) {}
        inline Mask(const __m128 &x) : data(_mm_castps_si128(x)) {}
        inline Mask(const __m128d &x) : data(_mm_castpd_si128(x)) {}
        inline Mask(const __m128i &x) : data(x) {}

        inline operator const Vector<unsigned int> &() const { return *reinterpret_cast<const Vector<unsigned int> *>(this); }
        inline operator bool() const {
            // _mm_movemask_epi8 creates a 16 bit mask containing the most significant bit of every byte in data
            return _mm_movemask_epi8(data);
        }
        inline bool operator==(const Mask &m) const {
            return _mm_movemask_epi8(data) == _mm_movemask_epi8(m.data);
        }
};
static const Mask &OneMask = *reinterpret_cast<const Mask *const>(_OneMaskData);
static const Mask &FullMask = *reinterpret_cast<const Mask *const>(_FullMaskData);
static inline Mask maskNthElement( int n ) {
    ALIGN(16) union
    {
        unsigned int i[4];
        _M128I m;
    } x = { { 0, 0, 0, 0 } };
    x.i[n] = 0xffffffff;
    return x.m;
}

template<typename T>
class Vector : public VectorBase<T, Vector<T> >
{
    friend struct VectorBase<T, Vector<T> >;
    protected:
        typedef typename VectorBase<T, Vector<T> >::IntrinType IntrinType;
        IntrinType data;
    public:
        typedef T Type;
        enum { Size = 16 / sizeof(T) };
        /**
         * uninitialized
         */
        inline Vector() {}
        /**
         * initialzed to 0 in all 512 bits
         */
        inline Vector(VectorSpecialInitializerZero::Enum) { makeZero(); }
        /**
         * initialize with given _M128 vector
         */
        inline Vector(const IntrinType &x) : data(x) {}
        inline Vector(const Mask &m) : data(m) {}
        /**
         * initialize all 16 or 8 values with the given value
         */
        inline Vector(T a)
        {
            data = VectorHelper<T>::set(a);
        }
        /**
         * initialize consecutive four vector entries with the given values
         */
        template<typename Other>
        inline Vector(Other a, Other b, Other c, Other d)
        {
            data = VectorHelper<T>::set(a, b, c, d);
        }
        /**
         * Initialize the vector with the given data. \param x must point to 64 byte aligned 512
         * byte data.
         */
        template<typename Other> inline Vector(const Other *x) : data(VectorHelper<T>::load(x)) {}

        template<typename Other> static inline Vector broadcast4(const Other *x) { return Vector<T>(x); }

        inline void makeZero() { VectorHelper<T>::setZero(data); }

        template<typename Other> inline void load(const Other *mem) { data = VectorHelper<T>::load(mem); }

        /**
         * Store the vector data to the given memory. The memory must be 64 byte aligned and of 512
         * bytes size.
         */
        template<typename Other> inline void store(Other *mem) const { VectorHelper<T>::store(mem, data); }

        /**
         * Non-temporal store variant. Writes to the memory without polluting the cache.
         */
        template<typename Other> inline void storeStreaming(Other *mem) const { VectorHelper<T>::storeStreaming(mem, data); }

        inline const Vector<T> &dcba() const { return *this; }
        inline const Vector<T> cdab() const { return _mm_shuffle_epi32(data, _MM_SHUFFLE(2, 3, 0, 1)); }
        inline const Vector<T> badc() const { return _mm_shuffle_epi32(data, _MM_SHUFFLE(1, 0, 3, 2)); }
        inline const Vector<T> aaaa() const { return _mm_shuffle_epi32(data, _MM_SHUFFLE(0, 0, 0, 0)); }
        inline const Vector<T> bbbb() const { return _mm_shuffle_epi32(data, _MM_SHUFFLE(1, 1, 1, 1)); }
        inline const Vector<T> cccc() const { return _mm_shuffle_epi32(data, _MM_SHUFFLE(2, 2, 2, 2)); }
        inline const Vector<T> dddd() const { return _mm_shuffle_epi32(data, _MM_SHUFFLE(3, 3, 3, 3)); }
        inline const Vector<T> dbac() const { return _mm_shuffle_epi32(data, _MM_SHUFFLE(3, 1, 0, 2)); }

        inline Vector(const T *array, const Vector<unsigned int> &indexes) { VectorHelper<T>::gather(data, indexes, array); }
        inline Vector(const T *array, const Vector<unsigned int> &indexes, const Mask &m) {
            VectorHelper<T>::gather(data, indexes & Vector<unsigned int>(m), array);
        }
        inline void gather(const T *array, const Vector<unsigned int> &indexes) { VectorHelper<T>::gather(data, indexes, array); }
        inline void gather(const T *array, const Vector<unsigned int> &indexes, const Mask &m) {
            VectorHelper<T>::gather(data, indexes & Vector<unsigned int>(m), array);
        }
        inline void scatter(T *array, const Vector<unsigned int> &indexes) const { VectorHelper<T>::scatter(data, indexes, array); }
        inline void scatter(T *array, const Vector<unsigned int> &indexes, const Mask &m) const {
            VectorHelper<T>::scatter(data, indexes & Vector<unsigned int>(m), array);
        }
        inline Vector(const T *array, const Vector<unsigned short> &indexes) { VectorHelper<T>::gather(data, indexes, array); }
        inline Vector(const T *array, const Vector<unsigned short> &indexes, const Mask &m) {
            VectorHelper<T>::gather(data, indexes & Vector<unsigned short>(m), array);
        }
        inline void gather(const T *array, const Vector<unsigned short> &indexes) { VectorHelper<T>::gather(data, indexes, array); }
        inline void gather(const T *array, const Vector<unsigned short> &indexes, const Mask &m) {
            VectorHelper<T>::gather(data, indexes & Vector<unsigned short>(m), array);
        }
        inline void scatter(T *array, const Vector<unsigned short> &indexes) const { VectorHelper<T>::scatter(data, indexes, array); }
        inline void scatter(T *array, const Vector<unsigned short> &indexes, const Mask &m) const {
            VectorHelper<T>::scatter(data, indexes & Vector<unsigned short>(m), array);
        }

        /**
         * \param array An array of objects where one member should be gathered
         * \param member1 A member pointer to the member of the class/struct that should be gathered
         * \param indexes The indexes in the array. The correct offsets are calculated
         *                automatically.
         * \param mask Optional mask to select only parts of the vector that should be gathered
         */
        template<typename S> inline Vector(const S *array, const T S::* member1,
                const Vector<unsigned int> &indexes, const Mask &mask = Mask()) {
            VectorHelper<T>::gather(data, (indexes & Vector<unsigned int>(mask)), array, member1);
        }
        template<typename S1, typename S2> inline Vector(const S1 *array, const S2 S1::* member1,
                const T S2::* member2, const Vector<unsigned int> &indexes, const Mask &mask = Mask()) {
            VectorHelper<T>::gather(data, (indexes & Vector<unsigned int>(mask)), array, member1, member2);
        }
        template<typename S> inline void gather(const S *array, const T S::* member1,
                const Vector<unsigned int> &indexes, const Mask &mask = Mask()) {
            VectorHelper<T>::gather(data, (indexes & Vector<unsigned int>(mask)), array, member1);
        }
        template<typename S1, typename S2> inline void gather(const S1 *array, const S2 S1::* member1,
                const T S2::* member2, const Vector<unsigned int> &indexes, const Mask &mask = Mask()) {
            VectorHelper<T>::gather(data, (indexes & Vector<unsigned int>(mask)), array, member1, member2);
        }

        template<typename S1> inline void scatter(S1 *array, T S1::* member1,
                const Vector<unsigned int> &indexes, const Mask &mask = Mask()) const {
            VectorHelper<T>::scatter(data, (indexes & Vector<unsigned int>(mask)), array, member1);
        }
        template<typename S1, typename S2> inline void scatter(S1 *array, S2 S1::* member1,
                T S2::* member2, const Vector<unsigned int> &indexes, const Mask &mask = Mask()) const {
            VectorHelper<T>::scatter(data, (indexes & Vector<unsigned int>(mask)), array, member1, member2);
        }

        template<typename S> inline Vector(const S *array, const T S::* member1,
                const Vector<unsigned short> &indexes, const Mask &mask = Mask()) {
            VectorHelper<T>::gather(data, (indexes & Vector<unsigned short>(mask)), array, member1);
        }
        template<typename S1, typename S2> inline Vector(const S1 *array, const S2 S1::* member1,
                const T S2::* member2, const Vector<unsigned short> &indexes, const Mask &mask = Mask()) {
            VectorHelper<T>::gather(data, (indexes & Vector<unsigned short>(mask)), array, member1, member2);
        }
        template<typename S> inline void gather(const S *array, const T S::* member1,
                const Vector<unsigned short> &indexes, const Mask &mask = Mask()) {
            VectorHelper<T>::gather(data, (indexes & Vector<unsigned short>(mask)), array, member1);
        }
        template<typename S1, typename S2> inline void gather(const S1 *array, const S2 S1::* member1,
                const T S2::* member2, const Vector<unsigned short> &indexes, const Mask &mask = Mask()) {
            VectorHelper<T>::gather(data, (indexes & Vector<unsigned short>(mask)), array, member1, member2);
        }

        template<typename S1> inline void scatter(S1 *array, T S1::* member1,
                const Vector<unsigned short> &indexes, const Mask &mask = Mask()) const {
            VectorHelper<T>::scatter(data, (indexes & Vector<unsigned short>(mask)), array, member1);
        }
        template<typename S1, typename S2> inline void scatter(S1 *array, S2 S1::* member1,
                T S2::* member2, const Vector<unsigned short> &indexes, const Mask &mask = Mask()) const {
            VectorHelper<T>::scatter(data, (indexes & Vector<unsigned short>(mask)), array, member1, member2);
        }

        //prefix
        inline Vector &operator++() { data = VectorHelper<T>::add(data, Vector<T>(1)); return *this; }
        //postfix
        inline Vector operator++(int) { const Vector<T> r = *this; data = VectorHelper<T>::add(data, Vector<T>(1)); return r; }

        inline T operator[](int index) const {
            const T *const x = reinterpret_cast<const T *>(&data);
            return x[index];
        }

#define OP1(fun) \
        inline Vector fun() const { return Vector<T>(VectorHelper<T>::fun(data)); } \
        inline Vector &fun##_eq() { data = VectorHelper<T>::fun(data); return *this; }
        OP1(sqrt)
        OP1(abs)
#undef OP1

#define OP(symbol, fun) \
        inline Vector &operator symbol##=(const Vector<T> &x) { data = VectorHelper<T>::fun(data, x.data); return *this; } \
        inline Vector &operator symbol##=(const T &x) { return operator symbol##=(Vector<T>(x)); } \
        inline Vector operator symbol(const Vector<T> &x) const { return Vector<T>(VectorHelper<T>::fun(data, x.data)); } \
        inline Vector operator symbol(const T &x) const { return operator symbol(Vector<T>(x)); }

        OP(+, add)
        OP(-, sub)
        OP(*, mul)
        OP(/, div)
        OP(|, or_)
        OP(&, and_)
        OP(^, xor_)
#undef OP
#define OPcmp(symbol, fun) \
        inline Mask operator symbol(const Vector<T> &x) const { return VectorHelper<T>::fun(data, x.data); } \
        inline Mask operator symbol(const T &x) const { return operator symbol(Vector<T>(x)); }

        OPcmp(==, cmpeq)
        OPcmp(!=, cmpneq)
        OPcmp(>=, cmpnlt)
        OPcmp(>, cmpnle)
        OPcmp(<, cmplt)
        OPcmp(<=, cmple)
#undef OPcmp

        inline void multiplyAndAdd(const Vector<T> &factor, const Vector<T> &summand) {
            VectorHelper<T>::multiplyAndAdd(data, factor, summand);
        }

        inline Vector &assign( const Vector<T> &v, const Mask &mask ) {
          // TODO
        }

        template<typename T2> inline Vector<T2> staticCast() const { return StaticCastHelper<T, T2>::cast(data); }
        template<typename T2> inline Vector<T2> reinterpretCast() const { return ReinterpretCastHelper<T, T2>::cast(data); }

        /**
         * \return \p true  This vector was completely filled. m2 might be 0 or != 0. You still have
         *                  to test this.
         *         \p false This vector was not completely filled. m2 is all 0.
         */
        inline bool pack(Mask &m1, Vector<T> &v2, Mask &m2) {
            return VectorHelper<T>::pack(data, m1, v2.data, m2);
        }
};

template<typename T> class SwizzledVector : public Vector<T> {};

template<typename T> inline Vector<T> operator+(const T &x, const Vector<T> &v) { return v.operator+(x); }
template<typename T> inline Vector<T> operator*(const T &x, const Vector<T> &v) { return v.operator+(x); }
template<typename T> inline Vector<T> operator-(const T &x, const Vector<T> &v) { return Vector<T>(x) - v; }
template<typename T> inline Vector<T> operator/(const T &x, const Vector<T> &v) { return Vector<T>(x) / v; }
template<typename T> inline Mask  operator< (const T &x, const Vector<T> &v) { return Vector<T>(x) <  v; }
template<typename T> inline Mask  operator<=(const T &x, const Vector<T> &v) { return Vector<T>(x) <= v; }
template<typename T> inline Mask  operator> (const T &x, const Vector<T> &v) { return Vector<T>(x) >  v; }
template<typename T> inline Mask  operator>=(const T &x, const Vector<T> &v) { return Vector<T>(x) >= v; }
template<typename T> inline Mask  operator==(const T &x, const Vector<T> &v) { return Vector<T>(x) == v; }
template<typename T> inline Mask  operator!=(const T &x, const Vector<T> &v) { return Vector<T>(x) != v; }

#define PARENT_DATA (static_cast<Vector<T> *>(this)->data)
#define PARENT_DATA_CONST (static_cast<const Vector<T> *>(this)->data)
#define OP_IMPL(symbol, fun) \
  template<> inline Vector<T> &VectorBase<T, Vector<T> >::operator symbol##=(const Vector<T> &x) { PARENT_DATA = VectorHelper<T>::fun(PARENT_DATA, x.data); return *static_cast<Vector<T> *>(this); } \
  template<> inline Vector<T> &VectorBase<T, Vector<T> >::operator symbol##=(const T &x) { return operator symbol##=(Vector<T>(x)); } \
  template<> inline Vector<T> VectorBase<T, Vector<T> >::operator symbol(const Vector<T> &x) const { return Vector<T>(VectorHelper<T>::fun(PARENT_DATA_CONST, x.data)); } \
  template<> inline Vector<T> VectorBase<T, Vector<T> >::operator symbol(const T &x) const { return operator symbol(Vector<T>(x)); }
#define T int
  OP_IMPL(&, and_)
  OP_IMPL(|, or_)
  OP_IMPL(^, xor_)
#undef T
#define T unsigned int
  OP_IMPL(&, and_)
  OP_IMPL(|, or_)
  OP_IMPL(^, xor_)
#undef T
#define T short
  OP_IMPL(&, and_)
  OP_IMPL(|, or_)
  OP_IMPL(^, xor_)
#undef T
#define T unsigned short
  OP_IMPL(&, and_)
  OP_IMPL(|, or_)
  OP_IMPL(^, xor_)
#undef T
#undef OP_IMPL
#undef PARENT_DATA_CONST
#undef PARENT_DATA

  template<typename T> static inline Vector<T> min (const Vector<T> &x, const Vector<T> &y) { return VectorHelper<T>::min(x, y); }
  template<typename T> static inline Vector<T> max (const Vector<T> &x, const Vector<T> &y) { return VectorHelper<T>::max(x, y); }
  template<typename T> static inline Vector<T> min (const Vector<T> &x, const T &y) { return min(x, Vector<T>(y)); }
  template<typename T> static inline Vector<T> max (const Vector<T> &x, const T &y) { return max(x, Vector<T>(y)); }
  template<typename T> static inline Vector<T> min (const T &x, const Vector<T> &y) { return min(Vector<T>(x), y); }
  template<typename T> static inline Vector<T> max (const T &x, const Vector<T> &y) { return max(Vector<T>(x), y); }
  template<typename T> static inline Vector<T> sqrt(const Vector<T> &x) { return VectorHelper<T>::sqrt(x); }
  template<typename T> static inline Vector<T> abs (const Vector<T> &x) { return VectorHelper<T>::abs(x); }
  template<typename T> static inline Vector<T> sin (const Vector<T> &x) { return VectorHelper<T>::sin(x); }
  template<typename T> static inline Vector<T> cos (const Vector<T> &x) { return VectorHelper<T>::cos(x); }
#undef ALIGN
} // namespace SSE

#endif // SSE_VECTOR_H
