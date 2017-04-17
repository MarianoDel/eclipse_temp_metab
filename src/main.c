/**
  ******************************************************************************
  * @file    Template_2/main.c
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Use this template for new projects with stm32f0xx family.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_adc.h"
#include "stdio.h"
//#include "string.h"
#include "adc.h"
#include "core_cm0.h"
#include "hard.h"
#include "tim.h"


#ifdef SETPOINT_PLUS_HYST
#define HYST	7
#endif

//--- VARIABLES EXTERNAS ---//
volatile unsigned char timer_1seg = 0;
//volatile unsigned short timer_standby = 0;
//volatile unsigned short timer_led_comm = 0;
volatile unsigned char buffrx_ready = 0;
volatile unsigned char *pbuffrx;
volatile unsigned short timer_relay = 0;
volatile unsigned short wait_ms_var;


//--- VARIABLES GLOBALES ---//
volatile unsigned char door_filter;
volatile unsigned char take_sample_pote;
volatile unsigned char take_sample_temp;
volatile unsigned char move_relay;

volatile unsigned char secs = 0;
volatile unsigned short minutes = 0;
volatile unsigned short led_timer = 0;

#ifdef OPEN_LOOP
volatile unsigned short pwm_total_min = 0;
volatile unsigned short pwm_current_min = 0;
#endif

//--- FUNCIONES DEL MODULO ---//
unsigned short ADC_Conf (void);
unsigned short ReadADC1 (unsigned int);
unsigned short ReadADC1_SameSampleTime (unsigned int);
void SetADC1_SampleTime (void);


unsigned char Door_Open (void);
unsigned short Get_Temp (void);
unsigned short Get_Pote (void);

//--- FILTROS DE SENSORES ---//
#define LARGO_FILTRO_POTE 16
#define DIVISOR_POTE      4   //2 elevado al divisor = largo filtro
#define LARGO_FILTRO_TEMP 32
#define DIVISOR_TEMP      5   //2 elevado al divisor = largo filtro
unsigned short vtemp [LARGO_FILTRO_TEMP + 1];
unsigned short vpote [LARGO_FILTRO_POTE + 1];

//--- FIN DEFINICIONES DE FILTRO ---//
enum Parts {

	NEVER_DEGREES = 0,
	TWELVE_DEGREES,	//1
	NINE_DEGREES,	//2
	SIX_DEGREES,	//3
	THREE_DEGREES,	//4
	ZERO_DEGREES,	//5
	CONT_DEGREES	//6
};
									 // MAX		<0	<3		<6	<9		<12		NUNCA
//const unsigned short vpote_ranges [] = {3584, 3072, 2560, 2048, 1536, 1024, 512, 0};
//const unsigned short vtemp_ranges [] = {3584, 3072, 2560, 2048, 1536, 1024, 512, 0};
const unsigned short vpote_ranges [] = {3510, 2925, 2340, 1755, 1170, 585, 0};
//const unsigned short vtemp_ranges [] = {2000, 1337, 1224, 1100, 930, 819, 0};
//const unsigned short vtemp_ranges [] = {1337, 713, 685, 657, 628, 600, 572};	//ajuste puntas 26-02
#ifdef SIMPLE_VECTOR_TEMP
//const unsigned short vtemp_ranges [] = {880, 816, 750, 687, 623, 559, 495};	//ajuste puntas 10-3-15 (mediciones 3-3-15)
const unsigned short vtemp_ranges [] = {796, 769, 742, 715, 688, 661, 495};	//ajuste puntas 10-3-15 (mediciones 3-3-15)
#endif

#ifdef DOBLE_VECTOR_TEMP
const unsigned short vtemp_ranges_encendido [] = {865, 839, 804, 779, 762, 736, 693};	//ajuste puntas 14-5-15 (mediciones 14-5-15)
const unsigned short vtemp_ranges_apagado [] = {880, 816, 750, 687, 623, 559, 495};	//ajuste puntas 14-5-15 (mediciones 14-5-15)
#endif

#ifdef SETPOINT_PLUS_HYST
//TEMPERATURAS EN CALIENTE
//(heladera funcionando)			   NUNCA  12   9    6    3    0    MAX
const unsigned short vtemp_ranges [] = {495, 627, 646, 665, 687, 708, 796};	//ajuste puntas 23-5-15 (mediciones 22-5-15)

#endif

#ifdef OPEN_LOOP
#define PWM_MIN_MAX		110
//PWM en minutos
//(heladera funcionando)		  NUNCA  12   9   6   3    0   MAX
const unsigned char vpwm_ranges [] = {0, 71, 77, 88, 93, 104, 110};	//ajuste puntas 26-5-15 (mediciones 22-5-15)

#endif

//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
#ifdef SIMPLE_VECTOR_TEMP
	enum Parts Temp_Range, Pote_Range;
#endif

#ifdef DOBLE_VECTOR_TEMP
	enum Parts Temp_Range, Pote_Range;
#endif

#ifdef SETPOINT_PLUS_HYST
	enum Parts Pote_Range;
//	unsigned char last_action = 0;		//last_action esta implicita en el codigo, no haciendo resolviendo pegar o despegar en el medio
#endif

#ifdef OPEN_LOOP
	enum Parts Pote_Range;
//	unsigned char last_action = 0;		//last_action esta implicita en el codigo, no haciendo resolviendo pegar o despegar en el medio
#endif

	unsigned short temp_filtered = 0;
	unsigned short pote_filtered = 0;
	unsigned char stop_state = 0;
	unsigned char led_state = 0;
	unsigned char blink = 0;
//	unsigned char relay_was_on = 0;
#ifdef RELAY_OFF_WITH_DOOR_OPEN
	unsigned char door_is_open = 0;
#endif
	//!< At this stage the microcontroller clock setting is already configured,
    //   this is done through SystemInit() function which is called from startup
    //   file (startup_stm32f0xx.s) before to branch to application main.
    //   To reconfigure the default setting of SystemInit() function, refer to
    //   system_stm32f0xx.c file

	//GPIO Configuration.
	GPIO_Config();

	//TIM Configuration.
	TIM_3_Init();
	TIM_16_Init();
	//Timer_2_Init();
	//Timer_3_Init();
	//Timer_4_Init();

	//UART configuration.
	//USART_Config();

	//ACTIVAR SYSTICK TIMER
	 if (SysTick_Config(48000))
	 {
		 while (1)	/* Capture error */
		 {
			 if (LED)
				 LED_ON;
			 else
				 LED_OFF;

			 Wait_ms(300);
		 }
	 }
	//SENSAR TEMPERATURA 	((OK))
	//ENVIAR ONE WIRE		((OK))

	//PRUEBA DE SYSTICK
	 /*
	 while(1)
	 {
		 if (LED_COMM)
			 LED_COMM_OFF;
		 else
			 LED_COMM_ON;

		 Delay(1);
	 }
	 */
	 //FIN PRUEBA DE SYSTICK

	//ADC configuration.
	if (ADC_Conf() == 0)
	{
		while (1)
		{
			if (LED)
				LED_ON;
			else
				LED_OFF;

			Wait_ms(150);
		}
	}

	TIM16Enable ();

	LED_ON;
    Wait_ms(1000);
    LED_OFF;

