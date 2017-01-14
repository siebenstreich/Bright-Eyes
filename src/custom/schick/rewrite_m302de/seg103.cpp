/*
 *      Rewrite of DSA1 v3.02_de functions of seg103 (skills)
 *      Functions rewritten: 8/8 (complete)
 *
 *	Borlandified and identical
 *	Compiler:	Borland C++ 3.1
 *	Call:		BCC.EXE -mlarge -O- -c -1 -Yo seg103.cpp
 *
 */

#include <stdio.h>
#include <string.h>

#include "v302de.h"

#include "seg002.h"
#include "seg007.h"
#include "seg026.h"
#include "seg047.h"
#include "seg053.h"
#include "seg097.h"
#include "seg104.h"

#if !defined(__BORLANDC__)
namespace M302de {
#endif

struct dummy {
	char a[6];
};

signed short LVL_select_skill(Bit8u *hero, signed short show_values)
{
	signed short i;

	signed short answer;
	signed short l1;
	signed short retval = -1;
	/* string on stack "%s~%d" */
	struct dummy format_str = *(struct dummy*)(p_datseg + SELECT_SKILL_LVLUP);

	if (show_values != 0) {

		strcpy((char*)Real2Host(ds_readd(TEXT_OUTPUT_BUF)), (char*)get_ttx(0x334));

		if (host_readbs(hero + HERO_TA_RISE) > 1) {
			strcat((char*)Real2Host(ds_readd(TEXT_OUTPUT_BUF)), (char*)get_ttx(0x624));
		}

		sprintf((char*)Real2Host(ds_readd(DTP2)),
			(char*)get_ttx(0x330),
			/* sind / ist */
			(host_readbs(hero + HERO_TA_RISE) > 1) ? get_ttx(0x4c4) : get_ttx(0x4c0),
			/* # of tries left */
			host_readbs(hero + HERO_TA_RISE),
			Real2Host(ds_readd(TEXT_OUTPUT_BUF)));
	} else {

		strcpy((char*)Real2Host(ds_readd(DTP2)), (char*)get_ttx(0x360));
	}

	/* ask for the skill category */
	answer = GUI_radio(Real2Host(ds_readd(DTP2)), 7,
				get_ttx(0x190), get_ttx(0x194),
				get_ttx(0x198), get_ttx(0x1a4),
				get_ttx(0x19c), get_ttx(0x1a0),
				get_ttx(0x1a8)) - 1;

	if (answer != -2) {

		l1 = ds_readbs(SKILLS_INDEX + 2 * answer);

		if (show_values != 0) {

			for (i = 0; ds_readbs((SKILLS_INDEX + 1) + 2 * answer) > i; i++) {

				sprintf((char*)Real2Host(ds_readd(DTP2)) + 50 * i,
					format_str.a,
					get_ttx((l1 + i + 48) * 4),
					host_readbs(hero + l1 + i + HERO_TA_FIGHT));

				ds_writed(RADIO_NAME_LIST + 4 * i, (Bit32u)((RealPt)ds_readd(DTP2) + 50 * i));
			}
		} else {

			for (i = 0; ds_readbs((SKILLS_INDEX + 1) + 2 * answer) > i; i++) {
				ds_writed(RADIO_NAME_LIST + 4 * i, (Bit32u)(host_readd(Real2Host(ds_readd(TEXT_LTX_INDEX)) + (l1 + i + 48) * 4)));
			}
		}

		retval = GUI_radio(get_ttx(0x368), ds_readbs((SKILLS_INDEX + 1) + 2 * answer),
				Real2Host(ds_readd(RADIO_NAME_LIST)),
				Real2Host(ds_readd((RADIO_NAME_LIST + 4))),
				Real2Host(ds_readd((RADIO_NAME_LIST + 2 * 4))),
				Real2Host(ds_readd((RADIO_NAME_LIST + 3 * 4))),
				Real2Host(ds_readd((RADIO_NAME_LIST + 4 * 4))),
				Real2Host(ds_readd((RADIO_NAME_LIST + 5 * 4))),
				Real2Host(ds_readd((RADIO_NAME_LIST + 6 * 4))),
				Real2Host(ds_readd((RADIO_NAME_LIST + 7 * 4))),
				Real2Host(ds_readd((RADIO_NAME_LIST + 8 * 4))),
				Real2Host(ds_readd((RADIO_NAME_LIST + 9 * 4))),
				Real2Host(ds_readd((RADIO_NAME_LIST + 10 * 4))),
				Real2Host(ds_readd((RADIO_NAME_LIST + 11 * 4))),
				Real2Host(ds_readd((RADIO_NAME_LIST + 12 * 4))),
				Real2Host(ds_readd((RADIO_NAME_LIST + 13 * 4))),
				Real2Host(ds_readd((RADIO_NAME_LIST + 14 * 4))),
				Real2Host(ds_readd((RADIO_NAME_LIST + 15 * 4))));

		if (retval != -1) {
			retval += l1 - 1;
		}
	} else {
		retval = -2;
	}

	return retval;
}

/**
 * \brief   returns hero which seems best for a skill
 *
 * \param   skill       skill
 */
RealPt get_proper_hero(signed short skill)
{
	signed short i;
	signed short cur;

	signed short max = -1;
	RealPt hero_i;
	RealPt retval;

#if !defined(__BORLANDC__)
	retval = 0;
#endif

	hero_i = (RealPt)ds_readd(HEROS);

	for (i = 0; i <= 6; i++, hero_i += SIZEOF_HERO) {
		if ((host_readbs(Real2Host(hero_i) + HERO_TYPE) != HERO_TYPE_NONE) &&
			/* Check if in current group */
			(host_readb(Real2Host(hero_i) + HERO_GROUP_NO) == ds_readb(CURRENT_GROUP)) &&
			/* Check hero is not dead */
			!hero_dead(Real2Host(hero_i))) {

			/* add current and maximum attibute values */
			cur =	host_readbs(Real2Host(hero_i) + HERO_ATTRIB + 3 * ds_readbs(SKILL_DESCRIPTIONS + 4 * skill)) +
				host_readbs(Real2Host(hero_i) + HERO_ATTRIB_MOD + 3 * ds_readbs(SKILL_DESCRIPTIONS + 4 * skill)) +
				host_readbs(Real2Host(hero_i) + HERO_ATTRIB + 3 * ds_readbs((SKILL_DESCRIPTIONS + 1) + 4 * skill)) +
				host_readbs(Real2Host(hero_i) + HERO_ATTRIB_MOD + 3 * ds_readbs((SKILL_DESCRIPTIONS + 1) + 4 * skill)) +
				host_readbs(Real2Host(hero_i) + HERO_ATTRIB + 3 * ds_readbs((SKILL_DESCRIPTIONS + 2) + 4 * skill)) +
				host_readbs(Real2Host(hero_i) + HERO_ATTRIB_MOD + 3 * ds_readbs((SKILL_DESCRIPTIONS + 2) + 4 * skill)) +
				host_readbs(Real2Host(hero_i) + HERO_TA_FIGHT + skill);

			if (cur > max) {

				/* if the sum is greater than the current best */
				max = cur;
				retval = hero_i;
			}
		}
	}

#if !defined(__BORLANDC__)
	/* sanity check for Original Bug hunting */
	if (retval == 0)
		D1_ERR("Original-Bug: %s undefinierter Rückgabewert\n",
			__func__);
#endif

	return retval;
}

/**
 * \brief   performs a skill test
 *
 * \param   hero        hero which should be tested
 * \param   skill       the skill to test
 * \param   bonus       the modification
 */
signed short test_skill(Bit8u *hero, signed short skill, signed char bonus)
{
	signed short randval;
	signed short e_skillval;

	/* dont test for weapon skills */
	if ((skill >= 7) && (skill <= 51)) {

#if !defined(__BORLANDC__)
		D1_INFO("Talentprobe %s %+d: ", names_skill[skill], bonus);
#endif

		/* special test if skill is a range weapon skill */
		if ((skill == 7) || (skill == 8)) {

			/* calculate range weapon base value */
			e_skillval = (host_readbs(hero + (HERO_ATTRIB + 3 * ATTRIB_KL)) + host_readbs(hero + (HERO_ATTRIB_MOD + 3 * ATTRIB_KL)) +
				host_readbs(hero + (HERO_ATTRIB + 3 * ATTRIB_GE)) + host_readbs(hero + (HERO_ATTRIB_MOD + 3 * ATTRIB_GE)) +
				host_readbs(hero + (HERO_ATTRIB + 3 * ATTRIB_KK)) + host_readbs(hero + (HERO_ATTRIB_MOD + 3 * ATTRIB_KK))) / 4;

			/* add skill value */
			e_skillval += host_readbs(hero + HERO_TA_FIGHT + skill);
			/* sub handycap */
			e_skillval -= bonus;

			randval = random_schick(20);

			/* Unlucky */
			if (randval == 20) {
#if !defined(__BORLANDC__)
				D1_INFO("Ungluecklich\n");
#endif
				return -1;
			}
			/* Lucky */
			if (randval == 1) {
#if !defined(__BORLANDC__)
				D1_INFO("Gluecklich\n");
#endif
				return 99;
			}
			if (randval <= e_skillval) {
				/* test successful */
#if !defined(__BORLANDC__)
				D1_INFO(" (%d) -> bestanden\n", randval);
#endif
				return e_skillval - randval + 1;
			}

			/* test unsuccessful */
#if !defined(__BORLANDC__)
			D1_INFO(" (%d) -> nicht bestanden\n", randval);
#endif
			return 0;
		}

		/* automatically get hero with best senses in beginner mode */
		if ((skill == 51) && (ds_readws(GAME_MODE) == 1)) {
			hero = Real2Host(get_proper_hero(51));

#if defined(__BORLANDC__)
			/* seems to have been debug stuff with conditional compilation */
			if (0) return 0;
#endif
		}

		/* do the test */
		bonus -= host_readbs(hero + HERO_TA_FIGHT + skill);

		return test_attrib3(hero, ds_readbs(SKILL_DESCRIPTIONS + skill * 4), ds_readbs((SKILL_DESCRIPTIONS + 1) + skill * 4), ds_readbs((SKILL_DESCRIPTIONS + 2) + skill * 4), bonus);

	}

	return 0;
}

struct dummy2 {
	signed char a[6];
};

signed short select_skill(void)
{
	signed short l_si = -1;
	signed short nr_skills = 3;
	/* available skills {44, 45, 46, -1, -1, -1} */
	struct dummy2 a = *(struct dummy2*)(p_datseg + SELECT_SKILL_DEFAULTS);

	/* add skills for special location */
	if (ds_readbs(LOCATION) == LOCATION_TAVERN) {
		a.a[nr_skills] = TA_AKROBATIK;
		nr_skills++;

		if (ds_readws(CHEATSKILL_USABLE) == 0) {
			a.a[nr_skills] = TA_FALSCHSPIEL;
			nr_skills++;
		}

		a.a[nr_skills] = TA_MUSIZIEREN;
		nr_skills++;
	} else if ((ds_readbs(LOCATION) == LOCATION_WILDCAMP) || (ds_readbs(LOCATION) == LOCATION_INN)) {
		a.a[nr_skills] = TA_ALCHIMIE;
		nr_skills++;
	} else if (ds_readbs(LOCATION) == LOCATION_MARKET) {
		a.a[nr_skills] = TA_AKROBATIK;
		nr_skills++;
		a.a[nr_skills] = TA_TASCHENDIEB;
		nr_skills++;
	} else if (ds_readbs(LOCATION) == LOCATION_MERCHANT) {
		a.a[nr_skills] = TA_TASCHENDIEB;
		nr_skills++;
	}

	l_si = GUI_radio(get_ttx(0x368), (signed char)nr_skills,
				get_ttx((a.a[0] + 48) * 4),
				get_ttx((a.a[1] + 48) * 4),
				get_ttx((a.a[2] + 48) * 4),
				get_ttx((a.a[3] + 48) * 4),
				get_ttx((a.a[4] + 48) * 4),
				get_ttx((a.a[5] + 48) * 4),
				get_ttx((a.a[6] + 48) * 4));

	if (l_si != -1) {
		return a.a[l_si - 1];
	}

	return l_si;
}

signed short use_skill(signed short hero_pos, signed char bonus, signed short skill)
{
	signed short l_si;
	signed short l_di;

	signed short patient_pos;
	signed short le;
	Bit8u *hero;
	Bit8u *patient;
	Bit32s money;
	signed short poison;
	signed short tx_file_bak;

	l_si = 1;

	hero = get_hero(hero_pos);

	if (skill != -1) {

		tx_file_bak = ds_readws(TX_FILE_INDEX);

		load_tx(ARCHIVE_FILE_SPELLTXT_LTX);

		switch(skill) {
		case 44 : {
			ds_writeb(HERO_SEL_EXCLUDE, (signed char)hero_pos);

			patient_pos = select_hero_from_group(get_ttx(0x730));

			if (patient_pos != -1) {
				patient = get_hero(patient_pos);
				if (is_hero_healable(patient)) {

					poison = hero_is_poisoned(patient);

					if (poison == 0) {
						/* patient is not poisoned */
						sprintf((char*)Real2Host(ds_readd(DTP2)),
							(char*)get_ttx(0x73c),
							(char*)patient + HERO_NAME2);
						GUI_output(Real2Host(ds_readd(DTP2)));
					} else if (host_readds(patient + HERO_HEAL_TIMER) > 0) {
						/* patient timer is not zero */
						sprintf((char*)Real2Host(ds_readd(DTP2)),
							(char*)get_ttx(0xae4),
							(char*)patient + HERO_NAME2);
						GUI_output(Real2Host(ds_readd(DTP2)));
					} else {
						/* set patient timer */
						host_writed(patient + HERO_HEAL_TIMER, 0x5460);

						if (test_skill(hero, TA_HEILEN_GIFT, bonus) > 0) {

							timewarp(0x708);

							if (test_skill(hero, TA_HEILEN_GIFT, ds_readbs(POISON_PRICES + 2 * poison) + bonus) > 0) {
								/* success */
								sprintf((char*)Real2Host(ds_readd(DTP2)),
									(char*)get_ttx(0xac8),
									(char*)hero + HERO_NAME2,
									(char*)patient + HERO_NAME2);

								GUI_output(Real2Host(ds_readd(DTP2)));

								host_writeb(patient + 0xd7 + 5 * poison, 0);
								host_writeb(patient + 0xd6 + 5 * poison, 1);

								sprintf((char*)Real2Host(ds_readd(DTP2)),
									(char*)get_ttx(0xad0),
									(char*)hero + HERO_NAME2,
									(char*)patient + HERO_NAME2);

								if (GUI_bool(Real2Host(ds_readd(DTP2)))) {

									do {
										le = GUI_input(get_ttx(0xad4), 2);
									} while (le <= 0);

									if ((l_si = test_skill(hero, TA_HEILEN_GIFT, le + bonus)) > 0) {

										sprintf((char*)Real2Host(ds_readd(DTP2)),
											(char*)get_ttx(0xacc),
											(char*)hero + HERO_NAME2,
											(char*)patient + HERO_NAME2,
											le);

										add_hero_le(patient, le);

										GUI_output(Real2Host(ds_readd(DTP2)));
									} else {
										l_di = 3;

										if (host_readws(patient + 0x60) <= l_di) {
											l_di = host_readws(patient + 0x60) - 1;
										}

										sub_hero_le(patient, l_di);

										sprintf((char*)Real2Host(ds_readd(DTP2)),
											(char*)get_ttx(0xad8),
											(char*)patient + HERO_NAME2,
											l_di);

										GUI_output(Real2Host(ds_readd(DTP2)));

										l_si = 0;
									}
								}
							} else {
								/* healing failed */
								sprintf((char*)Real2Host(ds_readd(DTP2)),
									(char*)get_ttx(0xac4),
									(char*)hero + HERO_NAME2,
									(char*)patient + HERO_NAME2);

								GUI_output(Real2Host(ds_readd(DTP2)));
							}
						} else {
							/* recognizing the poison failed */
							sprintf((char*)Real2Host(ds_readd(DTP2)),
								(char*)get_ttx(0xac0),
								(char*)hero + HERO_NAME2,
								(char*)patient + HERO_NAME2);

							GUI_output(Real2Host(ds_readd(DTP2)));
						}
					}
				}
			}
			break;
		}
		case 45 : {
			ds_writeb(HERO_SEL_EXCLUDE, (signed char)hero_pos);

			patient_pos = select_hero_from_group(get_ttx(0x730));

			if (patient_pos != -1) {
				patient = get_hero(patient_pos);

				skill_cure_disease(hero, patient, bonus, 0);
			}
			break;
		}
		case 46 : {
			ds_writeb(HERO_SEL_EXCLUDE, (signed char)hero_pos);

			patient_pos = select_hero_from_group(get_ttx(0x730));

			if (patient_pos != -1) {
				patient = get_hero(patient_pos);
				if (is_hero_healable(patient)) {

					if (host_readws(patient + 0x60) >= host_readws(patient + 0x5e)) {
						/* no need to heal */
						sprintf((char*)Real2Host(ds_readd(DTP2)),
							(char*)get_ttx(0x734),
							(char*)patient + HERO_NAME2);

						GUI_output(Real2Host(ds_readd(DTP2)));
					} else if (host_readds(patient + HERO_HEAL_TIMER) > 0) {
						/* timer is still running */
						sprintf((char*)Real2Host(ds_readd(DTP2)),
							(char*)get_ttx(0xae4),
							(char*)patient + HERO_NAME2);

						GUI_output(Real2Host(ds_readd(DTP2)));

					} else {
						host_writed(patient + HERO_HEAL_TIMER, 0x1fa40L);

						if (test_skill(hero, TA_HEILEN_WUNDEN, bonus) > 0) {
							if (test_skill(hero, TA_HEILEN_WUNDEN, bonus) > 0) {

								l_si = (host_readbs(hero + (HERO_TA_CRAFT+5)) > 1) ? host_readbs(hero + (HERO_TA_CRAFT+5)) : 1;

								add_hero_le(patient, l_si);

								sprintf((char*)Real2Host(ds_readd(DTP2)),
									(char*)get_ttx(0xacc),
									(char*)hero + HERO_NAME2,
									(char*)patient + HERO_NAME2,
									l_si);

								GUI_output(Real2Host(ds_readd(DTP2)));
							} else {
								l_di = 3;

								if (host_readws(patient + 0x60) <= l_di) {
									l_di = host_readws(patient + 0x60) - 1;
								}

								sub_hero_le(patient, l_di);

								sprintf((char*)Real2Host(ds_readd(DTP2)),
									(char*)get_ttx(0xad8),
									(char*)patient + HERO_NAME2,
									l_di);

								GUI_output(Real2Host(ds_readd(DTP2)));

								l_si = 0;

								host_writed(patient + HERO_STAFFSPELL_TIMER, 0x1fa40L);
							}
						} else {

							if (random_schick(20) <= 7) {
								/* infected */
								sprintf((char*)Real2Host(ds_readd(DTP2)),
									(char*)get_ttx(0xaec),
									(char*)hero + HERO_NAME2,
									(char*)patient + HERO_NAME2);

								host_writeb(patient + 0xb3, -1);
								host_writeb(patient + 0xb4, 0);
							} else {
								/* just failed */
								sprintf((char*)Real2Host(ds_readd(DTP2)),
									(char*)get_ttx(0xae8),
									(char*)hero + HERO_NAME2,
									(char*)patient + HERO_NAME2);
							}

							GUI_output(Real2Host(ds_readd(DTP2)));
						}
					}
				}
			}
			break;
		}
		case 9 : {

			if (ds_readds((INGAME_TIMERS + 0x18)) > 0) {

				GUI_output(get_tx(0x88));

			} else {

				if (test_skill(hero, TA_AKROBATIK, bonus) > 0) {

					money = random_interval(10, 200);

					make_valuta_str((char*)Real2Host(ds_readd(TEXT_OUTPUT_BUF)), money);

					sprintf((char*)Real2Host(ds_readd(DTP2)),
						(char*)get_tx(0x8c),
						(char*)hero + HERO_NAME2,
						Real2Host(ds_readd(TEXT_OUTPUT_BUF)));

					GUI_output(Real2Host(ds_readd(DTP2)));

					add_party_money(money);

					ds_writed((INGAME_TIMERS + 0x18), 0xa8c0);
					ds_writew(REQUEST_REFRESH, 1);
				} else {
					GUI_output(get_tx(0x90));

					ds_writed((INGAME_TIMERS + 0x18), 0xa8c0);
					l_si = -1;
				}
			}
			break;
		}
		case 47 : {

			if (ds_readds((INGAME_TIMERS + 0x1c)) > 0) {

				GUI_output(get_tx(0x94));

			} else {

				if (test_skill(hero, TA_MUSIZIEREN, bonus) > 0) {

					money = random_interval(100, 300);

					make_valuta_str((char*)Real2Host(ds_readd(TEXT_OUTPUT_BUF)), money);

					sprintf((char*)Real2Host(ds_readd(DTP2)),
						(char*)get_tx(0x8c),
						(char*)hero + HERO_NAME2,
						Real2Host(ds_readd(TEXT_OUTPUT_BUF)));

					GUI_output(Real2Host(ds_readd(DTP2)));

					add_party_money(money);

					ds_writed((INGAME_TIMERS + 0x1c), 0xa8c0);
					ds_writew(REQUEST_REFRESH, 1);
				} else {
					GUI_output(get_tx(0x90));

					ds_writed((INGAME_TIMERS + 0x1c), 0xa8c0);
					l_si = -1;
				}
			}
			break;
		}
		case 43 : {

			if (test_skill(hero, TA_FALSCHSPIEL, bonus) > 0) {

				money = random_interval(500, 1000);

				make_valuta_str((char*)Real2Host(ds_readd(TEXT_OUTPUT_BUF)), money);

				sprintf((char*)Real2Host(ds_readd(DTP2)),
					(char*)get_tx(0x98),
					Real2Host(ds_readd(TEXT_OUTPUT_BUF)),
					(char*)hero + HERO_NAME2);

				GUI_output(Real2Host(ds_readd(DTP2)));

				add_party_money(money);

				ds_writew(REQUEST_REFRESH, 1);
			} else {
				GUI_output(get_tx(0x9c));

				ds_writeb(TAV_CHEATED_FLAGS + ds_readws(TYPEINDEX), 1);

				l_si = -1;
			}

			break;
		}
		case 49 : {

			if (test_skill(hero, TA_TASCHENDIEB, bonus) > 0) {

				money = random_interval(500, 1000);

				make_valuta_str((char*)Real2Host(ds_readd(TEXT_OUTPUT_BUF)), money);

				sprintf((char*)Real2Host(ds_readd(DTP2)),
					(char*)get_tx(0xa0),
					Real2Host(ds_readd(TEXT_OUTPUT_BUF)),
					(char*)hero + HERO_NAME2);

				GUI_output(Real2Host(ds_readd(DTP2)));

				add_party_money(money);

				ds_writew(REQUEST_REFRESH, 1);
			} else {
				sprintf((char*)Real2Host(ds_readd(DTP2)),
					(char*)get_tx(0xa4),
					(char*)hero + HERO_NAME2);

				GUI_output(Real2Host(ds_readd(DTP2)));

				set_party_money(0);

				ds_writew(REQUEST_REFRESH, 1);

				l_si = -1;
			}

			break;
		}
		case 32 : {
			/* ALCHEMY */
			l_si = plan_alchemy(hero);
			break;
		}
		}

		if ((tx_file_bak != -1) && (tx_file_bak != ARCHIVE_FILE_SPELLTXT_LTX)) {
			load_tx(tx_file_bak);
		}
	}

	return l_si;
}

signed short GUI_use_skill(signed short hero_pos, signed char bonus)
{
	signed short skill;
	Bit8u *hero;

	hero = get_hero(hero_pos);

	if (!check_hero(hero)) {
		return -1;
	}

	skill = select_skill();
	return use_skill(hero_pos, bonus, skill);
}

signed short GUI_use_skill2(signed short bonus, Bit8u *msg)
{
	signed short hero_pos;
	signed short skill;

	skill = select_skill();

	if (skill != -1) {

		ds_writew(SKILLED_HERO_POS, get_skilled_hero_pos(skill));

		hero_pos = select_hero_ok(msg);

		if ((hero_pos != -1) && (hero_busy(get_hero(hero_pos)))) {
			GUI_output(get_ttx(0xb68));
			hero_pos = -1;
		}
		if (hero_pos != -1) {
			return use_skill(hero_pos, (signed char)bonus, skill);
		}
	}

	return 1;
}

/**
 * \brief   does a bargain throw
 *
 * \param   hero        the hero who bargain
 * \param   items       the number of different goods
 * \param   price       the total price
 * \param   percent     how many percent the player wants to get
 * \param   mod_init    initial value for the modificator
 * \return              the result of the throw. A value greater than zero
 *	means success, below or zero means failed.
 */
signed short bargain(Bit8u *hero, signed short items, Bit32s price,
	signed short percent, signed char mod_init)
{

	signed char mod = mod_init;

	/* NPC Harika gets a bonus on bargain */
	if (host_readb(get_hero(6) + HERO_NPC_ID) == NPC_HARIKA) {
		mod -= 2;
	}

	/* the more different items you buy, the easier the bargain */
	mod += (items == 1) ? 2 :
			((items == 2) ?  1 :
			((items < 5)  ?  0 :
			((items < 9)  ? -1 : -2)));

	/* the higher the price, the easier the bargain */
	mod += (price < 100) ? 2 :
			((price < 500)  ?  1 :
			((price < 1000) ?  0 :
			((price < 2000) ? -1 : -2 )));

	/* the lower the percent, the easier the bargain */
	mod += percent / 5 + 1;

	return test_skill(hero, TA_FEILSCHEN, mod);
}

#if !defined(__BORLANDC__)
}
#endif
