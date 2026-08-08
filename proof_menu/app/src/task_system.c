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
#include "task_sensor_attribute.h"
#include "task_memory_attribute.h"
#include "task_memory_interface.h"

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

/* Actualización de pantalla durante incubación: cada 60000 ticks = 60 segundos */
#define INCUBATION_UPDATE_TICKS     1000ul

/* Rotación del servo: 4 veces por día = cada 6 horas = 21600000 ticks */
#define SERVO_ROTATION_TICKS        4000ul

/* Días con rotación activa (primeros 18 días) */
#define SERVO_ACTIVE_DAYS           18ul

/* Días mínimos configurados para que el servo se active */
#define SERVO_MIN_CFG_DAYS          22ul

#define SYSTEM_DTA_QTY  1ul

/********************** internal data declaration ****************************/
extern RTC_HandleTypeDef hrtc;

task_system_dta_t task_system_dta_list[SYSTEM_DTA_QTY];

/* Variables de configuración de la incubadora */
static uint8_t cfg_temp  = TEMP_DEFAULT;
static uint8_t cfg_hum   = HUM_DEFAULT;
static uint8_t cfg_days  = DAYS_DEFAULT;
static uint8_t cfg_hours = HOURS_DEFAULT;

/* Objetivo de incubación en segundos desde epoch (día 1, hora 0, min 0) */
static uint32_t incubation_target_seconds = 0;

/* Contador de ticks para actualización periódica de pantalla */
static uint32_t incubation_tick_cnt = 0;

/* Contador de refrescos de pantalla, para alternar linea 1 cada 3 (3s) */
static uint8_t display_alt_cnt = 0;

/* Contador de ticks para rotación del servo */
static uint32_t servo_tick_cnt = 0;

/* Posición actual del servo: false=POS_A(45°), true=POS_B(135°) */
static bool servo_pos_b = false;

/* Flag: el servo solo se mueve cuando la incubación está activa */
static bool servo_active = false;

/* Histéresis del control de temperatura: enciende por debajo de
 * (cfg_temp - HEATER_HYSTERESIS) y apaga al alcanzar cfg_temp */
#define HEATER_HYSTERESIS      1

/* Estado actual conocido del calefactor (evita reenviar el mismo evento) */
static bool heater_on = false;

/* Cursor del submenú de NUEVO INICIO: 0=AJUSTAR, 1..3=PRESET 1..3 */
#define NEW_MENU_OPT_ADJUST     0u
#define NEW_MENU_OPT_QTY        (1u + MEMORY_PRESET_QTY)
static uint8_t new_menu_cursor = NEW_MENU_OPT_ADJUST;

/* Guardado periódico de la incubación en curso, para sobrevivir a un corte
 * de energía (la placa no tiene respaldo). Intervalo en tiempo REAL de
 * pared (ticks de 1ms), independiente de la escala comprimida de debug
 * de cfg_days/cfg_hours. 45s reales = margen seguro para la vida útil
 * de la EEPROM en una incubación de varias semanas. */
#define RUNNING_SAVE_TICKS      40000ul
static uint32_t running_save_tick_cnt = RUNNING_SAVE_TICKS;

/* Datos recuperados de EEPROM al elegir CONTINUAR, mostrados en
 * ST_SYS_LAST_DATA y usados para arrancar ST_SYS_INCUBATING_CONT */
static memory_running_t recovered_running;

/********************** internal functions declaration ***********************/
static void task_system_statechart(void);
static void display_main_new(void);
static void display_main_cont(void);
static void display_new_menu(void);
static void display_set_temp(void);
static void display_set_hum(void);
static void display_set_days(void);
static void display_set_hours(void);
static void display_incubating(void);
static void display_last_data(void);
static uint32_t rtc_get_seconds(void);
static void incubation_start(void);
static void incubation_run_common(task_system_dta_t *p_task_system_dta);

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

/* Muestra 2 opciones a la vez (línea 0 = actual, línea 1 = siguiente),
 * con el cursor '>' siempre en la línea 0. Al llegar a la última opción,
 * la línea 1 queda en blanco. */
