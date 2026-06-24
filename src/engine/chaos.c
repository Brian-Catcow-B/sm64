#include "chaos.h"

#include "behavior_script.h"
#include "game/area.h"
#include "game/print.h"

typedef union chaos_code_data_t {
    s16 u_sin_phase_shift;
    s16 u_cos_phase_shift;
    chaos_random_chance_t u_obj_grav_roll;
    chaos_random_chance_t u_fwd_grav_roll;
    chaos_vfx_common_e u_vfx_common;
} chaos_code_data_t;

typedef struct chaos_entry_t {
    chaos_code_type_e m_type;
    u32 m_passed_time;
    u32 m_end_time;
    chaos_code_data_t m_code_data;
} chaos_entry_t;

#define NUM_CHAOS_ACTIVE_ENTRIES 16
#define FPS 30

static level_class_e gCurrentLevelClass;
static chaos_entry_t gChaosActiveEntries[(u32) cLEVEL_CLASS_COUNT][NUM_CHAOS_ACTIVE_ENTRIES];
static u32 gChaosTotalWeight[(u32) cLEVEL_CLASS_COUNT];
static u32 gChaosTotalCurrentWeight[(u32) cLEVEL_CLASS_COUNT];

//priv decl start
chaos_code_details_t chaos_code_details_from_type(chaos_code_type_e a_type);
u32 random_u32(void);
void chaos_roll_entry(chaos_entry_t* a_entry);
//priv decl end

void strncpy(u32 a_max_copy, char* a_dst_str, const char* a_src_str) {
    u32 num_copied = 0;
    while (num_copied < a_max_copy && a_src_str[num_copied]) {
        a_dst_str[num_copied] = a_src_str[num_copied];
        num_copied++;
    }
    if (num_copied >= a_max_copy) {
        a_dst_str[a_max_copy - 1] = '\0';
    } else {
        a_dst_str[num_copied] = '\0';
    }
}

#define WAIT_BEFORE_FIRST_CODES 1

static char gDEBUGOUT[20];
static s32 gDEBUGOUT_INT;
void chaos_init(void) {
    u8 lc;
    u8 i;
    u32 code_type_int;
    gDEBUGOUT[0] = '%';
    gDEBUGOUT[1] = 'd';
    gDEBUGOUT[2] = '\0';
    gDEBUGOUT_INT = -1;
    gCurrentLevelClass = cLEVEL_CLASS_CASTLE;
    // fill with none and wait some seconds before rolling
    for (lc = 0; lc < cLEVEL_CLASS_COUNT; lc++) {
        for (i = 0; i < NUM_CHAOS_ACTIVE_ENTRIES; i++) {
            gChaosActiveEntries[(u32) lc][i].m_type = cCHAOS_CODE_NONE;
            gChaosActiveEntries[(u32) lc][i].m_passed_time = 0;
            gChaosActiveEntries[(u32) lc][i].m_end_time = WAIT_BEFORE_FIRST_CODES * FPS;
        }
    }
    // add weights into globals
    for (i = 0; i < (u8) cLEVEL_CLASS_COUNT; i++) {
        gChaosTotalWeight[i] = 0;
    }
    for (code_type_int = 0; code_type_int < (u32) cCHAOS_CODE_COUNT; code_type_int++) {
        chaos_code_details_t ccd = chaos_code_details_from_type((chaos_code_type_e) code_type_int);
        for (i = 0; i < (u8) cLEVEL_CLASS_COUNT; i++) {
            gChaosTotalWeight[i] += ccd.m_weight[i];
        }
    }
}

void chaos_update(void)
{
    u8 i;
    print_text_fmt_int(10, 20, gDEBUGOUT, gDEBUGOUT_INT);
    if ((gCurrLevelNum == LEVEL_CASTLE) != (gCurrentLevelClass == cLEVEL_CLASS_CASTLE)) {
        // change between castle and stage from last frame to this
        gCurrentLevelClass = (gCurrLevelNum == LEVEL_CASTLE) ? cLEVEL_CLASS_CASTLE : cLEVEL_CLASS_STAGE;
        // CHAOS_TODO: apply something like 20% reroll to each code
    } else {
        // no change between castle and stage from last frame to this
        for (i = 0; i < NUM_CHAOS_ACTIVE_ENTRIES; i++) {
            if (gChaosActiveEntries[(u32) gCurrentLevelClass][i].m_passed_time >= gChaosActiveEntries[(u32) gCurrentLevelClass][i].m_end_time) {
                chaos_roll_entry(&gChaosActiveEntries[(u32) gCurrentLevelClass][i]);
            } else {
                gChaosActiveEntries[(u32) gCurrentLevelClass][i].m_passed_time++;
            }
        }
    }
}

