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
#include "task_display_attribute.h"
#include "task_display_interface.h"
#include "task_system_attribute.h"
#include "task_system_interface.h"

/********************** macros and definitions *******************************/
#define DEL_SYS_MIN     0ul
#define DEL_SYS_MED     250ul
#define DEL_SYS_MAX     500ul

/* Rangos de configuración */
#define TEMP_MIN        20
#define TEMP_MAX        40
#define TEMP_DEFAULT    37

#define HUM_MIN         40
#define HUM_MAX         80
#define HUM_DEFAULT     60

#define DAYS_MIN        1
#define DAYS_MAX        30
#define DAYS_DEFAULT    21

#define HOURS_MIN       0
#define HOURS_MAX       23
#define HOURS_DEFAULT   0

#define SYSTEM_DTA_QTY  1ul

/********************** internal data declaration ****************************/
task_system_dta_t task_system_dta_list[SYSTEM_DTA_QTY];

/* Variables de configuración de la incubadora */
static uint8_t cfg_temp  = TEMP_DEFAULT;
static uint8_t cfg_hum   = HUM_DEFAULT;
static uint8_t cfg_days  = DAYS_DEFAULT;
static uint8_t cfg_hours = HOURS_DEFAULT;

/********************** internal functions declaration ***********************/
static void task_system_statechart(void);
static void display_main_new(void);
static void display_main_cont(void);
static void display_set_temp(void);
static void display_set_hum(void);
static void display_set_days(void);
static void display_set_hours(void);

/********************** internal data definition *****************************/
const char *p_task_system      = "Task System (Incubadora Statechart)";
const char *p_task_system_     = "Non-Blocking Code";
const char *p_task_system__    = "(Update by Time Code, period = 1mS)";

/********************** external functions definition ************************/
void task_system_init(void *parameters)
{
    uint32_t index;
    task_system_dta_t   *p_task_system_dta;
    task_system_state_t  state;
    task_system_ev_t     event;
    bool b_event;

    /* Print out: Task Initialized */
    LOGGER_INFO(" ");
    LOGGER_INFO("  %s is running - Tick [mS] = %lu", GET_NAME(task_system_init), HAL_GetTick());
    LOGGER_INFO("   %s is a %s", GET_NAME(task_system), p_task_system);
    LOGGER_INFO("   %s is a %s", GET_NAME(task_system), p_task_system_);
    LOGGER_INFO("   %s is a %s", GET_NAME(task_system), p_task_system__);

    init_event_task_system();

    for (index = 0; SYSTEM_DTA_QTY > index; index++)
    {
        p_task_system_dta = &task_system_dta_list[index];

        state = ST_SYS_IDLE;
        p_task_system_dta->state = state;

        event = EV_SYS_IDLE;
        p_task_system_dta->event = event;

        b_event = false;
        p_task_system_dta->flag = b_event;

        LOGGER_INFO(" ");
        LOGGER_INFO("   %s = %lu   %s = %lu   %s = %s",
                    GET_NAME(state),   (uint32_t)state,
                    GET_NAME(event),   (uint32_t)event,
                    GET_NAME(b_event), (b_event ? "true" : "false"));
    }

    /* Pantalla de bienvenida */
    put_event_task_display(0, 0, "   INCUBADORA");
    put_event_task_display(0, 1, "ACHINELLI-MADERO");
}

void task_system_update(void *parameters)
{
    task_system_statechart();
}

/********************** internal functions definition ************************/

/* Helpers de display ----------------------------------------------------- */
static void display_main_new(void)
{
    put_event_task_display(0, 0, ">NUEVO INICIO   ");
    put_event_task_display(0, 1, " CONTINUAR      ");
}

static void display_main_cont(void)
{
    put_event_task_display(0, 0, " NUEVO INICIO   ");
    put_event_task_display(0, 1, ">CONTINUAR      ");
}

static void display_set_temp(void)
{
    char buf[20];
    put_event_task_display(0, 0, "EST. TEMPERATURA");
    snprintf(buf, sizeof(buf), "T: %2u           ", (unsigned)cfg_temp);
    put_event_task_display(0, 1, buf);
}

static void display_set_hum(void)
{
    char buf[20];
    put_event_task_display(0, 0, "EST. HUMEDAD    ");
    snprintf(buf, sizeof(buf), "H: %2u%%          ", (unsigned)cfg_hum);
    put_event_task_display(0, 1, buf);
}

