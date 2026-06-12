
 <img src="https://www.fi.uba.ar/images/logo-fiuba.png" alt="image2" width="50%">

**Incubadora de huevos automática**

**Autores: Agustín Achinelli y Pablo Martínez Madero**

**Padrones: 110124; 106516**

**Fecha: 1er cuatrimestre 2026**


### **1. Selección del proyecto a implementar**

#### **1.1 Objetivo del proyecto y resultados esperados**

El objetivo de este proyecto es diseñar e implementar un mecanismo automático de cuidado de huevos para aves domésticas que permita **medir, registrar y modificar** variables ambientales (temperatura y humedad) en un ambiente controlado, e implementar **calentamiento y rotación de los huevos** para asegurar su desarrollo correcto. Además, se proyecta diseñar distintos modos de funcionamiento según el tipo de huevo y etapa de su desarrollo, seleccionables a través de una interfaz gráfica compuesta por un display LCD y un teclado matricial.

Los resultados esperados incluyen: un sistema capaz de mantener la temperatura y la humedad dentro de los rangos óptimos para cada especie, la rotación periódica y automatizada de los huevos, el almacenamiento del historial de variables en una memoria EEPROM externa y la notificación auditiva y visual ante eventos relevantes (alarmas, confirmaciones, cambio de etapa). El sistema deberá operar de forma autónoma, sin intervención del usuario una vez configurado el modo de incubación.

#### **1.2 Proyectos similares**

Se consideran tres tipos de proyectos posibles que cumplen con los objetivos y resultados esperados del proyecto, todos basados en el control de temperatura y humedad en un espacio reducido:

1. Incubadora de huevos domésticos.
2. Incubadora neonatal.
3. Invernadero automatizado.

Para comparar estas alternativas, se tienen en cuenta seis aspectos característicos:

1. Disponibilidad del hardware: Se evalúa si el proyecto es fácilmente implementable con hardware disponible en mercados regionales.
2. Facilidad de uso: Se comprende por facilidad de uso a la sencillez con la que un usuario sin conocimientos técnicos puede operar el sistema.
3. Responsabilidad: Se considera el nivel de riesgo asociado a un fallo del sistema; a mayor consecuencia de fallo, mayor ponderación.
4. Tiempo de implementación: Tiempo total que llevará diseñar e implementar el proyecto seleccionado.
5. Costo: El costo total estimado del proyecto.
6. Interés personal: La motivación de los autores para desarrollar el proyecto.

Se decide ponderar los aspectos de la siguiente manera, del 1 al 10: **Disponibilidad del hardware: 10, Facilidad de uso: 7, Responsabilidad: 9, Tiempo de implementación: 8, Costo: 6, Interés personal: 7.**

La disponibilidad del hardware se pondera con el valor máximo dado que el proyecto debe realizarse íntegramente con componentes adquiribles en el mercado local argentino. La responsabilidad tiene un puntaje alto ya que un fallo en el sistema puede resultar en la pérdida de los huevos incubados, lo que implica consecuencias económicas y biológicas. El tiempo de implementación es relevante dado el período acotado de la cursada. El costo es un factor secundario, y el interés personal refleja la motivación por aplicar sistemas embebidos a un problema concreto del ámbito agropecuario.

La siguiente tabla (Tabla 1.2.1) muestra los valores ponderados asignados a cada proyecto considerado:

| Criterio | Incubadora de huevos domésticos | | Incubadora neonatal | | Invernadero | |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| | **Puntaje** | **Pond.** | **Puntaje** | **Pond.** | **Puntaje** | **Pond.** |
| Disponibilidad del hardware (peso: 10) | 9 | 90 | 7 | 70 | 9 | 90 |
| Facilidad de uso (peso: 7) | 8 | 56 | 6 | 42 | 8 | 56 |
| Responsabilidad (peso: 9) | 7 | 63 | 4 | 36 | 8 | 72 |
| Tiempo de implementación (peso: 8) | 8 | 64 | 6 | 48 | 8 | 64 |
| Costo (peso: 6) | 8 | 48 | 5 | 30 | 7 | 42 |
| Interés personal (peso: 7) | 9 | 63 | 6 | 42 | 5 | 35 |
| **TOTAL** | | **384** | | **268** | | **359** |

