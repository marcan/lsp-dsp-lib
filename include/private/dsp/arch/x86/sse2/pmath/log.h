/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 31 мар. 2020 г.
 *
 * lsp-dsp-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-dsp-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-dsp-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PRIVATE_DSP_ARCH_X86_SSE2_PMATH_LOG_H_
#define PRIVATE_DSP_ARCH_X86_SSE2_PMATH_LOG_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE2_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE2_IMPL */

namespace lsp
{
    namespace sse2
    {
        IF_ARCH_X86(
            static const uint32_t LOG2_CONST[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x007fffff), // frac
                LSP_DSP_VEC4(0x3f800000), // 1.0f
                LSP_DSP_VEC4(0x0000007f), // 127
                LSP_DSP_VEC4(0x3d888889), // C0 = 1/15 = 0.0666666701436043
                LSP_DSP_VEC4(0x3d9d89d9), // C1 = 1/13 = 0.0769230797886848
                LSP_DSP_VEC4(0x3dba2e8c), // C2 = 1/11 = 0.0909090936183929
                LSP_DSP_VEC4(0x3de38e39), // C3 = 1/9 = 0.1111111119389534
                LSP_DSP_VEC4(0x3e124925), // C4 = 1/7 = 0.1428571492433548
                LSP_DSP_VEC4(0x3e4ccccd), // C5 = 1/5 = 0.2000000029802322
                LSP_DSP_VEC4(0x3eaaaaab), // C6 = 1/3 = 0.3333333432674408
                LSP_DSP_VEC4(0x3f800000)  // C7 = 1/1 = 1.0000000000000000
            };

            static const float LOGB_C[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(2.0f * M_LOG2E)
            };

            static const float LOGE_C[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(1.0f / M_LOG2E)
            };

            static const float LOGD_C[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(2.0f * M_LOG10E),
                LSP_DSP_VEC4(0.301029995663981f) // 1/log2(10)
            };
        )

