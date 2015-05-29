
#ifdef GENERATE_MACROS_DEFINED
#error "generate_macros.h included twice"
#endif

#define GENERATE_MACROS_DEFINED

#define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)  arg1##arg2

#define PP_NARG(...) PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)
/*[[[cog
import cog
maxElems = 32

cog.outl(\
"#define PP_ARG_N(" + ",".join("_%d"%i for i in range(1, maxElems)) + ",N,...) N" \
)
cog.outl(\
"#define PP_RSEQ_N() " + ",".join(reversed(["%d"%i for i in range(maxElems)])) \
)

for k in range(1, maxElems):
    cog.outl(\
'''\
#define PP_FOREACH_%d(MACRO, DELIM, MARG, %s) \\\n%s
''' % (k, ",".join("A%d"%i for i in range(1, k+1)), " DELIM \\\n".join("MACRO(A%d, MARG)"%i for i in range(1, k+1)))\
)
cog.outl(\
"#define PP_FOREACH(MACRO, DELIM, MARG, ...) CONCATENATE(PP_FOREACH_, PP_NARG(__VA_ARGS__))  (MACRO, DELIM, MARG, __VA_ARGS__)"
)

cog.outl('')

for k in range(1, maxElems):
    cog.outl(\
'''\
#define PP_FOREACH_COMMA_%d(MACRO, MARG, %s) \\\n%s
''' % (k, ",".join("A%d"%i for i in range(1, k+1)), ", \\\n".join("MACRO(A%d, MARG)"%i for i in range(1, k+1)))\
)
cog.outl(\
"#define PP_FOREACH_COMMA(MACRO, MARG, ...) CONCATENATE(PP_FOREACH_COMMA_, PP_NARG(__VA_ARGS__))  (MACRO, MARG, __VA_ARGS__)"
)

]]]*/
#define PP_ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,N,...) N
#define PP_RSEQ_N() 31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
#define PP_FOREACH_1(MACRO, DELIM, MARG, A1) \
MACRO(A1, MARG)

#define PP_FOREACH_2(MACRO, DELIM, MARG, A1,A2) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG)

#define PP_FOREACH_3(MACRO, DELIM, MARG, A1,A2,A3) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG)

#define PP_FOREACH_4(MACRO, DELIM, MARG, A1,A2,A3,A4) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG)

#define PP_FOREACH_5(MACRO, DELIM, MARG, A1,A2,A3,A4,A5) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG)

#define PP_FOREACH_6(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG)

#define PP_FOREACH_7(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG)

#define PP_FOREACH_8(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG)

#define PP_FOREACH_9(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG)

#define PP_FOREACH_10(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG)

#define PP_FOREACH_11(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG)

#define PP_FOREACH_12(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG)

#define PP_FOREACH_13(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG)

#define PP_FOREACH_14(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG)

#define PP_FOREACH_15(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG)

#define PP_FOREACH_16(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG) DELIM \
MACRO(A16, MARG)

#define PP_FOREACH_17(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG) DELIM \
MACRO(A16, MARG) DELIM \
MACRO(A17, MARG)

#define PP_FOREACH_18(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG) DELIM \
MACRO(A16, MARG) DELIM \
MACRO(A17, MARG) DELIM \
MACRO(A18, MARG)

#define PP_FOREACH_19(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG) DELIM \
MACRO(A16, MARG) DELIM \
MACRO(A17, MARG) DELIM \
MACRO(A18, MARG) DELIM \
MACRO(A19, MARG)

#define PP_FOREACH_20(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG) DELIM \
MACRO(A16, MARG) DELIM \
MACRO(A17, MARG) DELIM \
MACRO(A18, MARG) DELIM \
MACRO(A19, MARG) DELIM \
MACRO(A20, MARG)

#define PP_FOREACH_21(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG) DELIM \
MACRO(A16, MARG) DELIM \
MACRO(A17, MARG) DELIM \
MACRO(A18, MARG) DELIM \
MACRO(A19, MARG) DELIM \
MACRO(A20, MARG) DELIM \
MACRO(A21, MARG)