*Tabla 1.2.1: Comparación ponderada de proyectos alternativos*

## **1.3 Selección del proyecto**
Considerando la Tabla 1.2.1, se elige seguir con el proyecto de **incubadora de huevos domésticos**, utilizando LEDs, pulsadores, pantalla LCD, sensor de temperatura y humedad DHT22, buzzer y memoria EEPROM externa. Este proyecto obtiene la mayor puntuación ponderada (384 puntos) debido a su excelente disponibilidad de hardware, su facilidad de implementación y el elevado interés personal de los autores.

La incubadora neonatal tiene un enfoque similar al proporcionar un ambiente controlado para seres vivos, pero su nivel de responsabilidad es considerablemente mayor: un fallo podría implicar consecuencias directas sobre la salud de una persona. Esto la convierte en una opción de muy alta exigencia normativa y técnica, no adecuada para un proyecto académico. Además, requiere sensores y actuadores de mayor precisión y costo, reduciendo su puntaje en disponibilidad y costo.

El invernadero automatizado, si bien es viable tecnológicamente, no permite aprovechar en profundidad los periféricos específicos del microcontrolador STM32 orientados al control de procesos críticos (rotación sincronizada, alarmas de temperatura) ni desarrollar una arquitectura modular más compleja orientada a ciclos de vida. Su puntaje en interés personal es el más bajo de las tres opciones.

El proyecto seleccionado se destaca especialmente por incorporar múltiples modos de incubación configurables (gallina, pato, codorniz), un registro histórico de variables en EEPROM y un sistema de alertas multimodal (sonoro y visual). Esto lo diferencia de otros proyectos similares disponibles en el mercado, que suelen ofrecer un control fijo de temperatura sin personalización ni registro.

Los desafíos principales de este proyecto son la integración de los distintos periféricos (sensor DHT22, motor de paso, LCD, teclado), la precisión del control de temperatura y humedad, el manejo de interrupciones para el teclado y la correcta persistencia de datos ante cortes de energía.
### **1.3.1 Diagrama en bloques**
En la Figura 1.3.1 se muestra el diagrama en bloques del sistema con los principales módulos del proyecto.

<p align="center">
  <img src="https://github.com/user-attachments/assets/ac7a9076-2dc2-4aa0-80a8-0ea0f44c618c" width="60%">
</p>
<p align="center"><em>Figura 1.3.1: Diagrama en bloques del sistema</em></p>

# **2. Elicitación de requisitos y casos de uso**
En el mercado argentino e internacional existen a la venta varios productos similares que proporcionan solución a la incubación automática de huevos. Sin embargo, la mayoría de los productos comerciales disponibles localmente no combinan en un único dispositivo el control preciso de temperatura y humedad, la rotación automatizada con calendario configurable, el soporte para múltiples especies y el registro histórico de variables.

Como primer competidor del mercado se puede mencionar a **TINZER**, empresa que ofrece incubadoras para uso doméstico con control de temperatura básico y automática. Sus productos tienen precios que oscilan entre los USD 250 y USD 350, y no incluyen pantalla de seguimiento del ciclo, ni selector de modos.

Un segundo competidor es **Brinsea**, fabricante inglés con presencia en el mercado online argentino. Ofrece incubadoras con control digital de temperatura y humedad, rotación automática y alarmas. Sus precios rondan los USD 150 a USD 500. Sin embargo, no permiten configurar modos por especie ni almacenan histórico de variables, lo que limita el seguimiento del proceso.

