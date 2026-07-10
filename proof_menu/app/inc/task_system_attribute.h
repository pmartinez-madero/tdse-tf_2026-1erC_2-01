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

#ifndef TASK_SYSTEM_ATTRIBUTE_H_
#define TASK_SYSTEM_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* Events to excite Task System */
typedef enum task_system_ev {EV_SYS_IDLE,
							 EV_SYS_UP,
							 EV_SYS_DOWN,
							 EV_SYS_ENTER,
							 EV_SYS_BACK} task_system_ev_t;

/* State of Task System */


typedef enum task_system_st{
	/* Pantalla inicial: muestra nombre del proyecto, espera ENTER */
	ST_SYS_IDLE,

	/* Menú raíz: NUEVO INICIO / CONTINUAR */
	ST_SYS_MAIN_NEW,        /* cursor en NUEVO INICIO  */
	ST_SYS_MAIN_CONT,       /* cursor en CONTINUAR     */

	/* Rama NUEVO INICIO -> configuración paso a paso */
	ST_SYS_SET_TEMP,        /* EST. TEMPERATURA  T: XX  */
	ST_SYS_SET_HUM,         /* EST. HUMEDAD      H: XX% */
	ST_SYS_SET_DAYS,        /* EST. DIAS   D: XX        */
	ST_SYS_SET_HOURS,       /* EST. HORAS  H: XX        */
	ST_SYS_INCUBATING,      /* EN PROCESO / INCUBANDO   */

	ST_SYS_READING,         /* LEYENDO / DATOS          */
	ST_SYS_NO_DATA,         /* NO SE ENCUENTRAN DATOS   */
	ST_SYS_LAST_DATA,       /* ULTIMO T:XX / H:XX R:XX  */
	ST_SYS_INCUBATING_CONT  /* EN PROCESO / INCUBANDO (desde continuar) */

} task_system_state_t;

typedef struct
{
	uint32_t			tick;
	task_system_state_t	state;
	task_system_ev_t	event;
	bool				flag;
} task_system_dta_t;



/********************** external data declaration ****************************/
extern task_system_dta_t task_system_dta_list[];

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_SYSTEM_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
