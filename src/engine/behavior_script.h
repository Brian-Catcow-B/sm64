#ifndef BEHAVIOR_SCRIPT_H
#define BEHAVIOR_SCRIPT_H

#include <PR/ultratypes.h>

#define BHV_PROC_CONTINUE 0
#define BHV_PROC_BREAK    1

#define cur_obj_get_int(offset) gCurrentObject->OBJECT_FIELD_S32(offset)
#define cur_obj_get_float(offset) gCurrentObject->OBJECT_FIELD_F32(offset)

#define cur_obj_add_float(offset, value) gCurrentObject->OBJECT_FIELD_F32(offset) += (f32)(value)
#define cur_obj_set_float(offset, value) gCurrentObject->OBJECT_FIELD_F32(offset) = (f32)(value)
#define cur_obj_add_int(offset, value) gCurrentObject->OBJECT_FIELD_S32(offset) += (s32)(value)
#define cur_obj_set_int(offset, value) gCurrentObject->OBJECT_FIELD_S32(offset) = (s32)(value)
#define cur_obj_or_int(offset, value)  gCurrentObject->OBJECT_FIELD_S32(offset) |= (s32)(value)
#define cur_obj_and_int(offset, value) gCurrentObject->OBJECT_FIELD_S32(offset) &= (s32)(value)
#define cur_obj_set_vptr(offset, value) gCurrentObject->OBJECT_FIELD_VPTR(offset) = (void *)(value)

#define obj_and_int(object, offset, value) object->OBJECT_FIELD_S32(offset) &= (s32)(value)

u16 random_u16(void);
float random_float(void);
s32 random_sign(void);

void stub_behavior_script_2(void);

void cur_obj_update(void);

// chaos stuff

void strncpy(u32 a_max_copy, char* a_dst_str, const char* a_src_str);

void chaos_init(void);
void chaos_update(void);

typedef enum level_class_e {
    cLEVEL_CLASS_STAGE,
    cLEVEL_CLASS_CASTLE,
    cLEVEL_CLASS_COUNT
} level_class_e;

#define CHAOS_CODE_DETAILS_NAME_BUFLEN 10
typedef struct chaos_code_details_t {
    char m_shortname[CHAOS_CODE_DETAILS_NAME_BUFLEN];
    u32 m_weight[(u32) cLEVEL_CLASS_COUNT];
} chaos_code_details_t;

typedef enum chaos_code_type_e {
    // RARE
    cCHAOS_CODE_SIN_PHASE_SHIFT,
    cCHAOS_CODE_COS_PHASE_SHIFT,
    // COMMON
    cCHAOS_CODE_KICK_DIVE_SWAP,
    cCHAOS_CODE_VFX_COMMON,
    // end
    cCHAOS_CODE_COUNT,
    cCHAOS_CODE_NONE = cCHAOS_CODE_COUNT
} chaos_code_type_e;

typedef enum chaos_vfx_common_e {
    cCHAOS_VFX_COMMON_NO_SKYBOX_RENDER,
    cCHAOS_VFX_COMMON_NO_HUD,
    cCHAOS_VFX_COMMON_COUNT
} chaos_vfx_common_e;

u8 chaos_num_instances_of_code(chaos_code_type_e a_type);

s16 chaos_sum_active_sin_phase_shift(void);
s16 chaos_sum_active_cos_phase_shift(void);
u8 chaos_is_vfx_common_effect_active(chaos_vfx_common_e a_effect);

#endif // BEHAVIOR_SCRIPT_H