#define PP_FOREACH_22(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG) DELIM \
MACRO(A16, MARG) DELIM \
MACRO(A17, MARG) DELIM \
MACRO(A18, MARG) DELIM \
MACRO(A19, MARG) DELIM \
MACRO(A20, MARG) DELIM \
MACRO(A21, MARG) DELIM \
MACRO(A22, MARG)

#define PP_FOREACH_23(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG) DELIM \
MACRO(A16, MARG) DELIM \
MACRO(A17, MARG) DELIM \
MACRO(A18, MARG) DELIM \
MACRO(A19, MARG) DELIM \
MACRO(A20, MARG) DELIM \
MACRO(A21, MARG) DELIM \
MACRO(A22, MARG) DELIM \
MACRO(A23, MARG)

#define PP_FOREACH_24(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG) DELIM \
MACRO(A16, MARG) DELIM \
MACRO(A17, MARG) DELIM \
MACRO(A18, MARG) DELIM \
MACRO(A19, MARG) DELIM \
MACRO(A20, MARG) DELIM \
MACRO(A21, MARG) DELIM \
MACRO(A22, MARG) DELIM \
MACRO(A23, MARG) DELIM \
MACRO(A24, MARG)

#define PP_FOREACH_25(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG) DELIM \
MACRO(A16, MARG) DELIM \
MACRO(A17, MARG) DELIM \
MACRO(A18, MARG) DELIM \
MACRO(A19, MARG) DELIM \
MACRO(A20, MARG) DELIM \
MACRO(A21, MARG) DELIM \
MACRO(A22, MARG) DELIM \
MACRO(A23, MARG) DELIM \
MACRO(A24, MARG) DELIM \
MACRO(A25, MARG)

#define PP_FOREACH_26(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,A26) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG) DELIM \
MACRO(A16, MARG) DELIM \
MACRO(A17, MARG) DELIM \
MACRO(A18, MARG) DELIM \
MACRO(A19, MARG) DELIM \
MACRO(A20, MARG) DELIM \
MACRO(A21, MARG) DELIM \
MACRO(A22, MARG) DELIM \
MACRO(A23, MARG) DELIM \
MACRO(A24, MARG) DELIM \
MACRO(A25, MARG) DELIM \
MACRO(A26, MARG)

#define PP_FOREACH_27(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,A26,A27) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG) DELIM \
MACRO(A16, MARG) DELIM \
MACRO(A17, MARG) DELIM \
MACRO(A18, MARG) DELIM \
MACRO(A19, MARG) DELIM \
MACRO(A20, MARG) DELIM \
MACRO(A21, MARG) DELIM \
MACRO(A22, MARG) DELIM \
MACRO(A23, MARG) DELIM \
MACRO(A24, MARG) DELIM \
MACRO(A25, MARG) DELIM \
MACRO(A26, MARG) DELIM \
MACRO(A27, MARG)

#define PP_FOREACH_28(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,A26,A27,A28) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG) DELIM \
MACRO(A16, MARG) DELIM \
MACRO(A17, MARG) DELIM \
MACRO(A18, MARG) DELIM \
MACRO(A19, MARG) DELIM \
MACRO(A20, MARG) DELIM \
MACRO(A21, MARG) DELIM \
MACRO(A22, MARG) DELIM \
MACRO(A23, MARG) DELIM \
MACRO(A24, MARG) DELIM \
MACRO(A25, MARG) DELIM \
MACRO(A26, MARG) DELIM \
MACRO(A27, MARG) DELIM \
MACRO(A28, MARG)

