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
#include <string.h>

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_system_attribute.h"
#include "task_system_interface.h"
#include "task_sensor_attribute.h"

/********************** macros and definitions *******************************/
#define DEL_BTN_MIN		0ul
#define DEL_BTN_MED		25ul
#define DEL_BTN_MAX		50ul

#define SENSOR_CFG_QTY		(sizeof(task_sensor_cfg_list)/sizeof(task_sensor_cfg_t))
#define SENSOR_DTA_QTY		SENSOR_CFG_QTY

/* ---------------------------------------------------------------------------
 * SHT30 (Temperatura / Humedad)
 * ------------------------------------------------------------------------ */
/* Asumiendo que usas el I2C1, de lo contrario cambialo por hi2c2, etc. */
extern I2C_HandleTypeDef hi2c2;

/* Direccion 0x44 desplazada 1 bit a la izquierda para las HAL de STM32 */
#define SHT30_I2C_ADDR                  0x8A

/* Periodo entre muestreos: 2000 ticks de 1ms = 2s */
#define SHT30_SAMPLE_PERIOD_TICKS		2000ul

/* Tiempo de medicion para High Repeatability (~15ms) */
#define SHT30_MEASUREMENT_WAIT_TICKS    15ul

/********************** internal data declaration ****************************/

const task_sensor_cfg_t task_sensor_cfg_list[] = {
	{ID_BTN_UP,    BTN_UP_PORT,    BTN_UP_PIN,    BTN_UP_PRESSED,    DEL_BTN_MAX, EV_SYS_IDLE, EV_SYS_UP},
	{ID_BTN_DOWN,  BTN_DOWN_PORT,  BTN_DOWN_PIN,  BTN_DOWN_PRESSED,  DEL_BTN_MAX, EV_SYS_IDLE, EV_SYS_DOWN},
	{ID_BTN_ENTER, BTN_ENTER_PORT, BTN_ENTER_PIN, BTN_ENTER_PRESSED, DEL_BTN_MAX, EV_SYS_IDLE, EV_SYS_ENTER},
	{ID_BTN_BACK,  BTN_BACK_PORT,  BTN_BACK_PIN,  BTN_BACK_PRESSED,  DEL_BTN_MAX, EV_SYS_IDLE, EV_SYS_BACK}
};

task_sensor_dta_t task_sensor_dta_list[SENSOR_DTA_QTY];

/* Instancia unica de datos del SHT30 */
task_sht30_dta_t task_sht30_dta;

/********************** internal functions declaration ***********************/
void task_sensor_statechart(uint32_t index);

/* Declaracion de la nueva FSM del SHT30 */
static void task_sht30_statechart(void);

/********************** internal data definition *****************************/
const char *p_task_sensor 		= "Task Sensor (Sensor Statechart)";
const char *p_task_sensor_ 		= "Non-Blocking Code";
const char *p_task_sensor__ 	= "(Update by Time Code, period = 1mS)";

/********************** external data declaration ****************************/

/********************** external functions definition ************************/
void task_sensor_init(void *parameters)
{
	uint32_t index;
	task_sensor_dta_t *p_task_sensor_dta;
	task_sensor_st_t state;
	task_sensor_ev_t event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - Tick [mS] = %lu", GET_NAME(task_sensor_init), HAL_GetTick());
	LOGGER_INFO("   %s is a %s", GET_NAME(task_sensor), p_task_sensor);
	LOGGER_INFO("   %s is a %s", GET_NAME(task_sensor), p_task_sensor_);
	LOGGER_INFO("   %s is a %s", GET_NAME(task_sensor), p_task_sensor__);

	for (index = 0; SENSOR_DTA_QTY > index; index++)
	{
		/* Update Task Sensor Data Pointer */
		p_task_sensor_dta = &task_sensor_dta_list[index];

		/* Init & Print out: Index & Task execution FSM */
		state = ST_BTN_UP;
		p_task_sensor_dta->state = state;

		event = EV_BTN_UP;
		p_task_sensor_dta->event = event;

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %lu",GET_NAME(index), index,GET_NAME(state), (uint32_t)state,GET_NAME(event), (uint32_t)event);
	}

	/* --- Inicializacion del SHT30 --- */
		memset(&task_sht30_dta, 0, sizeof(task_sht30_dta));
		task_sht30_dta.state = ST_SHT30_IDLE;
		task_sht30_dta.tick  = SHT30_SAMPLE_PERIOD_TICKS;

		LOGGER_INFO(" ");
		LOGGER_INFO("   SHT30 I2C inicializado - primer muestreo en %lu ms",(uint32_t)SHT30_SAMPLE_PERIOD_TICKS);

}

void task_sensor_update(void *parameters)
{
	uint32_t index;

	for (index = 0; SENSOR_DTA_QTY > index; index++)
	{
		/* Run Task Statechart */
		task_sensor_statechart(index);
	}

	/* Run SHT30 Statechart (una transicion como maximo por tick) */
	task_sht30_statechart();
}

