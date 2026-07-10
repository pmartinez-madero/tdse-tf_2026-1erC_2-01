<p align="center">
 <img src="https://www.fi.uba.ar/images/logo-fiuba.png" aling="center" alt="image2" width="50%">
</p>

# Memoria del trabajo final: [Incubadora de huevos automática]

<table align="center">
  <tr>
    <th>Autor</th>
    <th>Padrón</th>
    <th>Mail</th>
  </tr>
  <tr>
    <td>Achinelli, Agustín Ezequiel</td>
    <td>110124</td>
    <td>aachinelli@fi.uba.ar</td>
  </tr>
  <tr>
    <td>Martinez Madero, Pablo</td>
    <td>106516</td>
    <td>pmartinezm@fi.uba.ar</td>
  </tr>
</table>

<p align="center">
  2026 | 1er Cuatrimestre
</p>

<p align="center">
  Taller de Sistemas Embebidos (TA134)
</p>

<p align="center">
  Universidad de Buenos Aires | Facultad de Ingeniería
</p>

<p align="center">
  Este trabajo ha sido realizado en la ciudad de Haedo y en la ciudad de Tigre, entre junio de 2026 y agosto de 2026.
</p>

# Resumen

En el trabajo a continuación se detallará el desarrollo de una incubadora de huevos automática, teniendo como soporte principal la placa NUCLEO-F103RB y código en lenguaje C. Para llevar a cabo este proyecto, se utilizarán varios módulos descriptos a continuación que se integrarán en conjunto para cumplir con los requisitos propuestos.
La idea del mismo es integrar en conjunto los conceptos vistos en la cátedra tales como lectura analógica digital, comunicación SPI, PWM, entre otros.

# Índice general

