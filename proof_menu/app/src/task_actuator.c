/*
 * Copyright (c) 2026 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 */

/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_actuator_attribute.h"
#include "task_actuator_interface.h"

/********************** macros and definitions *******************************/
#define DEL_LED_MIN		0ul
#define DEL_LED_MED		250ul
#define DEL_LED_MAX		500ul

/* Valores CCR para posiciones del servo (TIM2 CH1, Period=999, f=50Hz) */
#define SERVO_CCR_POS_A     62ul    /* 45°  ~ 1.25ms */
#define SERVO_CCR_CENTER    75ul    /* 90°  ~ 1.50ms */
#define SERVO_CCR_POS_B     87ul    /* 135° ~ 1.75ms */

/* Tiempo que tarda el servo en llegar a la posición: 500ms es suficiente */
#define SERVO_MOVE_TICKS    500ul

#define ACTUATOR_CFG_QTY	(sizeof(task_actuator_cfg_list)/sizeof(task_actuator_cfg_t))
#define ACTUATOR_DTA_QTY	ACTUATOR_CFG_QTY

/********************** internal data declaration ****************************/
extern TIM_HandleTypeDef htim2;

const task_actuator_cfg_t task_actuator_cfg_list[] = {
	{ID_LED_A,  LED_A_PORT,  LED_A_PIN, LED_A_ON,     LED_A_OFF,    DEL_LED_MAX, NULL,      0        },
	{ID_SERVO,  NULL,        0,         0,            0,            0,           NULL,      0        },
	{ID_HEATER, GPIOC,       RELE_Pin,  GPIO_PIN_SET, GPIO_PIN_RESET, 0,         GPIOC,     LED_T_Pin},
	{ID_HUMID,  LED_H_GPIO_Port, LED_H_Pin, GPIO_PIN_SET, GPIO_PIN_RESET, 0,     NULL,      0        }
};

task_actuator_dta_t task_actuator_dta_list[ACTUATOR_DTA_QTY];

/* Estado destino del servo mientras esta en ST_SERVO_MOVING, guardado en
 * software en vez de leido del registro CCR del timer al finalizar el
 * movimiento. Leer __HAL_TIM_GET_COMPARE() para decidir el estado destino
 * es fragil: si mientras el servo se mueve llega un evento nuevo que
 * cambia el CCR de nuevo (rotacion + fin de periodo casi simultaneos),
 * la lectura del registro al final puede no coincidir con la intencion
 * real, y ademas el evento nuevo se perdia silenciosamente porque
 * ST_SERVO_MOVING no revisaba el flag de eventos entrantes. */
static task_actuator_st_t servo_move_target = ST_SERVO_POS_A;

/********************** internal functions declaration ***********************/
void task_actuator_statechart(uint32_t index);

/********************** internal data definition *****************************/
const char *p_task_actuator 		= "Task Actuator (Actuator Statechart)";
const char *p_task_actuator_ 		= "Non-Blocking Code";
const char *p_task_actuator__ 		= "(Update by Time Code, period = 1mS)";

/********************** external data declaration ****************************/