//    //para pruebas
//    Wait_ms(9000);
//    while (1)
//    {
//    	if (!timer_relay)
//    	{
//    		if (RELAY)
//    			RELAY_OFF;
//    		else
//    			RELAY_ON;
//
//    		timer_relay = 10000;
//    	}
//    }
//    //para pruebas
#ifdef VER_2_0
    //3 segundos muestro sincro
    timer_relay = 3000;
    while (timer_relay)
    {
//    	if (EDGE_PIN)
//    		LED_ON;
//    	else
//    		LED_OFF;

    	if ((!temp_filtered) && (EDGE_PIN))		//flanco ascendente detector
    	{									//senoidal arriba
    		temp_filtered = 1;
    		SYNC_ON;
    	}

    	if ((temp_filtered) && (!EDGE_PIN))		//flanco descendente detector
    	{									//senoidal abajo
    		temp_filtered = 0;
    		SYNC_OFF;
    		if (LED)
    			LED_OFF;
    		else
    			LED_ON;
    	}
    }
    LED_OFF;
#endif

//    while (1)
//    {
//    	if (SYNC)
//    		SYNC_OFF;
//    	else
//    		SYNC_ON;
//
//    	Wait_ms (10);
//    }

	//--- Main loop ---//
	while(1)
	{
		//PROGRAMA DE PRODUCCION
		if (!take_sample_pote)	//tomo muestras cada 10ms
		{
			take_sample_pote = 10;
			pote_filtered = Get_Pote();

			//determino los rangos del pote
			if (pote_filtered > vpote_ranges[0])
				Pote_Range = CONT_DEGREES;
			else if (pote_filtered > vpote_ranges[1])
				Pote_Range = ZERO_DEGREES;
			else if (pote_filtered > vpote_ranges[2])
				Pote_Range = THREE_DEGREES;
			else if (pote_filtered > vpote_ranges[3])
				Pote_Range = SIX_DEGREES;
			else if (pote_filtered > vpote_ranges[4])
				Pote_Range = NINE_DEGREES;
			else if (pote_filtered > vpote_ranges[5])
				Pote_Range = TWELVE_DEGREES;
			else
				Pote_Range = NEVER_DEGREES;
		}

		if (!take_sample_temp)	//tomo muestras cada 100ms
		{
#ifdef SETPOINT_PLUS_HYST
			take_sample_temp = 100;
#else
			take_sample_temp = 10;
#endif
			temp_filtered = Get_Temp();

#ifdef SIMPLE_VECTOR_TEMP
			//determino los rangos de temperatura
			if (temp_filtered > vtemp_ranges[0])		//1337
				Temp_Range = CONT_DEGREES;
			else if (temp_filtered > vtemp_ranges[1])	//713
				Temp_Range = ZERO_DEGREES;
			else if (temp_filtered > vtemp_ranges[2])	//685
				Temp_Range = THREE_DEGREES;
			else if (temp_filtered > vtemp_ranges[3])	//657
				Temp_Range = SIX_DEGREES;
			else if (temp_filtered > vtemp_ranges[4])	//628
				Temp_Range = NINE_DEGREES;
			else if (temp_filtered > vtemp_ranges[5])	//600
				Temp_Range = TWELVE_DEGREES;
			else
				Temp_Range = NEVER_DEGREES;				//572
#endif

#ifdef DOBLE_VECTOR_TEMP
			//determino los rangos de temperatura
			if (RELAY)	//EQUIPO ENCENDIDO
			{
				if (temp_filtered > vtemp_ranges_encendido[0])		//1337
					Temp_Range = CONT_DEGREES;
				else if (temp_filtered > vtemp_ranges_encendido[1])	//713
					Temp_Range = ZERO_DEGREES;
				else if (temp_filtered > vtemp_ranges_encendido[2])	//685
					Temp_Range = THREE_DEGREES;
				else if (temp_filtered > vtemp_ranges_encendido[3])	//657
					Temp_Range = SIX_DEGREES;
				else if (temp_filtered > vtemp_ranges_encendido[4])	//628
					Temp_Range = NINE_DEGREES;
				else if (temp_filtered > vtemp_ranges_encendido[5])	//600
					Temp_Range = TWELVE_DEGREES;
				else
					Temp_Range = NEVER_DEGREES;				//572
			}
			else	//EQUIPO APAGADO, ES OTRA MEDICION
			{
				if (temp_filtered > vtemp_ranges_apagado[0])		//1337
					Temp_Range = CONT_DEGREES;
				else if (temp_filtered > vtemp_ranges_apagado[1])	//713
					Temp_Range = ZERO_DEGREES;
				else if (temp_filtered > vtemp_ranges_apagado[2])	//685
					Temp_Range = THREE_DEGREES;
				else if (temp_filtered > vtemp_ranges_apagado[3])	//657
					Temp_Range = SIX_DEGREES;
				else if (temp_filtered > vtemp_ranges_apagado[4])	//628
					Temp_Range = NINE_DEGREES;
				else if (temp_filtered > vtemp_ranges_apagado[5])	//600
					Temp_Range = TWELVE_DEGREES;
				else
					Temp_Range = NEVER_DEGREES;				//572
			}



#endif

		}

#ifdef RELAY_OFF_WITH_DOOR_OPEN
		if (Door_Open())
		{
			RelayOff();
			LED_OFF;
			door_is_open = 1;
			LIGHT_ON;
		}
		else
		{
			LIGHT_OFF;
			door_is_open = 0;
		}
#else
		if (Door_Open())
		{
			//LED_OFF;
			LIGHT_ON;
		}
		else
		{
			LIGHT_OFF;
		}
#endif

#ifdef RELAY_OFF_WITH_DOOR_OPEN
		if (!door_is_open)
		{
#endif
			switch (stop_state)
			{
				case NORMAL:
					if (move_relay == 0)		//el RELE lo muevo cada 10 segundos
					{
						move_relay = 10;

	#ifdef SIMPLE_VECTOR_TEMP
						//Modificacion 10-3-15 pongo histeresis de 1 paso completo
						if (Temp_Range >= Pote_Range)
							RELAY_OFF;

						if (Pote_Range > NEVER_DEGREES)
						{
							if (Temp_Range < (Pote_Range - 1))
								RELAY_ON;
						}
	#endif

	#ifdef DOBLE_VECTOR_TEMP
						//Modificacion 14-5-15 tego dos vectores, cuando esta encendido y cuando no
						if (Temp_Range >= Pote_Range)
							RELAY_OFF;

						if (Pote_Range > NEVER_DEGREES)
						{
							if (Temp_Range < (Pote_Range - 1))
								RELAY_ON;
						}
	#endif

	#ifdef SETPOINT_PLUS_HYST
						//Modificacion 23-5-15 pongo setpoint + hysteresis
						switch (Pote_Range)
						{
							case CONT_DEGREES:		//lo resuelvo en otra parte
								break;

							case ZERO_DEGREES:
								if (temp_filtered > (vtemp_ranges[ZERO_DEGREES] + HYST))
									RELAY_OFF;
								else if (temp_filtered < (vtemp_ranges[ZERO_DEGREES] - HYST))
									RELAY_ON;

								break;

							case THREE_DEGREES:
								if (temp_filtered > (vtemp_ranges[THREE_DEGREES] + HYST))
									RELAY_OFF;
								else if (temp_filtered < (vtemp_ranges[THREE_DEGREES] - HYST))
									RELAY_ON;

								break;

							case SIX_DEGREES:
								if (temp_filtered > (vtemp_ranges[SIX_DEGREES] + HYST))
									RELAY_OFF;
								else if (temp_filtered < (vtemp_ranges[SIX_DEGREES] - HYST))
									RELAY_ON;

								break;

							case NINE_DEGREES:
								if (temp_filtered > (vtemp_ranges[NINE_DEGREES] + HYST))
									RELAY_OFF;
								else if (temp_filtered < (vtemp_ranges[NINE_DEGREES] - HYST))
									RELAY_ON;

								break;

							case TWELVE_DEGREES:
								if (temp_filtered > (vtemp_ranges[TWELVE_DEGREES] + HYST))
									RELAY_OFF;
								else if (temp_filtered < (vtemp_ranges[TWELVE_DEGREES] - HYST))
									RELAY_ON;

								break;

							case NEVER_DEGREES:		//lo resuelvo en otra parte
								break;
						}
	#endif
	#ifdef OPEN_LOOP
						//Modificacion 23-5-15 pongo setpoint + hysteresis
						switch (Pote_Range)
						{
							case CONT_DEGREES:		//lo resuelvo en otra parte
								break;

							case ZERO_DEGREES:
								if (pwm_current_min < vpwm_ranges[ZERO_DEGREES])
									RelayOn();
								else
									RelayOff();

								if (pwm_current_min >= PWM_MIN_MAX)
								{
									pwm_current_min = 0;
								}
								break;

							case THREE_DEGREES:
								if (pwm_current_min < vpwm_ranges[THREE_DEGREES])
									RelayOn();
								else
									RelayOff();

								if (pwm_current_min >= PWM_MIN_MAX)
								{
									pwm_current_min = 0;
								}
								break;

							case SIX_DEGREES:
								if (pwm_current_min < vpwm_ranges[SIX_DEGREES])
									RelayOn();
								else
									RelayOff();

								if (pwm_current_min >= PWM_MIN_MAX)
								{
									pwm_current_min = 0;
								}
								break;

							case NINE_DEGREES:
								if (pwm_current_min < vpwm_ranges[NINE_DEGREES])
									RelayOn();
								else
									RelayOff();

								if (pwm_current_min >= PWM_MIN_MAX)
								{
									pwm_current_min = 0;
								}
								break;

							case TWELVE_DEGREES:
								if (pwm_current_min < vpwm_ranges[TWELVE_DEGREES])
									RelayOn();
								else
									RelayOff();

								if (pwm_current_min >= PWM_MIN_MAX)
								{
									pwm_current_min = 0;
								}
								break;

							case NEVER_DEGREES:		//lo resuelvo en otra parte
								break;
						}
	#endif //OPEN_LOOP
					}	//end move_relay

					if (minutes >= TT_MINUTES_DAY_ON)
						stop_state = GO_TO_STOP;

					//si se apago la heladera
					if (Pote_Range == NEVER_DEGREES)
						stop_state = TO_NEVER;

					//si se prende siempre
					if (Pote_Range == CONT_DEGREES)
						stop_state = TO_ALWAYS;

					break;

				case GO_TO_STOP:
					//tengo que apagar el rele durante 25 minutos
					minutes = 0;
					RelayOff();
					stop_state = STOPPED;
					break;

				case STOPPED:
					if (minutes >= TT_MINUTES_DAY_OFF)
					{
						stop_state = NORMAL;
						minutes = 0;
						pwm_current_min = 0;
					}
					break;

				case TO_NEVER:
					//apago el motor
					RelayOff();
					stop_state = NEVER;
					break;

				case NEVER:
					//mantengo motor apagado mientras este en NEVER
//					if (RELAY)
//						RelayOff();

					if (Pote_Range != NEVER_DEGREES)
					{
						minutes = 0;
						stop_state = NORMAL;
						pwm_current_min = 0;
					}
					break;

				case TO_ALWAYS:
					RelayOn();
					stop_state = ALWAYS;
					break;

				case ALWAYS:
					if (Pote_Range != CONT_DEGREES)
					{
						stop_state = NORMAL;
					}

#ifdef RELAY_OFF_WITH_DOOR_OPEN
					if (!RelayIsOn())		//agregado pos si abren la puerta
						RelayOn();
#endif

					if (minutes >= TT_MINUTES_DAY_ON)
						stop_state = GO_TO_STOP;

					break;

				default:
					stop_state = NORMAL;
					break;
			}
#ifdef RELAY_OFF_WITH_DOOR_OPEN
		}
#endif

#ifdef RELAY_OFF_WITH_DOOR_OPEN
		if (!door_is_open)
		{
#endif
			switch (led_state)
			{
				case START_BLINKING:
					blink = (unsigned char) Pote_Range;

					if (blink)
					{
						LED_ON;
						led_timer = 200;
						led_state++;
						blink--;
					}
					break;

				case WAIT_TO_OFF:
					if (!led_timer)
					{
						LED_OFF;
						led_timer = 200;
						led_state++;
					}
					break;

				case WAIT_TO_ON:
					if (!led_timer)
					{
						if (blink)
						{
							blink--;
							led_timer = 200;
							led_state = WAIT_TO_OFF;
							LED_ON;
						}
						else
						{
							led_state = WAIT_NEW_CYCLE;
							led_timer = 2000;
						}
					}
					break;

				case WAIT_NEW_CYCLE:
					if (!led_timer)
					{
						led_state = START_BLINKING;
					}
					break;


				default:
					led_state = START_BLINKING;
					break;
			}
#ifdef RELAY_OFF_WITH_DOOR_OPEN
		}
#endif
		//Cuestiones generales
#ifdef VER_2_0
		UpdateRelay();
#endif

	}	//End of while (1)
	return 0;
}
//--- End of file ---//