u8 chaos_num_instances_of_code(chaos_code_type_e a_type) {
    u8 i;
    u8 n = 0;
    for (i = 0; i < NUM_CHAOS_ACTIVE_ENTRIES; i++) {
        if (gChaosActiveEntries[(u32) gCurrentLevelClass][i].m_type == a_type) {
            n++;
        }
    }
    return n;
}

s16 chaos_sum_active_sin_phase_shift(void) {
    u8 i;
    s16 sum = 0;
    for (i = 0; i < NUM_CHAOS_ACTIVE_ENTRIES; i++) {
        if (gChaosActiveEntries[(u32) gCurrentLevelClass][i].m_type == cCHAOS_CODE_SIN_PHASE_SHIFT) {
            sum += gChaosActiveEntries[(u32) gCurrentLevelClass][i].m_code_data.u_sin_phase_shift;
        }
    }
    return sum;
}

s16 chaos_sum_active_cos_phase_shift(void) {
    u8 i;
    s16 sum = 0;
    for (i = 0; i < NUM_CHAOS_ACTIVE_ENTRIES; i++) {
        if (gChaosActiveEntries[(u32) gCurrentLevelClass][i].m_type == cCHAOS_CODE_COS_PHASE_SHIFT) {
            sum += gChaosActiveEntries[(u32) gCurrentLevelClass][i].m_code_data.u_cos_phase_shift;
        }
    }
    return sum;
}

chaos_random_chance_t chaos_sum_obj_grav_roll(void) {
    chaos_random_chance_t rc = { 0, 0 };
    u8 i;
    for (i = 0; i < NUM_CHAOS_ACTIVE_ENTRIES; i++) {
        if (gChaosActiveEntries[(u32) gCurrentLevelClass][i].m_type == cCHAOS_CODE_OBJ_GRAV_ROLL) {
            rc.m_chance_numerator += gChaosActiveEntries[(u32) gCurrentLevelClass][i].m_code_data.u_obj_grav_roll.m_chance_numerator;
            rc.m_chance_denominator += gChaosActiveEntries[(u32) gCurrentLevelClass][i].m_code_data.u_obj_grav_roll.m_chance_denominator;
        }
    }
    return rc;
}

chaos_random_chance_t chaos_sum_fwd_grav_roll(void) {
    chaos_random_chance_t rc = { 0, 0 };
    u8 i;
    for (i = 0; i < NUM_CHAOS_ACTIVE_ENTRIES; i++) {
        if (gChaosActiveEntries[(u32) gCurrentLevelClass][i].m_type == cCHAOS_CODE_FWD_GRAV_ROLL) {
            rc.m_chance_numerator += gChaosActiveEntries[(u32) gCurrentLevelClass][i].m_code_data.u_fwd_grav_roll.m_chance_numerator;
            rc.m_chance_denominator += gChaosActiveEntries[(u32) gCurrentLevelClass][i].m_code_data.u_fwd_grav_roll.m_chance_denominator;
        }
    }
    return rc;
}

u8 chaos_is_vfx_common_effect_active(chaos_vfx_common_e a_effect) {
    u8 i;
    for (i = 0; i < NUM_CHAOS_ACTIVE_ENTRIES; i++) {
        if (gChaosActiveEntries[(u32) gCurrentLevelClass][i].m_type == cCHAOS_CODE_VFX_COMMON && gChaosActiveEntries[(u32) gCurrentLevelClass][i].m_code_data.u_vfx_common == a_effect) {
            return 1;
        }
    }
    return 0;
}

//priv defn start
#define FILL_CCDETAILS_RETURN(ccdetails, strliteral_shortname, stageweight, castleweight) \
    strncpy(CHAOS_CODE_DETAILS_NAME_BUFLEN, ccdetails.m_shortname, strliteral_shortname); \
    ccdetails.m_weight[(u32) cLEVEL_CLASS_STAGE] = stageweight; \
    ccdetails.m_weight[(u32) cLEVEL_CLASS_CASTLE] = castleweight; \
    return ccdetails

// insanely rare (around 100h)
#define INSANELY_RARE (1)
// very rare (around 10h)
#define VERY_RARE (INSANELY_RARE * 10)
// rare (around 2h)
#define RARE (VERY_RARE * 5)
// common (around 30m)
#define COMMON (RARE * 4)
// very common (around 6m) typically don't multiply into this
#define VERY_COMMON (COMMON * 5)

