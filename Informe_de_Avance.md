## **Incubadora: informe de avance**

Autores: Agustin Ezequiel Achinelli -- Pablo Martinez Madero

Fecha: 12/06/2026 -- 1er Cuatrimestre



Se detallará a continuación el estado de cada uno de los requerimientos del sistema siguiendo las referencias:



| Estado | Descripción      |
|-----|---------------------|
| 🟢 | Implementado |
| 🟡 | Sin Implementar |
| 🔴 | Requerimiento descartado |

| ESTADO | GRUPO | ID | TAREA |
|:--:|----|----|----|
| 🟢 | Teperatura | 1.1 | El sistema medirá continuamente la temperatura interior mediante un sensor dedicado. |
| 🟡 |   | 1.2 | El sistema activará el elemento calefactor cuando la temperatura esté por debajo del umbral configurado. |
| 🟡 |   | 1.3 | El sistema desactivará el elemento calefactor cuando la temperatura supere el umbral configurado. |
| 🟢 | Humedad | 2.1 | El sistema medirá continuamente la humedad relativa interior. |
| 🟡 |   | 2.2 | El sistema activará el humidificador cuando la humedad esté por debajo del umbral configurado. |
| 🟡 |   | 2.3 | El sistema desactivará el humidificador cuando la humedad supere el umbral configurado. |
| 🟡 | Rotación | 3.1 | El sistema rotará los huevos automáticamente a intervalos de tiempo configurables por tipo de huevo. |
| 🟡 |   | 3.2 | El sistema indicará mediante un LED cada vez que se realice una rotación. |
| 🟡 |   | 3.3 | El sistema inhibirá la rotación durante los últimos días del ciclo de incubación (lockdown), según el modo seleccionado. |
| 🟡 | Interfaz | 4.1 | El sistema contará con una pantalla LCD para mostrar temperatura, humedad, día del ciclo y modo activo. |
| 🟢 |   | 4.2 | El sistema contará con un teclado matricial para navegar entre menús y configurar parámetros. |
| 🟡 |   | 4.3 | El sistema contará con LEDs indicadores de estado: calentador activo, humidificador activo y rotación. |
| 🟡 |   | 4.4 | El sistema contará con un buzzer para alertas sonoras (alarmas y confirmaciones). |
| 🟡 | Modos de incubación | 5.1 | El sistema soportará al menos tres modos predefinidos: huevo de gallina, pato y codorniz, con sus respectivos parámetros de temperatura, humedad y ciclo. |
| 🟡 |   | 5.2 | El usuario podrá seleccionar el modo desde el menú de la interfaz gráfica. |
| 🟡 |   | 5.3 | El sistema indicará en el display el día actual del ciclo de incubación. |
| 🟡 | Alarmas | 6.1 | El sistema emitirá una alarma sonora y visual si la temperatura supera un umbral crítico configurable. |
| 🟡 |   | 6.2 | El sistema emitirá una alarma sonora y visual si la humedad supera o cae por debajo de umbrales críticos. |
| 🟡 |   | 6.3 | El sistema emitirá una alarma si el motor de rotación no completa el giro en el tiempo esperado. |
| 🟡 | Almacenamiento | 7.1 | El sistema almacenará en la EEPROM externa el historial de temperatura, humedad y eventos de rotación. |
| 🟡 |   | 7.2 | El sistema preservará la configuración activa y el día del ciclo ante cortes de energía. |