static void display_new_menu(void)
{
    char line0[20];
    char line1[20];

    switch (new_menu_cursor)
    {
        case NEW_MENU_OPT_ADJUST:
            snprintf(line0, sizeof(line0), ">AJUSTAR        ");
            snprintf(line1, sizeof(line1), " POLLO          ");
            break;
        case 1u: /* PRESET 1 */
            snprintf(line0, sizeof(line0), ">POLLO          ");
            snprintf(line1, sizeof(line1), " CODORNIZ       ");
            break;
        case 2u: /* PRESET 2 */
            snprintf(line0, sizeof(line0), ">CODORNIZ       ");
            snprintf(line1, sizeof(line1), " PATO           ");
            break;
        case 3u: /* PRESET 3 */
        default:
            snprintf(line0, sizeof(line0), ">PATO           ");
            snprintf(line1, sizeof(line1), "                ");
            break;
    }

    put_event_task_display(0, 0, line0);
    put_event_task_display(0, 1, line1);
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

/* Convierte hora actual del RTC a segundos totales (días*86400 + horas*3600 + min*60 + seg) */
static uint32_t rtc_get_seconds(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    return ((uint32_t)sDate.Date  * 86400ul)
         + ((uint32_t)sTime.Hours * 3600ul)
         + ((uint32_t)sTime.Minutes * 60ul)
         + ((uint32_t)sTime.Seconds);
}

/* Guarda el objetivo al arrancar la incubación */
static void incubation_start(void)
{
    uint32_t now = rtc_get_seconds();
    incubation_target_seconds = now + ((uint32_t)cfg_days  * 24ul) + ((uint32_t)cfg_hours * 1ul);
    incubation_tick_cnt = INCUBATION_UPDATE_TICKS; /* fuerza update inmediato */
    display_alt_cnt = 0; /* arranca mostrando el temporizador */

    /* Inicializar servo en posición A y arrancar contador */
    servo_tick_cnt = SERVO_ROTATION_TICKS;
    servo_pos_b    = false;
    servo_active   = false;

    /* Calefactor arranca apagado (coincide con estado inicial del actuador) */
    heater_on = false;

    /* Reiniciar cadencia de guardado a EEPROM para esta nueva incubación */
    running_save_tick_cnt = RUNNING_SAVE_TICKS;
}

/* Muestra "EN PROCESO" fijo, y alterna en la linea 1 entre tiempo restante
 * y temperatura/humedad cada 3 refrescos (3 segundos, ya que esta funcion
 * se llama cada INCUBATION_UPDATE_TICKS = 1s) */
static void display_incubating(void)
{
    char buf[20];

    put_event_task_display(0, 0, "   EN PROCESO   ");

    /* Alterna cada 3 llamadas: 0,1 -> tiempo | 2 -> clima */
    if (display_alt_cnt < 2)
    {
        uint32_t now = rtc_get_seconds();
        uint32_t remaining = 0;

        if (incubation_target_seconds > now)
            remaining = incubation_target_seconds - now;

        uint32_t days_left  = remaining / 24ul;
        uint32_t hours_left = (remaining % 24ul) / 1ul;

        snprintf(buf, sizeof(buf), "D:%2u  H:%2u      ", (unsigned)days_left, (unsigned)hours_left);
    }
    else
    {
        if (task_sht30_dta.data_valid)
        {
            snprintf(buf, sizeof(buf), "T:%2u%cC  H:%2u%%   ",
                     (unsigned)task_sht30_dta.Temperature, 0xDF,
                     (unsigned)task_sht30_dta.Humidity);
        }
        else
        {
            snprintf(buf, sizeof(buf), "   LEYENDO...   ");
        }
    }
    put_event_task_display(0, 1, buf);

    display_alt_cnt++;
    if (display_alt_cnt >= 3)
    {
        display_alt_cnt = 0;
    }
}

/* Muestra temp/hum objetivo y tiempo restante recuperados de la EEPROM,
 * antes de confirmar si se retoma la incubación */
static void display_last_data(void)
{
    char buf[20];

    snprintf(buf, sizeof(buf), "T:%2u%cC  H:%2u%%   ",
             (unsigned)recovered_running.temp, 0xDF,
             (unsigned)recovered_running.hum);
    put_event_task_display(0, 0, buf);

    {
        uint32_t remaining     = recovered_running.remaining_seconds;
        uint32_t days_left     = remaining / 24ul;
        uint32_t hours_left    = (remaining % 24ul) / 1ul;
        snprintf(buf, sizeof(buf), "D:%2u   H:%2u     ", (unsigned)days_left, (unsigned)hours_left);
    }
    put_event_task_display(0, 1, buf);
}

/* Lógica común de "incubación activa": control de calefactor, rotación de
 * servo, guardado periódico a EEPROM (backup ante corte de luz) y chequeo
 * de finalización. La usan tanto ST_SYS_INCUBATING (desde NUEVO INICIO)
 * como ST_SYS_INCUBATING_CONT (desde CONTINUAR). */
static void incubation_run_common(task_system_dta_t *p_task_system_dta)
{
    /* --- Control del calefactor (LED_T + RELE) con histéresis --- */
    if (task_sht30_dta.data_valid)
    {
        if ((!heater_on) && (task_sht30_dta.Temperature < (cfg_temp - HEATER_HYSTERESIS)))
        {
            put_event_task_actuator(EV_HEATER_ON, ID_HEATER);
            heater_on = true;
        }
        else if (heater_on && (task_sht30_dta.Temperature >= cfg_temp))
        {
            put_event_task_actuator(EV_HEATER_OFF, ID_HEATER);
            heater_on = false;
        }
    }

    /* --- Control del servo --- */
    if (servo_active && cfg_days >= SERVO_MIN_CFG_DAYS)
    {
        /* Calcular días transcurridos desde inicio */
        uint32_t now_sec      = rtc_get_seconds();
        uint32_t start_sec    = incubation_target_seconds
                              - ((uint32_t)cfg_days  * 24ul)
                              - ((uint32_t)cfg_hours * 1ul);
        uint32_t elapsed_days = (now_sec - start_sec) / 24ul;

        if (elapsed_days < SERVO_ACTIVE_DAYS)
        {
            if (servo_tick_cnt > 0)
            {
                servo_tick_cnt--;
            }
            else
            {
                servo_tick_cnt = SERVO_ROTATION_TICKS;

                /* Alternar posición */
                if (servo_pos_b)
                {
                    put_event_task_actuator(EV_SERVO_POS_A, ID_SERVO);
                    servo_pos_b = false;
                }
                else
                {
                    put_event_task_actuator(EV_SERVO_POS_B, ID_SERVO);
                    servo_pos_b = true;
                }
            }
        }
        else
        {
            /* Fin del período de rotación: centrar y desactivar servo */
            put_event_task_actuator(EV_SERVO_CENTER, ID_SERVO);
            servo_active = false;
        }
    }

    /* --- Guardado periódico a EEPROM (backup ante corte de energía) --- */
    if (running_save_tick_cnt > 0)
    {
        running_save_tick_cnt--;
    }
    else
    {
        uint32_t now_sec = rtc_get_seconds();
        uint32_t remaining = (incubation_target_seconds > now_sec) ? (incubation_target_seconds - now_sec) : 0ul;

        running_save_tick_cnt = RUNNING_SAVE_TICKS;
        (void)task_memory_save_running(cfg_temp, cfg_hum, remaining);
    }

    /* --- Actualización periódica de pantalla + chequeo de finalización --- */
    if (incubation_tick_cnt > 0)
    {
        incubation_tick_cnt--;
    }
    else
    {
        incubation_tick_cnt = INCUBATION_UPDATE_TICKS;

        if (rtc_get_seconds() >= incubation_target_seconds)
        {
            /* Tiempo cumplido: detener servo, apagar calefactor, borrar
             * el respaldo de EEPROM (ya no hay nada que continuar) y avisar */
            servo_active = false;
            put_event_task_actuator(EV_SERVO_CENTER, ID_SERVO);
            if (heater_on)
            {
                put_event_task_actuator(EV_HEATER_OFF, ID_HEATER);
                heater_on = false;
            }
            (void)task_memory_clear_running();
            p_task_system_dta->state = ST_SYS_FINISHED;
            put_event_task_display(0, 0, "  INCUBACION    ");
            put_event_task_display(0, 1, "  FINALIZADA!   ");
        }
        else
        {
            display_incubating();
        }
    }
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
                    new_menu_cursor = NEW_MENU_OPT_ADJUST;
                    p_task_system_dta->state = ST_SYS_NEW_MENU;
                    display_new_menu();
                }
                /* UP y BACK no hacen nada en este estado */
            }
            break;

        /* ================================================================
         * SUBMENÚ DE NUEVO INICIO — AJUSTAR / PRESET 1 / 2 / 3
         * UP/DOWN mueven el cursor; ENTER confirma; BACK vuelve al menú raíz.
         * ================================================================ */
        case ST_SYS_NEW_MENU:

            if (p_task_system_dta->flag)
            {
                p_task_system_dta->flag = false;

                if (EV_SYS_DOWN == p_task_system_dta->event)
                {
                    if (new_menu_cursor < (NEW_MENU_OPT_QTY - 1u)) new_menu_cursor++;
                    display_new_menu();
                }
                else if (EV_SYS_UP == p_task_system_dta->event)
                {
                    if (new_menu_cursor > NEW_MENU_OPT_ADJUST) new_menu_cursor--;
                    display_new_menu();
                }
                else if (EV_SYS_BACK == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_MAIN_NEW;
                    display_main_new();
                }
                else if (EV_SYS_ENTER == p_task_system_dta->event)
                {
                    if (NEW_MENU_OPT_ADJUST == new_menu_cursor)
                    {
                        /* AJUSTAR: mismo camino manual de siempre */
                        cfg_temp  = TEMP_DEFAULT;
                        cfg_hum   = HUM_DEFAULT;
                        cfg_days  = DAYS_DEFAULT;
                        cfg_hours = HOURS_DEFAULT;
                        p_task_system_dta->state = ST_SYS_SET_TEMP;
                        display_set_temp();
                    }
                    else
                    {
                        /* PRESET 1/2/3: cargar de EEPROM y arrancar directo */
                        memory_preset_t preset;
                        uint8_t preset_index = new_menu_cursor - 1u;

                        if (task_memory_load_preset(preset_index, &preset))
                        {
                            cfg_temp  = preset.temp;
                            cfg_hum   = preset.hum;
                            cfg_days  = preset.days;
                            cfg_hours = preset.hours;
                        }
                        else
                        {
                            /* Fallback si falla la lectura I2C: valores
                             * por defecto, para no arrancar con basura */
                            cfg_temp  = TEMP_DEFAULT;
                            cfg_hum   = HUM_DEFAULT;
                            cfg_days  = DAYS_DEFAULT;
                            cfg_hours = HOURS_DEFAULT;
                        }

                        incubation_start();
                        servo_active   = true;
                        p_task_system_dta->state = ST_SYS_INCUBATING;
                        display_incubating();
                    }
                }
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
                    incubation_start();
                    p_task_system_dta->state = ST_SYS_INCUBATING;
                    display_incubating();
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
         * - Cuenta regresiva actualizada cada 60 segundos.
         * - Servo activo los primeros 18 días, 4 veces/día (cada 6hs),
         *   solo si cfg_days >= SERVO_MIN_CFG_DAYS.
         * ================================================================ */
        case ST_SYS_INCUBATING:

            p_task_system_dta->flag = false;
            servo_active   = true;
            incubation_run_common(p_task_system_dta);
            break;

        /* ================================================================
         * INCUBACIÓN FINALIZADA
         * Espera ENTER para volver al menú principal.
         * ================================================================ */
        case ST_SYS_FINISHED:

            if (p_task_system_dta->flag)
            {
                p_task_system_dta->flag = false;

                if (EV_SYS_ENTER == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_MAIN_NEW;
                    display_main_new();
                }
            }
            break;

            /* ================================================================
             * LEYENDO DATOS (desde CONTINUAR)
             * ================================================================ */
            case ST_SYS_READING:

                p_task_system_dta->flag = false;

                /* En este tick SOLO leemos la EEPROM */
                if (task_memory_load_running(&recovered_running) && (recovered_running.remaining_seconds > 0ul))
                {
                    p_task_system_dta->state = ST_SYS_LAST_DATA_SHOW; /* Nuevo estado intermedio */
                }
                else
                {
                    p_task_system_dta->state = ST_SYS_NO_DATA;
                    put_event_task_display(0, 0, "NO SE ENCUENTRAN");
                    put_event_task_display(0, 1, "     DATOS      ");
                }
                break;

            case ST_SYS_LAST_DATA_SHOW:

                /* En este tick formateamos y mostramos los datos */
                p_task_system_dta->state = ST_SYS_LAST_DATA;
                display_last_data();
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
                    /* "Pausar y resumir": no hay forma de saber cuánto tiempo
                     * real estuvo la placa sin energía (no hay RTC con
                     * respaldo), así que el cronómetro arranca de nuevo
                     * desde el remanente guardado. */
                    cfg_temp  = recovered_running.temp;
                    cfg_hum   = recovered_running.hum;
                    incubation_target_seconds = rtc_get_seconds() + recovered_running.remaining_seconds;

                    incubation_tick_cnt     = INCUBATION_UPDATE_TICKS;
                    display_alt_cnt         = 0;
                    running_save_tick_cnt   = RUNNING_SAVE_TICKS;
                    heater_on                = false; /* se re-evalúa solo en el próximo tick */
                    servo_active             = false;  /* rotación no se retoma tras un corte */

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

            p_task_system_dta->flag = false;
            incubation_run_common(p_task_system_dta);
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
