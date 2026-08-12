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

#ifndef TASK_ACTUATOR_ATTRIBUTE_H_
#define TASK_ACTUATOR_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* Events to excite Task Actuator */
typedef enum task_actuator_ev {EV_LED_IDLE,
							   EV_LED_ACTIVE,
							   EV_SERVO_POS_A,    /* Mover servo a 45°  */
							   EV_SERVO_POS_B,    /* Mover servo a 135° */
							   EV_SERVO_CENTER,    /* Mover servo a 90°  */
							   EV_HEATER_OFF,      /* Apagar calefactor (LED_T + RELE) */
							   EV_HEATER_ON,       /* Encender calefactor (LED_T + RELE) */
							   EV_HUMID_OFF,       /* Apagar indicador de humedad baja (LED_H) */
							   EV_HUMID_ON         /* Encender indicador de humedad baja (LED_H) */
							   } task_actuator_ev_t;

/* States of Task Actuator */
typedef enum task_actuator_st {ST_LED_IDLE,
							   ST_LED_ACTIVE,
							   ST_SERVO_POS_A,    /* Servo en 45°, PWM apagado  */
							   ST_SERVO_POS_B,    /* Servo en 135°, PWM apagado */
							   ST_SERVO_MOVING,   /* Servo en movimiento, espera llegar */
							   ST_HEATER_OFF,     /* Calefactor apagado */
							   ST_HEATER_ON,      /* Calefactor encendido */
							   ST_HUMID_OFF,      /* Indicador de humedad baja (LED_H) apagado */
							   ST_HUMID_ON        /* Indicador de humedad baja (LED_H) encendido */
							   } task_actuator_st_t;

/* Identifier of Task Actuator */
typedef enum task_actuator_id {ID_LED_A,
							   ID_SERVO,
							   ID_HEATER,
							   ID_HUMID} task_actuator_id_t;

typedef struct
{
	task_actuator_id_t	identifier;
	GPIO_TypeDef *		gpio_port;
	uint16_t			pin;
	GPIO_PinState		led_on;
	GPIO_PinState		led_off;
	uint32_t			tick_max;
	GPIO_TypeDef *		gpio_port2;  /* Segundo pin opcional (p.ej. indicador LED_T del heater) */
	uint16_t			pin2;        /* NULL/0 si no se usa */
} task_actuator_cfg_t;

typedef struct
{
	uint32_t			tick;
	task_actuator_st_t	state;
	task_actuator_ev_t	event;
	bool				flag;
} task_actuator_dta_t;

/********************** external data declaration ****************************/
extern task_actuator_dta_t task_actuator_dta_list[];

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_ACTUATOR_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