    #define LOGN_CORE_X8 \
        /* xmm0 = x */ \
        __ASM_EMIT("movdqa      %%xmm0, %%xmm1")                /* xmm1 = x */ \
        __ASM_EMIT("movdqa      %%xmm4, %%xmm5")                \
        __ASM_EMIT("pand        0x00 + %[L2C], %%xmm0")         /* xmm0 = x & F_MASK */ \
        __ASM_EMIT("pand        0x00 + %[L2C], %%xmm4")         \
        __ASM_EMIT("psrld       $23, %%xmm1")                   /* xmm1 = ilog2(x) + 127 */ \
        __ASM_EMIT("psrld       $23, %%xmm5")                   \
        __ASM_EMIT("por         0x10 + %[L2C], %%xmm0")         /* xmm0 = X = (x & F_MASK) | 1.0f */ \
        __ASM_EMIT("por         0x10 + %[L2C], %%xmm4")         \
        __ASM_EMIT("psubd       0x20 + %[L2C], %%xmm1")         /* xmm1 = r = ilog2(x) */ \
        __ASM_EMIT("psubd       0x20 + %[L2C], %%xmm5")         \
        __ASM_EMIT("movaps      %%xmm0, %%xmm2")                /* xmm2 = X */ \
        __ASM_EMIT("movaps      %%xmm4, %%xmm6")                \
        __ASM_EMIT("cvtdq2ps    %%xmm1, %%xmm1")                /* xmm1 = R = float(r) */ \
        __ASM_EMIT("cvtdq2ps    %%xmm5, %%xmm5")                \
        __ASM_EMIT("subps       0x10 + %[L2C], %%xmm0")         /* xmm0 = X - 1 */ \
        __ASM_EMIT("subps       0x10 + %[L2C], %%xmm4")         \
        __ASM_EMIT("addps       0x10 + %[L2C], %%xmm2")         /* xmm2 = X + 1 */ \
        __ASM_EMIT("addps       0x10 + %[L2C], %%xmm6")         \
        __ASM_EMIT("divps       %%xmm2, %%xmm0")                /* xmm0 = y = (X-1)/(X+1) */ \
        __ASM_EMIT("divps       %%xmm6, %%xmm4")                \
        __ASM_EMIT("movaps      %%xmm0, %%xmm2")                /* xmm2 = y */ \
        __ASM_EMIT("movaps      %%xmm4, %%xmm6")                \
        __ASM_EMIT("movaps      0x30 + %[L2C], %%xmm3")         /* xmm3 = C0 */ \
        __ASM_EMIT("movaps      0x30 + %[L2C], %%xmm7")         \
        __ASM_EMIT("mulps       %%xmm2, %%xmm2")                /* xmm2 = Y = y*y */ \
        __ASM_EMIT("mulps       %%xmm6, %%xmm6")                \
        /* xmm0 = y, xmm1 = R, xmm2 = Y */ \
        __ASM_EMIT("mulps       %%xmm2, %%xmm3")                /* xmm3 = C0*Y */ \
        __ASM_EMIT("mulps       %%xmm6, %%xmm7")                \
        __ASM_EMIT("addps       0x40 + %[L2C], %%xmm3")         /* xmm3 = C1+C0*Y */ \
        __ASM_EMIT("addps       0x40 + %[L2C], %%xmm7")         \
        __ASM_EMIT("mulps       %%xmm2, %%xmm3")                /* xmm3 = Y*(C1+C0*Y) */ \
        __ASM_EMIT("mulps       %%xmm6, %%xmm7")                \
        __ASM_EMIT("addps       0x50 + %[L2C], %%xmm3")         /* xmm3 = C2+Y*(C1+C0*Y) */ \
        __ASM_EMIT("addps       0x50 + %[L2C], %%xmm7")         \
        __ASM_EMIT("mulps       %%xmm2, %%xmm3")                /* xmm3 = Y*(C2+Y*(C1+C0*Y)) */ \
        __ASM_EMIT("mulps       %%xmm6, %%xmm7")                \
        __ASM_EMIT("addps       0x60 + %[L2C], %%xmm3")         /* xmm3 = C3+Y*(C2+Y*(C1+C0*Y)) */ \
        __ASM_EMIT("addps       0x60 + %[L2C], %%xmm7")         \
        __ASM_EMIT("mulps       %%xmm2, %%xmm3")                /* xmm3 = Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
        __ASM_EMIT("mulps       %%xmm6, %%xmm7")                \
        __ASM_EMIT("addps       0x70 + %[L2C], %%xmm3")         /* xmm3 = C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
        __ASM_EMIT("addps       0x70 + %[L2C], %%xmm7")         \
        __ASM_EMIT("mulps       %%xmm2, %%xmm3")                /* xmm3 = Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
        __ASM_EMIT("mulps       %%xmm6, %%xmm7")                \
        __ASM_EMIT("addps       0x80 + %[L2C], %%xmm3")         /* xmm3 = C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
        __ASM_EMIT("addps       0x80 + %[L2C], %%xmm7")         \
        __ASM_EMIT("mulps       %%xmm2, %%xmm3")                /* xmm3 = Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \
        __ASM_EMIT("mulps       %%xmm6, %%xmm7")                \
        __ASM_EMIT("addps       0x90 + %[L2C], %%xmm3")         /* xmm3 = C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \
        __ASM_EMIT("addps       0x90 + %[L2C], %%xmm7")         \
        __ASM_EMIT("mulps       %%xmm2, %%xmm3")                /* xmm3 = Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))))) */ \
        __ASM_EMIT("mulps       %%xmm6, %%xmm7")                \
        __ASM_EMIT("addps       0xa0 + %[L2C], %%xmm3")         /* xmm3 = C7+Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))))) */ \
        __ASM_EMIT("addps       0xa0 + %[L2C], %%xmm7")         \
        __ASM_EMIT("mulps       %%xmm3, %%xmm0")                /* xmm0 = y*(C7+Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))))) */ \
        __ASM_EMIT("mulps       %%xmm7, %%xmm4")                \
        /* xmm0 = y*L, xmm1 = R */

