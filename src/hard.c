/*
 * hard.c
 *
 *  Created on: 28/03/2017
 *      Author: Mariano
 */

#include "hard.h"
#include "tim.h"

/* Externals variables ---------------------------------------------------------*/
extern unsigned short timer_relay;


/* Global variables ------------------------------------------------------------*/
unsigned char relay_state = 0;
unsigned char last_edge;

/* Module Functions ------------------------------------------------------------*/

//Pide conectar el relay
void RelayOn (void)
{
#ifdef VER_2_0
	if (!RelayIsOn())
	{
		relay_state = ST_WAIT_ON;
		timer_relay = TT_RELAY;
	}
#endif
#ifdef VER_1_0
	RELAY_ON;
#endif
}

//Pide desconectar el relay
void RelayOff (void)
{
#ifdef VER_2_0
	if (!RelayIsOff())
	{
		relay_state = ST_WAIT_OFF;
		timer_relay = TT_RELAY;
	}
#endif
#ifdef VER_1_0
	RELAY_OFF;
#endif

}

//Revisa el estado del relay
unsigned char RelayIsOn (void)
{
#ifdef VER_2_0
	if ((relay_state == ST_WAIT_ON) ||
			(relay_state == ST_DELAYED_ON) ||
			(relay_state == ST_ON))
		return 1;
	else
		return 0;
#endif
#ifdef VER_1_0
	if (RELAY)
		return 1;
	else
		return 0;
#endif
}

//Revisa el estado del relay
unsigned char RelayIsOff (void)
{
#ifdef VER_2_0
	if ((relay_state == ST_WAIT_OFF) ||
			(relay_state == ST_DELAYED_OFF) ||
			(relay_state == ST_OFF))
		return 1;
	else
		return 0;
#endif
#ifdef VER_1_0
	if (!RELAY)
		return 1;
	else
		return 0;
#endif

}

#ifdef VER_2_0
//chequeo continuo del estado del relay
void UpdateRelay (void)
{
	unsigned char edge = 0;

	if ((!last_edge) && (EDGE_PIN))		//flanco ascendente detector
	{									//senoidal arriba
//		edge = 1;
		last_edge = 1;
		SYNC_ON;
	}

	if ((last_edge) && (!EDGE_PIN))		//flanco descendente detector
	{									//senoidal abajo
		edge = 1;
		last_edge = 0;
		SYNC_OFF;
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
				TIM16->CNT = 0;
			}

			if (!timer_relay)		//agoto el timer y no encontro sincro, pega igual
			{
				RELAY_ON;
				relay_state = ST_ON;
			}
			break;

		case ST_DELAYED_ON:
			if (TIM16->CNT > TT_DELAYED_ON)
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
				TIM16->CNT = 0;
			}

			if (!timer_relay)		//agoto el timer y no encontro sincro, despega igual
			{
				RELAY_OFF;
				relay_state = ST_OFF;
			}

			break;

		case ST_DELAYED_OFF:
			if (TIM16->CNT > TT_DELAYED_OFF)
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
#endif