#define PP_FOREACH_29(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,A26,A27,A28,A29) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG) DELIM \
MACRO(A16, MARG) DELIM \
MACRO(A17, MARG) DELIM \
MACRO(A18, MARG) DELIM \
MACRO(A19, MARG) DELIM \
MACRO(A20, MARG) DELIM \
MACRO(A21, MARG) DELIM \
MACRO(A22, MARG) DELIM \
MACRO(A23, MARG) DELIM \
MACRO(A24, MARG) DELIM \
MACRO(A25, MARG) DELIM \
MACRO(A26, MARG) DELIM \
MACRO(A27, MARG) DELIM \
MACRO(A28, MARG) DELIM \
MACRO(A29, MARG)

#define PP_FOREACH_30(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,A26,A27,A28,A29,A30) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG) DELIM \
MACRO(A16, MARG) DELIM \
MACRO(A17, MARG) DELIM \
MACRO(A18, MARG) DELIM \
MACRO(A19, MARG) DELIM \
MACRO(A20, MARG) DELIM \
MACRO(A21, MARG) DELIM \
MACRO(A22, MARG) DELIM \
MACRO(A23, MARG) DELIM \
MACRO(A24, MARG) DELIM \
MACRO(A25, MARG) DELIM \
MACRO(A26, MARG) DELIM \
MACRO(A27, MARG) DELIM \
MACRO(A28, MARG) DELIM \
MACRO(A29, MARG) DELIM \
MACRO(A30, MARG)

#define PP_FOREACH_31(MACRO, DELIM, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,A26,A27,A28,A29,A30,A31) \
MACRO(A1, MARG) DELIM \
MACRO(A2, MARG) DELIM \
MACRO(A3, MARG) DELIM \
MACRO(A4, MARG) DELIM \
MACRO(A5, MARG) DELIM \
MACRO(A6, MARG) DELIM \
MACRO(A7, MARG) DELIM \
MACRO(A8, MARG) DELIM \
MACRO(A9, MARG) DELIM \
MACRO(A10, MARG) DELIM \
MACRO(A11, MARG) DELIM \
MACRO(A12, MARG) DELIM \
MACRO(A13, MARG) DELIM \
MACRO(A14, MARG) DELIM \
MACRO(A15, MARG) DELIM \
MACRO(A16, MARG) DELIM \
MACRO(A17, MARG) DELIM \
MACRO(A18, MARG) DELIM \
MACRO(A19, MARG) DELIM \
MACRO(A20, MARG) DELIM \
MACRO(A21, MARG) DELIM \
MACRO(A22, MARG) DELIM \
MACRO(A23, MARG) DELIM \
MACRO(A24, MARG) DELIM \
MACRO(A25, MARG) DELIM \
MACRO(A26, MARG) DELIM \
MACRO(A27, MARG) DELIM \
MACRO(A28, MARG) DELIM \
MACRO(A29, MARG) DELIM \
MACRO(A30, MARG) DELIM \
MACRO(A31, MARG)

#define PP_FOREACH(MACRO, DELIM, MARG, ...) CONCATENATE(PP_FOREACH_, PP_NARG(__VA_ARGS__))  (MACRO, DELIM, MARG, __VA_ARGS__)

#define PP_FOREACH_COMMA_1(MACRO, MARG, A1) \
MACRO(A1, MARG)

#define PP_FOREACH_COMMA_2(MACRO, MARG, A1,A2) \
MACRO(A1, MARG), \
MACRO(A2, MARG)

#define PP_FOREACH_COMMA_3(MACRO, MARG, A1,A2,A3) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG)

#define PP_FOREACH_COMMA_4(MACRO, MARG, A1,A2,A3,A4) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG)

#define PP_FOREACH_COMMA_5(MACRO, MARG, A1,A2,A3,A4,A5) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG)

#define PP_FOREACH_COMMA_6(MACRO, MARG, A1,A2,A3,A4,A5,A6) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG)

#define PP_FOREACH_COMMA_7(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG)

#define PP_FOREACH_COMMA_8(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG)

#define PP_FOREACH_COMMA_9(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG)

#define PP_FOREACH_COMMA_10(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG)

