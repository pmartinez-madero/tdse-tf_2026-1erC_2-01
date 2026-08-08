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

#ifndef TASK_MEMORY_INTERFACE_H_
#define TASK_MEMORY_INTERFACE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/
/* Carga el preset [0..MEMORY_PRESET_QTY-1] hardcodeado, ya grabado en la
 * EEPROM, en la estructura provista. Devuelve true si la lectura I2C fue ok. */
extern bool task_memory_load_preset(uint8_t index, memory_preset_t *dst);

/* Guarda un snapshot de la incubación en curso (para poder recuperarla
 * con CONTINUAR tras un corte de energía). Sobrescribe siempre el mismo
 * slot. Devuelve true si la escritura I2C fue ok. */
extern bool task_memory_save_running(uint8_t temp, uint8_t hum, uint32_t remaining_seconds);

/* Lee el snapshot de incubación en curso. Devuelve true solo si la
 * lectura I2C fue ok Y el magic del slot es válido (hay datos reales
 * guardados, no una EEPROM recién provisionada). */
extern bool task_memory_load_running(memory_running_t *dst);

/* Invalida el snapshot de incubación en curso (se llama al finalizar
 * una incubación, para que CONTINUAR no vuelva a ofrecerla). */
extern bool task_memory_clear_running(void);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_MEMORY_INTERFACE_H_ */

/********************** end of file ******************************************/
