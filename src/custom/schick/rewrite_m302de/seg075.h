#if !defined(__BORLANDC__)
namespace M302de {
#endif

/* 0x20 */
void DNG_update_pos(void);

/* 0x25 */
void DNG_inc_level(void);

/* 0x2a */
void DNG_dec_level(void);

/* 0x2f */
void DNG_stub1(void);

/* 0x34 */
void DNG_stub4(void);

/* 0x39 */
void DNG_draw_walls(signed short, signed short, signed short);

/* 0x3e */
void DNG_stub5(void);

/* 0x43 */
void DNG_timestep(signed short);

/* 0x48 */
void DNG_stub2(void);

/* 0x4d */
void DNG_floor_ceil(void);

/* 0x52 */
void DNG_stub3(void);

/* 0x57 */
void DNG_open_door(void);

/* 0x5c */
void DNG_close_door(void);

/* 0x61 */
void DNG_lights(void);

/* 0x66 */
signed short DNG_fallpit(signed short);

/* 0x6b */
signed short is_staff_lvl2_in_group(void);

/* 0x70 */
void DNG_stub6(void);

/* 0x75 */
signed short DNG_check_climb_tools(void);

/* 0x7a */
void DNG_enter_dungeon(signed short);

#if !defined(__BORLANDC__)
/* static */
void DNG_turn(void);
#endif

#if !defined(__BORLANDC__)
}
#endif
