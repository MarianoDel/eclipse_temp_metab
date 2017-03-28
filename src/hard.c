/*
 * hard.c
 *
 *  Created on: 28/03/2017
 *      Author: Mariano
 */

#include "hard.h"

/* Externals variables ---------------------------------------------------------*/
extern unsigned short timer_relay;


/* Global variables ------------------------------------------------------------*/
unsigned char relay_state = 0;
unsigned char last_edge;

/* Module Functions ------------------------------------------------------------*/

//Pide conectar el relay
void RelayOn (void)
{
	relay_state = ST_WAIT_ON;
	timer_relay = TT_RELAY;
}

//Pide desconectar el relay
void RelayOff (void)
{
	relay_state = ST_WAIT_OFF;
	timer_relay = TT_RELAY;
}

//chequeo continuo del estado del relay
void UpdateRelay (void)
{
	unsigned char edge = 0;

	if ((!last_edge) && (EDGE_PIN))
	{
		edge = 1;
		last_edge = 1;
	}

	if ((last_edge) && (!EDGE_PIN))
	{
		edge = 1;
		last_edge = 0;
	}

	switch (relay_state)
	{
		case ST_OFF:

			break;

		case ST_WAIT_ON:
			if (edge)
			{
				edge = 0;
				relay_state = ST_DELAYED_ON;
				timer_relay = TT_DELAYED_ON;
			}

			if (!timer_relay)		//agoto el timer y no encontro sincro, pega igual
				relay_state = ST_DELAYED_ON;

			break;

		case ST_DELAYED_ON:
			if (!timer_relay)
			{
				RELAY_ON;
				relay_state = ST_ON;
			}
			break;

		case ST_ON:

			break;

		case ST_WAIT_OFF:
			if (edge)
			{
				edge = 0;
				relay_state = ST_DELAYED_OFF;
				timer_relay = TT_DELAYED_OFF;
			}

			if (!timer_relay)		//agoto el timer y no encontro sincro, despega igual
				relay_state = ST_DELAYED_OFF;

			break;

		case ST_DELAYED_OFF:
			if (!timer_relay)
			{
				RELAY_OFF;
				relay_state = ST_OFF;
			}
			break;

		default:
			RELAY_OFF;
			relay_state = ST_OFF;
			break;
	}
}