unsigned short ADC_Conf (void)
{
	unsigned short cal = 0;
	ADC_InitTypeDef ADC_InitStructure;

	if (!RCC_ADC_CLK)
		RCC_ADC_CLK_ON;

	ADC_ClockModeConfig(ADC1, ADC_ClockMode_SynClkDiv4);

	// preseteo de registros a default
	  /* ADCs DeInit */
	  ADC_DeInit(ADC1);

	  /* Initialize ADC structure */
	  ADC_StructInit(&ADC_InitStructure);

	  /* Configure the ADC1 in continuous mode with a resolution equal to 12 bits  */
	  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	  ADC_Init(ADC1, &ADC_InitStructure);

	//software by setting bit ADCAL=1.
	//Calibration can only be initiated when the ADC is disabled (when ADEN=0).
	//ADCAL bit stays at 1 during all the calibration sequence.
	//It is then cleared by hardware as soon the calibration completes
	cal = ADC_GetCalibrationFactor(ADC1);

	// Enable ADC1
	ADC_Cmd(ADC1, ENABLE);

	SetADC1_SampleTime ();

	return cal;
}

unsigned short ReadADC1 (unsigned int channel)
{
	uint32_t tmpreg = 0;
	//GPIOA_PIN4_ON;
	// Set channel and sample time
	//ADC_ChannelConfig(ADC1, channel, ADC_SampleTime_7_5Cycles);	//pifia la medicion 2800 o 3400 en ves de 4095
	//ADC_ChannelConfig(ADC1, channel, ADC_SampleTime_239_5Cycles);
	//ADC_ChannelConfig(ADC1, ADC_Channel_0, ADC_SampleTime_239_5Cycles);

	//ADC_ChannelConfig INTERNALS
	/* Configure the ADC Channel */
	ADC1->CHSELR = channel;

	/* Clear the Sampling time Selection bits */
	tmpreg &= ~ADC_SMPR1_SMPR;

	/* Set the ADC Sampling Time register */
	tmpreg |= (uint32_t)ADC_SampleTime_239_5Cycles;

	/* Configure the ADC Sample time register */
	ADC1->SMPR = tmpreg ;


	// Start the conversion
	ADC_StartOfConversion(ADC1);
	// Wait until conversion completion
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	// Get the conversion value
	//GPIOA_PIN4_OFF;	//tarda 20us en convertir
	return ADC_GetConversionValue(ADC1);
}

