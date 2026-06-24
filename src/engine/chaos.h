#ifndef CHAOS_H
#define CHAOS_H

#include <PR/ultratypes.h>

#include "behavior_script.h"

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
    cCHAOS_CODE_SIN_UNIT_INTERVAL_COEFFICIENT,
    cCHAOS_CODE_COS_UNIT_INTERVAL_COEFFICIENT,
    // COMMON
    cCHAOS_CODE_KICK_DIVE_SWAP,
    cCHAOS_CODE_OBJ_GRAV_ROLL,
    cCHAOS_CODE_FWD_GRAV_ROLL,
    cCHAOS_CODE_VFX_COMMON,
    // end
    cCHAOS_CODE_COUNT,
    cCHAOS_CODE_NONE = cCHAOS_CODE_COUNT
} chaos_code_type_e;

typedef struct chaos_random_chance_t {
    u16 m_chance_numerator;
    u16 m_chance_denominator;
} chaos_random_chance_t;
#define ROLL_RANDOM_CHANCE_UNDEF_FALSE(chaos_random_chance) \
    (chaos_random_chance.m_chance_denominator > 0 && random_u16() % chaos_random_chance.m_chance_denominator < chaos_random_chance.m_chance_numerator)

typedef enum chaos_vfx_common_e {
    cCHAOS_VFX_COMMON_NO_SKYBOX_RENDER,
    cCHAOS_VFX_COMMON_NO_HUD,
    cCHAOS_VFX_LEFT_TRANSITIONS_ONLY,
    cCHAOS_VFX_COMMON_COUNT
} chaos_vfx_common_e;

u8 chaos_num_instances_of_code(chaos_code_type_e a_type);

s16 chaos_sum_active_sin_phase_shift(void);
s16 chaos_sum_active_cos_phase_shift(void);
f32 chaos_prod_active_sin_ui_coeff(void);
f32 chaos_prod_active_cos_ui_coeff(void);
chaos_random_chance_t chaos_sum_obj_grav_roll(void);
chaos_random_chance_t chaos_sum_fwd_grav_roll(void);
u8 chaos_is_vfx_common_effect_active(chaos_vfx_common_e a_effect);

#endif // CHAOS_H
