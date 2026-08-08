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

#ifndef TASK_MEMORY_ATTRIBUTE_H_
#define TASK_MEMORY_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/
/* Dirección I2C del dispositivo 24C256 (A0,A1,A2 a GND) ya shifteada 1 bit
 * a la izquierda, como espera HAL_I2C_Mem_xxx */
#define EEPROM_I2C_ADDR         (0x50 << 1)

/* La 24C256 direcciona con 16 bits (32KB) */
#define EEPROM_MEMADD_SIZE      I2C_MEMADD_SIZE_16BIT

/* Tamaño de página de escritura del chip: 64 bytes.
 * Cada slot se alinea a un múltiplo de este valor para que ninguna
 * escritura cruce el límite de página. */
#define EEPROM_PAGE_SIZE        64u

/* Cantidad de presets hardcodeados en el firmware */
#define MEMORY_PRESET_QTY       3u

/* Mapa de memoria: cada slot ocupa una página completa (64 bytes),
 * aunque el struct use solo unos pocos. Deja margen para crecer. */
#define MEMORY_ADDR_MAGIC       0x0000u   /* 2 bytes: magic number       */
#define MEMORY_ADDR_PRESET(i)   (EEPROM_PAGE_SIZE * (1u + (i)))  /* slots 1..3 */
#define MEMORY_ADDR_RUNNING     (EEPROM_PAGE_SIZE * (1u + MEMORY_PRESET_QTY)) /* slot 4 */

/* Valor mágico que indica "la EEPROM ya fue inicializada por este firmware".
 * Si al bootear no coincide, se asume EEPROM virgen y se graban los presets. */
#define MEMORY_MAGIC_VALUE      0xCAFEu

/* Valor mágico propio del slot de "incubación en curso": distingue una
 * EEPROM recién provisionada (running.magic == 0) de un CONTINUAR válido. */
#define MEMORY_RUNNING_MAGIC    0xBEEFu

/********************** typedef **********************************************/
/* Configuración de incubación guardable/cargable desde memoria */
typedef struct
{
	uint8_t		temp;
	uint8_t		hum;
	uint8_t		days;
	uint8_t		hours;
} memory_preset_t;

/* Snapshot de una incubación en curso, para recuperar tras un corte de
 * energía. Como la placa no tiene respaldo (ni siquiera el RTC persiste),
 * se guarda el tiempo RESTANTE (no un objetivo absoluto): al continuar,
 * el cronómetro se reinicia desde ese remanente ("pausar y resumir"). */
typedef struct
{
	uint16_t	magic;              /* MEMORY_RUNNING_MAGIC si hay datos válidos */
	uint8_t		temp;
	uint8_t		hum;
	uint32_t	remaining_seconds;  /* tiempo restante al momento de guardar */
} memory_running_t;

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_MEMORY_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
