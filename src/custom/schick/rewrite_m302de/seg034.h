#if !defined(__BORLANDC__)
namespace M302de {
#endif

/* 0x20 */
void FIG_move_hero(Bit8u*, signed short, Bit8u*, Bit8u*);

/* 0x25 */
signed short seg034_000(signed short, signed short, signed short, signed short,
                        signed short, signed short, signed short);

/* 0x2a */
signed char FIG_cb_select_target(Bit8u*, Bit8u*, const signed short);

/* 0x2f */
void seg034_718(signed short, signed short, Bit8u*, Bit8u*, signed short, signed short);

/* 0x34 */
void FIG_latecomers(void);

/* 0x39 */
signed short FIG_move_pathlen(void);

#if !defined(__BORLANDC__)
}
#endif
