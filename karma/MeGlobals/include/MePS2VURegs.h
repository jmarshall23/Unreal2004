#ifndef _MEPS2VUREGS_H
#define _MEPS2VUREGS_H

#define _VU_K                   "vf00"
#define _VU_Z                   "vi00"

#define _VU_SP                  "$vi14"
#define _VU_L                   "$vi15"

#define _VU_STATUS              "$vi16"
#define _VU_MAC                 "$vi17"
#define _VU_CLIPPING            "$vi18"
#define _VU_RESERVED1           "$vi19"
#define _VU_R                   "$vi20"
#define _VU_I                   "$vi21"
#define _VU_Q                   "$vi22"
#define _VU_RESERVED2           "$vi23"
#define _VU_RESERVED3           "$vi24"
#define _VU_RESERVED5           "$vi25"
#define _VU_TPC                 "$vi26"
#define _VU_CMSAR0              "$vi27"
#define _VU_FBRST               "$vi28"
#define _VU_VPUSTAT             "$vi29"
#define _VU_RESERVED6           "$vi30"
#define _VU_CMSAR1              "$vi31"

#define _VU_STATUS_Z            "0x00000001"
#define _VU_STATUS_S            "0x00000002"
#define _VU_STATUS_U            "0x00000004"
#define _VU_STATUS_O            "0x00000008"
#define _VU_STATUS_I            "0x00000010"
#define _VU_STATUS_D            "0x00000020"
#define _VU_STATUS_ZS           "0x00000040"
#define _VU_STATUS_SS           "0x00000080"
#define _VU_STATUS_US           "0x00000100"
#define _VU_STATUS_OS           "0x00000200"
#define _VU_STATUS_IS           "0x00000400"
#define _VU_STATUS_DS           "0x00000800"

#define _VU_MAC_Z_SHIFT         "0"
#define _VU_MAC_ZX              "0x00000008"
#define _VU_MAC_ZY              "0x00000004"
#define _VU_MAC_ZZ              "0x00000002"
#define _VU_MAC_ZW              "0x00000001"
#define _VU_MAC_Z               "0x0000000f"

#define _VU_MAC_S_SHIFT         "4"
#define _VU_MAC_SX              "0x00000080"
#define _VU_MAC_SY              "0x00000040"
#define _VU_MAC_SZ              "0x00000020"
#define _VU_MAC_SW              "0x00000010"
#define _VU_MAC_S               "0x000000f0"

#define _VU_MAC_U_SHIFT         "8"
#define _VU_MAC_UX              "0x00000800"
#define _VU_MAC_UY              "0x00000400"
#define _VU_MAC_UZ              "0x00000200"
#define _VU_MAC_UW              "0x00000100"
#define _VU_MAC_U               "0x00000f00"

#define _VU_MAC_O_SHIFT         "12"
#define _VU_MAC_OX              "0x00008000"
#define _VU_MAC_OY              "0x00004000"
#define _VU_MAC_OZ              "0x00002000"
#define _VU_MAC_OW              "0x00001000"
#define _VU_MAC_O               "0x0000f000"

#define _VU_MAC_ZSX             "0x00000088"
#define _VU_MAC_ZSY             "0x00000044"
#define _VU_MAC_ZSZ             "0x00000022"
#define _VU_MAC_ZSW             "0x00000011"
#define _VU_MAC_ZS              "0x000000ff"

#define _VU_CLIPPING0_SHIFT     "0"
#define _VU_CLIPPING0_PX        "0x00000001"
#define _VU_CLIPPING0_NX        "0x00000002"
#define _VU_CLIPPING0_PY        "0x00000004"
#define _VU_CLIPPING0_NY        "0x00000008"
#define _VU_CLIPPING0_PZ        "0x00000010"
#define _VU_CLIPPING0_NZ        "0x00000020"
#define _VU_CLIPPING0           "0x0000003F"

#define _VU_CLIPPING1_SHIFT     "6"
#define _VU_CLIPPING1_PX        "0x00000040"
#define _VU_CLIPPING1_NX        "0x00000080"
#define _VU_CLIPPING1_PY        "0x00000100"
#define _VU_CLIPPING1_NY        "0x00000200"
#define _VU_CLIPPING1_PZ        "0x00000400"
#define _VU_CLIPPING1_NZ        "0x00000800"
#define _VU_CLIPPING1           "0x00000fc0"

#define _VU_CLIPPING2_SHIFT     "12"
#define _VU_CLIPPING2_PX        "0x00001000"
#define _VU_CLIPPING2_NX        "0x00002000"
#define _VU_CLIPPING2_PY        "0x00004000"
#define _VU_CLIPPING2_NY        "0x00008000"
#define _VU_CLIPPING2_PZ        "0x00010000"
#define _VU_CLIPPING2_NZ        "0x00020000"
#define _VU_CLIPPING2           "0x0003f000"