void task_sensor_statechart(uint32_t index)
{
	const task_sensor_cfg_t *p_task_sensor_cfg;
	task_sensor_dta_t *p_task_sensor_dta;

	for (index = 0; SENSOR_DTA_QTY > index; index++)
	{
		/* Update Task Sensor Configuration & Data Pointer */
		p_task_sensor_cfg = &task_sensor_cfg_list[index];
		p_task_sensor_dta = &task_sensor_dta_list[index];

		if (p_task_sensor_cfg->pressed == HAL_GPIO_ReadPin(p_task_sensor_cfg->gpio_port, p_task_sensor_cfg->pin))
		{
			p_task_sensor_dta->event =	EV_BTN_DOWN;
		}
		else
		{
			p_task_sensor_dta->event =	EV_BTN_UP;
		}

		switch (p_task_sensor_dta->state)
		{
			case ST_BTN_UP:

				if (EV_BTN_DOWN == p_task_sensor_dta->event)
				{
					p_task_sensor_dta->tick = p_task_sensor_cfg->tick_max;
					p_task_sensor_dta->state = ST_BTN_FALLING;
				}

				break;

			case ST_BTN_FALLING:

				p_task_sensor_dta->tick--;
				if (DEL_BTN_MIN == p_task_sensor_dta->tick)
				{
					if (EV_BTN_DOWN == p_task_sensor_dta->event)
					{
						put_event_task_system(p_task_sensor_cfg->signal_down);
						p_task_sensor_dta->state = ST_BTN_DOWN;
					}
					else
					{
						p_task_sensor_dta->state = ST_BTN_UP;
					}
				}

				break;

			case ST_BTN_DOWN:

				if (EV_BTN_UP == p_task_sensor_dta->event)
				{
					p_task_sensor_dta->state = ST_BTN_RISING;
					p_task_sensor_dta->tick = p_task_sensor_cfg->tick_max;
				}

				break;

			case ST_BTN_RISING:

				p_task_sensor_dta->tick--;
				if (DEL_BTN_MIN == p_task_sensor_dta->tick)
				{
					if (EV_BTN_UP == p_task_sensor_dta->event)
					{
						put_event_task_system(p_task_sensor_cfg->signal_up);
						p_task_sensor_dta->state = ST_BTN_UP;
					}
					else
					{
						p_task_sensor_dta->state = ST_BTN_DOWN;
					}
				}

				break;

			default:

				p_task_sensor_dta->tick  = DEL_BTN_MIN;
				p_task_sensor_dta->state = ST_BTN_UP;
				p_task_sensor_dta->event = EV_BTN_UP;

			break;
		}
	}
}

/* ---------------------------------------------------------------------------
 * SHT30 - Maquina de estados (no bloqueante para el planificador)
 * ------------------------------------------------------------------------ */
static void task_sht30_statechart(void)
{
	switch (task_sht30_dta.state)
	{
		case ST_SHT30_IDLE:
			if (task_sht30_dta.tick > 0)
			{
				task_sht30_dta.tick--;
			}
			else
			{
				task_sht30_dta.state = ST_SHT30_TRIGGER;
			}
			break;

		case ST_SHT30_TRIGGER:
		{
            /* Comando SHT30: Single Shot, High Repeatability, Clock Stretching Disabled */
            uint8_t cmd[2] = {0x24, 0x00};

            /* Enviamos el comando por I2C (timeout corto de 10ms porque es solo escritura) */
            if (HAL_I2C_Master_Transmit(&hi2c2, SHT30_I2C_ADDR, cmd, 2, 100) == HAL_OK)
            {
                task_sht30_dta.tick  = SHT30_MEASUREMENT_WAIT_TICKS;
                task_sht30_dta.state = ST_SHT30_WAIT;
            }
            else
            {
                LOGGER_INFO("SHT30: fallo TX comando (addr=0x%02X) ErrorCode=0x%lX",SHT30_I2C_ADDR, HAL_I2C_GetError(&hi2c2));
                task_sht30_dta.state = ST_SHT30_ERROR;
            }
            break;
		}

		case ST_SHT30_WAIT:
			if (task_sht30_dta.tick > 0)
			{
				task_sht30_dta.tick--;
			}
			else
			{
				task_sht30_dta.state = ST_SHT30_READ;
			}
			break;

		case ST_SHT30_READ:
		{
            uint8_t rx_data[6];

            /* Leemos los 6 bytes de respuesta: Temp MSB, Temp LSB, Temp CRC, Hum MSB, Hum LSB, Hum CRC */
            if (HAL_I2C_Master_Receive(&hi2c2, SHT30_I2C_ADDR, rx_data, 6, 100) == HAL_OK)
            {
                uint16_t raw_temp = (uint16_t)((rx_data[0] << 8) | rx_data[1]);
                uint16_t raw_hum  = (uint16_t)((rx_data[3] << 8) | rx_data[4]);

                /* Formulas de conversion oficiales del Datasheet del SHT30 */
                task_sht30_dta.Temperature = -45.0f + (175.0f * ((float)raw_temp / 65535.0f));
                task_sht30_dta.Humidity    = 100.0f * ((float)raw_hum / 65535.0f);
                task_sht30_dta.data_valid  = true;

                task_sht30_dta.tick  = SHT30_SAMPLE_PERIOD_TICKS;
                task_sht30_dta.state = ST_SHT30_IDLE;
            }
            else
            {
                task_sht30_dta.state = ST_SHT30_ERROR;
            }
			break;
		}

		case ST_SHT30_ERROR:
			task_sht30_dta.data_valid = false;
			task_sht30_dta.tick  = 50ul;
			task_sht30_dta.state = ST_SHT30_IDLE;
			break;

		default:
			task_sht30_dta.state = ST_SHT30_IDLE;
			task_sht30_dta.tick  = SHT30_SAMPLE_PERIOD_TICKS;
			break;
	}
}

/********************** end of file ******************************************/
