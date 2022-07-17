#ifndef _HMX_HEXAGON_PROTOS_H_
#define _HMX_HEXAGON_PROTOS_H_ 1

#ifdef __HMX__
#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.hf=mxmem(Rs32,Rt32):deep
   C Intrinsic Prototype: void Q6_weight_hf_mxmem_RR_deep(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_hf_mxmem_RR_deep __builtin_HEXAGON_M8_mxmemdp_wei_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):after:cm:sat.ub=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_after_cm_sat_ub(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_after_cm_sat_ub __builtin_HEXAGON_M8_mxcvtr_dm_sat_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):after.ub=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_after_ub(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_after_ub __builtin_HEXAGON_M8_mxcvtr_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.ubit=mxmem(Rs32,Rt32):dilate
   C Intrinsic Prototype: void Q6_weight_ubit_mxmem_RR_dilate(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_ubit_mxmem_RR_dilate __builtin_HEXAGON_M8_mxmemdi_wei_b1
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):before:retain:cm:sat.ub=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_before_retain_cm_sat_ub(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_before_retain_cm_sat_ub __builtin_HEXAGON_M8_mxcvtl_dm_sat_ub_r
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):before:sat.ub=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_before_sat_ub(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_before_sat_ub __builtin_HEXAGON_M8_mxcvtl_sat_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):after.uh=acc:2x1
   C Intrinsic Prototype: void Q6_mxmem_AR_after_uh_2x1(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_after_uh_2x1 __builtin_HEXAGON_M8_mxcvta_uh
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.b=mxmem(Rs32,Rt32):dilate
   C Intrinsic Prototype: void Q6_weight_b_mxmem_RR_dilate(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_b_mxmem_RR_dilate __builtin_HEXAGON_M8_mxmemdi_wei_b
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.c=mxmem(Rs32,Rt32):dilate
   C Intrinsic Prototype: void Q6_weight_c_mxmem_RR_dilate(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_c_mxmem_RR_dilate __builtin_HEXAGON_M8_mxmemdi_wei_c
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):after:retain:pos.hf=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_after_retain_pos_hf(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_after_retain_pos_hf __builtin_HEXAGON_M8_mxcvtr_sat_pos_hf_r
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       activation.hf=mxmem(Rs32,Rt32):single
   C Intrinsic Prototype: void Q6_activation_hf_mxmem_RR_single(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT01
   ========================================================================== */

#define Q6_activation_hf_mxmem_RR_single __builtin_HEXAGON_M8_mxmems_blk_sm_act_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):before.hf=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_before_hf(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_before_hf __builtin_HEXAGON_M8_mxcvtl_sat_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sbit=mxmem(Rs32,Rt32):drop
   C Intrinsic Prototype: void Q6_weight_sbit_mxmem_RR_drop(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sbit_mxmem_RR_drop __builtin_HEXAGON_M8_mxmemdr_wei_sb1
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sc=mxmem(Rs32,Rt32):after
   C Intrinsic Prototype: void Q6_weight_sc_mxmem_RR_after(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sc_mxmem_RR_after __builtin_HEXAGON_M8_mxmema_wei_sc
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.hf=mxmem(Rs32,Rt32):drop
   C Intrinsic Prototype: void Q6_weight_hf_mxmem_RR_drop(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_hf_mxmem_RR_drop __builtin_HEXAGON_M8_mxmemdr_wei_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sbit=mxmem(Rs32,Rt32):dilate
   C Intrinsic Prototype: void Q6_weight_sbit_mxmem_RR_dilate(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sbit_mxmem_RR_dilate __builtin_HEXAGON_M8_mxmemdi_wei_sb1
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.n=mxmem(Rs32,Rt32):after
   C Intrinsic Prototype: void Q6_weight_n_mxmem_RR_after(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_n_mxmem_RR_after __builtin_HEXAGON_M8_mxmema_wei_n
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.c=mxmem(Rs32,Rt32):after
   C Intrinsic Prototype: void Q6_weight_c_mxmem_RR_after(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_c_mxmem_RR_after __builtin_HEXAGON_M8_mxmema_wei_c
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.b=mxmem(Rs32,Rt32):after
   C Intrinsic Prototype: void Q6_weight_b_mxmem_RR_after(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_b_mxmem_RR_after __builtin_HEXAGON_M8_mxmema_wei_b
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):before:cm:sat.ub=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_before_cm_sat_ub(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_before_cm_sat_ub __builtin_HEXAGON_M8_mxcvtl_dm_sat_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       activation.ub=mxmem(Rs32,Rt32):dilate
   C Intrinsic Prototype: void Q6_activation_ub_mxmem_RR_dilate(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT01
   ========================================================================== */

#define Q6_activation_ub_mxmem_RR_dilate __builtin_HEXAGON_M8_mxmemd_blk_sm_act_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sm=mxmem(Rs32,Rt32):drop
   C Intrinsic Prototype: void Q6_weight_sm_mxmem_RR_drop(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sm_mxmem_RR_drop __builtin_HEXAGON_M8_mxmemdr_wei_sm
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sc=mxmem(Rs32,Rt32):drop
   C Intrinsic Prototype: void Q6_weight_sc_mxmem_RR_drop(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sc_mxmem_RR_drop __builtin_HEXAGON_M8_mxmemdr_wei_sc
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):after:retain.ub=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_after_retain_ub(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_after_retain_ub __builtin_HEXAGON_M8_mxcvtr_ub_r
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.ubit=mxmem(Rs32,Rt32):single
   C Intrinsic Prototype: void Q6_weight_ubit_mxmem_RR_single(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_ubit_mxmem_RR_single __builtin_HEXAGON_M8_mxmems_wei_b1
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):before.ub=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_before_ub(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_before_ub __builtin_HEXAGON_M8_mxcvtl_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):before.uh=acc:2x1
   C Intrinsic Prototype: void Q6_mxmem_AR_before_uh_2x1(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_before_uh_2x1 __builtin_HEXAGON_M8_mxcvtb_uh
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       activation.hf=mxmem(Rs32,Rt32)
   C Intrinsic Prototype: void Q6_activation_hf_mxmem_RR(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT01
   ========================================================================== */

#define Q6_activation_hf_mxmem_RR __builtin_HEXAGON_M8_mxmem_blk_sm_act_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.hf=mxmem(Rs32,Rt32):dilate
   C Intrinsic Prototype: void Q6_weight_hf_mxmem_RR_dilate(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_hf_mxmem_RR_dilate __builtin_HEXAGON_M8_mxmemdi_wei_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.c=mxmem(Rs32,Rt32):drop
   C Intrinsic Prototype: void Q6_weight_c_mxmem_RR_drop(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_c_mxmem_RR_drop __builtin_HEXAGON_M8_mxmemdr_wei_c
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.b=mxmem(Rs32,Rt32):drop
   C Intrinsic Prototype: void Q6_weight_b_mxmem_RR_drop(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_b_mxmem_RR_drop __builtin_HEXAGON_M8_mxmemdr_wei_b
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.n=mxmem(Rs32,Rt32):drop
   C Intrinsic Prototype: void Q6_weight_n_mxmem_RR_drop(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_n_mxmem_RR_drop __builtin_HEXAGON_M8_mxmemdr_wei_n
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):before:sat.uh=acc:2x1
   C Intrinsic Prototype: void Q6_mxmem_AR_before_sat_uh_2x1(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_before_sat_uh_2x1 __builtin_HEXAGON_M8_mxcvtb_sat_uh
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.hf=mxmem(Rs32,Rt32):after
   C Intrinsic Prototype: void Q6_weight_hf_mxmem_RR_after(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_hf_mxmem_RR_after __builtin_HEXAGON_M8_mxmema_wei_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.hf=mxmem(Rs32,Rt32):single
   C Intrinsic Prototype: void Q6_weight_hf_mxmem_RR_single(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_hf_mxmem_RR_single __builtin_HEXAGON_M8_mxmems_wei_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):before:retain:pos.hf=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_before_retain_pos_hf(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_before_retain_pos_hf __builtin_HEXAGON_M8_mxcvtl_sat_pos_hf_r
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       activation.ub=mxmem(Rs32,Rt32):deep
   C Intrinsic Prototype: void Q6_activation_ub_mxmem_RR_deep(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT01
   ========================================================================== */

#define Q6_activation_ub_mxmem_RR_deep __builtin_HEXAGON_M8_mxmem_sm_act_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.ubit=mxmem(Rs32,Rt32):deep
   C Intrinsic Prototype: void Q6_weight_ubit_mxmem_RR_deep(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_ubit_mxmem_RR_deep __builtin_HEXAGON_M8_mxmemdp_wei_b1
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxclracc
   C Intrinsic Prototype: void Q6_mxclracc()
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxclracc __builtin_HEXAGON_M8_mxclracc
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):before:retain.ub=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_before_retain_ub(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_before_retain_ub __builtin_HEXAGON_M8_mxcvtl_ub_r
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       activation.hf=mxmem(Rs32,Rt32):deep
   C Intrinsic Prototype: void Q6_activation_hf_mxmem_RR_deep(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT01
   ========================================================================== */

#define Q6_activation_hf_mxmem_RR_deep __builtin_HEXAGON_M8_mxmem_sm_act_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       activation.hf=mxmem(Rs32,Rt32):dilate
   C Intrinsic Prototype: void Q6_activation_hf_mxmem_RR_dilate(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT01
   ========================================================================== */

#define Q6_activation_hf_mxmem_RR_dilate __builtin_HEXAGON_M8_mxmemd_blk_sm_act_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.n=mxmem(Rs32,Rt32):dilate
   C Intrinsic Prototype: void Q6_weight_n_mxmem_RR_dilate(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_n_mxmem_RR_dilate __builtin_HEXAGON_M8_mxmemdi_wei_n
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):after.hf=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_after_hf(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_after_hf __builtin_HEXAGON_M8_mxcvtr_sat_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       activation.ub=mxmem(Rs32,Rt32):cm
   C Intrinsic Prototype: void Q6_activation_ub_mxmem_RR_cm(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT01
   ========================================================================== */

#define Q6_activation_ub_mxmem_RR_cm __builtin_HEXAGON_M8_mxmem_blk_dm_act_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sm=mxmem(Rs32,Rt32)
   C Intrinsic Prototype: void Q6_weight_sm_mxmem_RR(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sm_mxmem_RR __builtin_HEXAGON_M8_mxmem_wei_sm
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       activation.ub=mxmem(Rs32,Rt32):dilate:cm
   C Intrinsic Prototype: void Q6_activation_ub_mxmem_RR_dilate_cm(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT01
   ========================================================================== */

#define Q6_activation_ub_mxmem_RR_dilate_cm __builtin_HEXAGON_M8_mxmemd_blk_dm_act_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):after:sat.ub=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_after_sat_ub(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_after_sat_ub __builtin_HEXAGON_M8_mxcvtr_sat_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       bias=mxmem(Rs32)
   C Intrinsic Prototype: void Q6_bias_mxmem_A(Address Rs)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_bias_mxmem_A __builtin_HEXAGON_M8_mxmem_bias
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       bias=mxmem2(Rs32)
   C Intrinsic Prototype: void Q6_bias_mxmem2_A(Address Rs)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_bias_mxmem2_A __builtin_HEXAGON_M8_mxmem2_bias
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.n=mxmem(Rs32,Rt32)
   C Intrinsic Prototype: void Q6_weight_n_mxmem_RR(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_n_mxmem_RR __builtin_HEXAGON_M8_mxmem_wei_n
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):after:cm.ub=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_after_cm_ub(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_after_cm_ub __builtin_HEXAGON_M8_mxcvtr_dm_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32)=bias
   C Intrinsic Prototype: void Q6_mxmem_bias_A(Address Rs)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_bias_A __builtin_HEXAGON_M8_mxmem_st_bias
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):after:sat.uh=acc:2x1
   C Intrinsic Prototype: void Q6_mxmem_AR_after_sat_uh_2x1(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_after_sat_uh_2x1 __builtin_HEXAGON_M8_mxcvta_sat_uh
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxswapacc.hf
   C Intrinsic Prototype: void Q6_mxswapacc_hf()
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxswapacc_hf __builtin_HEXAGON_M8_mxswap_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxclracc.hf
   C Intrinsic Prototype: void Q6_mxclracc_hf()
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxclracc_hf __builtin_HEXAGON_M8_mxclracc_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       activation.ub=mxmem(Rs32,Rt32):deep:cm
   C Intrinsic Prototype: void Q6_activation_ub_mxmem_RR_deep_cm(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT01
   ========================================================================== */

#define Q6_activation_ub_mxmem_RR_deep_cm __builtin_HEXAGON_M8_mxmem_dm_act_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):before:retain:cm.ub=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_before_retain_cm_ub(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_before_retain_cm_ub __builtin_HEXAGON_M8_mxcvtl_dm_ub_r
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.c=mxmem(Rs32,Rt32)
   C Intrinsic Prototype: void Q6_weight_c_mxmem_RR(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_c_mxmem_RR __builtin_HEXAGON_M8_mxmem_wei_c
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.b=mxmem(Rs32,Rt32)
   C Intrinsic Prototype: void Q6_weight_b_mxmem_RR(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_b_mxmem_RR __builtin_HEXAGON_M8_mxmem_wei_b
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):after:retain.hf=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_after_retain_hf(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_after_retain_hf __builtin_HEXAGON_M8_mxcvtr_sat_hf_r
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxswapacc
   C Intrinsic Prototype: void Q6_mxswapacc()
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxswapacc __builtin_HEXAGON_M8_mxswap
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sc=mxmem(Rs32,Rt32):dilate
   C Intrinsic Prototype: void Q6_weight_sc_mxmem_RR_dilate(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sc_mxmem_RR_dilate __builtin_HEXAGON_M8_mxmemdi_wei_sc
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):after:retain:cm.ub=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_after_retain_cm_ub(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_after_retain_cm_ub __builtin_HEXAGON_M8_mxcvtr_dm_ub_r
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):before:pos.hf=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_before_pos_hf(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_before_pos_hf __builtin_HEXAGON_M8_mxcvtl_sat_pos_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):after:retain.uh=acc:2x1
   C Intrinsic Prototype: void Q6_mxmem_AR_after_retain_uh_2x1(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_after_retain_uh_2x1 __builtin_HEXAGON_M8_mxcvta_uh_r
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sm=mxmem(Rs32,Rt32):single
   C Intrinsic Prototype: void Q6_weight_sm_mxmem_RR_single(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sm_mxmem_RR_single __builtin_HEXAGON_M8_mxmems_wei_sm
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.ubit=mxmem(Rs32,Rt32):after
   C Intrinsic Prototype: void Q6_weight_ubit_mxmem_RR_after(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_ubit_mxmem_RR_after __builtin_HEXAGON_M8_mxmema_wei_b1
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sc=mxmem(Rs32,Rt32):single
   C Intrinsic Prototype: void Q6_weight_sc_mxmem_RR_single(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sc_mxmem_RR_single __builtin_HEXAGON_M8_mxmems_wei_sc
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.ubit=mxmem(Rs32,Rt32):drop
   C Intrinsic Prototype: void Q6_weight_ubit_mxmem_RR_drop(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_ubit_mxmem_RR_drop __builtin_HEXAGON_M8_mxmemdr_wei_b1
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):before:retain.uh=acc:2x1
   C Intrinsic Prototype: void Q6_mxmem_AR_before_retain_uh_2x1(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_before_retain_uh_2x1 __builtin_HEXAGON_M8_mxcvtb_uh_r
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       acc=mxshl(acc,#16)
   C Intrinsic Prototype: void Q6_acc_mxshl_acc()
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_acc_mxshl_acc __builtin_HEXAGON_M8_mxaccshl
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.n=mxmem(Rs32,Rt32):single
   C Intrinsic Prototype: void Q6_weight_n_mxmem_RR_single(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_n_mxmem_RR_single __builtin_HEXAGON_M8_mxmems_wei_n
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.c=mxmem(Rs32,Rt32):single
   C Intrinsic Prototype: void Q6_weight_c_mxmem_RR_single(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_c_mxmem_RR_single __builtin_HEXAGON_M8_mxmems_wei_c
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.b=mxmem(Rs32,Rt32):single
   C Intrinsic Prototype: void Q6_weight_b_mxmem_RR_single(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_b_mxmem_RR_single __builtin_HEXAGON_M8_mxmems_wei_b
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):before:retain:sat.uh=acc:2x1
   C Intrinsic Prototype: void Q6_mxmem_AR_before_retain_sat_uh_2x1(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_before_retain_sat_uh_2x1 __builtin_HEXAGON_M8_mxcvtb_sat_uh_r
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):before:retain.hf=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_before_retain_hf(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_before_retain_hf __builtin_HEXAGON_M8_mxcvtl_sat_hf_r
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sbit=mxmem(Rs32,Rt32):deep
   C Intrinsic Prototype: void Q6_weight_sbit_mxmem_RR_deep(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sbit_mxmem_RR_deep __builtin_HEXAGON_M8_mxmemdp_wei_sb1
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sc=mxmem(Rs32,Rt32)
   C Intrinsic Prototype: void Q6_weight_sc_mxmem_RR(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sc_mxmem_RR __builtin_HEXAGON_M8_mxmem_wei_sc
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem2(Rs32)=bias
   C Intrinsic Prototype: void Q6_mxmem2_bias_A(Address Rs)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem2_bias_A __builtin_HEXAGON_M8_mxmem2_st_bias
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sm=mxmem(Rs32,Rt32):deep
   C Intrinsic Prototype: void Q6_weight_sm_mxmem_RR_deep(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sm_mxmem_RR_deep __builtin_HEXAGON_M8_mxmemdp_wei_sm
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sc=mxmem(Rs32,Rt32):deep
   C Intrinsic Prototype: void Q6_weight_sc_mxmem_RR_deep(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sc_mxmem_RR_deep __builtin_HEXAGON_M8_mxmemdp_wei_sc
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sbit=mxmem(Rs32,Rt32)
   C Intrinsic Prototype: void Q6_weight_sbit_mxmem_RR(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sbit_mxmem_RR __builtin_HEXAGON_M8_mxmem_wei_sb1
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sbit=mxmem(Rs32,Rt32):single
   C Intrinsic Prototype: void Q6_weight_sbit_mxmem_RR_single(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sbit_mxmem_RR_single __builtin_HEXAGON_M8_mxmems_wei_sb1
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):before:cm.ub=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_before_cm_ub(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_before_cm_ub __builtin_HEXAGON_M8_mxcvtl_dm_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sm=mxmem(Rs32,Rt32):after
   C Intrinsic Prototype: void Q6_weight_sm_mxmem_RR_after(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sm_mxmem_RR_after __builtin_HEXAGON_M8_mxmema_wei_sm
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.b=mxmem(Rs32,Rt32):deep
   C Intrinsic Prototype: void Q6_weight_b_mxmem_RR_deep(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_b_mxmem_RR_deep __builtin_HEXAGON_M8_mxmemdp_wei_b
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       activation.ub=mxmem(Rs32,Rt32):single
   C Intrinsic Prototype: void Q6_activation_ub_mxmem_RR_single(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT01
   ========================================================================== */

#define Q6_activation_ub_mxmem_RR_single __builtin_HEXAGON_M8_mxmems_blk_sm_act_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.ubit=mxmem(Rs32,Rt32)
   C Intrinsic Prototype: void Q6_weight_ubit_mxmem_RR(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_ubit_mxmem_RR __builtin_HEXAGON_M8_mxmem_wei_b1
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       activation.ub=mxmem(Rs32,Rt32):single:cm
   C Intrinsic Prototype: void Q6_activation_ub_mxmem_RR_single_cm(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT01
   ========================================================================== */

#define Q6_activation_ub_mxmem_RR_single_cm __builtin_HEXAGON_M8_mxmems_blk_dm_act_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       activation.ub=mxmem(Rs32,Rt32)
   C Intrinsic Prototype: void Q6_activation_ub_mxmem_RR(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT01
   ========================================================================== */

#define Q6_activation_ub_mxmem_RR __builtin_HEXAGON_M8_mxmem_blk_sm_act_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):after:retain:cm:sat.ub=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_after_retain_cm_sat_ub(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_after_retain_cm_sat_ub __builtin_HEXAGON_M8_mxcvtr_dm_sat_ub_r
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       activation.hf=mxmem(Rs32,Rt32):above
   C Intrinsic Prototype: void Q6_activation_hf_mxmem_RR_above(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT01
   ========================================================================== */

#define Q6_activation_hf_mxmem_RR_above __builtin_HEXAGON_M8_mxmemu_blk_sm_act_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):after:pos.hf=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_after_pos_hf(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_after_pos_hf __builtin_HEXAGON_M8_mxcvtr_sat_pos_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sm=mxmem(Rs32,Rt32):dilate
   C Intrinsic Prototype: void Q6_weight_sm_mxmem_RR_dilate(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sm_mxmem_RR_dilate __builtin_HEXAGON_M8_mxmemdi_wei_sm
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       activation.ub=mxmem(Rs32,Rt32):above:cm
   C Intrinsic Prototype: void Q6_activation_ub_mxmem_RR_above_cm(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT01
   ========================================================================== */

#define Q6_activation_ub_mxmem_RR_above_cm __builtin_HEXAGON_M8_mxmemu_blk_dm_act_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       activation.ub=mxmem(Rs32,Rt32):above
   C Intrinsic Prototype: void Q6_activation_ub_mxmem_RR_above(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT01
   ========================================================================== */

#define Q6_activation_ub_mxmem_RR_above __builtin_HEXAGON_M8_mxmemu_blk_sm_act_ub
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):after:retain:sat.uh=acc:2x1
   C Intrinsic Prototype: void Q6_mxmem_AR_after_retain_sat_uh_2x1(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_after_retain_sat_uh_2x1 __builtin_HEXAGON_M8_mxcvta_sat_uh_r
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):before:retain:sat.ub=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_before_retain_sat_ub(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_before_retain_sat_ub __builtin_HEXAGON_M8_mxcvtl_sat_ub_r
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.sbit=mxmem(Rs32,Rt32):after
   C Intrinsic Prototype: void Q6_weight_sbit_mxmem_RR_after(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_sbit_mxmem_RR_after __builtin_HEXAGON_M8_mxmema_wei_sb1
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       mxmem(Rs32,Rt32):after:retain:sat.ub=acc
   C Intrinsic Prototype: void Q6_mxmem_AR_after_retain_sat_ub(Address Rs, Word32 Rt)
   Instruction Type:      ST
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_mxmem_AR_after_retain_sat_ub __builtin_HEXAGON_M8_mxcvtr_sat_ub_r
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.n=mxmem(Rs32,Rt32):deep
   C Intrinsic Prototype: void Q6_weight_n_mxmem_RR_deep(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_n_mxmem_RR_deep __builtin_HEXAGON_M8_mxmemdp_wei_n
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.c=mxmem(Rs32,Rt32):deep
   C Intrinsic Prototype: void Q6_weight_c_mxmem_RR_deep(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_c_mxmem_RR_deep __builtin_HEXAGON_M8_mxmemdp_wei_c
#endif /* __HEXAGON_ARCH___ >= 68 */

#if __HMX_ARCH__ >= 68
/* ==========================================================================
   Assembly Syntax:       weight.hf=mxmem(Rs32,Rt32)
   C Intrinsic Prototype: void Q6_weight_hf_mxmem_RR(UWord32 Rs, Word32 Rt)
   Instruction Type:      LD
   Execution Slots:       SLOT0
   ========================================================================== */

#define Q6_weight_hf_mxmem_RR __builtin_HEXAGON_M8_mxmem_wei_hf
#endif /* __HEXAGON_ARCH___ >= 68 */

#endif /* __HMX__ */

#endif
