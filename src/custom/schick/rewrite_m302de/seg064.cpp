/*
 *	Rewrite of DSA1 v3.02_de functions of seg064 (harbour_helper)
 *	Functions rewritten: 6/6 (complete)
 *
 *	Borlandified and identical
 *	Compiler:	Borland C++ 3.1
 *	Call:		BCC.EXE -mlarge -O- -c -1 -Yo seg064.cpp
 */

#include <stdio.h>
#include <string.h>

#include "v302de.h"

#include "seg007.h"
#include "seg028.h"

#if !defined(__BORLANDC__)
namespace M302de {
#endif

/**
 * \brief   returns a pointer to the name of a ship
 *
 * \param   ship_type   eg 0 = longship, 5 = coastal ship, 6 = cutter,7 = fishing boat
 * \param   arg2        ???
 */
RealPt get_ship_name(signed char ship_type, signed short arg2)
{
	signed char done, i;
	signed short name;

	done = 0;

	do {
		name = ship_type * 10 + random_schick(10) + 0x29;
		done = 1;
		for (i = 0; i < arg2; i++) {
			if (ds_readd(SEA_TRAVEL_MENU_PASSAGES + i * 12)
				== host_readd(Real2Host(ds_readd(TX_INDEX)) + name * 4)) {
				done = 0;
				break;
			}
		}

	} while (!done);

	return (RealPt)host_readd(Real2Host(ds_readd(TX_INDEX)) + name * 4);
}

/**
 * \brief   */
unsigned short prepare_passages(void)
{
	signed short prepared;
	signed short i;
	RealPt ent;

#if !defined(__BORLANDC__)
	ent = RealMake(datseg, SEA_TRAVEL_PASSAGES);
#else
	ent = p_datseg + SEA_TRAVEL_PASSAGES;
#endif

	for (i = prepared = 0; i < 45; ent += 8, i++) {
		if (!host_readbs(Real2Host(ent) + 4) &&
			(host_readb(Real2Host(ent)) == ds_readb(CURRENT_TOWN) ||
			(host_readb(Real2Host(ent) + 1) == ds_readb(CURRENT_TOWN)))) {

			/* prepare an entry of 12 byte for a passage today */
			ds_writeb((SEA_TRAVEL_MENU_PASSAGES + 11) + prepared * 12, (unsigned char)i);
			ds_writed((SEA_TRAVEL_MENU_PASSAGES + 4) + prepared * 12, (Bit32u)ent);
			ds_writeb((SEA_TRAVEL_MENU_PASSAGES + 8) + prepared * 12, 0);
			ds_writeb((SEA_TRAVEL_MENU_PASSAGES + 9) + prepared * 12, host_readb(Real2Host(ent) + 6));
			ds_writed(SEA_TRAVEL_MENU_PASSAGES + prepared * 12,
				(Bit32u)get_ship_name(host_readb(Real2Host(ent) + 6), prepared));

			ds_writeb((SEA_TRAVEL_MENU_PASSAGES + 10) + prepared * 12,
				host_readb(Real2Host(ent)) == ds_readb(CURRENT_TOWN) ?
					host_readb(Real2Host(ent) + 1) :
					host_readb(Real2Host(ent)));
			prepared++;
		} else {
			/* not before 14.00 o'clock */
			if (((signed long)ds_readd(DAY_TIMER) > (0x1518 * 14L))
				/* only for ships tomorrow */
				&& (host_readb(Real2Host(ent) + 4) == 1)
				/* only in this city */
				&& ((host_readb(Real2Host(ent)) == ds_readb(CURRENT_TOWN))
				|| (host_readb(Real2Host(ent) + 1) == ds_readb(CURRENT_TOWN))))
			{
				/* prepare an entry of 12 byte for a passage tomorrow */
				ds_writeb((SEA_TRAVEL_MENU_PASSAGES + 11) + prepared * 12, (unsigned char)i);
				ds_writed((SEA_TRAVEL_MENU_PASSAGES + 4) + prepared * 12, (Bit32u)ent);
				ds_writeb((SEA_TRAVEL_MENU_PASSAGES + 8) + prepared * 12, 1);
				ds_writeb((SEA_TRAVEL_MENU_PASSAGES + 9) + prepared * 12, host_readb(Real2Host(ent) + 6));
				ds_writed(SEA_TRAVEL_MENU_PASSAGES + prepared * 12,
					(Bit32u)get_ship_name(host_readb(Real2Host(ent) + 6), prepared));
				ds_writeb((SEA_TRAVEL_MENU_PASSAGES + 10) + prepared * 12 ,
					host_readb(Real2Host(ent)) == ds_readb(CURRENT_TOWN) ?
						host_readb(Real2Host(ent) + 1) :
						host_readb(Real2Host(ent)));
				prepared++;
			}
		}
	}
	return prepared;
}

/**
 * \brief   calculates the price and prints to buffer
 *
 * \param   price       a price factor
 * \param   entry       pointer to the schedule
 * \return              a pointer to the buffer.
 */
RealPt print_passage_price(signed short price, Bit8u *entry)
{
	unsigned short di;

	if (price != 0) {
		/* calc price per distance */
		di = ((unsigned char)host_readb(entry + 7) * price + 4) / 10;
		/* multiply with distance */
		di = di * (unsigned char)host_readb(entry + 2);
		/* round up and divide by 100 */
		price = (di + 49) / 100;

		/* generate a price string "%d^HELLER" */
		sprintf((char*)Real2Host(ds_readd(TEXT_OUTPUT_BUF)),
			(char*)p_datseg + SEA_TRAVEL_STR_HELLER, price);
	} else {
		/* "NICHTS" */
		strcpy((char*)Real2Host(ds_readd(TEXT_OUTPUT_BUF)),
			(char*)p_datseg + SEA_TRAVEL_STR_NOTHING);
	}
	ds_writew(SEA_TRAVEL_PASSAGE_PRICE, price);

	return (RealPt)ds_readd(TEXT_OUTPUT_BUF);

}

unsigned short get_passage_travel_hours(signed short arg1, signed short arg2)
{
	Bit32u hours;

	arg2 = (arg2 * 10 + 11) / 24;

	/*	WEATHER1 = random(6)
	 *	WEATHER2 = random(7) */
	ds_writew(SEA_TRAVEL_PASSAGE_UNKN2,
		(arg2 * (ds_readw(WEATHER2) + 6) * (ds_readw(WEATHER1) * 15 + 100) + 499L) / 1000L);

	hours = (ds_readws(SEA_TRAVEL_PASSAGE_UNKN2) + 4) / 10;

	if (hours == 0)
		hours = 1;

	hours = arg1 / hours;

	return (unsigned short)hours;
}

/**
 * \brief   get destination harbours
 *
 * \param   type        1 = passages next days / 2 = all passages
 */
unsigned short get_next_passages(unsigned short type)
{
	Bit8u *entry;
	signed short destinations;
	signed short i;

	entry = p_datseg + SEA_TRAVEL_PASSAGES;

	for (i = destinations = 0; i < 45; entry += 8, i++) {

		if (type == 1) {
			/* check passages in the next two days */
			if (host_readb(entry + 4) == 1 || host_readb(entry + 4) == 2) {
				/* compare town */
				if (host_readb(entry) == ds_readb(CURRENT_TOWN) ||
					host_readb(entry + 1) == ds_readb(CURRENT_TOWN))
				{
#if !defined(__BORLANDC__)
					ds_writeb(SEA_TRAVEL_MENU_PASSAGES + 10 + destinations * 12,
						host_readb(entry) == ds_readb(CURRENT_TOWN) ?
							host_readb(entry + 1):
							host_readb(entry));
#else
					((struct passages*)(p_datseg + SEA_TRAVEL_MENU_PASSAGES))[destinations].town =
						host_readb(entry) == ds_readb(CURRENT_TOWN) ?
							host_readb(entry + 1):
							host_readb(entry);
#endif
					destinations++;
				}
			}
		} else {
			/* compare town */
			if (host_readb(entry) == ds_readb(CURRENT_TOWN) ||
				host_readb(entry + 1) == ds_readb(CURRENT_TOWN))
			{
#if !defined(__BORLANDC__)
				ds_writeb(SEA_TRAVEL_MENU_PASSAGES + 10 + destinations * 12,
					host_readb(entry) == ds_readb(CURRENT_TOWN) ?
						host_readb(entry + 1):
						host_readb(entry));
#else
					((struct passages*)(p_datseg + SEA_TRAVEL_MENU_PASSAGES))[destinations].town =
						host_readb(entry) == ds_readb(CURRENT_TOWN) ?
							host_readb(entry + 1):
							host_readb(entry);
#endif
				destinations++;
			}
		}
	}

	return destinations;
}

unsigned short passage_arrival(void)
{
	signed short tmp;
	Bit8u *harbor_ptr;
	Bit8u *buildings;
	Bit8u *p_sched;
	signed short si;
	signed short harbor_id;

	harbor_id = 0;
	harbor_ptr = p_datseg + HARBORS;

	p_sched = p_datseg + SEA_TRAVEL_PASSAGES + ds_readb(SEA_TRAVEL_PASSAGE_ID) * 8;

	/* write the destination to a global variable (assignement in condition)*/
	if ((ds_writew(TRV_DEST_REACHED, host_readb(p_sched))) == ds_readbs(CURRENT_TOWN))
		ds_writew(TRV_DEST_REACHED, host_readb(p_sched + 1));

	do {
		if (host_readb(harbor_ptr) == ds_readw(TRV_DEST_REACHED)) {
			si = 0;
			do {
				tmp = host_readb(Real2Host(host_readd(harbor_ptr + 2)) + si) - 1;
				if (host_readb(p_datseg + SEA_TRAVEL_PASSAGES + tmp * 8) == ds_readb(CURRENT_TOWN) ||
					host_readb(p_datseg + SEA_TRAVEL_PASSAGES + tmp * 8 + 1) == ds_readb(CURRENT_TOWN)) {
					harbor_id = (unsigned char)host_readb(harbor_ptr + 1);
					break;
				}

				si++;
			} while (host_readb(Real2Host(host_readd(harbor_ptr + 2)) + si) != 0xff);
		}
		/* set pointer to the next structure */
		harbor_ptr += 6;
	} while (harbor_id == 0 && host_readb(harbor_ptr) != 0xff);

	if (harbor_id != 0) {

		/* save the old town in tmp */
		tmp = (signed char)ds_readb(CURRENT_TOWN);
		/* set the new current_town */
		ds_writeb(CURRENT_TOWN, ds_readb(TRV_DEST_REACHED));

		/* load the area  of the new town */
		call_load_area(1);


		/* search for the harbour in the map */
		buildings = p_datseg + LOCATIONS_TAB;
		while ((host_readb(buildings + 2) != 0x0b) ||
				(host_readb(buildings + 3) != harbor_id)) {
			buildings += 6;
		}

		/* set the position of the party */
		si = host_readw(buildings + 4);
		ds_writew(ARRIVAL_X_TARGET, (si >> 8) & 0xff);
		ds_writew(ARRIVAL_Y_TARGET, si & 0x0f);
		ds_writew(ARRIVAL_DIRECTION, (si >> 4) & 0x0f);

		/* restore the old town area / TODO: a bit bogus */
		ds_writeb(CURRENT_TOWN, (unsigned char)tmp);
		call_load_area(1);
	}

	return 0;

}

#if !defined(__BORLANDC__)
}
#endif