#define PP_FOREACH_COMMA_11(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG)

#define PP_FOREACH_COMMA_12(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG)

#define PP_FOREACH_COMMA_13(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG)

#define PP_FOREACH_COMMA_14(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG)

#define PP_FOREACH_COMMA_15(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG)

#define PP_FOREACH_COMMA_16(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG), \
MACRO(A16, MARG)

#define PP_FOREACH_COMMA_17(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG), \
MACRO(A16, MARG), \
MACRO(A17, MARG)

#define PP_FOREACH_COMMA_18(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG), \
MACRO(A16, MARG), \
MACRO(A17, MARG), \
MACRO(A18, MARG)

#define PP_FOREACH_COMMA_19(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG), \
MACRO(A16, MARG), \
MACRO(A17, MARG), \
MACRO(A18, MARG), \
MACRO(A19, MARG)

#define PP_FOREACH_COMMA_20(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG), \
MACRO(A16, MARG), \
MACRO(A17, MARG), \
MACRO(A18, MARG), \
MACRO(A19, MARG), \
MACRO(A20, MARG)

#define PP_FOREACH_COMMA_21(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG), \
MACRO(A16, MARG), \
MACRO(A17, MARG), \
MACRO(A18, MARG), \
MACRO(A19, MARG), \
MACRO(A20, MARG), \
MACRO(A21, MARG)

#define PP_FOREACH_COMMA_22(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG), \
MACRO(A16, MARG), \
MACRO(A17, MARG), \
MACRO(A18, MARG), \
MACRO(A19, MARG), \
MACRO(A20, MARG), \
MACRO(A21, MARG), \
MACRO(A22, MARG)

#define PP_FOREACH_COMMA_23(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG), \
MACRO(A16, MARG), \
MACRO(A17, MARG), \
MACRO(A18, MARG), \
MACRO(A19, MARG), \
MACRO(A20, MARG), \
MACRO(A21, MARG), \
MACRO(A22, MARG), \
MACRO(A23, MARG)

#define PP_FOREACH_COMMA_24(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG), \
MACRO(A16, MARG), \
MACRO(A17, MARG), \
MACRO(A18, MARG), \
MACRO(A19, MARG), \
MACRO(A20, MARG), \
MACRO(A21, MARG), \
MACRO(A22, MARG), \
MACRO(A23, MARG), \
MACRO(A24, MARG)

#define PP_FOREACH_COMMA_25(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG), \
MACRO(A16, MARG), \
MACRO(A17, MARG), \
MACRO(A18, MARG), \
MACRO(A19, MARG), \
MACRO(A20, MARG), \
MACRO(A21, MARG), \
MACRO(A22, MARG), \
MACRO(A23, MARG), \
MACRO(A24, MARG), \
MACRO(A25, MARG)

#define PP_FOREACH_COMMA_26(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,A26) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG), \
MACRO(A16, MARG), \
MACRO(A17, MARG), \
MACRO(A18, MARG), \
MACRO(A19, MARG), \
MACRO(A20, MARG), \
MACRO(A21, MARG), \
MACRO(A22, MARG), \
MACRO(A23, MARG), \
MACRO(A24, MARG), \
MACRO(A25, MARG), \
MACRO(A26, MARG)

#define PP_FOREACH_COMMA_27(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,A26,A27) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG), \
MACRO(A16, MARG), \
MACRO(A17, MARG), \
MACRO(A18, MARG), \
MACRO(A19, MARG), \
MACRO(A20, MARG), \
MACRO(A21, MARG), \
MACRO(A22, MARG), \
MACRO(A23, MARG), \
MACRO(A24, MARG), \
MACRO(A25, MARG), \
MACRO(A26, MARG), \
MACRO(A27, MARG)

