#ifndef GENERATE_MACROS_DEFINED
#error "generate_unmacros.h included without corresponding generate_macros.h include"
#endif

/*[[[cog
import cog
import re

with open("Util/generate_macros.h") as f:
    for line in f:
        if(line.startswith("#define ")):
            macro = line.replace(' ', '(').split('(',2)[1]
            if(re.match('^\w+(\w\d)?$', macro)):
                cog.outl("#undef %s" % macro)
]]]*/
#undef GENERATE_MACROS_DEFINED

#undef CONCATENATE
#undef CONCATENATE1
#undef CONCATENATE2
#undef PP_NARG
#undef PP_NARG_
#undef PP_ARG_N
#undef PP_RSEQ_N
#undef PP_FOREACH_1
#undef PP_FOREACH_2
#undef PP_FOREACH_3
#undef PP_FOREACH_4
#undef PP_FOREACH_5
#undef PP_FOREACH_6
#undef PP_FOREACH_7
#undef PP_FOREACH_8
#undef PP_FOREACH_9
#undef PP_FOREACH_10
#undef PP_FOREACH_11
#undef PP_FOREACH_12
#undef PP_FOREACH_13
#undef PP_FOREACH_14
#undef PP_FOREACH_15
#undef PP_FOREACH_16
#undef PP_FOREACH_17
#undef PP_FOREACH_18
#undef PP_FOREACH_19
#undef PP_FOREACH_20
#undef PP_FOREACH_21
#undef PP_FOREACH_22
#undef PP_FOREACH_23
#undef PP_FOREACH_24
#undef PP_FOREACH_25
#undef PP_FOREACH_26
#undef PP_FOREACH_27
#undef PP_FOREACH_28
#undef PP_FOREACH_29
#undef PP_FOREACH_30
#undef PP_FOREACH_31
#undef PP_FOREACH
#undef PP_FOREACH_COMMA_1
#undef PP_FOREACH_COMMA_2
#undef PP_FOREACH_COMMA_3
#undef PP_FOREACH_COMMA_4
#undef PP_FOREACH_COMMA_5
#undef PP_FOREACH_COMMA_6
#undef PP_FOREACH_COMMA_7
#undef PP_FOREACH_COMMA_8
#undef PP_FOREACH_COMMA_9
#undef PP_FOREACH_COMMA_10
#undef PP_FOREACH_COMMA_11
#undef PP_FOREACH_COMMA_12
#undef PP_FOREACH_COMMA_13
#undef PP_FOREACH_COMMA_14
#undef PP_FOREACH_COMMA_15
#undef PP_FOREACH_COMMA_16
#undef PP_FOREACH_COMMA_17
#undef PP_FOREACH_COMMA_18
#undef PP_FOREACH_COMMA_19
#undef PP_FOREACH_COMMA_20
#undef PP_FOREACH_COMMA_21
#undef PP_FOREACH_COMMA_22
#undef PP_FOREACH_COMMA_23
#undef PP_FOREACH_COMMA_24
#undef PP_FOREACH_COMMA_25
#undef PP_FOREACH_COMMA_26
#undef PP_FOREACH_COMMA_27
#undef PP_FOREACH_COMMA_28
#undef PP_FOREACH_COMMA_29
#undef PP_FOREACH_COMMA_30
#undef PP_FOREACH_COMMA_31
#undef PP_FOREACH_COMMA
#undef PP_ID
#undef FIELD_LHV
#undef FIELD_RHV
#undef GENERATE_EQ
#undef GENERATE_LESS
#undef CONSTRUCTOR_PARAM
#undef CONSTRUCTOR_INIT_FIELD
#undef CONSTRUCTOR_COPY_FIELD
#undef CONSTRUCTOR_MOVE_FIELD
#undef GENERATE_CONSTRUCTOR
#undef GENERATE_COPY_CONSTRUCTOR
#undef GENERATE_MOVE_CONSTRUCTOR
#undef ASSIGN_FIELD_RHV
#undef ASSIGN_FIELD_RHV_MOVE
#undef GENERATE_ASSIGN
#undef GENERATE_MOVE_ASSIGN
#undef PRINT_FIELD
#undef GENERATE_PRINT
#undef PRINT_FIELD_CUSTOM
#undef GENERATE_PRINT_CUSTOM
#undef GENERATE_INLINE_HASH
#undef GENERATE_OUTLINE_HASH
#undef JSON_DICT_WRITE
#undef PP_VAL_DECLTYPE
#undef FIELD_NAMES
#undef GENERATE_OUTLINE_JSON_TRAITS
#undef ENUM_PRINT_CASE
#undef GENERATE_ENUM_PRINT
#undef QUAL_ENUM
#undef GENERATE_ENUM_VALUES_FUNC
#undef GENERATE_FANCY_ENUM
#undef DEFAULT_CONSTRUCTOR_AND_ASSIGN
//[[[end]]]