    #define LOGN_CORE_X4 \
        /* xmm0 = x */ \
        __ASM_EMIT("movdqa      %%xmm0, %%xmm1")                /* xmm1 = x */ \
        __ASM_EMIT("pand        0x00 + %[L2C], %%xmm0")         /* xmm0 = x & F_MASK */ \
        __ASM_EMIT("psrld       $23, %%xmm1")                   /* xmm1 = ilog2(x) + 127 */ \
        __ASM_EMIT("por         0x10 + %[L2C], %%xmm0")         /* xmm0 = X = (x & F_MASK) | 1.0f */ \
        __ASM_EMIT("psubd       0x20 + %[L2C], %%xmm1")         /* xmm1 = r = ilog2(x) */ \
        __ASM_EMIT("movaps      %%xmm0, %%xmm2")                /* xmm2 = X */ \
        __ASM_EMIT("cvtdq2ps    %%xmm1, %%xmm1")                /* xmm1 = R = float(r) */ \
        __ASM_EMIT("subps       0x10 + %[L2C], %%xmm0")         /* xmm0 = X - 1 */ \
        __ASM_EMIT("addps       0x10 + %[L2C], %%xmm2")         /* xmm2 = X + 1 */ \
        __ASM_EMIT("divps       %%xmm2, %%xmm0")                /* xmm0 = y = (X-1)/(X+1) */ \
        __ASM_EMIT("movaps      %%xmm0, %%xmm2")                /* xmm2 = y */ \
        __ASM_EMIT("movaps      0x30 + %[L2C], %%xmm3")         /* xmm3 = C0 */ \
        __ASM_EMIT("mulps       %%xmm2, %%xmm2")                /* xmm2 = Y = y*y */ \
        /* xmm0 = y, xmm1 = R, xmm2 = Y */ \
        __ASM_EMIT("mulps       %%xmm2, %%xmm3")                /* xmm3 = C0*Y */ \
        __ASM_EMIT("addps       0x40 + %[L2C], %%xmm3")         /* xmm3 = C1+C0*Y */ \
        __ASM_EMIT("mulps       %%xmm2, %%xmm3")                /* xmm3 = Y*(C1+C0*Y) */ \
        __ASM_EMIT("addps       0x50 + %[L2C], %%xmm3")         /* xmm3 = C2+Y*(C1+C0*Y) */ \
        __ASM_EMIT("mulps       %%xmm2, %%xmm3")                /* xmm3 = Y*(C2+Y*(C1+C0*Y)) */ \
        __ASM_EMIT("addps       0x60 + %[L2C], %%xmm3")         /* xmm3 = C3+Y*(C2+Y*(C1+C0*Y)) */ \
        __ASM_EMIT("mulps       %%xmm2, %%xmm3")                /* xmm3 = Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
        __ASM_EMIT("addps       0x70 + %[L2C], %%xmm3")         /* xmm3 = C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))) */ \
        __ASM_EMIT("mulps       %%xmm2, %%xmm3")                /* xmm3 = Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
        __ASM_EMIT("addps       0x80 + %[L2C], %%xmm3")         /* xmm3 = C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))) */ \
        __ASM_EMIT("mulps       %%xmm2, %%xmm3")                /* xmm3 = Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \
        __ASM_EMIT("addps       0x90 + %[L2C], %%xmm3")         /* xmm3 = C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))) */ \
        __ASM_EMIT("mulps       %%xmm2, %%xmm3")                /* xmm3 = Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))))) */ \
        __ASM_EMIT("addps       0xa0 + %[L2C], %%xmm3")         /* xmm3 = C7+Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y)))))) */ \
        __ASM_EMIT("mulps       %%xmm3, %%xmm0")                /* xmm0 = y*(C7+Y*(C6+Y*(C5+Y*(C4+Y*(C3+Y*(C2+Y*(C1+C0*Y))))))) */ \
        /* xmm0 = y*L, xmm1 = R */