#define PP_FOREACH_COMMA_28(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,A26,A27,A28) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG), \
MACRO(A16, MARG), \
MACRO(A17, MARG), \
MACRO(A18, MARG), \
MACRO(A19, MARG), \
MACRO(A20, MARG), \
MACRO(A21, MARG), \
MACRO(A22, MARG), \
MACRO(A23, MARG), \
MACRO(A24, MARG), \
MACRO(A25, MARG), \
MACRO(A26, MARG), \
MACRO(A27, MARG), \
MACRO(A28, MARG)

#define PP_FOREACH_COMMA_29(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,A26,A27,A28,A29) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG), \
MACRO(A16, MARG), \
MACRO(A17, MARG), \
MACRO(A18, MARG), \
MACRO(A19, MARG), \
MACRO(A20, MARG), \
MACRO(A21, MARG), \
MACRO(A22, MARG), \
MACRO(A23, MARG), \
MACRO(A24, MARG), \
MACRO(A25, MARG), \
MACRO(A26, MARG), \
MACRO(A27, MARG), \
MACRO(A28, MARG), \
MACRO(A29, MARG)

#define PP_FOREACH_COMMA_30(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,A26,A27,A28,A29,A30) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG), \
MACRO(A16, MARG), \
MACRO(A17, MARG), \
MACRO(A18, MARG), \
MACRO(A19, MARG), \
MACRO(A20, MARG), \
MACRO(A21, MARG), \
MACRO(A22, MARG), \
MACRO(A23, MARG), \
MACRO(A24, MARG), \
MACRO(A25, MARG), \
MACRO(A26, MARG), \
MACRO(A27, MARG), \
MACRO(A28, MARG), \
MACRO(A29, MARG), \
MACRO(A30, MARG)

#define PP_FOREACH_COMMA_31(MACRO, MARG, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,A26,A27,A28,A29,A30,A31) \
MACRO(A1, MARG), \
MACRO(A2, MARG), \
MACRO(A3, MARG), \
MACRO(A4, MARG), \
MACRO(A5, MARG), \
MACRO(A6, MARG), \
MACRO(A7, MARG), \
MACRO(A8, MARG), \
MACRO(A9, MARG), \
MACRO(A10, MARG), \
MACRO(A11, MARG), \
MACRO(A12, MARG), \
MACRO(A13, MARG), \
MACRO(A14, MARG), \
MACRO(A15, MARG), \
MACRO(A16, MARG), \
MACRO(A17, MARG), \
MACRO(A18, MARG), \
MACRO(A19, MARG), \
MACRO(A20, MARG), \
MACRO(A21, MARG), \
MACRO(A22, MARG), \
MACRO(A23, MARG), \
MACRO(A24, MARG), \
MACRO(A25, MARG), \
MACRO(A26, MARG), \
MACRO(A27, MARG), \
MACRO(A28, MARG), \
MACRO(A29, MARG), \
MACRO(A30, MARG), \
MACRO(A31, MARG)

#define PP_FOREACH_COMMA(MACRO, MARG, ...) CONCATENATE(PP_FOREACH_COMMA_, PP_NARG(__VA_ARGS__))  (MACRO, MARG, __VA_ARGS__)
//[[[end]]]

#define PP_ID(X,...) X

#define FIELD_LHV(X,...) lhv.X
#define FIELD_RHV(X,...) rhv.X

#define GENERATE_EQ(TYPE, ...) \
    friend bool operator==(const TYPE& lhv, const TYPE& rhv) { \
        return std::tie(PP_FOREACH_COMMA(FIELD_LHV,, __VA_ARGS__)) == std::tie(PP_FOREACH_COMMA(FIELD_RHV,, __VA_ARGS__)); \
    }

#define GENERATE_LESS(TYPE, ...) \
    friend bool operator<(const TYPE& lhv, const TYPE& rhv) { \
        return std::tie(PP_FOREACH_COMMA(FIELD_LHV,, __VA_ARGS__)) < std::tie(PP_FOREACH_COMMA(FIELD_RHV,, __VA_ARGS__)); \
    }