En resumen, el mercado ofrece soluciones parciales que cubren algunos de los objetivos del proyecto, pero ninguna combina la personalización por modo de incubación, el registro en EEPROM y la interfaz configurable mediante display y teclado. Esto justifica el desarrollo de un sistema propio que cubra todas estas funcionalidades con hardware disponible localmente.

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

*Tabla 2.1: Requisitos del proyecto*

En las tablas 2.2 a 2.4 se presentan 3 casos de uso para el sistema.

|**Disparador**|El usuario desea iniciar un ciclo de incubación seleccionando un modo y confirmando los parámetros.|
| :- | :- |
|**Precondiciones**|El sistema está encendido. No hay un ciclo de incubación activo. La pantalla LCD muestra el menú principal.|
|**Flujo principal**|El usuario navega con el teclado hasta el menú «Nuevo ciclo». Selecciona el tipo de huevo (gallina, pato o codorniz). El sistema muestra en el display los parámetros predefinidos para ese modo (temperatura, humedad, duración, intervalo de rotación). El usuario confirma con la tecla de aceptar. El sistema guarda la configuración en la EEPROM y comienza el ciclo: activa el calentador si es necesario, programa las rotaciones y emite un pitido de confirmación. El LCD pasa a mostrar la pantalla de monitoreo con temperatura, humedad y día del ciclo.|
|**Flujos alternativos**|a. El usuario cancela la selección con la tecla de retroceso: el sistema vuelve al menú principal sin iniciar ningún ciclo. b. La temperatura inicial está fuera del rango objetivo: el sistema activa inmediatamente el calentador y espera hasta alcanzar el rango antes de dar la confirmación sonora.|

*Tabla 2.2: Caso de uso 1 — El usuario inicia un ciclo de incubación*

|**Disparador**|La temperatura interior sale del rango permitido para el modo activo.|
| :- | :- |
|**Precondiciones**|El sistema está encendido con un ciclo de incubación activo. El sensor DHT22 está operativo.|
|**Flujo principal**|El sensor detecta que la temperatura supera o cae por debajo del umbral configurado. El sistema activa o desactiva el elemento calefactor según corresponda. El LED de calentador activo se enciende o apaga acorde. El LCD actualiza el valor de temperatura en tiempo real. Cuando la temperatura vuelve al rango, el sistema estabiliza el calentador y registra el evento en la EEPROM.|
|**Flujos alternativos**|a. La temperatura supera el umbral crítico (fallo grave): el sistema emite una alarma sonora prolongada mediante el buzzer, parpadea el LED de alarma y muestra un mensaje de error en el LCD hasta que el usuario reconozca la alarma. b. El sensor no responde: el sistema emite una alarma y detiene el ciclo de forma segura, registrando el fallo en la EEPROM.|

*Tabla 2.3: Caso de uso 2 — El sistema detecta temperatura fuera de rango*

|**Disparador**|Se alcanza el intervalo de tiempo programado para la rotación de huevos.|
| :- | :- |
|**Precondiciones**|El sistema está encendido con un ciclo de incubación activo. No se está en el período de lockdown (últimos días antes de la eclosión).|
|**Flujo principal**|El temporizador de rotación expira. El sistema activa el motor de paso para girar los huevos el ángulo configurado (típicamente 45° alternados). El LED de rotación parpadea durante el movimiento. El buzzer emite un pitido breve de confirmación. El sistema registra el evento de rotación en la EEPROM con timestamp. El motor se detiene y el sistema reanuda el monitoreo normal.|
|**Flujos alternativos**|a. El motor no completa el movimiento en el tiempo esperado (posible traba mecánica): el sistema emite una alarma sonora y visual, detiene el motor y registra el fallo en la EEPROM. b. El ciclo está en período de lockdown: el sistema omite la rotación, registra el evento y muestra en el LCD un mensaje indicando que la rotación está inhibida.|

*Tabla 2.4: Caso de uso 3 — El sistema rota los huevos automáticamente*

