/**
 *	Rewrite of DSA1 v3.02_de functions of seg042 (fightsystem: hero action)
 *	Functions rewritten: 1/1 (complete)
 *
 *	Borlandified and identical
 *	Compiler:	Borland C++ 3.1
 *	Call:		BCC.EXE -mlarge -O- -c -1 -Yo seg042.cpp
 */

#include <stdio.h>

#include "v302de.h"

#include "seg002.h"
#include "seg004.h"
#include "seg005.h"
#include "seg006.h"
#include "seg007.h"
#include "seg032.h"
#include "seg038.h"
#include "seg039.h"
#include "seg041.h"
#include "seg043.h"
#include "seg044.h"
#include "seg045.h"
#include "seg096.h"
#include "seg098.h"

#if !defined(__BORLANDC__)
namespace M302de {
#endif

struct coords {
	signed char x,y;
};

struct dummy {
	struct coords a[4];
};

struct msg {
	signed short pos, type;
};

/**
 * \brief   executes the fight action of hero
 *
 * \param   hero        pointer to the hero
 * \param   hero_pos    position in the group (fighter_id = hero_pos + 1)
 */
void FIG_do_hero_action(RealPt hero, const signed short hero_pos)
{
	signed short damage;
	Bit8u *target_monster;
	Bit8u *target_hero;
	Bit8u *p_weapon;
	Bit8u *p_weapon_target;
	signed short weapon_type;
	signed short weapon_type_target;
	signed short two_w_6;
	signed short l5;
	signed short l6;
	signed short randval1;
	signed short randval2;
	signed short atpa;
	signed short l10;
	signed short l11;
	signed short l12;
	signed short l13;
	signed short attack_hero = 0;
	signed short l15;
	Bit8u *p3;
	signed short width;
	signed short height;
	signed short l16 = 0;
	signed short l17 = 0;
	signed short fighter_id;
	struct dummy dst = *(struct dummy*)(p_datseg + VIEWDIR_INVOFFSETS2);
	signed short hero_x;
	signed short hero_y;
	signed short target_x;
	signed short target_y;
	signed short dir;
	Bit8u *p4;
	struct msg tmp;
	signed short fg_bak;
	signed short bg_bak;
	struct nvf_desc nvf;

	update_mouse_cursor();

	if (ds_readws(IN_FIGHT) != 0) {

		FIG_clear_msgs();

		l5 = ds_writews(ATTACKER_ATTACKS_AGAIN, ds_writews(DEFENDER_ATTACKS, ds_writews(SPELL_ILLUSIONEN, ds_writews(ATTACKER_DEAD, ds_writews(DEFENDER_DEAD, 0)))));

		weapon_type = weapon_type_target = -1;

		ds_writew(FIG_ACTOR_GRAMMAR_TYPE, 2);
		ds_writew(FIG_ACTOR_GRAMMAR_ID, hero_pos);

		if (host_readbs(Real2Host(hero) + HERO_ENEMY_ID) >= 10) {

			/* attack monster */
			if (host_readbs(Real2Host(hero) + HERO_ENEMY_ID) >= 30) {
				sub_ptr_bs(Real2Host(hero) + HERO_ENEMY_ID, 20);
				l16 = 1;
				l17 = 1;
			}

			target_monster = p_datseg + (ENEMY_SHEETS - 10*SIZEOF_ENEMY_SHEET) + SIZEOF_ENEMY_SHEET * host_readbs(Real2Host(hero) + HERO_ENEMY_ID);

			and_ptr_bs(target_monster + ENEMY_SHEET_STATUS1, 0xfd);

			ds_writew(FIG_TARGET_GRAMMAR_TYPE, 1);
			ds_writew(FIG_TARGET_GRAMMAR_ID, host_readbs(target_monster));

			if (!host_readbs(target_monster) ||
				(enemy_dead(target_monster)
				    && ((host_readbs(Real2Host(hero) + HERO_ACTION_ID) != FIG_ACTION_SPELL)
				        || (host_readbs(Real2Host(hero) + HERO_SPELL_ID) != 23))))
			{
				refresh_screen_size();
				return;
			}

			if ((is_in_byte_array(host_readbs(target_monster + 1), p_datseg + TWO_FIELDED_SPRITE_ID)) &&
				(l16 == 0))
			{
				FIG_search_obj_on_cb(host_readbs(Real2Host(hero) + HERO_ENEMY_ID), &target_x, &target_y);
				FIG_search_obj_on_cb(hero_pos + 1, &hero_x, &hero_y);

#if !defined(__BORLANDC__)
				/* BE-fix */
				target_x = host_readws((Bit8u*)&target_x);
				target_y = host_readws((Bit8u*)&target_y);
				hero_x = host_readws((Bit8u*)&hero_x);
				hero_y = host_readws((Bit8u*)&hero_y);
#endif
				if (hero_x == target_x) {

					if (target_y < hero_y) {
						dir = 3;
					} else {
						dir = 1;
					}
				} else {
					if (target_x < hero_x) {
						dir = 0;
					} else {
						dir = 2;
					}
				}

				if (host_readbs(target_monster + ENEMY_SHEET_VIEWDIR) != dir) {

					fighter_id = get_cb_val(hero_x + dst.a[dir].x, hero_y + dst.a[dir].y);

					if (fighter_id != 0) {

						if ((fighter_id >= 50) ||
							((fighter_id < 10) && !hero_dead(get_hero(fighter_id - 1))) ||
							((fighter_id >= 10) && (fighter_id < 30) && !enemy_dead((p_datseg + (ENEMY_SHEETS - 10*SIZEOF_ENEMY_SHEET)) + (SIZEOF_ENEMY_SHEET * fighter_id))) ||
							((fighter_id >= 30) && (fighter_id < 50) && !enemy_dead((p_datseg + (ENEMY_SHEETS - 30*SIZEOF_ENEMY_SHEET)) + (SIZEOF_ENEMY_SHEET * fighter_id))))
						{
							l16 = 1;
						}
					}
				}
			}
		} else {
			if (host_readbs(Real2Host(hero) + HERO_ENEMY_ID) > 0) {

				/* hero attacks another hero */
				target_hero = get_hero(host_readbs(Real2Host(hero) + HERO_ENEMY_ID) - 1);

				ds_writew(FIG_TARGET_GRAMMAR_TYPE, 2);
				ds_writew(FIG_TARGET_GRAMMAR_ID, host_readbs(Real2Host(hero) + HERO_ENEMY_ID) - 1);

				if (hero_sleeps(target_hero)) {

					/* wake up target hero */

					and_ptr_bs(target_hero + HERO_STATUS1, 0xfd);

					p4 = Real2Host(FIG_get_ptr(host_readbs(target_hero + HERO_FIGHTER_ID)));

					host_writeb(p4 + 0x02, host_readbs(target_hero + HERO_VIEWDIR));
					host_writeb(p4 + 0x0d, -1);
					host_writeb(p4 + 0x05, 0);
					host_writeb(p4 + 0x06, 0);
				}

				if (hero_dead(target_hero) || !host_readbs(target_hero + HERO_TYPE)) {
					refresh_screen_size();
					return;
				}

				attack_hero = 1;
			}
		}

		if (host_readbs(Real2Host(hero) + HERO_ACTION_ID) == FIG_ACTION_ATTACK) {

			/* attack a hero */

			p_weapon = Real2Host(hero) + HERO_ITEM_RIGHT;

			weapon_type = weapon_check(Real2Host(hero));

			if (attack_hero != 0) {
				p_weapon_target = target_hero + HERO_ITEM_RIGHT;

				weapon_type_target = weapon_check(target_hero);
			}

			if (weapon_type == -1) {
				/* no valid weapon == bare hands */
				atpa = host_readbs(Real2Host(hero) + HERO_AT) + host_readbs(Real2Host(hero) + HERO_ATTACK_TYPE) - host_readbs(Real2Host(hero) + HERO_RS_BE) / 2;
			} else {
				atpa = host_readbs(Real2Host(hero) + HERO_AT + host_readbs(Real2Host(hero) + HERO_WP_CLASS)) + host_readbs(Real2Host(hero) + HERO_ATTACK_TYPE) + host_readbs(Real2Host(hero) + HERO_AT_MOD) - host_readbs(Real2Host(hero) + HERO_RS_BE) / 2;
			}

			if (host_readbs(Real2Host(hero) + HERO_RS_BE) & 1) {
				atpa--;
			}

			/* after destroying the orc statuette between Oberorken and Felsteyn, dwarfs get a PA-bonus against orcs */
			if ((ds_readbs(TEVENT071_ORCSTATUE) != 0) &&
				(host_readbs(Real2Host(hero) + HERO_TYPE) == HERO_TYPE_DWARF) &&
				(attack_hero == 0) &&
				(host_readbs(target_monster + ENEMY_SHEET_GFX_ID) == 24))
			{
				atpa++;
			}

			/* spell_dunkelheit is active */
			if (ds_readd(INGAME_TIMERS + 0x24)) {
				atpa -= 4;
			}

			if (attack_hero != 0) {

				if (weapon_type_target == -1) {
					l11 = host_readbs(target_hero + HERO_AT) + host_readbs(target_hero + HERO_ATTACK_TYPE) - host_readbs(Real2Host(hero) + HERO_RS_BE) / 2;
				} else {
					l11 = host_readbs(target_hero + HERO_AT + host_readbs(target_hero + HERO_WP_CLASS)) + host_readbs(target_hero + HERO_ATTACK_TYPE) - host_readbs(Real2Host(hero) + HERO_RS_BE) / 2;
				}

				if (host_readbs(Real2Host(hero) + HERO_RS_BE) & 1) {
					l11--;
				}

				/* spell_dunkelheit is active */
				if (ds_readd(INGAME_TIMERS + 0x24)) {
					l11 -= 4;
				}

				if (ds_readbs((HERO_IS_TARGET-1) + host_readbs(Real2Host(hero) + HERO_ENEMY_ID)) == 1) {
					atpa += 2;
				}

			} else {
				if ((ds_readbs(0xd82d + host_readbs(Real2Host(hero) + HERO_ENEMY_ID)) == 1) || (l16 != 0)) {
					atpa += 2;
				}
			}

			randval1 = random_schick(20);

			if ((randval1 == 20) && (random_schick(20) > atpa - 9)) {

				damage = 0;
				FIG_add_msg(1, 0);

				two_w_6 = random_interval(2, 12);

				if ((two_w_6 == 2) && (weapon_type != -1) &&
					(host_readbs(p_weapon + 0x06) != (signed char)0x9d))
				{
					or_ptr_bs(p_weapon + 0x4 , 1);
					FIG_add_msg(6, 0);

				} else if ((two_w_6 >= 3) && (two_w_6 <= 8) && (l16 == 0)) {

					ds_writew(DEFENDER_ATTACKS, 1);

					if (attack_hero != 0) {

						if (random_schick(20) <= l11) {
							damage = FIG_get_hero_melee_attack_damage(target_hero, Real2Host(hero), 1);
						}

					} else {

						if (random_schick(20) <= host_readbs(target_monster + ENEMY_SHEET_AT)) {

							damage = FIG_get_enemy_attack_damage(target_monster, Real2Host(hero), 0);
						}
					}

					if (damage > 0) {

						sub_hero_le(Real2Host(hero), damage);

						FIG_add_msg(8, damage);

						/* swap msg struct */
						tmp = *(struct msg*)(p_datseg + FIG_TARGET_GRAMMAR_TYPE);
						*(struct msg*)(p_datseg + FIG_TARGET_GRAMMAR_TYPE) = *(struct msg*)(p_datseg + FIG_ACTOR_GRAMMAR_TYPE);
						*(struct msg*)(p_datseg + FIG_ACTOR_GRAMMAR_TYPE) = tmp;

					}

					if (hero_dead(Real2Host(hero))) {
						ds_writew(ATTACKER_DEAD, 1);
					}

				} else if ((two_w_6 >= 9) && (two_w_6 <= 11)) {

					damage = random_schick(6);

					sub_hero_le(Real2Host(hero), damage);

					FIG_add_msg(8, damage);

					*(struct msg*)(p_datseg + FIG_TARGET_GRAMMAR_TYPE) = *(struct msg*)(p_datseg + FIG_ACTOR_GRAMMAR_TYPE);

					if (hero_dead(Real2Host(hero))) {
						ds_writew(ATTACKER_DEAD, 1);
					}
				}

			} else if (randval1 <= atpa) {

				if (((attack_hero == 0) && !ds_readbs(0xd82d + host_readbs(Real2Host(hero) + HERO_ENEMY_ID)) && (l16 == 0)) ||
					((attack_hero != 0) && !ds_readbs((HERO_IS_TARGET-1) + host_readbs(Real2Host(hero) + HERO_ENEMY_ID))))
				{

					if (attack_hero != 0) {

						if (weapon_type_target == -1) {
							l10 = host_readbs(target_hero + HERO_PA) - host_readbs(target_hero + HERO_ATTACK_TYPE) - host_readbs(target_hero + HERO_RS_BE) / 2;
						} else {
							l10 = host_readbs(target_hero + HERO_PA + host_readbs(target_hero + HERO_WP_CLASS)) -host_readbs(target_hero + HERO_ATTACK_TYPE) - host_readbs(target_hero + HERO_RS_BE) / 2 + host_readbs(target_hero + HERO_AT_MOD);
						}

						if (host_readbs(target_hero + HERO_ACTION_ID) == FIG_ACTION_GUARD) {
							l10 += 3;
						}
					} else {

						l10 = host_readbs(target_monster + ENEMY_SHEET_PA);

						if (host_readbs(target_monster + ENEMY_SHEET_BLIND) != 0) {
							l10 -= 5;
						}

						if (test_bit5(target_monster + ENEMY_SHEET_STATUS1)) {
							l10 -= 2;
						} else if (test_bit3(target_monster + ENEMY_SHEET_STATUS2)) {
							l10 -= 3;
						}

						if (test_bit2(target_monster + ENEMY_SHEET_STATUS1) ||
							test_bit3(target_monster + ENEMY_SHEET_STATUS1) ||
							test_bit6(target_monster + ENEMY_SHEET_STATUS1) ||
							test_bit0(target_monster + ENEMY_SHEET_STATUS2) ||
							test_bit1(target_monster + ENEMY_SHEET_STATUS2))
						{
							l10 = 0;
						}
					}

					/* spell_dunkelheit is active */
					if (ds_readd(INGAME_TIMERS + 0x24)) {
						l10 -= 4;
					}

					if (l10 < 0) {
						l10 = 0;
					}

					randval2 = random_schick(20);

					if ((randval2 == 20) && (random_schick(20) > l10 - 7)) {

						damage = 0;

						FIG_add_msg(4, 0);

						two_w_6 = random_interval(2, 12);

						if ((two_w_6 >= 3) && (two_w_6 <= 8)) {

							ds_writew(DEFENDER_ATTACKS, 1);

							if (random_schick(20) <= atpa) {

								if (attack_hero != 0) {

									damage = FIG_get_hero_melee_attack_damage(Real2Host(hero), target_hero, 1);

									if (damage > 0) {

										sub_hero_le(target_hero, damage);

										FIG_add_msg(8, damage);

										if (hero_dead(target_hero)) {
											ds_writew(DEFENDER_DEAD, 1);
										}
									}
								} else {

									damage = FIG_get_hero_melee_attack_damage(Real2Host(hero), target_monster, 0);

									if (damage > 0) {

										FIG_damage_enemy(target_monster, damage, 0);

										FIG_add_msg(11, damage);

										if (enemy_dead(target_monster)) {
											ds_writew(DEFENDER_DEAD, 1);
										}
									}
								}
							}
						} else if ((two_w_6 >= 9) && (two_w_6 <= 11)) {

							damage = random_schick(6);

							if (attack_hero != 0) {

								sub_hero_le(target_hero, damage);

								FIG_add_msg(8, damage);

								if (hero_dead(target_hero)) {
									ds_writew(DEFENDER_DEAD, 1);
								}
							} else {
								FIG_damage_enemy(target_monster, damage, 1);

								FIG_add_msg(11, damage);

								if (enemy_dead(target_monster)) {
									ds_writew(DEFENDER_DEAD, 1);
								}
							}
						}
					} else {

						if (randval2 > l10) {
							l5 = 1;
						} else {
							FIG_add_msg(5, 0);

							if ((randval2 == randval1) && (host_readbs(p_weapon + 0x06) != (signed char)0x9d)) {

								if (host_readbs(p_weapon + 0x06) > 3) {

									if (random_schick(12) + host_readbs(p_weapon + 0x06) > 15) {

										if ((attack_hero != 0) && (host_readbs(p_weapon_target + 0x06) != (signed char)0x9d))
										{
											or_ptr_bs(p_weapon_target + 0x04, 1);
										}

										or_ptr_bs(p_weapon + 0x04, 1);

										FIG_add_msg(6, 0);

									} else {
										if (attack_hero != 0) {
											inc_ptr_bs(p_weapon_target + 0x06);
										}

										inc_ptr_bs(p_weapon + 0x06);
									}
								} else {

									if (attack_hero != 0) {
										inc_ptr_bs(p_weapon_target + 0x06);
									}

									inc_ptr_bs(p_weapon + 0x06);
								}
							}
						}
					}
				} else {
					l5 = 1;
				}

				if (l5 != 0) {

					if (attack_hero != 0) {

						damage = FIG_get_hero_melee_attack_damage(Real2Host(hero), target_hero, 1);

						if (damage > 0) {

							sub_hero_le(target_hero, damage);

							FIG_add_msg(8, damage);

							if (hero_dead(target_hero)) {
								ds_writew(DEFENDER_DEAD, 1);
							}
						}
					} else {

						damage = FIG_get_hero_melee_attack_damage(Real2Host(hero), target_monster, 0);

						if (damage > 0 ) {

							FIG_damage_enemy(target_monster, damage, 0);

							FIG_add_msg(11, damage);

							if (enemy_dead(target_monster)) {
								ds_writew(DEFENDER_DEAD, 1);
							}
						}
					}
				}
			}

			seg041_8c8();

			FIG_prepare_hero_fight_ani(0, Real2Host(hero), weapon_type,
							2, hero_pos + 1,
							l17 == 0 ? host_readbs(Real2Host(hero) + HERO_ENEMY_ID) : host_readbs(Real2Host(hero) + HERO_ENEMY_ID) + 20, 0);

			if (attack_hero != 0) {

				if (check_hero(target_hero) || (ds_readws(DEFENDER_DEAD) != 0)) {

					FIG_prepare_hero_fight_ani(1, target_hero, weapon_type_target,
								100, host_readbs(Real2Host(hero) + HERO_ENEMY_ID), hero_pos + 1, 1);
				}
			} else {

				if (l16 == 0) {
					FIG_prepare_enemy_fight_ani(1, target_monster, 100, host_readbs(Real2Host(hero) + HERO_ENEMY_ID), hero_pos + 1, 1);
				} else {
					if (ds_readws(DEFENDER_DEAD) != 0) {
						FIG_prepare_enemy_fight_ani(1, target_monster, 0, host_readbs(Real2Host(hero) + HERO_ENEMY_ID), hero_pos + 1, 1);
					}
				}
			}

			ds_writew(FIG_CONTINUE_PRINT, 1);

			draw_fight_screen_pal(0);

			seg041_8c8();

		} else {

			if (host_readbs(Real2Host(hero) + HERO_ACTION_ID) == FIG_ACTION_RANGE_ATTACK) {

				weapon_type = FIG_get_range_weapon_type(Real2Host(hero));

				if (weapon_type != -1) {

					if (!range_attack_check_ammo(Real2Host(hero), 0)) {
						return;
					}


					if (attack_hero != 0) {

						damage = FIG_get_hero_melee_attack_damage(Real2Host(hero), target_hero, 1);

						if (damage > 0) {

							sub_hero_le(target_hero, damage);

							FIG_add_msg(8, damage);

							if (hero_dead(target_hero)) {
								ds_writew(DEFENDER_DEAD, 1);
							}
						}
					} else {

						damage = FIG_get_hero_melee_attack_damage(Real2Host(hero), target_monster, 0);

						if (damage > 0 ) {

							FIG_damage_enemy(target_monster, damage, 0);

							FIG_add_msg(11, damage);

							if (enemy_dead(target_monster)) {
								ds_writew(DEFENDER_DEAD, 1);
							}
						}
					}

					seg041_8c8();

					l12 = weapon_type;
					l13 = 0;

					FIG_call_draw_pic();

					FIG_prepare_hero_fight_ani(0, Real2Host(hero), weapon_type,
							15, hero_pos + 1,
							l17 == 0 ? host_readbs(Real2Host(hero) + HERO_ENEMY_ID) : host_readbs(Real2Host(hero) + HERO_ENEMY_ID) + 20, 0);

					l13 = seg045_01a0(7, l12, hero_pos + 1,
						l17 == 0 ? host_readbs(Real2Host(hero) + HERO_ENEMY_ID) : host_readbs(Real2Host(hero) + HERO_ENEMY_ID) + 20,
						host_readbs(Real2Host(hero) + HERO_VIEWDIR));

					FIG_set_0e(host_readbs(Real2Host(hero) + HERO_FIGHTER_ID), 0);

					draw_fight_screen_pal(0);

					if (FIG_get_range_weapon_type(Real2Host(hero)) == -1) {

						p4 = Real2Host(FIG_get_ptr(host_readbs(Real2Host(hero) + HERO_FIGHTER_ID)));

						host_writeb(p4 + 0x02, host_readbs(Real2Host(hero) + HERO_VIEWDIR));
						host_writeb(p4 + 0x0d, -1);
					}

					if (l13 != 0) {

						FIG_set_0e(ds_readbs(FIG_SHOT_BOLT_ID), 7);

						draw_fight_screen(l13 == 0 && ds_readws(DEFENDER_DEAD) == 0 ? 0 : 1);

						FIG_make_invisible(ds_readbs(FIG_SHOT_BOLT_ID));
					}

					ds_writew(FIG_CONTINUE_PRINT, 1);

					if (ds_readws(DEFENDER_DEAD) != 0) {

						if (attack_hero != 0) {
							FIG_prepare_hero_fight_ani(1, target_hero, -1, 0,
								host_readbs(Real2Host(hero) + HERO_ENEMY_ID), hero_pos + 1, 1);
						} else {
							FIG_prepare_enemy_fight_ani(1, target_monster, 0,
								host_readbs(Real2Host(hero) + HERO_ENEMY_ID), hero_pos + 1, 1);
						}
					}

					FIG_remove_smth();

					draw_fight_screen(0);

					seg041_8c8();
				}

			} else if (host_readbs(Real2Host(hero) + HERO_ACTION_ID) == FIG_ACTION_SPELL) {

				/* cast a spell */

				if (ds_readws(CURRENT_FIG_NO) == 192) {
					/* no spells allowed in the final fight */

					sub_hero_le(Real2Host(hero), host_readws(Real2Host(hero) + HERO_LE) + 1);

					ds_writew(ATTACKER_DEAD, 1);
				}

				l6 = ds_readbs((SPELL_DESCRIPTIONS + 6) + 10 * host_readbs(Real2Host(hero) + HERO_SPELL_ID));

				host_writeb(Real2Host(ds_readd(DTP2)), 0);

				l15 = use_spell(hero, 0, 0);

				seg041_8c8();

				if (ds_readw(AUTOFIGHT) != 0) {
					ds_writew(PIC_COPY_X1, ds_writew(PIC_COPY_V1, 0));
					ds_writew(PIC_COPY_Y1, ds_writew(PIC_COPY_V2, 194));
					ds_writew(PIC_COPY_X2, 318);
					ds_writew(PIC_COPY_Y2, 199);
					ds_writed(PIC_COPY_SRC, ds_readd(BUFFER8_PTR));

					do_pic_copy(3);

					get_textcolor(&fg_bak, &bg_bak);
					set_textcolor(0xff, 0x00);

					sprintf((char*)Real2Host(ds_readd(TEXT_OUTPUT_BUF)),
						(char*)p_datseg + STRING_CASTS_SPELL,		/* "%s ZAUBERT %s" */
						(char*)Real2Host(hero) + HERO_NAME2,
						(char*)get_ttx(host_readbs(Real2Host(hero) + HERO_SPELL_ID) + 0x6a));

					GUI_print_string(Real2Host(ds_readd(TEXT_OUTPUT_BUF)), 1, 194);

					set_textcolor(fg_bak, bg_bak);

					delay_or_keypress(50);
				}

				if (host_readbs(Real2Host(hero) + HERO_ENEMY_ID) != 0) {

					l12 = l13 = 0;

					if (random_schick(100) > 50) {
						l12 = 1;
					}

					if (host_readbs(Real2Host(hero) + HERO_ENEMY_ID) < 10) {
						l12 = 2;
					}

					FIG_call_draw_pic();

					if (l15 != -1) {

						seg044_002a(0, Real2Host(hero), 4, hero_pos + 1,
								l17 == 0 ? host_readbs(Real2Host(hero) + HERO_ENEMY_ID) : host_readbs(Real2Host(hero) + HERO_ENEMY_ID) + 20, l12, 0);
					}

					if (l15 > 0) {

						if (l6 > 0) {

							if (l6 != 4) {
								seg045_0394(6, Real2Host(hero), l6);
							}

						} else {

							if (host_readbs(Real2Host(hero) + HERO_ENEMY_ID) > 0) {

								if (attack_hero == 0) {

									seg044_002f(1, target_monster, 99,
											l17 == 0 ? host_readbs(Real2Host(hero) + HERO_ENEMY_ID) : host_readbs(Real2Host(hero) + HERO_ENEMY_ID) + 20,
											hero_pos + 1, 1);
								} else {

									if (check_hero(target_hero) || (ds_readws(DEFENDER_DEAD) != 0)) {

										seg044_002a(1, target_hero, 99, host_readbs(Real2Host(hero) + HERO_ENEMY_ID), 0 , -1, 1);
									}
								}
							}
						}

						if ((host_readbs(Real2Host(hero) + HERO_SPRITE_NO) != 7) &&
							(host_readbs(Real2Host(hero) + HERO_SPRITE_NO) != 18) &&
							(host_readbs(Real2Host(hero) + HERO_ENEMY_ID) > 0 ))
						{

							l13 = seg045_01a0(7, l12, hero_pos + 1,
								l17 == 0 ? host_readbs(Real2Host(hero) + HERO_ENEMY_ID) : host_readbs(Real2Host(hero) + HERO_ENEMY_ID) + 20,
								host_readbs(Real2Host(hero) + HERO_VIEWDIR));
						}
					}

					if (l15 != -1) {

						FIG_set_0e(host_readbs(Real2Host(hero) + HERO_FIGHTER_ID), 0);
						draw_fight_screen_pal(1);
					}

					if (l15 > 0) {

						if (l13 != 0) {

							FIG_set_0e(ds_readbs(FIG_SHOT_BOLT_ID), 7);

							draw_fight_screen(1);

							FIG_make_invisible(ds_readbs(FIG_SHOT_BOLT_ID));
						}

						if (l6 > 0) {

							if (l6 != 4) {
								FIG_set_0e(ds_readbs(FIG_SPELLGFX_ID), 6);
							} else {

								FIG_call_draw_pic();

								FIG_remove_from_list(host_readbs(target_monster + ENEMY_SHEET_FIGHTER_ID), 1);


								nvf.dst = Real2Host(ds_readd((FIG_LIST_ELEM + FIGHTER_GFXBUF)));
								nvf.src = Real2Host(ds_readd(SPELLOBJ_NVF_BUF));
								nvf.no = 26;
								nvf.type = 0;
								nvf.width =(Bit8u*) &width;
								nvf.height = (Bit8u*)&height;

								process_nvf(&nvf);
#if !defined(__BORLANDC__)
								/* BE-fix */
								width = host_readws((Bit8u*)&width);
								height = host_readws((Bit8u*)&height);
#endif

								ds_writeb((FIG_LIST_ELEM + FIGHTER_OFFSETX), 0);
								ds_writeb((FIG_LIST_ELEM + FIGHTER_OFFSETY), 0);
								ds_writeb((FIG_LIST_ELEM + FIGHTER_HEIGHT), (signed char)height);
								ds_writeb((FIG_LIST_ELEM + FIGHTER_WIDTH), (signed char)width);
								ds_writeb((FIG_LIST_ELEM + FIGHTER_X1), 0);
								ds_writeb((FIG_LIST_ELEM + FIGHTER_Y1), 0);
								ds_writeb((FIG_LIST_ELEM + FIGHTER_X2), (signed char)(width - 1));
								ds_writeb((FIG_LIST_ELEM + FIGHTER_Y2), (signed char)(height - 1));
								ds_writeb((FIG_LIST_ELEM + FIGHTER_RELOAD), 0);

								FIG_add_to_list(host_readbs(target_monster + ENEMY_SHEET_FIGHTER_ID));

							}
						} else {

							if (attack_hero == 0) {

								FIG_set_0e(host_readbs(target_monster + ENEMY_SHEET_FIGHTER_ID), 1);

								if (is_in_byte_array(host_readbs(target_monster + 1), p_datseg + TWO_FIELDED_SPRITE_ID))
								{
									p3 = Real2Host(FIG_get_ptr(host_readbs(target_monster + ENEMY_SHEET_FIGHTER_ID)));

									FIG_set_0e(ds_readbs(FIG_TWOFIELDED_TABLE + host_readbs(p3 + 0x13)), 3);
								}

							} else {

								if (host_readbs(Real2Host(hero) + HERO_ENEMY_ID) > 0) {

									FIG_set_0e(host_readbs(target_hero + HERO_FIGHTER_ID), 1);
								}
							}
						}

						ds_writew(FIG_CONTINUE_PRINT, 1);

						draw_fight_screen(1);

						if (l6 > 0) {
							FIG_make_invisible(ds_readbs(FIG_SHOT_BOLT_ID));
						}

						if (ds_readws(SPELL_ILLUSIONEN) != 0) {

							if (host_readbs(Real2Host(hero) + HERO_ENEMY_ID) >= 10) {

								FIG_make_invisible(host_readbs(target_monster + ENEMY_SHEET_FIGHTER_ID));

								if (is_in_byte_array(host_readbs(target_monster + 1), p_datseg + TWO_FIELDED_SPRITE_ID))
								{
									p3 = Real2Host(FIG_get_ptr(host_readbs(target_monster + ENEMY_SHEET_FIGHTER_ID)));

									FIG_make_invisible(ds_readbs(FIG_TWOFIELDED_TABLE + host_readbs(p3 + 0x13)));
								}
							} else {
								if (host_readbs(Real2Host(hero) + HERO_ENEMY_ID) > 0) {

									FIG_make_invisible(host_readbs(target_hero + HERO_FIGHTER_ID));
								}
							}
						}

						if (l13 != 0) {
							FIG_remove_smth();
						}

						if ((l6 > 0) && (l6 != 3) && (l6 != 4)) {
							FIG_remove_smth2();
						}

						FIG_draw_figures();
					}

					FIG_output(Real2Host(ds_readd(DTP2)));

					seg041_8c8();

				} else {
					FIG_output(Real2Host(ds_readd(DTP2)));
				}

			} else if (host_readbs(Real2Host(hero) + HERO_ACTION_ID) == FIG_ACTION_USE_ITEM) {

				FIG_use_item(Real2Host(hero), target_monster, target_hero, attack_hero, hero_pos);

			}
		}
	}

	refresh_screen_size();

	if (l17 != 0) {
		add_ptr_bs(Real2Host(hero) + HERO_ENEMY_ID, 20);
	}
}

#if !defined(__BORLANDC__)
}
#endif