#define CONSTRUCTOR_PARAM(X,...) const decltype(X)& X
#define CONSTRUCTOR_INIT_FIELD(X,...) X{X}
#define CONSTRUCTOR_COPY_FIELD(X,...) X{rhv.X}
#define CONSTRUCTOR_MOVE_FIELD(X,...) X{std::move(rhv.X)}

#define GENERATE_CONSTRUCTOR(TYPE, ...) \
    TYPE( PP_FOREACH_COMMA(CONSTRUCTOR_PARAM,, __VA_ARGS__) ): \
        PP_FOREACH_COMMA(CONSTRUCTOR_INIT_FIELD,, __VA_ARGS__) {}

#define GENERATE_COPY_CONSTRUCTOR(TYPE, ...) \
    TYPE( const TYPE& rhv) : \
        PP_FOREACH_COMMA(CONSTRUCTOR_COPY_FIELD,, __VA_ARGS__) {}


#define GENERATE_MOVE_CONSTRUCTOR(TYPE, ...) \
    TYPE( TYPE&& rhv) : \
        PP_FOREACH_COMMA(CONSTRUCTOR_MOVE_FIELD,, __VA_ARGS__) {}

#define ASSIGN_FIELD_RHV(X, ...) X = rhv.X;
#define ASSIGN_FIELD_RHV_MOVE(X, ...) X = std::move(rhv.X);

#define GENERATE_ASSIGN(TYPE, ...) \
\
    TYPE& operator=(const TYPE& rhv) { \
        PP_FOREACH(ASSIGN_FIELD_RHV,,,__VA_ARGS__) \
        return *this; \
    }

#define GENERATE_MOVE_ASSIGN(TYPE, ...) \
\
    TYPE& operator=(TYPE&& rhv) { \
        PP_FOREACH(ASSIGN_FIELD_RHV_MOVE,,,__VA_ARGS__) \
        return *this; \
    }

#define PRINT_FIELD(X,...) #X << " = " << rhv.X
#define GENERATE_PRINT(TYPE, ...) \
    friend std::ostream& operator<<(std::ostream& ost, const TYPE& rhv) { \
        return ost << #TYPE << "{" << PP_FOREACH(PRINT_FIELD, << ", " <<,, __VA_ARGS__) << "}"; \
    }

#define PRINT_FIELD_CUSTOM(X,...) rhv.X
#define GENERATE_PRINT_CUSTOM(BEFORE, AFTER, SEP, TYPE, ...) \
    friend std::ostream& operator<<(std::ostream& ost, const TYPE& rhv) { \
        return ost << BEFORE << PP_FOREACH(PRINT_FIELD_CUSTOM, << SEP <<,, __VA_ARGS__) << AFTER; \
    }

#define GENERATE_INLINE_HASH(TYPE, ...) \
    friend size_t hash_value(const TYPE& rhv) { \
        return borealis::util::hash::simple_hash_value(PP_FOREACH_COMMA(FIELD_RHV,, __VA_ARGS__)); \
    }

#define GENERATE_OUTLINE_HASH(TYPE, ...) \
\
    namespace std{ \
    template<> \
    struct hash<TYPE> {\
        size_t operator()(const TYPE& rhv) const noexcept { \
            return borealis::util::hash::simple_hash_value(PP_FOREACH_COMMA(FIELD_RHV,, __VA_ARGS__)); \
        } \
    }; \
    }