- [Registro de versiones](#registro-de-versiones)
- [Introducción general](#introducción-general)
  - [1.1. Objetivo del trabajo](#11-objetivo-del-trabajo)
  - [1.2. Funcionamiento de una incubadora](#13-funcionamiento-de-una-incubadora)
  - [1.3. Desarrollo de las funcionalidades en el microcontrolador](#14-desarrollo-de-las-funcionalidades-en-el-microcontrolador)
- [Introducción específica](#introducción-específica)
  - [2.1. Requisitos](#21-requisitos)
  - [2.2. Servomotor](#25-servomotor)
  - [2.3. Sensor DHT22](#25-sensor)
  - [2.4. LCD 16X2](#25-lcd)
- [Diseño e implementación](#diseño-e-implementación)
  - [3.1. Hardware](#31-hardware)
    - [3.1.1. Placa con microcontrolador](#311-placa-con-microcontrolador)
    - [3.1.2. Teclado de membrana](#313-teclado)
    - [3.1.3. LEDs](#314-leds)
  - [3.2. Firmware del microcontrolador](#32-firmware-del-microcontrolador)
- [Ensayos y resultados](#ensayos-y-resultados)
  - [4.1. Pruebas funcionales del hardware](#41-pruebas-funcionales-del-hardware)
  - [4.2. Pruebas funcionales del firmware](#42-pruebas-funcionales-del-firmware)
  - [4.3. Pruebas de integración](#43-pruebas-de-integración)
  - [4.4. Circuito esquemático](#44-circuito-esquemático)
  - [4.5. Medición y análisis del consumo energético](#45-medición-y-análisis-del-consumo-energético)
  - [4.6. Medición y análisis de tiempos de ejecución](#46-medición-y-análisis-de-tiempos-de-ejecución)
  - [4.7. Cumplimiento de requisitos](#47-cumplimiento-de-requisitos)
  - [4.8. Comparación con otros sistemas similares](#48-comparación-con-otros-sistemas-similares)
  - [4.9. Documentación del desarrollo realizado](#49-documentación-del-desarrollo-realizado)
- [Conclusiones](#conclusiones)
  - [5.1. Resultados obtenidos](#51-resultados-obtenidos)
  - [5.2. Próximos pasos](#52-próximos-pasos)
- [Bibliografía](#bibliografía)

# Registro de versiones

| Revisión | Cambios realizados | Fecha |
| --- | --- | --- |
| Versión 1.0 | N/A | 10/7/2026 |


# CAPÍTULO 1
# Introducción general

## 1.1. Objetivo del trabajo

El objetivo de este proyecto es diseñar e implementar un mecanismo automático de cuidado de huevos para aves domésticas que permita medir, 
registrar y modificar variables ambientales (temperatura y humedad) en un ambiente controlado, e implementar calentamiento y rotación de los huevos para asegurar su desarrollo correcto. 
Además, se proyecta diseñar distintos modos de funcionamiento según el tipo de huevo y etapa de su desarrollo, seleccionables a través de una interfaz gráfica compuesta por un display LCD y un teclado matricial.


## 1.2. Funcionamiento de una incubadora

La idea general de una incubadora doméstica, que es a lo que se apunta con este proyecto, es que una vez encendido el prototipo se pueda comenzar a incubar un nuevo tipo de especie, cargando los parámetros que se requieren desde 0, ya precargados en la memoria, o bien continuar con este proceso en caso de alguna interrupción externa. 
Luego de haber seteado los parámetros de una u otra forma, comienza a calentar o ventilar el interior de la incubadora hasta llegar a los valores fijados que serán sensados cada cierto tiempo para obtener el control del mismo. Es importante que durante lo que dure el proceso según el tipo de huevo se muestren por pantalla los valores actuales del interior y notifique en caso de un exceso o disminución de los mismos.


## 1.3. Desarrollo de las funcionalidades en el microcontrolador
Para llevar a cabo las funciones, desde la programación se diseña un ejecutor cíclico el cuál va realizando el llamado a las distintas tareas que se requieren dentro del sistema la limitación en recorrer todas las operaciones es de un milisegundo de forma que se vaya ejecutando en tiempo real.
Para sensar la temperatura y humedad se optó por el DHT22 que integra ambas funciones y se maneja gracias a una librería externa; El LCD 16X2 que se utiliza para mostrar al usuario se maneja con el protocolo SPI y también es llamado con funciones de librería externa.
El control para las opciones dentro del menú se realiza con una botonera del tipo membrana 4X1 configurada como pines de entrada a la placa y gracias al control del ciclo de trabajo por PWM, controlamos la posición de un servomotor.



# CAPÍTULO 2
# Introducción específica

## 2.1. Requisitos

En la siguiente tabla se pueden agrupar por módulos los requerimientos del proyecto

|**Grupo**|**ID**|**Descripción**|
| :-: | :-: | :-: |
|**Temperatura**|1\.1|El sistema medirá continuamente la temperatura interior mediante un sensor dedicado.|
||1\.2|El sistema activará el elemento calefactor cuando la temperatura esté por debajo del umbral configurado.|
||1\.3|El sistema desactivará el elemento calefactor cuando la temperatura supere el umbral configurado.|
|**Humedad**|2\.1|El sistema medirá continuamente la humedad relativa interior.|
||2\.2|El sistema activará el humidificador cuando la humedad esté por debajo del umbral configurado.|
||2\.3|El sistema desactivará el humidificador cuando la humedad supere el umbral configurado.|
|**Rotación**|3\.1|El sistema rotará los huevos automáticamente a intervalos de tiempo configurables por tipo de huevo.|
||3\.2|El sistema indicará mediante un LED cada vez que se realice una rotación.|
||3\.3|El sistema inhibirá la rotación durante los últimos días del ciclo de incubación (lockdown), según el modo seleccionado.|
|**Interfaz de usuario**|4\.1|El sistema contará con una pantalla LCD para mostrar temperatura, humedad, día del ciclo y modo activo.|
||4\.2|El sistema contará con un teclado matricial para navegar entre menús y configurar parámetros.|
||4\.3|El sistema contará con LEDs indicadores de estado: calentador activo, humidificador activo y rotación.|
||4\.4|El sistema contará con un buzzer para alertas sonoras (alarmas y confirmaciones).|
|**Modos de incubación**|5\.1|El sistema soportará al menos tres modos predefinidos: huevo de gallina, pato y codorniz, con sus respectivos parámetros de temperatura, humedad y ciclo.|
||5\.2|El usuario podrá seleccionar el modo desde el menú de la interfaz gráfica.|
||5\.3|El sistema indicará en el display el día actual del ciclo de incubación.|
|**Alarmas**|6\.1|El sistema emitirá una alarma sonora y visual si la temperatura supera un umbral crítico configurable.|
||6\.2|El sistema emitirá una alarma sonora y visual si la humedad supera o cae por debajo de umbrales críticos.|
||6\.3|El sistema emitirá una alarma si el motor de rotación no completa el giro en el tiempo esperado.|
|**Almacenamiento**|7\.1|El sistema almacenará en la EEPROM externa el historial de temperatura, humedad y eventos de rotación.|
||7\.2|El sistema preservará la configuración activa y el día del ciclo ante cortes de energía.|


## 2.2. Servomotor
Para simular la rotación del soporte para los huevos, fue conveniente considerar una salida PWM del microcontrolador y controlar el servomotor SG90, 
que si bien tiene un bajo consumo de corriente es uno de los dispositivos que se debe alimentar por fuera de la placa ya que por algún esfuerzo en particular puede superar la corriente máxima del pin y quemar este puerto.

<p align="center">
 <img src="https://github.com/aachinelli/TDSE_INT_PF/blob/85cc1a70a32646fde2cac301acd4e5d191e399b5/servo.jpg" alt="image2" width="40%">
</p>

## 2.3. Sensor DHT22
Este sensor se eligió ya que integra ambos sensores y se puede controlar mediante un solo pin de datos. Mide temperaturas de -40°C a 80°C con ± 0.5°C de precisión gracias a su termistor interno y humedad del 0% al 100% con una exactitud del 2% gracias un sensor capacitivo que envían señales analógicas a un pequeño chip integrado que se encarga de convertirlas en una señal digital.

<p align="center">
 <img src="https://github.com/aachinelli/TDSE_INT_PF/blob/5c0944f3a0e169877e78a95acd58ae4edaa9749e/dht22.jpg" alt="image2" width="25%">
</p>

## 2.4. Sensor LCD 16X2
Este display alfanumérico se consideró a modo de interfaz con el usuario para poder establecer comunicación con el sistema a través de la selección en el menú, los modos de incubación y realizar una visualización en tiempo real del estado actual.

<p align="center">
 <img src="https://github.com/aachinelli/TDSE_INT_PF/blob/85cc1a70a32646fde2cac301acd4e5d191e399b5/lcd_16x2.jpg" alt="image2" width="40%">
</p>

# CAPÍTULO 3
# Diseño e implementación

## 3.1. Hardware
En la figura que sigue se muestra el diagrama en bloques del proyecto general par tener una visión más abstracta de cada módulo y su conexión con la placa.
<p align="center">
 <img src="https://github.com/aachinelli/TDSE_INT_PF/blob/5c0944f3a0e169877e78a95acd58ae4edaa9749e/diag_bloques.jpeg" alt="image2" width="50%">
</p>

### 3.1.1. Placa con microcontrolador
El trabajo se desarrolló en torno a la placa NUCLEO-F103RB de STMicroelectronics, la cual cuenta con un procesador ARM 32-bit Cortex-M3. Se utilizó dicha placa por haber sido la plataforma adoptada por la cátedra en el presente cuatrimestre, la cual cuenta con todas las características necesarias para desarrollar el trabajo.
En lo que respecta a la alimentación, durante los momentos de pruebas en el desarrollo desde el puerto USB, si bien es suficiente al no tener los dispositivos que más consumen, sería de amplia ventaja el diseño de un bus general de alimentación y desde ahí centrarse en alimentar la placa.

<p align="center">
 <img src="https://github.com/aachinelli/TDSE_INT_PF/blob/5c0944f3a0e169877e78a95acd58ae4edaa9749e/nucleo_f103rb.jpg" alt="image2" width="40%">
</p>


### 3.1.2. Teclado de membrana
Configurados como pines de entrada, se utilizarán por el usuario para manejarse por el menú de la incubadora siendo: arriba, abajo, entrar y volver.

<p align="center">
 <img src="https://github.com/aachinelli/TDSE_INT_PF/blob/5c0944f3a0e169877e78a95acd58ae4edaa9749e/tecla_mem.jpg" alt="image2" width="40%">
</p>

### 3.1.3. LEDs
Se tendrán en consideración 3 leds testigos que indiquen estratégicamente falta de tensión o falla de algún tipo dentro del proceso de incubación.

<p align="center">
 <img src="https://github.com/aachinelli/TDSE_INT_PF/blob/5c0944f3a0e169877e78a95acd58ae4edaa9749e/leds.png" alt="image2" width="40%">
</p>

## 3.2. Firmware del microcontrolador

El firmware ha sido desarrolado implementando en lenguaje C un gestor cíclico de tareas con cuatro tareas bajo su control. Cada tarea tiene una interfaz asociada para recibir eventos provenientes de las otras tareas y se toma como base, uno de los códigos utilizados en la práctica de la cátedra donde el código comienza en el main.c, el cual únicamente inicializa los periféricos y pasa el control a app.c. Seguidamente, cyclic_executive se encarga de inicializar el DWT (Data Watchpoint and Trace) para poder medir el tiempo con alta precisión, posteriormente inicializa todas las tareas, para comenzar a ejecutar todas, cada vez que se produce un SysTick, una vez por milisegundo.

Cyclic_executive se encarga también de medir el peor tiempo de ejecución (WCET) de cada tarea, contando el tiempo que le lleva a cada una actualizarse, cada vez que da dicha orden. Esto permite mantener los márgenes de ejecución de cada tarea dentro de un marco lo más estable y predecible posible.

El flujo general del código está orientado en el archivo 'task_system.c' donde se tiene una estructura del tipo 'task_system_dta_t' con 3 campos: 'state' indicando el lugar donde esta en el menú, 'event' para saber qué botón se presionó y 'flag' que registra si hay un evento pendiente. En cada pasada del ejecutor está pendiente si se lee algo desde 'task_sensor' y comanda a la interfaz de los actuadores o del display para mostrar algo en pantalla.

# CAPÍTULO 4
# Ensayos y resultados

## 4.1. Pruebas funcionales del hardware

Se propuso que el eje principal del proyecto como así de los ensayos previos, sea la codificación del menú ya que es el que va a dictar sobre el caso de uso que se le esté dando a la incubadora, y a raíz de esto comenzar a integrar para cada caso las partes externas que se requieran en cada momento del proceso.

En primera instancia se procese a diseñar un menu interactivo con los distintos flujos que puede tener a partir de la interacción con los botones programados.

<p align="center">
 <img src="https://github.com/aachinelli/TDSE_INT_PF/blob/9063ff282824e0079197323261d128a90a2382f7/flujo_menu.png" alt="image2" width="85%">
</p>

Una vez codificado se debuggea para depurar posibles errores y corroborar que funciones con la lógica del ejecutor.

<p align="center">
 <img src="https://github.com/aachinelli/TDSE_INT_PF/blob/4341f2d81e6b30fb72fb8ade3ceb7649e71eb6a2/menu_0.jpg" alt="image2" width="42%">
 <img src="https://github.com/aachinelli/TDSE_INT_PF/blob/4341f2d81e6b30fb72fb8ade3ceb7649e71eb6a2/menu_1.jpg" alt="image2" width="40%">
</p>

En lo que respecta al sensor principal de temperatura y humedad, tambíen se pudo realizar un ensayo para probar el funcionamiento de su librería, su inicialización dentro de código y el protocolo de comunicación interno dado por su hoja de características. Si bien no está integrado aún, con el osciloscopio se pudo extraer una imagen tomada desde el pin de datos cada vez que actualiza la medición y desde el debugger se comprobó el correcto funcionamiento.

<p align="center">
 <img src="https://github.com/aachinelli/TDSE_INT_PF/blob/4341f2d81e6b30fb72fb8ade3ceb7649e71eb6a2/osc_dht.jpg" alt="image2" width="45%">
</p>


## 4.2. Pruebas funcionales del firmware


## 4.3. Pruebas de integración


## 4.4. Circuito esquemático


## 4.5. Medición y análisis del consumo energético



## 4.6. Medición y análisis de tiempos de ejecución



## 4.7. Cumplimiento de requisitos



## 4.8. Comparación con otros sistemas similares

(...) Comparación resaltando principalmente ventajas de este sistema con respecto a otros (...)

## 4.9. Documentación del desarrollo realizado



# CAPÍTULO 5
# Conclusiones

## 5.1. Resultados obtenidos


## 5.2. Próximos pasos

(...) Próximos pasos a implementar para expandir el alcance del trabajo (...)

# Bibliografía

El trabajo se realizó exclusivamente con los ejemplos provistos por la cátedra, algunos recursos de internet y consultas varias a las distintas IA.

\[1\] Sensor DHT22. Ejemplo de aplicación - https://www.youtube.com/watch?v=09C1dyXvSbg - https://controllerstech.com/temperature-measurement-using-dht22-in-stm32/

\[2\] Placa NUCLEO-F103RB. [Online]. Available: https://os.mbed.com/platforms/ST-Nucleo-F103RB

\[3\] MB1136 - Electrical Schematic - STM32 Nucleo-64 boards. [Online]. Available: https://www.st.com/resource/en/schematic_pack/mb1136-default-c04_schematic.pdf

\[4\] LCD 16X2. Ejemplo de aplicación - https://www.youtube.com/watch?v=czcEovgO4Gk

