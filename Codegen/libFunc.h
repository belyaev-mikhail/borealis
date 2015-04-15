//
// Created by belyaev on 4/15/15.
//

#ifndef AURORA_SANDBOX_LIBFUNC_H
#define AURORA_SANDBOX_LIBFUNC_H

#include <unordered_map>

#include <llvm/Target/TargetLibraryInfo.h>

#include "Util/string_ref.hpp"
#include "Util/collections.hpp"
#include "Util/enums.hpp"

#include "Util/macros.h"

namespace std {
template<>
struct hash<llvm::LibFunc::Func> : borealis::util::enums::enum_hash<llvm::LibFunc::Func> {};
} /* namespace std */

namespace llvm {
namespace LibFunc {

static const std::unordered_map<Func, borealis::util::string_ref>& encoding() {
    static const std::unordered_map<Func, borealis::util::string_ref> retVal {
        {under_IO_getc,        "under_IO_getc"},
        {under_IO_putc,        "under_IO_putc"},
        {ZdaPv,                "ZdaPv"},
        {ZdaPvRKSt9nothrow_t,  "ZdaPvRKSt9nothrow_t"},
        {ZdlPv,                "ZdlPv"},
        {ZdlPvRKSt9nothrow_t,  "ZdlPvRKSt9nothrow_t"},
        {Znaj,                 "Znaj"},
        {ZnajRKSt9nothrow_t,   "ZnajRKSt9nothrow_t"},
        {Znam,                 "Znam"},
        {ZnamRKSt9nothrow_t,   "ZnamRKSt9nothrow_t"},
        {Znwj,                 "Znwj"},
        {ZnwjRKSt9nothrow_t,   "ZnwjRKSt9nothrow_t"},
        {Znwm,                 "Znwm"},
        {ZnwmRKSt9nothrow_t,   "ZnwmRKSt9nothrow_t"},
        {cospi,                "cospi"},
        {cospif,               "cospif"},
        {cxa_atexit,           "cxa_atexit"},
        {cxa_guard_abort,      "cxa_guard_abort"},
        {cxa_guard_acquire,    "cxa_guard_acquire"},
        {cxa_guard_release,    "cxa_guard_release"},
        {dunder_isoc99_scanf,  "dunder_isoc99_scanf"},
        {dunder_isoc99_sscanf, "dunder_isoc99_sscanf"},
        {memcpy_chk,           "memcpy_chk"},
        {sincospi_stret,       "sincospi_stret"},
        {sincospif_stret,      "sincospif_stret"},
        {sinpi,                "sinpi"},
        {sinpif,               "sinpif"},
        {sqrt_finite,          "sqrt_finite"},
        {sqrtf_finite,         "sqrtf_finite"},
        {sqrtl_finite,         "sqrtl_finite"},
        {dunder_strdup,        "dunder_strdup"},
        {dunder_strndup,       "dunder_strndup"},
        {dunder_strtok_r,      "dunder_strtok_r"},
        {abs,                  "abs"},
        {access,               "access"},
        {acos,                 "acos"},
        {acosf,                "acosf"},
        {acosh,                "acosh"},
        {acoshf,               "acoshf"},
        {acoshl,               "acoshl"},
        {acosl,                "acosl"},
        {asin,                 "asin"},
        {asinf,                "asinf"},
        {asinh,                "asinh"},
        {asinhf,               "asinhf"},
        {asinhl,               "asinhl"},
        {asinl,                "asinl"},
        {atan,                 "atan"},
        {atan2,                "atan2"},
        {atan2f,               "atan2f"},
        {atan2l,               "atan2l"},
        {atanf,                "atanf"},
        {atanh,                "atanh"},
        {atanhf,               "atanhf"},
        {atanhl,               "atanhl"},
        {atanl,                "atanl"},
        {atof,                 "atof"},
        {atoi,                 "atoi"},
        {atol,                 "atol"},
        {atoll,                "atoll"},
        {bcmp,                 "bcmp"},
        {bcopy,                "bcopy"},
        {bzero,                "bzero"},
        {calloc,               "calloc"},
        {cbrt,                 "cbrt"},
        {cbrtf,                "cbrtf"},
        {cbrtl,                "cbrtl"},
        {ceil,                 "ceil"},
        {ceilf,                "ceilf"},
        {ceill,                "ceill"},
        {chmod,                "chmod"},
        {chown,                "chown"},
        {clearerr,             "clearerr"},
        {closedir,             "closedir"},
        {copysign,             "copysign"},
        {copysignf,            "copysignf"},
        {copysignl,            "copysignl"},
        {cos,                  "cos"},
        {cosf,                 "cosf"},
        {cosh,                 "cosh"},
        {coshf,                "coshf"},
        {coshl,                "coshl"},
        {cosl,                 "cosl"},
        {ctermid,              "ctermid"},
        {exp,                  "exp"},
        {exp10,                "exp10"},
        {exp10f,               "exp10f"},
        {exp10l,               "exp10l"},
        {exp2,                 "exp2"},
        {exp2f,                "exp2f"},
        {exp2l,                "exp2l"},
        {expf,                 "expf"},
        {expl,                 "expl"},
        {expm1,                "expm1"},
        {expm1f,               "expm1f"},
        {expm1l,               "expm1l"},
        {fabs,                 "fabs"},
        {fabsf,                "fabsf"},
        {fabsl,                "fabsl"},
        {fclose,               "fclose"},
        {fdopen,               "fdopen"},
        {feof,                 "feof"},
        {ferror,               "ferror"},
        {fflush,               "fflush"},
        {ffs,                  "ffs"},
        {ffsl,                 "ffsl"},
        {ffsll,                "ffsll"},
        {fgetc,                "fgetc"},
        {fgetpos,              "fgetpos"},
        {fgets,                "fgets"},
        {fileno,               "fileno"},
        {fiprintf,             "fiprintf"},
        {flockfile,            "flockfile"},
        {floor,                "floor"},
        {floorf,               "floorf"},
        {floorl,               "floorl"},
        {fmax,                 "fmax"},
        {fmaxf,                "fmaxf"},
        {fmaxl,                "fmaxl"},
        {fmin,                 "fmin"},
        {fminf,                "fminf"},
        {fminl,                "fminl"},
        {fmod,                 "fmod"},
        {fmodf,                "fmodf"},
        {fmodl,                "fmodl"},
        {fopen,                "fopen"},
        {fopen64,              "fopen64"},
        {fprintf,              "fprintf"},
        {fputc,                "fputc"},
        {fputs,                "fputs"},
        {fread,                "fread"},
        {free,                 "free"},
        {frexp,                "frexp"},
        {frexpf,               "frexpf"},
        {frexpl,               "frexpl"},
        {fscanf,               "fscanf"},
        {fseek,                "fseek"},
        {fseeko,               "fseeko"},
        {fseeko64,             "fseeko64"},
        {fsetpos,              "fsetpos"},
        {fstat,                "fstat"},
        {fstat64,              "fstat64"},
        {fstatvfs,             "fstatvfs"},
        {fstatvfs64,           "fstatvfs64"},
        {ftell,                "ftell"},
        {ftello,               "ftello"},
        {ftello64,             "ftello64"},
        {ftrylockfile,         "ftrylockfile"},
        {funlockfile,          "funlockfile"},
        {fwrite,               "fwrite"},
        {getc,                 "getc"},
        {getc_unlocked,        "getc_unlocked"},
        {getchar,              "getchar"},
        {getenv,               "getenv"},
        {getitimer,            "getitimer"},
        {getlogin_r,           "getlogin_r"},
        {getpwnam,             "getpwnam"},
        {gets,                 "gets"},
        {gettimeofday,         "gettimeofday"},
        {htonl,                "htonl"},
        {htons,                "htons"},
        {iprintf,              "iprintf"},
        {isascii,              "isascii"},
        {isdigit,              "isdigit"},
        {labs,                 "labs"},
        {lchown,               "lchown"},
        {ldexp,                "ldexp"},
        {ldexpf,               "ldexpf"},
        {ldexpl,               "ldexpl"},
        {llabs,                "llabs"},
        {log,                  "log"},
        {log10,                "log10"},
        {log10f,               "log10f"},
        {log10l,               "log10l"},
        {log1p,                "log1p"},
        {log1pf,               "log1pf"},
        {log1pl,               "log1pl"},
        {log2,                 "log2"},
        {log2f,                "log2f"},
        {log2l,                "log2l"},
        {logb,                 "logb"},
        {logbf,                "logbf"},
        {logbl,                "logbl"},
        {logf,                 "logf"},
        {logl,                 "logl"},
        {lstat,                "lstat"},
        {lstat64,              "lstat64"},
        {malloc,               "malloc"},
        {memalign,             "memalign"},
        {memccpy,              "memccpy"},
        {memchr,               "memchr"},
        {memcmp,               "memcmp"},
        {memcpy,               "memcpy"},
        {memmove,              "memmove"},
        {memrchr,              "memrchr"},
        {memset,               "memset"},
        {memset_pattern16,     "memset_pattern16"},
        {mkdir,                "mkdir"},
        {mktime,               "mktime"},
        {modf,                 "modf"},
        {modff,                "modff"},
        {modfl,                "modfl"},
        {nearbyint,            "nearbyint"},
        {nearbyintf,           "nearbyintf"},
        {nearbyintl,           "nearbyintl"},
        {ntohl,                "ntohl"},
        {ntohs,                "ntohs"},
        {open,                 "open"},
        {open64,               "open64"},
        {opendir,              "opendir"},
        {pclose,               "pclose"},
        {perror,               "perror"},
        {popen,                "popen"},
        {posix_memalign,       "posix_memalign"},
        {pow,                  "pow"},
        {powf,                 "powf"},
        {powl,                 "powl"},
        {pread,                "pread"},
        {printf,               "printf"},
        {putc,                 "putc"},
        {putchar,              "putchar"},
        {puts,                 "puts"},
        {pwrite,               "pwrite"},
        {qsort,                "qsort"},
        {read,                 "read"},
        {readlink,             "readlink"},
        {realloc,              "realloc"},
        {reallocf,             "reallocf"},
        {realpath,             "realpath"},
        {remove,               "remove"},
        {rename,               "rename"},
        {rewind,               "rewind"},
        {rint,                 "rint"},
        {rintf,                "rintf"},
        {rintl,                "rintl"},
        {rmdir,                "rmdir"},
        {round,                "round"},
        {roundf,               "roundf"},
        {roundl,               "roundl"},
        {scanf,                "scanf"},
        {setbuf,               "setbuf"},
        {setitimer,            "setitimer"},
        {setvbuf,              "setvbuf"},
        {sin,                  "sin"},
        {sinf,                 "sinf"},
        {sinh,                 "sinh"},
        {sinhf,                "sinhf"},
        {sinhl,                "sinhl"},
        {sinl,                 "sinl"},
        {siprintf,             "siprintf"},
        {snprintf,             "snprintf"},
        {sprintf,              "sprintf"},
        {sqrt,                 "sqrt"},
        {sqrtf,                "sqrtf"},
        {sqrtl,                "sqrtl"},
        {sscanf,               "sscanf"},
        {stat,                 "stat"},
        {stat64,               "stat64"},
        {statvfs,              "statvfs"},
        {statvfs64,            "statvfs64"},
        {stpcpy,               "stpcpy"},
        {stpncpy,              "stpncpy"},
        {strcasecmp,           "strcasecmp"},
        {strcat,               "strcat"},
        {strchr,               "strchr"},
        {strcmp,               "strcmp"},
        {strcoll,              "strcoll"},
        {strcpy,               "strcpy"},
        {strcspn,              "strcspn"},
        {strdup,               "strdup"},
        {strlen,               "strlen"},
        {strncasecmp,          "strncasecmp"},
        {strncat,              "strncat"},
        {strncmp,              "strncmp"},
        {strncpy,              "strncpy"},
        {strndup,              "strndup"},
        {strnlen,              "strnlen"},
        {strpbrk,              "strpbrk"},
        {strrchr,              "strrchr"},
        {strspn,               "strspn"},
        {strstr,               "strstr"},
        {strtod,               "strtod"},
        {strtof,               "strtof"},
        {strtok,               "strtok"},
        {strtok_r,             "strtok_r"},
        {strtol,               "strtol"},
        {strtold,              "strtold"},
        {strtoll,              "strtoll"},
        {strtoul,              "strtoul"},
        {strtoull,             "strtoull"},
        {strxfrm,              "strxfrm"},
        {system,               "system"},
        {tan,                  "tan"},
        {tanf,                 "tanf"},
        {tanh,                 "tanh"},
        {tanhf,                "tanhf"},
        {tanhl,                "tanhl"},
        {tanl,                 "tanl"},
        {times,                "times"},
        {tmpfile,              "tmpfile"},
        {tmpfile64,            "tmpfile64"},
        {toascii,              "toascii"},
        {trunc,                "trunc"},
        {truncf,               "truncf"},
        {truncl,               "truncl"},
        {uname,                "uname"},
        {ungetc,               "ungetc"},
        {unlink,               "unlink"},
        {unsetenv,             "unsetenv"},
        {utime,                "utime"},
        {utimes,               "utimes"},
        {valloc,               "valloc"},
        {vfprintf,             "vfprintf"},
        {vfscanf,              "vfscanf"},
        {vprintf,              "vprintf"},
        {vscanf,               "vscanf"},
        {vsnprintf,            "vsnprintf"},
        {vsprintf,             "vsprintf"},
        {vsscanf,              "vsscanf"},
        {write,                "write"}
    };

    return retVal;
}

static const std::unordered_map<borealis::util::string_ref, Func>& decoding() {
    static const std::unordered_map<borealis::util::string_ref, Func> reverse =
        borealis::util::viewContainer(encoding())
       .map(LAM(pair, std::make_pair(pair.second, pair.first)))
       .to< std::unordered_map<borealis::util::string_ref, Func> >();

    return reverse;
}

static std::ostream& operator<<(std::ostream& ost, Func func) {
    return ost << encoding().at(func);
}

static std::istream& operator>>(std::istream& ist, Func& func) {
    std::string ret;
    ist >> ret;
    auto&& table = decoding();
    auto it = table.find(ret);
    if(it == std::end(table)) {
        ist.setstate(std::ios::failbit);
        return ist;
    } else {
        func = it->second;
        return ist;
    }
}


} /* namespace LibFunc */
} /* namespace llvm */

#include "Util/unmacros.h"

#endif //AURORA_SANDBOX_LIBFUNC_H