//Setea el sample time en el ADC
void SetADC1_SampleTime (void)
{
	uint32_t tmpreg = 0;

	/* Clear the Sampling time Selection bits */
	tmpreg &= ~ADC_SMPR1_SMPR;

	/* Set the ADC Sampling Time register */
	tmpreg |= (uint32_t)ADC_SampleTime_239_5Cycles;

	/* Configure the ADC Sample time register */
	ADC1->SMPR = tmpreg ;
}


//lee el ADC sin cambiar el sample time anterior
unsigned short ReadADC1_SameSampleTime (unsigned int channel)
{
	// Configure the ADC Channel
	ADC1->CHSELR = channel;

	// Start the conversion
	ADC1->CR |= (uint32_t)ADC_CR_ADSTART;

	// Wait until conversion completion
	while((ADC1->ISR & ADC_ISR_EOC) == 0);

	// Get the conversion value
	return (uint16_t) ADC1->DR;
}


unsigned short Get_Temp (void)
{
	unsigned int total_ma;
	unsigned char j;

	//Kernel mejorado ver 2
	//si el vector es de 0 a 7 (+1) sumo todas las posiciones entre 1 y 8, acomodo el nuevo vector entre 0 y 7
	total_ma = 0;
	//vtemp[LARGO_FILTRO_TEMP] = ReadADC1 (CH_IN_TEMP);
	vtemp[LARGO_FILTRO_TEMP] = ReadADC1_SameSampleTime (CH_IN_TEMP);
    for (j = 0; j < (LARGO_FILTRO_TEMP); j++)
    {
    	total_ma += vtemp[j + 1];
    	vtemp[j] = vtemp[j + 1];
    }

    return total_ma >> DIVISOR_TEMP;
}

