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
#include "task_memory_attribute.h"
#include "task_memory_interface.h"
#include "task_memory.h"

/********************** macros and definitions *******************************/
/* Tiempo máximo de espera del HAL para cada transacción I2C (ms) */
#define EEPROM_I2C_TIMEOUT      100u

/* Tiempo de escritura interna del chip tras un Mem_Write (datasheet: ~5ms) */
#define EEPROM_WRITE_DELAY_MS   5u

/* Presets hardcodeados en firmware. Se graban a la EEPROM una sola vez,
 * la primera vez que se detecta que el chip está "virgen" (magic inválido).
 * AJUSTAR valores según lo que se quiera precargar. */
static const memory_preset_t memory_preset_defaults[MEMORY_PRESET_QTY] =
{
	/* temp, hum, days, hours */
	{ 37u, 55u, 21u,  0u },   /* Preset 1: pollo (gallina) */
	{ 37u, 45u, 18u,  0u },   /* Preset 2: codorniz */
	{ 38u, 65u, 28u,  0u }    /* Preset 3: pato */
};

/********************** internal data declaration ****************************/
extern I2C_HandleTypeDef hi2c1;

/********************** internal functions declaration ***********************/
static bool eeprom_write(uint16_t mem_addr, const uint8_t *data, uint16_t size);
static bool eeprom_read(uint16_t mem_addr, uint8_t *data, uint16_t size);
static bool memory_write_preset(uint8_t index, const memory_preset_t *src);
static void memory_provision_defaults(void);

/********************** internal data definition *****************************/
const char *p_task_memory 		= "Task Memory (EEPROM 24C256, I2C1)";
const char *p_task_memory_ 	= "On-demand access, no periodic statechart";

/********************** external data declaration ****************************/

/********************** external functions definition ************************/
void task_memory_init(void *parameters)
{
	uint16_t magic = 0;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - Tick [mS] = %lu", GET_NAME(task_memory_init), HAL_GetTick());
	LOGGER_INFO("   %s is a %s", GET_NAME(task_memory), p_task_memory);
	LOGGER_INFO("   %s is a %s", GET_NAME(task_memory), p_task_memory_);

	LOGGER_INFO("   EEPROM Info: Presets ocupan %u bytes, Estado Running ocupa %u bytes",
		            sizeof(memory_preset_defaults),
		            sizeof(memory_running_t));


	/* Leer magic number para detectar si la EEPROM ya fue provisionada */
	if (eeprom_read(MEMORY_ADDR_MAGIC, (uint8_t *)&magic, sizeof(magic)))
	{
		if (MEMORY_MAGIC_VALUE != magic)
		{
			LOGGER_INFO("   EEPROM virgen (0x%04X) -> grabando presets", magic);
			memory_provision_defaults();
		}
		else
		{
			LOGGER_INFO("   EEPROM ya provisionada (magic OK)");
		}
	}
	else
	{
		LOGGER_INFO("   ERROR: fallo lectura EEPROM en init (rev. I2C)");
	}
}

void task_memory_update(void *parameters)
{
	/* Intencionalmente vacío: task_memory no requiere trabajo periódico.
	 * Se mantiene para respetar el patrón task_x_init/task_x_update de
	 * app_cfg_list[] en app.c. */
}

bool task_memory_load_preset(uint8_t index, memory_preset_t *dst)
{
	if ((NULL == dst) || (MEMORY_PRESET_QTY <= index))
	{
		return false;
	}

	return eeprom_read(MEMORY_ADDR_PRESET(index), (uint8_t *)dst, sizeof(memory_preset_t));
}

bool task_memory_save_running(uint8_t temp, uint8_t hum, uint32_t remaining_seconds)
{
	memory_running_t running;

	running.magic             = MEMORY_RUNNING_MAGIC;
	running.temp              = temp;
	running.hum               = hum;
	running.remaining_seconds = remaining_seconds;

	return eeprom_write(MEMORY_ADDR_RUNNING, (const uint8_t *)&running, sizeof(running));
}

bool task_memory_load_running(memory_running_t *dst)
{
	if (NULL == dst)
	{
		return false;
	}

	if (!eeprom_read(MEMORY_ADDR_RUNNING, (uint8_t *)dst, sizeof(memory_running_t)))
	{
		return false;
	}

	return (MEMORY_RUNNING_MAGIC == dst->magic);
}