static void display_set_days(void)
{
    char buf[20];
    put_event_task_display(0, 0, "EST. DIAS       ");
    snprintf(buf, sizeof(buf), "D: %2u           ", (unsigned)cfg_days);
    put_event_task_display(0, 1, buf);
}

static void display_set_hours(void)
{
    char buf[20];
    put_event_task_display(0, 0, "EST. HORAS      ");
    snprintf(buf, sizeof(buf), "H: %2u           ", (unsigned)cfg_hours);
    put_event_task_display(0, 1, buf);
}

/* Statechart principal --------------------------------------------------- */
static void task_system_statechart(void)
{
    task_system_dta_t *p_task_system_dta = &task_system_dta_list[0];

    if (true == any_event_task_system())
    {
        p_task_system_dta->flag  = true;
        p_task_system_dta->event = get_event_task_system();
    }

    switch (p_task_system_dta->state)
    {
        /* ================================================================
         * PANTALLA RAÍZ: INCUBADORA / ACHINELLI-MADERO
         * Espera ENTER para avanzar al menú principal.
         * ================================================================ */
        case ST_SYS_IDLE:

            if (p_task_system_dta->flag && EV_SYS_ENTER == p_task_system_dta->event)
            {
                p_task_system_dta->flag  = false;
                p_task_system_dta->state = ST_SYS_MAIN_NEW;
                display_main_new();
            }
            else
            {
                /* Ignorar cualquier otro botón; BACK no hace nada aquí */
                p_task_system_dta->flag = false;
            }
            break;

        /* ================================================================
         * MENÚ PRINCIPAL — cursor en NUEVO INICIO
         * UP/DOWN cambia selección; ENTER confirma.
         * ================================================================ */
        case ST_SYS_MAIN_NEW:

            if (p_task_system_dta->flag)
            {
                p_task_system_dta->flag = false;

                if (EV_SYS_DOWN == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_MAIN_CONT;
                    display_main_cont();
                }
                else if (EV_SYS_ENTER == p_task_system_dta->event)
                {
                    cfg_temp  = TEMP_DEFAULT;
                    cfg_hum   = HUM_DEFAULT;
                    cfg_days  = DAYS_DEFAULT;
                    cfg_hours = HOURS_DEFAULT;
                    p_task_system_dta->state = ST_SYS_SET_TEMP;
                    display_set_temp();
                }
                /* UP y BACK no hacen nada en este estado */
            }
            break;

        /* ================================================================
         * MENÚ PRINCIPAL — cursor en CONTINUAR
         * ================================================================ */
        case ST_SYS_MAIN_CONT:

            if (p_task_system_dta->flag)
            {
                p_task_system_dta->flag = false;

                if (EV_SYS_UP == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_MAIN_NEW;
                    display_main_new();
                }
                else if (EV_SYS_ENTER == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_READING;
                    put_event_task_display(0, 0, "    LEYENDO     ");
                    put_event_task_display(0, 1, "     DATOS      ");
                }
                /* DOWN y BACK no hacen nada en este estado */
            }
            break;

        /* ================================================================
         * CONFIGURACIÓN — TEMPERATURA
         * UP/DOWN ajustan valor; ENTER avanza; BACK vuelve al menu
         * ================================================================ */
        case ST_SYS_SET_TEMP:

            if (p_task_system_dta->flag)
            {
                p_task_system_dta->flag = false;

                if (EV_SYS_UP == p_task_system_dta->event)
                {
                    if (cfg_temp < TEMP_MAX) cfg_temp++;
                    display_set_temp();
                }
                else if (EV_SYS_DOWN == p_task_system_dta->event)
                {
                    if (cfg_temp > TEMP_MIN) cfg_temp--;
                    display_set_temp();
                }
                else if (EV_SYS_ENTER == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_SET_HUM;
                    display_set_hum();
                }
                else if (EV_SYS_BACK == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_MAIN_NEW;
                    display_main_new();
                }
            }
            break;

        /* ================================================================
         * CONFIGURACIÓN — HUMEDAD
         * ================================================================ */
        case ST_SYS_SET_HUM:

            if (p_task_system_dta->flag)
            {
                p_task_system_dta->flag = false;

                if (EV_SYS_UP == p_task_system_dta->event)
                {
                    if (cfg_hum < HUM_MAX) cfg_hum++;
                    display_set_hum();
                }
                else if (EV_SYS_DOWN == p_task_system_dta->event)
                {
                    if (cfg_hum > HUM_MIN) cfg_hum--;
                    display_set_hum();
                }
                else if (EV_SYS_ENTER == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_SET_DAYS;
                    display_set_days();
                }
                else if (EV_SYS_BACK == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_SET_TEMP;
                    display_set_temp();
                }
            }
            break;

        /* ================================================================
         * CONFIGURACIÓN — DÍAS Y HORAS
         * UP/DOWN ajustan días; ENTER avanza a incubando; BACK vuelve.
         * ================================================================ */
        case ST_SYS_SET_DAYS:

            if (p_task_system_dta->flag)
            {
                p_task_system_dta->flag = false;

                if (EV_SYS_UP == p_task_system_dta->event)
                {
                    if (cfg_days < DAYS_MAX) cfg_days++;
                    display_set_days();
                }
                else if (EV_SYS_DOWN == p_task_system_dta->event)
                {
                    if (cfg_days > DAYS_MIN) cfg_days--;
                    display_set_days();
                }
                else if (EV_SYS_ENTER == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_SET_HOURS;
                    display_set_hours();
                }
                else if (EV_SYS_BACK == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_SET_HUM;
                    display_set_hum();
                }
            }
            break;

        /* ================================================================
         * CONFIGURACIÓN — HORAS
         * UP/DOWN ajustan horas; ENTER avanza a incubando; BACK vuelve a días.
         * ================================================================ */
        case ST_SYS_SET_HOURS:

            if (p_task_system_dta->flag)
            {
                p_task_system_dta->flag = false;

                if (EV_SYS_UP == p_task_system_dta->event)
                {
                    if (cfg_hours < HOURS_MAX) cfg_hours++;
                    display_set_hours();
                }
                else if (EV_SYS_DOWN == p_task_system_dta->event)
                {
                    if (cfg_hours > HOURS_MIN) cfg_hours--;
                    display_set_hours();
                }
                else if (EV_SYS_ENTER == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_INCUBATING;
                    put_event_task_display(0, 0, "   EN PROCESO   ");
                    put_event_task_display(0, 1, "   INCUBANDO    ");
                }
                else if (EV_SYS_BACK == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_SET_DAYS;
                    display_set_days();
                }
            }
            break;

        /* ================================================================
         * EN PROCESO / INCUBANDO (desde NUEVO INICIO)
         * ================================================================ */
        case ST_SYS_INCUBATING:
            /* Placeholder: lógica de incubación */
            p_task_system_dta->flag = false;
            break;

        /* ================================================================
         * LEYENDO DATOS (desde CONTINUAR)
         * ================================================================ */
        case ST_SYS_READING:
            /* TODO: leer memoria y transicionar a NO_DATA o LAST_DATA */
            p_task_system_dta->flag = false;
            break;

        /* ================================================================
         * NO SE ENCUENTRAN DATOS EN MEMORIA
         * ================================================================ */
        case ST_SYS_NO_DATA:

            if (p_task_system_dta->flag)
            {
                p_task_system_dta->flag = false;
                p_task_system_dta->state = ST_SYS_MAIN_NEW;
                display_main_new();
            }
            break;

        /* ================================================================
         * ÚLTIMO DATO ENCONTRADO EN MEMORIA
         * ENTER arranca incubación; BACK vuelve al menú.
         * ================================================================ */
        case ST_SYS_LAST_DATA:

            if (p_task_system_dta->flag)
            {
                p_task_system_dta->flag = false;

                if (EV_SYS_ENTER == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_INCUBATING_CONT;
                    put_event_task_display(0, 0, "   EN PROCESO   ");
                    put_event_task_display(0, 1, "   INCUBANDO    ");
                }
                else if (EV_SYS_BACK == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_MAIN_CONT;
                    display_main_cont();
                }
            }
            break;

        /* ================================================================
         * EN PROCESO / INCUBANDO (desde CONTINUAR)
         * ================================================================ */
        case ST_SYS_INCUBATING_CONT:
            /* Placeholder: lógica de incubación con parámetros recuperados */
            p_task_system_dta->flag = false;
            break;

        default:
            p_task_system_dta->tick  = DEL_SYS_MIN;
            p_task_system_dta->state = ST_SYS_IDLE;
            p_task_system_dta->event = EV_SYS_IDLE;
            p_task_system_dta->flag  = false;
            put_event_task_display(0, 0, "   INCUBADORA");
            put_event_task_display(0, 1, "ACHINELLI-MADERO");
            break;
    }
}

/********************** end of file ******************************************/