unsigned short Get_Pote (void)
{
	unsigned int total_ma;
	unsigned char j;

	//Kernel mejorado ver 2
	//si el vector es de 0 a 7 (+1) sumo todas las posiciones entre 1 y 8, acomodo el nuevo vector entre 0 y 7
	total_ma = 0;
	//vpote[LARGO_FILTRO_POTE] = ReadADC1 (CH_IN_POTE);
	vpote[LARGO_FILTRO_POTE] = ReadADC1_SameSampleTime (CH_IN_POTE);
    for (j = 0; j < (LARGO_FILTRO_POTE); j++)
    {
    	total_ma += vpote[j + 1];
    	vpote[j] = vpote[j + 1];
    }

    return total_ma >> DIVISOR_POTE;
}

unsigned char Door_Open (void)
{
	if (door_filter >= DOOR_THRESH)
		return 1;
	else
		return 0;
}

/**
  * @brief  Decrements the TimingDelay variable.	ESTA ES LA QUE LLAMA SYSTICK CADA 1MSEG
  * @param  None
  * @retval None
  */
volatile unsigned short relay_dumb = 0;

void TimingDelay_Decrement(void)
{
	if (wait_ms_var)
		wait_ms_var--;

	//filtro de ruido para la puerta
	if (DOOR)
	{
		if (door_filter < DOOR_ROOF)
			door_filter++;
	}
	else if (door_filter)
		door_filter--;

	//indice para el filtro del pote
	if (take_sample_pote)
		take_sample_pote--;

	//indice para el filtro de la temp
	if (take_sample_temp)
		take_sample_temp--;

	if (relay_dumb)		//entro cada 1 segundo
		relay_dumb--;
	else
	{
		relay_dumb = 1000;
		if (move_relay)
			move_relay--;

		if (secs < 60)
			secs++;
		else
		{
			secs = 0;
			minutes++;
#ifdef OPEN_LOOP
			pwm_current_min++;		//contador de minutos
#endif
		}
	}

	if (timer_relay)
		timer_relay--;

	if (led_timer)
		led_timer--;
}