chaos_code_details_t chaos_code_details_from_type(chaos_code_type_e a_type) {
    chaos_code_details_t ccd_out;
    switch (a_type)
    {
        case cCHAOS_CODE_SIN_PHASE_SHIFT: FILL_CCDETAILS_RETURN(ccd_out, "sinphase", RARE, RARE);
        case cCHAOS_CODE_COS_PHASE_SHIFT: FILL_CCDETAILS_RETURN(ccd_out, "cosphase", RARE, RARE);
        case cCHAOS_CODE_KICK_DIVE_SWAP: FILL_CCDETAILS_RETURN(ccd_out, "kckdveswp", COMMON * 2, COMMON);
        case cCHAOS_CODE_OBJ_GRAV_ROLL: FILL_CCDETAILS_RETURN(ccd_out, "objgrvroll", COMMON, COMMON);
        case cCHAOS_CODE_FWD_GRAV_ROLL: FILL_CCDETAILS_RETURN(ccd_out, "fwdgrvroll", COMMON, COMMON);
        case cCHAOS_CODE_VFX_COMMON: FILL_CCDETAILS_RETURN(ccd_out, "vfxcommon", COMMON * 3, COMMON * 3);
        case cCHAOS_CODE_NONE: FILL_CCDETAILS_RETURN(ccd_out, "none", 0, 0);
    }
    // CHAOS_TODO: find some way to throw
}

u32 random_u32(void) {
    u32 result;
    result = random_u16();
    result <<= 16;
    result |= random_u16();
    return result;
}

#define RARE_HUGE_TIME_CHANCE_RECIP (1000)
#define RARE_HUGE_TIME (120)
#define COMMON_MIN_TIME (5)
#define COMMON_MAX_TIME (60)
#define NONE_CHANCE_DENOM (10)
#define NONE_CHANCE_NUMER (7)

// 70% chance of new entry being none
// time is 5-60 seconds with a pre-rolled 0.1% chance of being 120 seconds
void chaos_roll_entry(chaos_entry_t* a_entry) {
    u32 chaos_code_int;
    u32 passed_weight = 0;
    u16 none_rng = random_u16();
    u16 big_time_rng = random_u16();
    u16 time_rng;
    u32 code_rng;
    chaos_code_details_t ccd;
    a_entry->m_passed_time = 0;
    if (big_time_rng % RARE_HUGE_TIME_CHANCE_RECIP == 0) {
        time_rng = RARE_HUGE_TIME * FPS;
    } else {
        time_rng = (random_u16() % (COMMON_MAX_TIME - COMMON_MIN_TIME + 1) + COMMON_MIN_TIME) * FPS;
    }
    if (none_rng % NONE_CHANCE_DENOM < NONE_CHANCE_NUMER) {
        // rolled none for new value
        a_entry->m_type = cCHAOS_CODE_NONE;
        a_entry->m_end_time = ((u32) time_rng);
        return;
    }
    code_rng = random_u32() % gChaosTotalWeight[(u32) gCurrentLevelClass];
    gDEBUGOUT_INT = gChaosTotalWeight[(u32) gCurrentLevelClass];
    for (chaos_code_int = 0; chaos_code_int < (u32) cCHAOS_CODE_COUNT; chaos_code_int++) {
        ccd = chaos_code_details_from_type((chaos_code_type_e) chaos_code_int);
        passed_weight += ccd.m_weight[(u32) gCurrentLevelClass];
        if (code_rng < passed_weight) {
            a_entry->m_type = (chaos_code_type_e) chaos_code_int;
            a_entry->m_end_time = ((u32) time_rng);
            break;
        }
    }

    // code data
    switch (a_entry->m_type) {
        case cCHAOS_CODE_SIN_PHASE_SHIFT:
            //a_entry->m_code_data.u_sin_phase_shift = (s16) (random_u16() % 512 - 256);
            //break; // identical code result from sin/cos so collapse
        case cCHAOS_CODE_COS_PHASE_SHIFT:
            a_entry->m_code_data.u_cos_phase_shift = (s16) (random_u16() % 512 - 256);
            break;
        case cCHAOS_CODE_KICK_DIVE_SWAP:
            break;
        case cCHAOS_CODE_OBJ_GRAV_ROLL:
            a_entry->m_code_data.u_obj_grav_roll.m_chance_numerator = random_u16() % 2 + 1;
            a_entry->m_code_data.u_obj_grav_roll.m_chance_denominator = random_u16() % 6 + 3;
            break;
        case cCHAOS_CODE_FWD_GRAV_ROLL:
            a_entry->m_code_data.u_fwd_grav_roll.m_chance_numerator = random_u16() % 2 + 1;
            a_entry->m_code_data.u_fwd_grav_roll.m_chance_denominator = random_u16() % 6 + 3;
            break;
        case cCHAOS_CODE_VFX_COMMON:
            a_entry->m_code_data.u_vfx_common = (chaos_vfx_common_e) (random_u16() % (u16) cCHAOS_VFX_COMMON_COUNT);
            break;
        case cCHAOS_CODE_NONE:
            break;
    }
}
//priv defn end