    #define LOGB_CORE_X8 \
        LOGN_CORE_X8 \
        __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0") \
        __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm4") \
        __ASM_EMIT("addps           %%xmm1, %%xmm0") \
        __ASM_EMIT("addps           %%xmm5, %%xmm4")

    #define LOGB_CORE_X4 \
        LOGN_CORE_X4 \
        __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0") \
        __ASM_EMIT("addps           %%xmm1, %%xmm0")

    #define LOGE_CORE_X8 \
        LOGN_CORE_X8 \
        __ASM_EMIT("addps           %%xmm0, %%xmm0") \
        __ASM_EMIT("addps           %%xmm4, %%xmm4") \
        __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm1") \
        __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm5") \
        __ASM_EMIT("addps           %%xmm1, %%xmm0") \
        __ASM_EMIT("addps           %%xmm5, %%xmm4")

    #define LOGE_CORE_X4 \
        LOGN_CORE_X4 \
        __ASM_EMIT("addps           %%xmm0, %%xmm0") \
        __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm1") \
        __ASM_EMIT("addps           %%xmm1, %%xmm0")

    #define LOGD_CORE_X8 \
        LOGN_CORE_X8 \
        __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0") \
        __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm4") \
        __ASM_EMIT("mulps           0x10 + %[LOGC], %%xmm1") \
        __ASM_EMIT("mulps           0x10 + %[LOGC], %%xmm5") \
        __ASM_EMIT("addps           %%xmm1, %%xmm0") \
        __ASM_EMIT("addps           %%xmm5, %%xmm4")

    #define LOGD_CORE_X4 \
        LOGN_CORE_X4 \
        __ASM_EMIT("mulps           0x00 + %[LOGC], %%xmm0") \
        __ASM_EMIT("mulps           0x10 + %[LOGC], %%xmm1") \
        __ASM_EMIT("addps           %%xmm1, %%xmm0")

        void logb2(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM(
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
                __ASM_EMIT("movups          0x10(%[src]), %%xmm4")
                LOGB_CORE_X8
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
                LOGB_CORE_X4
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             12f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("movss           0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("6:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("movhps          0x00(%[src]), %%xmm0")
                __ASM_EMIT("8:")
                LOGB_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGB_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void logb1(float *dst, size_t count)
        {
            IF_ARCH_X86(float *src);

            ARCH_X86_ASM(
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups          0x00(%[dst]), %%xmm0")
                __ASM_EMIT("movups          0x10(%[dst]), %%xmm4")
                LOGB_CORE_X8
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("movups          0x00(%[dst]), %%xmm0")
                LOGB_CORE_X4
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             12f")
                __ASM_EMIT("mov             %[dst], %[src]")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("movss           0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("6:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("movhps          0x00(%[src]), %%xmm0")
                __ASM_EMIT("8:")
                LOGB_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "=&r" (src), [count] "+r" (count)
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGB_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void loge2(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM(
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
                __ASM_EMIT("movups          0x10(%[src]), %%xmm4")
                LOGE_CORE_X8
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
                LOGE_CORE_X4
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             12f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("movss           0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("6:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("movhps          0x00(%[src]), %%xmm0")
                __ASM_EMIT("8:")
                LOGE_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void loge1(float *dst, size_t count)
        {
            IF_ARCH_X86(float *src);

            ARCH_X86_ASM(
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups          0x00(%[dst]), %%xmm0")
                __ASM_EMIT("movups          0x10(%[dst]), %%xmm4")
                LOGE_CORE_X8
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("movups          0x00(%[dst]), %%xmm0")
                LOGE_CORE_X4
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             12f")
                __ASM_EMIT("mov            %[dst], %[src]")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("movss           0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("6:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("movhps          0x00(%[src]), %%xmm0")
                __ASM_EMIT("8:")
                LOGE_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "=&r" (src), [count] "+r" (count)
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGE_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void logd2(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM(
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
                __ASM_EMIT("movups          0x10(%[src]), %%xmm4")
                LOGD_CORE_X8
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("movups          0x00(%[src]), %%xmm0")
                LOGD_CORE_X4
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             12f")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("movss           0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("6:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("movhps          0x00(%[src]), %%xmm0")
                __ASM_EMIT("8:")
                LOGD_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")

                // End
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGD_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void logd1(float *dst, size_t count)
        {
            IF_ARCH_X86(float *src);

            ARCH_X86_ASM(
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups          0x00(%[dst]), %%xmm0")
                __ASM_EMIT("movups          0x10(%[dst]), %%xmm4")
                LOGD_CORE_X8
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("movups          0x00(%[dst]), %%xmm0")
                LOGD_CORE_X4
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             12f")
                __ASM_EMIT("mov             %[dst], %[src]")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("movss           0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("6:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("movhps          0x00(%[src]), %%xmm0")
                __ASM_EMIT("8:")
                LOGD_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")

                // End
                __ASM_EMIT("12:")

                : [dst] "+r" (dst), [src] "=&r" (src), [count] "+r" (count)
                : [L2C] "o" (LOG2_CONST),
                  [LOGC] "o" (LOGD_C)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    /*
        float t_log2(float x)
        {
            union { float f; uint32_t i; } u;

            x = fabs(x);
            u.f = x;
            int f = u.i >> 23; // f = mant(abs(x))
            u.i = (u.i & 0x007fffff) | 0x3f800000;

            float X = u.f; // x = frac(abs(x))
            printf("  x = %.6f, f=%d (%d), u.f=%.6f, X=%.6f\n", x, f, f-127, u.f, X);

            float y = (X-1)/(X+1);
            float y2 = y*y;

            float L = 1/13.0f + y2 * 1/15.0f;
            L = 1/11.0f + y2 * L;
            L = 1/9.0f + y2 * L;
            L = 1/7.0f + y2 * L;
            L = 1/5.0f + y2 * L;
            L = 1/3.0f + y2 * L;
            L = 1 + y2 * L;
            L = 2 * y * L * M_LOG2E;

            return L + (f - 127);
        }
    */
    } /* namespace sse */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_SSE2_PMATH_LOG_H_ */