#define _VU_CLIPPING3_SHIFT     "18"
#define _VU_CLIPPING3_PX        "0x00040000"
#define _VU_CLIPPING3_NX        "0x00080000"
#define _VU_CLIPPING3_PY        "0x00100000"
#define _VU_CLIPPING3_NY        "0x00200000"
#define _VU_CLIPPING3_PZ        "0x00400000"
#define _VU_CLIPPING3_NZ        "0x00800000"
#define _VU_CLIPPING3           "0x00fc0000"

#define VU_STATUS_Z             0x00000001u
#define VU_STATUS_S             0x00000002u
#define VU_STATUS_U             0x00000004u
#define VU_STATUS_O             0x00000008u
#define VU_STATUS_I             0x00000010u
#define VU_STATUS_D             0x00000020u
#define VU_STATUS_ZS            0x00000040u
#define VU_STATUS_SS            0x00000080u
#define VU_STATUS_US            0x00000100u
#define VU_STATUS_OS            0x00000200u
#define VU_STATUS_IS            0x00000400u
#define VU_STATUS_DS            0x00000800u

#define VU_MAC_Z_SHIFT          0u
#define VU_MAC_ZX               0x00000008u
#define VU_MAC_ZY               0x00000004u
#define VU_MAC_ZZ               0x00000002u
#define VU_MAC_ZW               0x00000001u
#define VU_MAC_Z                0x0000000fu

#define VU_MAC_S_SHIFT          4u
#define VU_MAC_SX               0x00000080u
#define VU_MAC_SY               0x00000040u
#define VU_MAC_SZ               0x00000020u
#define VU_MAC_SW               0x00000010u
#define VU_MAC_S                0x000000f0u

#define VU_MAC_U_SHIFT          8u
#define VU_MAC_UX               0x00000800u
#define VU_MAC_UY               0x00000400u
#define VU_MAC_UZ               0x00000200u
#define VU_MAC_UW               0x00000100u
#define VU_MAC_U                0x00000f00u

#define VU_MAC_O_SHIFT          12u
#define VU_MAC_OX               0x00008000u
#define VU_MAC_OY               0x00004000u
#define VU_MAC_OZ               0x00002000u
#define VU_MAC_OW               0x00001000u
#define VU_MAC_O                0x0000f000u

#define VU_MAC_ZSX              0x00000088u
#define VU_MAC_ZSY              0x00000044u
#define VU_MAC_ZSZ              0x00000022u
#define VU_MAC_ZSW              0x00000011u
#define VU_MAC_ZS               0x000000ffu

#define VU_CLIPPING0_SHIFT      0u
#define VU_CLIPPING0_PX         0x00000001u
#define VU_CLIPPING0_NX         0x00000002u
#define VU_CLIPPING0_PY         0x00000004u
#define VU_CLIPPING0_NY         0x00000008u
#define VU_CLIPPING0_PZ         0x00000010u
#define VU_CLIPPING0_NZ         0x00000020u
#define VU_CLIPPING0            0x0000003Fu

#define VU_CLIPPING1_SHIFT      6u
#define VU_CLIPPING1_PX         0x00000040u
#define VU_CLIPPING1_NX         0x00000080u
#define VU_CLIPPING1_PY         0x00000100u
#define VU_CLIPPING1_NY         0x00000200u
#define VU_CLIPPING1_PZ         0x00000400u
#define VU_CLIPPING1_NZ         0x00000800u
#define VU_CLIPPING1            0x00000fc0u

#define VU_CLIPPING2_SHIFT      12u
#define VU_CLIPPING2_PX         0x00001000u
#define VU_CLIPPING2_NX         0x00002000u
#define VU_CLIPPING2_PY         0x00004000u
#define VU_CLIPPING2_NY         0x00008000u
#define VU_CLIPPING2_PZ         0x00010000u
#define VU_CLIPPING2_NZ         0x00020000u
#define VU_CLIPPING2            0x0003f000u

#define VU_CLIPPING3_SHIFT      18u
#define VU_CLIPPING3_PX         0x00040000u
#define VU_CLIPPING3_NX         0x00080000u
#define VU_CLIPPING3_PY         0x00100000u
#define VU_CLIPPING3_NY         0x00200000u
#define VU_CLIPPING3_PZ         0x00400000u
#define VU_CLIPPING3_NZ         0x00800000u
#define VU_CLIPPING3            0x00fc0000u

#endif
