#ifndef _MDTKEAPROFILE_H
#define _MDTKEAPROFILE_H

#ifndef PROFILE_MDTKEA
#   define PROFILE_MDTKEA               0
#endif

#define USE_TIC                         0
#define USE_MACRO                       1

#if (!PROFILE_MDTKEA)
#   define MdtKeaProfileStart(s)        ((void) 0)
#   define MdtKeaProfileEnd(s)          ((void) 0)
#else
#   include <MeProfile.h>
#   if   (USE_TIC)
#       define MdtKeaProfileStart(s)    tic(s)
#       define MdtKeaProfileEnd(s)      tic("end"##s)
#   elif (USE_MACRO)
#       define MdtKeaProfileStart(s)    MeProfileStartSection((s),0)
#       define MdtKeaProfileEnd(s)      MeProfileEndSection(s)
#   elif (__GNUC__)
        static __inline void MdtKeaProfileStart(const char *const s)
        { MeProfileStartSection(s,0); }
        static __inline void MdtKeaProfileEnd(const char *const s)
        { MeProfileEndSection(s); }
#   elif (_MSC_VER)
        static _inline void MdtKeaProfileStart(const char *const s)
        { MeProfileStartSection(s,0); }
        static _inline void MdtKeaProfileEnd(const char *const s)
        { MeProfileEndSection(s); }
#   else
#       define MdtKeaProfileStart(s)    MeProfileStartSection((s),0)
#       define MdtKeaProfileEnd(s)      MeProfileEndSection(s)
#   endif
#endif

#endif