/********************** external functions definition ************************/
void task_actuator_init(void *parameters)
{
	uint32_t index;
	const task_actuator_cfg_t *p_task_actuator_cfg;
	task_actuator_dta_t *p_task_actuator_dta;
	task_actuator_st_t state;
	task_actuator_ev_t event;
	bool b_event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - Tick [mS] = %lu", GET_NAME(task_actuator_init), HAL_GetTick());
	LOGGER_INFO("   %s is a %s", GET_NAME(task_actuator), p_task_actuator);
	LOGGER_INFO("   %s is a %s", GET_NAME(task_actuator), p_task_actuator_);
	LOGGER_INFO("   %s is a %s", GET_NAME(task_actuator), p_task_actuator__);

	for (index = 0; ACTUATOR_DTA_QTY > index; index++)
	{
		/* Update Task Actuator Configuration & Data Pointer */
		p_task_actuator_cfg = &task_actuator_cfg_list[index];
		p_task_actuator_dta = &task_actuator_dta_list[index];

		/* Init & Print out: Index & Task execution FSM */
		state = ST_LED_IDLE;
		p_task_actuator_dta->state = state;

		event = EV_LED_IDLE;
		p_task_actuator_dta->event = event;

		b_event = false;
		p_task_actuator_dta->flag = b_event;

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu   %s = %s",
					 GET_NAME(index), index,
					 GET_NAME(state), (uint32_t)state,
					 GET_NAME(event), (uint32_t)event,
					 GET_NAME(b_event), (b_event ? "true" : "false"));

		if (ID_SERVO == p_task_actuator_cfg->identifier)
		{
			/* Posición inicial 90°: el init ocurre antes del ciclo,
			 * por lo que el HAL_Delay aquí es aceptable. */
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, SERVO_CCR_CENTER);
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
			HAL_Delay(500);
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
			p_task_actuator_dta->state = ST_SERVO_POS_A;
		}
		else if (ID_HEATER == p_task_actuator_cfg->identifier)
		{
			HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port,  p_task_actuator_cfg->pin,  p_task_actuator_cfg->led_off);
			HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port2, p_task_actuator_cfg->pin2, p_task_actuator_cfg->led_off);
			p_task_actuator_dta->state = ST_HEATER_OFF;
		}
		else if (ID_HUMID == p_task_actuator_cfg->identifier)
		{
			HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->led_off);
			p_task_actuator_dta->state = ST_HUMID_OFF;
		}
		else
		{
			HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->led_off);
		}
	}
}

void task_actuator_update(void *parameters)
{
	uint32_t index;

	for (index = 0; ACTUATOR_DTA_QTY > index; index++)
	{
		/* Run Task Statechart */
		task_actuator_statechart(index);
	}
}