#define JSON_DICT_WRITE(NAME, ...) dict[#NAME] = borealis::util::toJson(val.NAME);
#define PP_VAL_DECLTYPE(NAME, PARENT_TYPE) decltype(std::declval<PARENT_TYPE>().NAME)
#define FIELD_NAMES(NAME, ...) #NAME
#define GENERATE_OUTLINE_JSON_TRAITS(TYPE, ...) \
\
    namespace borealis { namespace util { \
    template<> \
    struct json_traits<TYPE> {\
        typedef std::unique_ptr<TYPE> optional_ptr_t; \
\
        static Json::Value toJson(const TYPE& val) { \
            Json::Value dict; \
            PP_FOREACH(JSON_DICT_WRITE, , , __VA_ARGS__) \
            return dict; \
        } \
\
        static optional_ptr_t fromJson(const Json::Value& json) { \
            using borealis::util::json_object_builder; \
\
            json_object_builder<TYPE, PP_FOREACH_COMMA(PP_VAL_DECLTYPE, TYPE, __VA_ARGS__)> builder { \
                PP_FOREACH_COMMA(FIELD_NAMES, , __VA_ARGS__) \
            }; \
            return optional_ptr_t { \
                builder.build(json) \
            };\
        } \
    }; \
    } \
    }

#define ENUM_CHECK(NAME, ENAME) case ENAME::NAME: return #NAME;
#define ENUM_REVERSE_CHECK(NAME, ENAME) else if (json == #NAME) return optional_ptr_t(new ENAME(ENAME::NAME));

#define GENERATE_OUTLINE_ENUM_JSON_TRAITS(TYPE, ...) \
\
    namespace borealis { namespace util { \
    template<> \
    struct json_traits<TYPE> {\
        typedef std::unique_ptr<TYPE> optional_ptr_t; \
\
        static Json::Value toJson(TYPE val) { \
            switch(val) { \
                PP_FOREACH(ENUM_CHECK, , TYPE, __VA_ARGS__); \
            }\
        } \
\
        static optional_ptr_t fromJson(const Json::Value& json) { \
            if(0) return nullptr; \
            PP_FOREACH(ENUM_REVERSE_CHECK, , TYPE, __VA_ARGS__); \
            return nullptr; \
        } \
    }; \
    } \
    }

#define ENUM_PRINT_CASE(X,ENAME) case ENAME::X : ost << #ENAME << "::" << #X; break;
#define GENERATE_ENUM_PRINT(ename, ...) \
    static std::ostream& operator<<(std::ostream& ost, ename rhv) { \
        switch(rhv) { \
            PP_FOREACH(ENUM_PRINT_CASE,,ename,__VA_ARGS__) \
        } \
        return ost; \
    }

#define ENUM_READ(X,ENAME) { #X, ENAME::X }
#define GENERATE_ENUM_READ(ename, ...) \
    static std::istream& operator>>(std::istream& ist, ename& rhv) { \
        static std::unordered_map<std::string, ename> names { \
            PP_FOREACH_COMMA(ENUM_READ,ename,__VA_ARGS__) \
        }; \
        std::string ret; \
        ist >> ret; \
        auto it = names.find(ret); \
        if(it == std::end(names)) { \
            ist.setstate(std::ios::failbit); \
        } else { \
            rhv = it->second; \
        } \
        return ist; \
    }

#define QUAL_ENUM(X,ENAME) ENAME::X
#define GENERATE_ENUM_VALUES_FUNC(ename, ...) \
    static const std::array<ename, PP_NARG(__VA_ARGS__)>& enumValuesImpl(ename) { \
        static const std::array<ename, PP_NARG(__VA_ARGS__)>& retVal {{ \
            PP_FOREACH_COMMA(QUAL_ENUM, ename, __VA_ARGS__) \
        }}; \
        return retVal; \
    }

#define GENERATE_FANCY_ENUM(ename, ...) \
    enum class ename{ __VA_ARGS__ }; \
    GENERATE_ENUM_PRINT(ename, __VA_ARGS__) \
    GENERATE_ENUM_VALUES_FUNC(ename, __VA_ARGS__)


#define DEFAULT_CONSTRUCTOR_AND_ASSIGN(CLASSNAME) \
    \
    CLASSNAME() = default; \
    CLASSNAME(const CLASSNAME&) = default; \
    CLASSNAME(CLASSNAME&&) = default; \
    CLASSNAME& operator=(const CLASSNAME&) = default; \
    CLASSNAME& operator=(CLASSNAME&&) = default;