bool task_memory_clear_running(void)
{
	memory_running_t running;

	memset(&running, 0, sizeof(running));
	running.magic = 0; /* != MEMORY_RUNNING_MAGIC -> load_running() lo verá inválido */

	return eeprom_write(MEMORY_ADDR_RUNNING, (const uint8_t *)&running, sizeof(running));
}

/********************** internal functions definition ************************/

/* Escritura cruda a la EEPROM. 'size' debe ser <= EEPROM_PAGE_SIZE y
 * mem_addr debe estar alineado a página (lo garantizan las macros
 * MEMORY_ADDR_* del header de atributos) para no cruzar límite de página. */
static bool eeprom_write(uint16_t mem_addr, const uint8_t *data, uint16_t size)
{
	HAL_StatusTypeDef status;

	status = HAL_I2C_Mem_Write(&hi2c1, EEPROM_I2C_ADDR, mem_addr,
								EEPROM_MEMADD_SIZE, (uint8_t *)data, size,
								EEPROM_I2C_TIMEOUT);

	if (HAL_OK != status)
	{
		LOGGER_INFO("   ERROR: eeprom_write addr=0x%04X size=%u status=%d",
					 mem_addr, size, (int)status);
		return false;
	}

	/* NOTA: no se espera aquí el ciclo de escritura interno del chip
	 * (~5ms, ver datasheet 24C256) con HAL_Delay, para no bloquear la
	 * task que llama a esta función (p.ej. task_system_update corriendo
	 * cada 1ms). El próximo acceso a esta misma página de EEPROM ocurre
	 * muchísimo después (el guardado periódico es cada 45s reales), así
	 * que el chip tiene tiempo de sobra para terminar de escribir.
	 * Ver memory_provision_defaults() para el caso donde sí se necesita
	 * esperar entre escrituras consecutivas. */

	return true;
}

static bool eeprom_read(uint16_t mem_addr, uint8_t *data, uint16_t size)
{
	HAL_StatusTypeDef status;

	status = HAL_I2C_Mem_Read(&hi2c1, EEPROM_I2C_ADDR, mem_addr,
							   EEPROM_MEMADD_SIZE, data, size,
							   EEPROM_I2C_TIMEOUT);

	if (HAL_OK != status)
	{
		LOGGER_INFO("   ERROR: eeprom_read addr=0x%04X size=%u status=%d",
					 mem_addr, size, (int)status);
		return false;
	}

	return true;
}

static bool memory_write_preset(uint8_t index, const memory_preset_t *src)
{
	if (MEMORY_PRESET_QTY <= index)
	{
		return false;
	}

	return eeprom_write(MEMORY_ADDR_PRESET(index), (const uint8_t *)src, sizeof(memory_preset_t));
}

/* Graba los 3 presets de fábrica y finalmente el magic number.
 * El magic se escribe último a propósito: si se corta la alimentación
 * a mitad de la provisión, en el próximo boot el magic seguirá siendo
 * inválido y se reintentará la grabación completa. */
static void memory_provision_defaults(void)
{
	uint8_t i;
	uint16_t magic = MEMORY_MAGIC_VALUE;
	bool ok = true;

	for (i = 0; MEMORY_PRESET_QTY > i; i++)
	{
		if (!memory_write_preset(i, &memory_preset_defaults[i]))
		{
			ok = false;
			LOGGER_INFO("   ERROR: fallo al grabar preset %u", (unsigned)i);
		}

		/* Escrituras consecutivas: sí hay que esperar el ciclo de
		 * escritura interno del chip (~5ms) entre una y otra. Corre
		 * una única vez en task_memory_init(), antes de que arranque
		 * el loop principal de tareas periódicas, así que este delay
		 * bloqueante no afecta el WCET de ninguna task en runtime. */
		HAL_Delay(EEPROM_WRITE_DELAY_MS);
	}

	if (ok)
	{
		if (eeprom_write(MEMORY_ADDR_MAGIC, (const uint8_t *)&magic, sizeof(magic)))
		{
			LOGGER_INFO("   Presets de fabrica grabados OK (magic=0x%04X)", magic);
		}
		else
		{
			LOGGER_INFO("   ERROR: presets grabados pero fallo al grabar magic");
		}
	}
}

/********************** end of file ******************************************/