void task_actuator_statechart(uint32_t index)
{
	const task_actuator_cfg_t *p_task_actuator_cfg;
	task_actuator_dta_t *p_task_actuator_dta;

	/* Update Task Actuator Configuration & Data Pointer */
	p_task_actuator_cfg = &task_actuator_cfg_list[index];
	p_task_actuator_dta = &task_actuator_dta_list[index];

	switch (p_task_actuator_dta->state)
	{
		case ST_LED_IDLE:

			if ((true == p_task_actuator_dta->flag) && (EV_LED_ACTIVE == p_task_actuator_dta->event))
			{
				p_task_actuator_dta->flag = false;
				HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->led_on);
				p_task_actuator_dta->state = ST_LED_ACTIVE;
			}

			break;

		case ST_LED_ACTIVE:

			if ((true == p_task_actuator_dta->flag) && (EV_LED_IDLE == p_task_actuator_dta->event))
			{
				p_task_actuator_dta->flag = false;
				HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->led_off);
				p_task_actuator_dta->state = ST_LED_IDLE;
			}

			break;

		/* ================================================================
		 * SERVO — posición A (45°), PWM apagado, esperando evento
		 * ================================================================ */
		case ST_SERVO_POS_A:

			if (true == p_task_actuator_dta->flag)
			{
				p_task_actuator_dta->flag = false;

				if (EV_SERVO_POS_B == p_task_actuator_dta->event)
				{
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, SERVO_CCR_POS_B);
					HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
					p_task_actuator_dta->tick  = SERVO_MOVE_TICKS;
					servo_move_target          = ST_SERVO_POS_B;
					p_task_actuator_dta->state = ST_SERVO_MOVING;
				}
				else if (EV_SERVO_CENTER == p_task_actuator_dta->event)
				{
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, SERVO_CCR_CENTER);
					HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
					p_task_actuator_dta->tick  = SERVO_MOVE_TICKS;
					servo_move_target          = ST_SERVO_POS_A;
					p_task_actuator_dta->state = ST_SERVO_MOVING;
				}
			}

			break;

		/* ================================================================
		 * SERVO — posición B (135°), PWM apagado, esperando evento
		 * ================================================================ */
		case ST_SERVO_POS_B:

			if (true == p_task_actuator_dta->flag)
			{
				p_task_actuator_dta->flag = false;

				if (EV_SERVO_POS_A == p_task_actuator_dta->event)
				{
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, SERVO_CCR_POS_A);
					HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
					p_task_actuator_dta->tick  = SERVO_MOVE_TICKS;
					servo_move_target          = ST_SERVO_POS_A;
					p_task_actuator_dta->state = ST_SERVO_MOVING;
				}
				else if (EV_SERVO_CENTER == p_task_actuator_dta->event)
				{
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, SERVO_CCR_CENTER);
					HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
					p_task_actuator_dta->tick  = SERVO_MOVE_TICKS;
					servo_move_target          = ST_SERVO_POS_A;
					p_task_actuator_dta->state = ST_SERVO_MOVING;
				}
			}

			break;

		/* ================================================================
		 * SERVO — en movimiento, espera SERVO_MOVE_TICKS y apaga PWM
		 * ================================================================ */
		case ST_SERVO_MOVING:

			/* Si llega un evento nuevo mientras el servo todavia se esta
			 * moviendo, lo descartamos de forma explicita (en vez de
			 * dejarlo en el struct sin consumir, donde antes se perdia
			 * silenciosamente sin ni siquiera bajar el flag). El servo
			 * termina el movimiento en curso antes de aceptar otro. */
			if (true == p_task_actuator_dta->flag)
			{
				p_task_actuator_dta->flag = false;
			}

			if (p_task_actuator_dta->tick > 0)
			{
				p_task_actuator_dta->tick--;
			}
			else
			{
				HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);

				/* Ir al estado destino guardado en software al iniciar
				 * el movimiento, no al que indique el registro CCR del
				 * timer en este instante. */
				p_task_actuator_dta->state = servo_move_target;
			}

			break;

		/* ================================================================
		 * HEATER — apagado (RELE + LED_T indicador apagados)
		 * ================================================================ */
		case ST_HEATER_OFF:

			if ((true == p_task_actuator_dta->flag) && (EV_HEATER_ON == p_task_actuator_dta->event))
			{
				p_task_actuator_dta->flag = false;
				HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port,  p_task_actuator_cfg->pin,  p_task_actuator_cfg->led_on);
				HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port2, p_task_actuator_cfg->pin2, p_task_actuator_cfg->led_on);
				p_task_actuator_dta->state = ST_HEATER_ON;
			}

			break;

		/* ================================================================
		 * HEATER — encendido (RELE + LED_T indicador encendidos)
		 * ================================================================ */
		case ST_HEATER_ON:

			if ((true == p_task_actuator_dta->flag) && (EV_HEATER_OFF == p_task_actuator_dta->event))
			{
				p_task_actuator_dta->flag = false;
				HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port,  p_task_actuator_cfg->pin,  p_task_actuator_cfg->led_off);
				HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port2, p_task_actuator_cfg->pin2, p_task_actuator_cfg->led_off);
				p_task_actuator_dta->state = ST_HEATER_OFF;
			}

			break;

		/* ================================================================
		 * HUMID — indicador de humedad baja apagado (LED_H)
		 * ================================================================ */
		case ST_HUMID_OFF:

			if ((true == p_task_actuator_dta->flag) && (EV_HUMID_ON == p_task_actuator_dta->event))
			{
				p_task_actuator_dta->flag = false;
				HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->led_on);
				p_task_actuator_dta->state = ST_HUMID_ON;
			}

			break;

		/* ================================================================
		 * HUMID — indicador de humedad baja encendido (LED_H)
		 * ================================================================ */
		case ST_HUMID_ON:

			if ((true == p_task_actuator_dta->flag) && (EV_HUMID_OFF == p_task_actuator_dta->event))
			{
				p_task_actuator_dta->flag = false;
				HAL_GPIO_WritePin(p_task_actuator_cfg->gpio_port, p_task_actuator_cfg->pin, p_task_actuator_cfg->led_off);
				p_task_actuator_dta->state = ST_HUMID_OFF;
			}

			break;

		default:

			p_task_actuator_dta->tick  = DEL_LED_MIN;
			p_task_actuator_dta->state = ST_LED_IDLE;
			p_task_actuator_dta->event = EV_LED_IDLE;
			p_task_actuator_dta->flag = false;

			break;
	}
}

/********************** end of file ******************************************/
