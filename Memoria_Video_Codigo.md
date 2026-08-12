<div align="center">

<img width="535"  alt="image" src="https://github.com/Embebidos-Fran-Marcos-Nacho/tdse-tf_1-2/blob/08290a7a62c8a7d3fcd22fc57871dafbbf35ab15/logo-fiuba.png" />

**UNIVERSIDAD DE BUENOS AIRES**  
**Facultad de Ingeniería**  
**TA134 – Sistemas Embebidos**  
Curso 2 – Grupo 1

# Incubadora automática de huevos de ave  
Sistema de monitoreo atmósferico, control de humedad y temperatura y rotación de huevos.

## Autores
Pablo Martínez Madero — Legajo 106516  
Agustín Ezequiel Achinelli — Legajo 110124


**Fecha:** 08/08/2026  
**Cuatrimestre de cursada:** 1er cuatrimestre 2026  

*Trabajo realizado en Haedo, Tigre y la Ciudad Autonóma de Buenos Aires entre junio y agosto 2026.*
</div>

---

## Resumen
---

Se desarrolló el prototipo de una incubadora de huevos automática, un sistema embebido en un microcontrolador.
El mismo cuenta con sensores, displays y actuadores como periféricos sobre la arquitectura STM32. Se utilizó una placa NUCLEOF103RB.

Esta memoria justifica su necesidad, detalla los requisitos y proporciona información y esquemas sobre su diseño en hardware y firmware. Además, se documentan los ensayos y pruebas realizados y el estado final del proyecto.


---

## Registro de versiones

*Historial de revisiones del documento.*

La Tabla 0.1 resume el historial de revisiones y entregas de esta memoria.

| Revisión | Cambios realizados | Fecha |
| :---: | --- | :---: |
| 1.0 | Escritura inicial de la memoria | 02/08/2026 |
| 1.1 | Primer entrega: sin video | 08/08/2026 |
| 1.2 | Corrección de errores de formato y agregado del video. | 12/08/2026 |
<p align="center">
  <b>Tabla 0.1</b>: Registro de versiones del documento.
</p>
---



# Índice General

- [Capítulo 1: Introducción general](#capítulo-1-introducción-general)
  - [1.1 Análisis de necesidad y objetivo](#11-análisis-de-necesidad-y-objetivo)
  - [1.2 Productos comparables](#12-productos-comparables)
  - [1.3 Alcance y limitaciones](#13-alcance-y-limitaciones)
- [Capítulo 2: Introducción específica](#capítulo-2-introducción-específica)
  - [2.1 Requisitos](#21-requisitos)
  - [2.2 Casos de uso](#22-casos-de-uso)
  - [2.3 Descripción de módulos y Hardware](#23-descripción-de-módulos-y-Hardware)
    - [2.3.1 Módulo de control (NUCLEO-F103RB)](#231-módulo-de-control)
    - [2.3.2 Módulo sensor de temperatura y humedad ](#232-módulo-sensor-de-temperatura-y-humedad)
    - [2.3.3 Módulo de actuadores](#233-módulo-de-actuadores)
    - [2.3.4 Módulo de interfaz](#234-módulo-de-interfaz)
    - [2.3.5 Memoria EEPROM](#235-memoria-eeprom)
- [Capítulo 3: Diseño e implementación](#capítulo-3-diseño-e-implementación)
  - [3.1 Arquitectura general](#31-arquitectura-general)
  - [3.2 Diseño de hardware](#32-diseño-de-hardware)
  - [3.3 Diseño de firmware](#33-diseño-de-firmware)
    - [3.3.1 Arquitectura de ejecución](#231)
    - [3.3.2 Máquina de estados del sistema](#232)
- [Capítulo 4: Ensayos y resultados](#capítulo-4-ensayos-y-resultados)
  - [4.1 Integración](#41-integración)
  - [4.2 Pruebas funcionales de hardware Y software](#42-pruebas-funcionales-de-hardware-y-software)
  - [4.3 Ocupación de memoria: Console & Build Analyzer](#43-ocupación-de-memoria-console-&-build-analyzer)
  - [4.4 Medición y análisis de tiempos de ejecución (WCET)](#44-medición-y-análisis-de-tiempos-de-ejecución-(WCET))
  - [4.5 Cálculo del Factor de Uso (U) de la CPU](#45-Cálculo-del-Factor-de-Uso-(U)-de-la-CPU)
  - [4.6 Medición y análisis de consumo](#46-medición-y-análisis-de-consumo)
  - [4.7 Cumplimiento de requisitos](#47-cumplimiento-de-requisitos)
- [Capítulo 5: Conclusiones](#capítulo-5-conclusiones)
  - [5.1 Resultados obtenidos](#51-resultados-obtenidos)
  - [5.2 Posibles mejoras](#52-posibles-mejoras)
  - [5.3 Aprendizajes](#53-aprendizajes)
- [Capítulo 6: Uso de herramientas de IA](#capítulo-6-uso-de-herramientas-de-ia)
- [Capítulo 7: Bibliografía y referencias](#capítulo-7-bibliografía-y-referencias)





# Capítulo 1: Introducción general

## 1.1 Análisis de necesidad y objetivo

Las incubadoras de huevos de ave cubren una necesidad industrial y doméstica que puede ser abordada por un sistema embebido. Su funcionamiento requiere mantener una 
temperatura y humedad ideal de incubación según el tipo de ave durante un período de tiempo (algunas semanas). También requieren una rotación periódica de los huevos durante las primeras semanas. El objetivo de este trabajo es diseñar e implementar un prototipo 
funcional de incubadora utilizando la arquitecura STM32.


## 1.2 Productos comparables

Se realizó un relevamiento del mercado local y se encontraron 2 alternativas similares, una de estilo industrial, de la marca Tinzer, y otra de un estilo casero o de baja gama, de la marca Brinsea.


1. **Incubadora estilo industrial Tinzer**  

La marca Tinzer vende incubadoras para uso doméstico e industrial. Estas incubadoras tienen capacidad de hasta 90 huevos de gallina. 
Las mismas tienen un diseño industrial, utilizando metales y plásticos, similar a un horno eléctrico, cómo se puede ver en la figura 1.1.


<div align="center">
<img src="https://github.com/aachinelli/TDSE_INT_PF/blob/main/Gr%C3%A1ficos%20y%20fotos/incubadoraTinzer.jpg" width="500" /><br>
<b>Figura 1.1</b>: Imágen ilustrativa de una incubadora estilo industrial de la marca Tinzer, tomada de la página del fabricante.<br>
</div>



2. **Incubadora estilo doméstico BRINSEA**  

Las incubadoras de la marca Brinsea son productos de estilo doméstico o aptos para pequeños productores por sus tamaños y especificaciones.
Son incubadoras listas para la conexión a la red doméstica. Tienen modelos que incuban desde 6 hasta 56 huevos de gallina. Son de un diseño sencillo, con pocas funcionalidades y de carcasa de plástico,
como se ilustra en la figura 1.2.

<div align="center">
<img src="https://github.com/aachinelli/TDSE_INT_PF/blob/main/Gr%C3%A1ficos%20y%20fotos/incubadora-brinsea.jpg" width="600" /><br>
<b>Figura 1.2</b>: Imágen ilustrativa de una incubadora estilo doméstico de la marca Brinsea, tomada de la página de un revendedor.<br>
</div>


La Tabla 1.1 contrasta las prestaciones de los dos productos comerciales de referencia contra el prototipo desarrollado en este trabajo.

<div align="center">
<b>Tabla 1.1</b>: Comparación entre características de modelos comerciales y el prototipo.<br>

| *Aspecto* | *TINZER*  | *BRINSEA* | *Prototipo desarrollado* |
| :--- | :--- | :--- | :--- |
| **Monitoreo de temperatura y humedad** | POSEE | POSEE | POSEE |
| **Calefacción** | Luz halógena | Termoresistencia | No posee (fuera del alcance)  |
| **Interfaz de usuario** | Display 7 segmentos | Display 7 segmentos | Pantalla LCD 16×2 con menú interactivo de dos niveles |
| **Selección de modos** | No disponible | No disponible | 3 modos precargados en una EEPROM externa. Modos personalizables. |
| **Registro de eventos** | No disponible | No disponible | Guardado del estado previo a un corte de suministro |
| **Consumo** | 220 W | De 60 W a 115 W | De 0.5 W a 0.65 W (no posee componentes térmicos) | ***Se podría llegar a agregar un modo sleep?***
| **Rotación de huevos** | Motor eléctrico de bajo consumo | Servomotor | Servomotor SG90 |
| **Construcción de la capsula** | Acero, melamina y policarbonato | Plástico transparente | No implementada (fuera de alcance) |
| **Costo** | ~$350.000 ARS | ~$300 USD a ~$700  | Prototipo de laboratorio - Precio estimado en componentes: ~$70000 ARS |
</div>

En resumen, el mercado ofrece soluciones parciales que cubren algunos de los objetivos del proyecto, pero ninguna combina la personalización por modo de incubación, el registro de estado en EEPROM y la interfaz configurable mediante display LCD y teclado. 
Esto justifica el desarrollo de un sistema propio que cubra todas estas funcionalidades con hardware disponible localmente.
Quedan fuera del alcance del proyecto implementaciones utilizadas por los productos comerciales, como el diseño de la caja incubadora.


## 1.3 Alcance y limitaciones

Alcance implementado:
- Electrónica de control
- Interfaz de usuario 
- Lógica de funcionamiento automático
- Guardado de eventos en memoria externa

Fuera de alcance actual:
- Construcción de la caja contenedora de huevos.
- Implementación de elementos térmicos y de aporte de humedad (se modelan con luces)

El detalle de los requisitos cumplidos se aborda en la sección 4.7.


---

# Capítulo 2: Introducción específica

Se procede a listar los requisitos a cumplir en el presente trabajo, en su [propuesta inicial](https://github.com/pmartinez-madero/tdse-tf_2026-1erC_2-01/blob/main/PROPUESTA.md) y en el [informe de avance](https://github.com/pmartinez-madero/tdse-tf_2026-1erC_2-01/blob/main/Informe_de_Avance.md). Además, se listan los casos de uso.
En la sección 4.7 se detalla el cumplimiento final de los requisitos.

## 2.1 Requisitos

Se plantearon una serie de requisitos a cumplir por el prototipo. Estos sirvieron como guía para el diseño y la programación del trabajo, siendo consensuados a lo largo
del período de desarrollo con los tutores del proyecto. Al inicio del proyecto se estipularon los requisitos presentes en la tabla 2.1.


<p align="center">
<b>Tabla 2.1</b>: Requisitos iniciales del proyecto.<br>
</p>

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


A medida que se avanzó con el prototipado se actualizaron los requisitos, generando un informe de avance diseñado a la mitad del tiempo de desarrollo del prototipo.
El detalle del avance de los requisitos se encuentra presente en la tabla 2.2: 

<p align="center">
<b>Tabla 2.2</b>: Requisitos revisados en el informe de avances.<br>
</p>


| <b>Estado | Descripción</b> |
|-----|---------------------|
| 🟢 | Implementado |
| 🟡 | Sin Implementar |
| 🔴 | Requerimiento descartado |

| <b>ESTADO | GRUPO | ID | TAREA</b> |
|:--:|----|----|----|
| 🟢 | Temperatura | 1.1 | El sistema medirá continuamente la temperatura interior mediante un sensor dedicado. |
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



Estos requisitos son los tenidos en cuenta al avanzar en el diseño, escritura de código, compra de componentes y armado final. 

## 2.2 Casos de uso

En las tablas 2.3 a 2.5 se presentan 3 casos de uso para el sistema. En estos casos de uso se puede apreciar el funcionamiento esperado del prototipo con flujos principales y flujos alternativos.

<div align="center">

<b>Tabla 2.3</b>: Caso de uso 1 — El usuario inicia un ciclo de incubación<br>
|**Disparador**|El usuario desea iniciar un ciclo de incubación seleccionando un modo y confirmando los parámetros.|
| :- | :- |
|**Precondiciones**|El sistema está encendido. No hay un ciclo de incubación activo. La pantalla LCD muestra el menú principal.|
|**Flujo principal**|El usuario navega con el teclado hasta el menú «Nuevo ciclo». Selecciona el tipo de huevo (gallina, pato o codorniz). El sistema muestra en el display los parámetros predefinidos para ese modo (temperatura, humedad, duración, intervalo de rotación). El usuario confirma con la tecla de aceptar. El sistema guarda la configuración en la EEPROM y comienza el ciclo: activa el calentador si es necesario, programa las rotaciones y emite un pitido de confirmación. El LCD pasa a mostrar la pantalla de monitoreo con temperatura, humedad y día del ciclo.|
|**Flujos alternativos**|a. El usuario cancela la selección con la tecla de retroceso: el sistema vuelve al menú principal sin iniciar ningún ciclo. b. La temperatura inicial está fuera del rango objetivo: el sistema activa inmediatamente el calentador y espera hasta alcanzar el rango antes de dar la confirmación sonora.|
<br>

<b>Tabla 2.4</b>: Caso de uso 2 — El sistema detecta temperatura fuera de rango<br>
|**Disparador**|La temperatura interior sale del rango permitido para el modo activo.|
| :- | :- |
|**Precondiciones**|El sistema está encendido con un ciclo de incubación activo. El sensor DHT22 está operativo.|
|**Flujo principal**|El sensor detecta que la temperatura supera o cae por debajo del umbral configurado. El sistema activa o desactiva el elemento calefactor según corresponda. El LED de calentador activo se enciende o apaga acorde. El LCD actualiza el valor de temperatura en tiempo real. Cuando la temperatura vuelve al rango, el sistema estabiliza el calentador y registra el evento en la EEPROM.|
|**Flujos alternativos**|a. La temperatura supera el umbral crítico (fallo grave): el sistema emite una alarma sonora prolongada mediante el buzzer, parpadea el LED de alarma y muestra un mensaje de error en el LCD hasta que el usuario reconozca la alarma. b. El sensor no responde: el sistema emite una alarma y detiene el ciclo de forma segura, registrando el fallo en la EEPROM.|
<br>

<b>Tabla 2.5</b>: Caso de uso 3 — El sistema rota los huevos automáticamente.<br>
|**Disparador**|Se alcanza el intervalo de tiempo programado para la rotación de huevos.|
| :- | :- |
|**Precondiciones**|El sistema está encendido con un ciclo de incubación activo. No se está en el período de lockdown (últimos días antes de la eclosión).|
|**Flujo principal**|El temporizador de rotación expira. El sistema activa el motor de paso para girar los huevos el ángulo configurado (típicamente 45° alternados). El LED de rotación parpadea durante el movimiento. El buzzer emite un pitido breve de confirmación. El sistema registra el evento de rotación en la EEPROM con timestamp. El motor se detiene y el sistema reanuda el monitoreo normal.|
|**Flujos alternativos**|a. El motor no completa el movimiento en el tiempo esperado (posible traba mecánica): el sistema emite una alarma sonora y visual, detiene el motor y registra el fallo en la EEPROM. b. El ciclo está en período de lockdown: el sistema omite la rotación, registra el evento y muestra en el LCD un mensaje indicando que la rotación está inhibida.|
<br>
</div>

## 2.3 Descripción de módulos y Hardware

En esta sección se muestran los principales módulos de hardware utilizados. Los mismos fueron seleccionados para cumplir con los requisitos mínimos de Hardware y su posibilidad de implementación junto a la placa NUCLEO STM32.
Además, se busco que los componentes sean compatibles con los requisitos de Software y permitan un ciclo de ejecución menor a 1 ms, es decir, no sean bloqueantes a la hora de sincronizarse con el resto del sistema. Por ejemplo, 
en el caso de los sensores de humedad y temperatura, la familia DHT (muy común en desarrollos de este estilo) utiliza un protocolo de comunicación propietario por el cual transmite las lecturas actuales en un lapso de unos 3.5 ms, por lo 
que fue descartada.


### 2.3.1 Módulo de control (NUCLEO-F103RB)
La placa NUCLEO-F103RB fue la provista para ser de procesador central. Todos los componentes de Hardware son controlados por su microcontrolador STM32. Se muestra esta placa
en la figura 2.1.

<div align="center">
<img width="500" alt="NUCLEO" src="https://github.com/aachinelli/TDSE_INT_PF/blob/main/nucleo_f103rb.jpg" /><br>
<b>Figura 2.1</b>: Imágen de la placa NUCLEO F103RB, tomada de la página oficial de su fabricante.
</div>

### 2.3.2 Módulo sensor de temperatura y humedad

Para la tarea de sensado atmósferico de temperatura y humedad se optó por un <i>shield</i> compatible con STM32 que contiene un sensor SHT30.
El mismo es analógico, tiene comunicación por I2C, es de bajo consumo y de rápido funcionamiento (400 kHz). Como se ve en la figura 2.2.<br>

<div align="center">
<img width="500" alt="SHT30" src="https://github.com/aachinelli/TDSE_INT_PF/blob/main/Gr%C3%A1ficos%20y%20fotos/SHT30.jfif" /><br>
<b>Figura 2.2</b>: Imágen ilustrativa del sensor de temperatura y humedad SHT30.
</div>


### 2.3.3 Módulo de actuadores

Para simular el funcionamiento de un motor que rote los huevos con cierta periodicidad se utilizó un servomotor SG90. El mismo se comunica por PWM, lo que permite programar rotaciones parciales según la necesidad.
La figura 2.3 ilustra el modelo utilizado.

<div align="center">
<img width="500" alt="SERVO" src="https://github.com/aachinelli/TDSE_INT_PF/blob/main/Gr%C3%A1ficos%20y%20fotos/servo.jpg" /><br>
<b>Figura 2.3</b>: Figura ilustrativa de un servomotor SG90.<br><br>
</div>

El actuador que simula el funcionamiento de un componente que aporte calor (resistencia térmica o lámparas de calor) es un led en serie con un relé genérico de 5 V. 
Este mismo se usa para demostrar que se conecta el led en un bus con suministro eléctrico, logrando calentar el recinto cuando sea necesario solamente alimentando el relé
En la figura 2.4 se puede ver el relé utilizado, mientras que se utilizaron leds de colores genéricos como los que se ven en la figura 2.5.

<div align="center">
<img width="500" alt="RELÉ" src="https://github.com/aachinelli/TDSE_INT_PF/blob/main/Gr%C3%A1ficos%20y%20fotos/Rele.jpg" /><br>
<b>Figura 2.4</b>: Imágen ilustrativa de un relé de 5 V.<br><br>
</div>

<div align="center">
<img width="500" alt="RELÉ/LED" src="https://github.com/aachinelli/TDSE_INT_PF/raw/5c0944f3a0e169877e78a95acd58ae4edaa9749e/leds.png" /><br>
<b>Figura 2.5</b>: Imágen ilustrativa de LEDs genéricos, utilizados como indicadores.<br><br>
</div>

Se simula también el funcionamiento de un vaporizador, que aporta humedad al ambiente cuando es necesario. Para esto, se utiliza un led indicador como el de la figura 2.5 que se enciende cuando la humedad está
por debajo del umbral.

### 2.3.4 Módulo de interfaz

Por la necesidad de mostrar el estado actual de la incubación, la elección de modos, de mostrar estados previos a cortes de suministro y la elección de parámetros personalizados se buscó un componente único que pueda mostrar estos mensajes con facilidad.
El componente elegido fue un display LCD 16x2, como el visto en la figura 2.6. Nos permite mostrar simbolos alfanuméricos y presentar los mensajes necesarios con un bajo consumo y con la posibilidad de cambiar su brillo.<br>

<div align="center">
<img width="500" alt="LCD" src="https://github.com/aachinelli/TDSE_INT_PF/blob/main/Gr%C3%A1ficos%20y%20fotos/display_LCD.jpg" /><br>
<b>Figura 2.6</b>: Imágen ilustrativa del display LCD 16x2 con backlight.<br><br>
</div>

Se diseñaron opciones de entrada de datos al sistema acotadas y sencillas para disminuir el código y la lógica. Por esto se optó por un teclado de membrana de 4x1, como el visto en la figura
2.7 al que se le mapearon 4 funciones: "OK/CONTINUAR", "VOLVER", "ARRIBA" y "ABAJO". Se conectó mediante las entradas GPIO a la placa NUCLEO.<br>

<div align="center">
<img width="500" alt="TECLADO" src="https://github.com/aachinelli/TDSE_INT_PF/blob/main/Gr%C3%A1ficos%20y%20fotos/Teclado-Matricial-de-Membrana-4-Teclas-1-1.jpg" /><br>
<b>Figura 2.7</b>: Imágen ilustrativa de un teclado de membrana de 4 botones.
</div>



### 2.3.5 Memoria EEPROM

Para proporcionar una memoria externa con lectura y escritura se utilizó una memoria EEPROM 24C256. La misma nos da versatilidad y aproximadamente 32 kB de memoria. En este proyecto se utilizó un <i>shield</i> compatible
con STM32, como el visto en la figura 2.8, lo que nos permite una fácil integración al sistema y comunicación por I2C. El uso de esta memoria será monitoreado en la sección 4.3. <br><br>

<div align="center">
<img width="500" alt="EEPROM" src="https://github.com/aachinelli/TDSE_INT_PF/blob/main/Gr%C3%A1ficos%20y%20fotos/memoria_eeprom.jpg" /><br>
<b>Figura 2.8</b>: Imágen del shield y la memoria EEPROM 24C256 utilizados.
</div>



# Capítulo 3: Diseño e implementación

En el siguiente capítulo se expone el diseño de Hardware y Firmware implementados en este prototipo.

## 3.1 Arquitectura general

En la Figura 3.1 se presenta el diagrama en bloques general. En el se presentan los distintos módulos y se muestran los sensores, actuadores y displays.

<div align="center">

<img src="https://github.com/aachinelli/TDSE_INT_PF/blob/main/Gr%C3%A1ficos%20y%20fotos/Incubadora%20Final.jpeg" width="600" /><br>
<b>Figura 3.1</b>: Diagrama en bloques general.<br><br>

</div>

A partir del diagrama de la figura 3.1 se obtuvo una idea de la cantidad de conexiones con la placa NUCLEO. 
A medida que las pruebas fueron avanzando y considerando los puertos disponibles por la placa NUCLEO, se optó por utilizar esquema de la Figura 3.2:


<div align="center">

<img src="https://github.com/aachinelli/TDSE_INT_PF/blob/main/Gr%C3%A1ficos%20y%20fotos/Diagrama%20conexiones.jpeg" width="600" />
<br><b>Figura 3.2</b>: Plano de las conexiones de los periféricos.<br><br>

</div>



## 3.2 Diseño de hardware

En el siguiente apartado se detallan las soluciones de Hardware adoptadas en línea con las pautas de entrega. El objetivo final es conseguir un conexionado de todo el hardware de forma ordenada procurando que no queden cortocircuitos en las soludaduras y conexiones firmes para que no se interrumpan las comunicaciones. Para esto se opto por el montaje sobre una placa experimental, sin cables dupont extremo a extremo.

Como se mencionó previamente, se utilizo el diagrama 3.2 de guia para el conexionado de la placa y los periféricos. Considerando que es necesario conectar actuadores y el sensor principal, se definieron los pines como se puede ver en la tabla 3.1.

<div align="center">
<b>Tabla 3.1</b>: Asignación de pines de la placa núcleo.

| <b>Función | Puerto | Protocolo</b> |
| :--- | :--- | :--- |
|Teclado-Volver|PC7|GPIO|
|Teclado-Enter|PB6|GPIO|
|Teclado-Abajo|PA7|GPIO|
|Teclado-Arriba|PA6|GPIO|
|SCL - 1 (LCD y EEPROM)|PB10|I2C| 
|SDA - 1 (LCD Y EEPROM)|PB11|I2C|
|SCL - 2 (SENSOR)|PB8|I2C|
|SDA - 2 (SENSOR)|PB9|I2C|
|Led indicador (aporte térmico)|PC8|GPIO|   
|Relé|PC9|GPIO|
|Led indicador (humidificador)|PB13|GPIO|
|Servo|PA15|PWM (TIM2 CH1)|


<br></div>


Además se tuvieron en cuenta las necesidades de alimentación de la placa, por lo que se le proporcionaron 5 V de alimentación a la placa NUCLEO y se diseñó un bus con 5 V 
para alimentar los periféricos. 

## 3.3 Diseño de firmware
En este apartado se detalla la construcción del Software desarrollado para el funcionamiento del prototipo de incubadora. El mismo fue desarrollado bajo una estructura 
Bare Metal, con un sistema determinado por eventos. Se utilizó una estructura modular. 
Se utilizaron 3 módulos: modulos de sensado, procesado y actuación.
Se buscó cuidar el consumo energético, la memoria y obtener un código legible y de fácil comprensión y una óptima interacción entre los periféricos.



### 3.3.1 Arquitectura de ejecución

Todas las tareas a realizar se rigen por un ejecutor cíclico con velocidad de 1kHz, obteniendo tareas que se realizan en su totalidad en menos de 1 ms. El ejecutor cíclico se 
guía por el `SysTick`, que a lo largo del <i>loop</i> ejecuta la lista de tareas.
Además, se utiliza el `SysTick` para no utilizar tareas bloqueantes, ni utilizar `HAL_DELAY()`. Todos las temporizaciones se rigen por estos contadores de ticks.

Se recorren las tareas en el siguiente orden:

1. `task_sensor_update`
2. `task_memory_update`
3. `task_system_update`
4. `task_actuator_update`
5. `task_display_update`

Cada tarea se ejecuta en cada tick y su tiempo se mide con contador de ciclos (`DWT->CYCCNT`) para cálculo de WCET.
La figura 3.3 describe como se ejecutan las tareas.<br>

<div align="center">
<img width="650" alt="ejecutor cíclico" src="https://github.com/aachinelli/TDSE_INT_PF/blob/main/Gr%C3%A1ficos%20y%20fotos/Diagrama%20en%20blanco.jpeg" /><br>
<p><b>Figura 3.3</b>: Orden de tareas en un ciclo de ejecución.</p><br>
</div>
 
Las tareas se comunican por interfaces. No hay comunicación por variables globales. Cada tarea tiene asignada una interfaz, como se ve en la tabla 3.2. 

<div align="center">

<b>Tabla 3.2</b>: Interfaces asignadas a cada tarea.

| Tarea | Interfaz |
| --- | --- |
| `task_sensor_update` | `task_sensor_interface` |
| `task_memory_update` | `task_memory_interface` |
| `task_system_update` | `task_system_interface` |
| `task_actuator_update` | `task_actuator_interface` |
| `task_display_update`| `task_display_interface` |

</div>


### 3.3.2 Máquina de estados del sistema

El código se modeló utilizando la maquina de estados del sistema. Esta misma se utilizó para el armado del menú interactivo, visible en la figura 3.4.

<div align="center">
<img width="650" alt="máquina de estados" src="https://github.com/aachinelli/TDSE_INT_PF/blob/main/Gr%C3%A1ficos%20y%20fotos/flujo_menu.png" />
<p><b>Figura 3.4</b>: Statechart del menú interactivo.</p>
</div>

En la figura 3.4 se puede seguir el orden de acciones del sistema. Al iniciar el sistema, se muestra un mensaje de bienvenida que nos lleva a una pantalla donde 
se pueden tomar dos caminos: `Continuar` y `Nuevo inicio`. La opción `Nuevo inicio` nos lleva a establecer los parámetros del ciclo de icubación, ya sean los precargados en la memoria
o datos de una sesión personalizada. Una vez seleccionados los parámetros se entra en el proceso de incubación donde se muestran por pantalla la humedad y temperatura actuales y
el tiempo restante en días y horas. Si se elige la opción `Continuar` se verifica que efectivamente haya una sesión en progreso y se continúa al ciclo de incubación en proceso, con el tiempo restante.

Durante el proceso de incubación, el sistema recibe datos del sensor de temperatura y humedad. Si la temperatura es menor a la necesaria se envía la señal al módulo del relé que simula el encendido del componente térmico.
En simultáneo se envían los datos de temperatura y humedad al display. La rotación está dada por un contador. Se envía la señal al servomotor por PWM para rotar los huevos algunos grados cada cierta cantidad de horas. Este proceso se detiene
al alcanzar los últimos días de la incubación.



# Capítulo 4: Ensayos y resultados

Esta sección está dedicada a mostrar los resultados de la integración de Software y Hardware en el armado del prototipo de la incubadora automática.

## 4.1 Integración 

En el siguiente video se muestra el correcto funcionamiento del prototipo integrado, simulando distintas situaciones atmósfericas y mostrando las respuestas del sistema a estas.

[VIDEO](https://drive.google.com/file/d/1lzKZgfwIwhwxn-eZ1z3Sk5OSUK5yr5XN/view?usp=sharing) 



## 4.2 Pruebas funcionales de hardware Y software

Se redactaron pruebas para verificar el funcionamiente esperado del prototipo, proporcionando una guía que asegure la robustez y confiabilidad del prototipo ensamblado.
Las mismas se muestran en la tabla 4.1.

<div align="center">
<b>Tabla 4.1</b>: Resumen de ensayos funcionales de hardware y firmware.

| Subsistema | Ensayo realizado | Resultado / Criterio de validación | Estado |
| :--- | :--- | :--- | :---: |
| **Hardware** | Verificación de continuidad  | Ausencia de cortocircuitos o falsos contactos en la placa experimental | ✅ |
| **Hardware** | Respuesta correcta del teclado | Mapeo correcto de los 4 botones a sus 4 funciones. Correcta navegación por el menú | ✅ |
| **Hardware** | Correcta visualización del LCD | Lectura de todas las pantallas programadas y disponibles. Actualización en tiempo real | ✅ |
| **Hardware** | Correcto funcionamiento del sensor | Medición por osciloscopio. Traspaso correcto de información (bits) a la velocidad esperada. | ✅ |
| **Firmware** | FSM de antirrebote de pulsadores | Filtrado exitoso de rebotes mecánicos en el botón de confirmación y sensor magnético | ✅ |
| **Firmware** | Correcta inerpretación del sensor | Mapeo estable del sensor por código | ✅ |
| **Firmware** | Persistencia en EEPROM y RTC | Lectura y escritura correcta de marcas de tiempo e historial por bus I²C | ✅ |
| **Firmware** | Máquina de estados global | Transiciones robustas entre las pantallas del menú. Correcto traspaso entre la entrada de datos y el proceso de incubación | ✅ |


<br></div>

Durante el ensayo "Correcto funcionamiento del sensor" se observó un problema con un sensor previamente elegido. Se había elegido un DHT22 que necesitaba de algunos milisegundos
para enviar los datos a la placa NUCLEO, lo que generaba un retardo general en el resto del sistema. Por esto, se optó por el sensor SHT30, como se explica en la 
subsección 2.3.2.

Estas pruebas determinan un correcto ensamblaje y comunicación entre las entradas/salidas analógicas y el código. Se espera que hayan conexiones robustas, por lo que se 
soldaron los componentes a una placa experimental y se utilizaron zócalos.


## 4.3 Ocupación de memoria: Console & Build Analyzer

Al depurar el código, el STM32CubeIDE reproduce una pantalla llamada *Build Analyzer*. En ella se muestra el desglose del uso de la memoria FLASH Y RAM. 
Se muestran los resultados de una instancia de depuración del prototipo en la figura 4.2.

<div align="center">
<img width="500" alt="MEMORIA" src="https://github.com/aachinelli/TDSE_INT_PF/blob/main/Gr%C3%A1ficos%20y%20fotos/mem_usage.png" /><br>
<p><b>Figura 4.2</b>: Uso de memoria visible en el Build Analyzer.</p>
</div>


En la tabla 4.2 se vuelcan estos resultados para facilitar la comprensión de lo obtenido en la depuración.

<div align="center">
<b>Tabla 4.2</b>: Uso y ocupación de memoria.

| <b>MEMORIA | Usado [Bytes] | Total Disponible [Bytes] | Porcentaje de ocupación</b> |
| :--- | :--- | :---: | :---: |
| RAM | 2.91 k  | 20 k | 14.57% |
| FLASH | 41.7 k  | 128 k | 32.57% |


</div>

En conclusión, el firmware utiliza aproximadamente el 15% de la memoria dinámica y alrededor del 33% de la memoria flash. Esto demuestra un uso seguro de la memoria, 
con baja ocupación de la misma y un buen margen para agregar funcionalidades y mejoras. Al mismo tiempo, no se observaron fallos que generen perdida excesiva de memoria 
a la hora de realizar depuraciones.

 
## 4.4 Medición y análisis de tiempos de ejecución (WCET)

En esta sección se busca comprender el comportamiento temporal del programa en la búsqueda de cumplir con los requisitos máximos de tiempo del ejecutor cíclico.
Para esto se observa la variable <b>WCET</b> (<i>Worst-Case Execution Time</i>), lo que nos mostrará el peor caso de ejecución de una tarea durante su depuración. La misma
se obtiene invocando el contador de ciclos del DWT (<i>Data Watchpoint and Trace</i>) del Cortex-M3, que cuenta el tiempo de ejecución de cada tarea en el ciclo de ejecución. 
Es común observar que las tareas más costosas generan mayor tiempo de ejecución por lo que se procedió a la depuración y se simuló una incubación completa, es decir
que incluya todas las acciones disponibles por nuestro prototipo.

En la figura 4.3 se observan los resultados observados en la pantalla *Live Expressions* de la depuración en el STM32CubeIDE.<br><br>

<div align="center">
<img width="500" alt="WCET" src="https://github.com/aachinelli/TDSE_INT_PF/blob/main/Gr%C3%A1ficos%20y%20fotos/ov_umbral.png" />
<p><em><b>Figura 4.3</b>: Pantalla de Live Expressions con los peores tiempos de ejecución.</em></p>
</div>
<br>

Se toman en cuenta los resultados de los valores de `task_dta_list[].WCET`, siendo que cada uno de los elementos de `task_dta_list[]` es una de las tareas del ciclo de ejecución.
El valor devuelto es el tiempo en microsegundos (µs) devuelto por el DWT. 
En la Tabla 4.3 se observa la suma de los WCET de todas las tareas, simulando el peor caso posible del tiempo de vuelta del ciclo de ejecución.<br><br>

<div align="center">
<b>Tabla 4.3</b>: Peores casos de tiempo de ejecución según tarea.

| <b>Tarea | WCET[µs]</b>|
| :---: | :---: |
| `task_sensor` | 229 |
| `task_memory` | 1 |
| `task_system`| 294 |
|`task_actuator`| 10 |
|`task_display`| 371 |
|TOTAL (ciclo completo)| 905 |


<br></div>


Como se observa, en el peor de los casos, es decir cuando se acumulan todos los peores tiempos de ejecución, se obtiene un WCET total de 905 µs. Es un resultado
que cumple con los requisitos de tiempo y deja un margen de 95 µs a la hora de agregar funcionalidades. Se toma en cuenta que las tareas de mayor tiempo de ejecución son `task_display` y `task_sensor`, 
tareas que incluye los tiempos de debounce de los botones del teclado y proporcionan el funcionamiento del display y el teclado, es decir tareas con poca posibilidad de 
mejora temporal mediante código. El display también incluye su propio delay por su tasa de funcionamiento.

El tiempo de mejora posible se encuentra en `task_system`, donde se ejecuta la lógica del prototipo. Consideramos que esta tarea está lo suficientemente optimizada, es 
decir, el margen de mejora no es significativo.

## 4.5 Cálculo del Factor de Uso (U) de la CPU

El factor de uso de la CPU se calcula como el cociente entre el peor caso de ejecución (C) y el tiempo de vuelta del ejecutor cíclico (T), siendo T los 1000 us seteados por el SysTick.
Este cociente se expresa en la ecuación 

$$U = \frac{C}{T} \qquad (4.1)$$

Reemplazando C con el tiempo total de obtenido en la Tabla 4.3 se obtiene un factor de uso de 0.905 , es decir un 90.5% del tiempo. Eso nos deja 9.5% del tiempo en el que el microcontrolador
no está ejecutando tareas. Cabe aclarar que esta estimación está dada sobre el peor de los casos posibles. Esto sucede solo cuando se ejecutan las tareas más costosas
computacionalmente en simultáneo. Esto no sucede a menudo, nuestro sistema está diseñado para ejecutarse durante largos períodos de tiempo, con acciones ocasionales como la 
rotación de los huevos que solamente sucede entre 3 y 6 veces por día .

En conclusión, el de la CPU es acorde a las especificaciones y deja margen para utilizar en otras tareas y mejoras.

## 4.6 Medición y análisis de consumo
 
Se midió el consumo energético del sistema poniendo un multímetro en modo amperímetro en serie con la alimentación mientras se ejecutaban las diferentes tareas.
El prototipo en funcionamiento no cuenta con ningún dispositivo térmico para ofrecer el calor, por lo que el consumo estará dictado por el de la placa NUCLEO y los 
periféricos previamente mencionados. 
Tomando en cuenta que la alimentación de la placa y los periféricos es de 5 V, se utiliza ese dato junto a la Ecuación 4.2 para calcular la potencia consumida.

$$P = {V}*{I} \qquad (4.2)$$


<div align="center">
<b>Tabla 4.4</b>: Mediciones de consumo eléctrico.

| Medición | Corriente consumida[mA] | Potencia consumida[mW] |
| :---: | :---: | :---: |
| Placa + Display + Sensor | 100 | 500 |
| Placa + Display + Sensor + Servomotor | 130 | 650 |


<br></div>
 
Las mediciones nos muestran que la potencia consumida por la placa y el display la mayoría del tiempo (cuando no funciona), es de 0.5 W. Esto es acorde a la cantidad
de periféricos utilizados, como el display LCD al máximo del brillo. Es un consumo relativamente bajo, aunque la incubadora debe estar en funcionamiento largos períodos de tiempo.
Se concluye, además, que de implementar el prototipo, el mismo deberá contar con un componente térmico que será la mayor fuente de consumo. Las resistencias o lámparas térmicas
consumen desde los 30 W por lo que el consumo del prototipo es reducido en comparación al esperado por una incubadora.


## 4.7 Cumplimiento de requisitos

Se hizo un relevamiento de los requisitos expuestos en la tabla 2.2 y se verificó su cumplimiento. La tabla 4.5 pone en manifiesto los requisitos y su estado final.

<div align="center">
<b>Tabla 4.5</b>: Cumplimiento de requisitos.

| Estado | Descripción |
|-----|---------------------|
| 🟢 | Implementado |
| 🟡 | Implementado parcialmente |
| 🔴 | Sin implementar/requerimiento descartado |

</div>

| ESTADO | GRUPO | ID | TAREA |
|:--:|----|----|----|
| 🟢 | Temperatura | 1.1 | El sistema medirá continuamente la temperatura interior mediante un sensor dedicado. |
| 🟢 |   | 1.2 | El sistema activará el elemento calefactor cuando la temperatura esté por debajo del umbral configurado. |
| 🟢 |   | 1.3 | El sistema desactivará el elemento calefactor cuando la temperatura supere el umbral configurado. |
| 🟢 | Humedad | 2.1 | El sistema medirá continuamente la humedad relativa interior. |
| 🟢 |   | 2.2 | El sistema activará el humidificador cuando la humedad esté por debajo del umbral configurado. |
| 🟢 |   | 2.3 | El sistema desactivará el humidificador cuando la humedad supere el umbral configurado. |
| 🟢 | Rotación | 3.1 | El sistema rotará los huevos automáticamente a intervalos de tiempo configurables por tipo de huevo. |
| 🔴 |   | 3.2 | El sistema indicará mediante un LED cada vez que se realice una rotación. |
| 🟢 |   | 3.3 | El sistema inhibirá la rotación durante los últimos días del ciclo de incubación (lockdown), según el modo seleccionado. |
| 🟢 | Interfaz | 4.1 | El sistema contará con una pantalla LCD para mostrar temperatura, humedad, día del ciclo y modo activo. |
| 🟢 |   | 4.2 | El sistema contará con un teclado matricial para navegar entre menús y configurar parámetros. |
| 🟢 |   | 4.3 | El sistema contará con LEDs indicadores de estado: calentador activo, humidificador activo y rotación. |
| 🔴 |   | 4.4 | El sistema contará con un buzzer para alertas sonoras (alarmas y confirmaciones). |
| 🟢 | Modos de incubación | 5.1 | El sistema soportará al menos tres modos predefinidos: huevo de gallina, pato y codorniz, con sus respectivos parámetros de temperatura, humedad y ciclo. |
| 🟢 |   | 5.2 | El usuario podrá seleccionar el modo desde el menú de la interfaz gráfica. |
| 🟢 |   | 5.3 | El sistema indicará en el display el día actual del ciclo de incubación. |
| 🔴 | Alarmas | 6.1 | El sistema emitirá una alarma sonora y visual si la temperatura supera un umbral crítico configurable. |
| 🔴 |   | 6.2 | El sistema emitirá una alarma sonora y visual si la humedad supera o cae por debajo de umbrales críticos. |
| 🔴 |   | 6.3 | El sistema emitirá una alarma si el motor de rotación no completa el giro en el tiempo esperado. |
| 🟢 | Almacenamiento | 7.1 | El sistema almacenará en la EEPROM externa el historial de temperatura, humedad y eventos de rotación. |
| 🟢 |   | 7.2 | El sistema preservará la configuración activa y el día del ciclo ante cortes de energía. |

<br>
Como se muestra en la tabla 4.5, no todos los requisitos fueron cumplidos. A raíz de la disponibilidad, percances y cambios necesarios en el desarrollo fundamental se optó
por disminuir el alcance. Aunque pueda significar una mejora en el proyecto, consideramos que los requerimientos descartados eran los menos relevantes: las alarmas. Siendo las alarmas una herramienta que 
mejoran la experiencia de usuario, no son fundamentales en el funcionamiento de las incubadoras. Por último, consideramos finalmente que el requisito 3.2 era redundante en la tarea de indicar que los huevos están rotando. 
Al implementar un servomotor, se ve que el grupo "Rotación" está en correcto funcionamiento.
Obviando este cambio en el alcance el resto de los requisitos fue cumplido satisfactoriamente y consideramos que se simula correctamente un ciclo de incubación.

---

# Capítulo 5: Conclusiones

## 5.1 Resultados obtenidos

El prototipo simula satisfactoriamente el funcionamiento de un ciclo de incubación en tiempo y forma. Además, se cumplieron los requisitos mínimos de Hardware y Firmware. Por último se cumplieron la mayoría de los objetivos,
 dejando de lado aquellos que significaban un aumento sustancial en tiempo de desarrollo.

## 5.2 Posibles mejoras

A futuro se pueden implementar mejoras y agregados de Software y Hardware, ya sea aquellos requisitos que no fueron incluidos por los motivos previamente planteados, o 
posibles agregados que podrían mejorar el atractivo y la viabilidad del producto. Algunos de las posibles mejoras son:

- Implementación de un cooler para disminuir la temperatura y la humedad.
- Implementación del servomotor para accionar un dispositivo humidificador.
- Implementación de las alarmas mediante buzzers. 
- Agregado de un modo de reposo para cuidado del consumo. Podría disminuir el uso de la resistencia térmica, bajar el brillo o apagar la pantalla LCD y disminuir al mínimo las funciones.


## 5.3 Aprendizajes

Este trabajo nos proporcionó aprendizaje técnico y organizativos acordes al trabajo de un ingeniero electrónico. Algunas instancias de aprendizaje incluyen 
la compra de componentes que no cumplían con las especificaciones técnicas pedidas y la extensión del alcance establecido por los integrantes.
En conclusión, consideramos que de hacer otra vez un trabajo de este estilo contamos con el conocimiento del tiempo que conlleva, con el conocimiento del
 desarrollo en STM32 y con una noción organizativa y profesional requerida por este tipo de tareas.

---

# Capítulo 6: Uso de herramientas de IA

Se explicita el uso de herramientas de Inteligencia Artificial en las siguientes tareas:

- Corrección de errores en los informes

- Programación STM32 (estructura y funcionamiento)

- Depuración y solución de problemas: consultas a las herramientas sobre problemas en el código o posibles errores no tenidos en cuenta

---

# Capítulo 7: Bibliografía y referencias

[1] STMicroelectronics, *UM1724 - STM32 Nucleo-64 boards user manual*.  
[2] STMicroelectronics, *MB1136 - Electrical Schematic - STM32 Nucleo-64 boards*.  
[3] STMicroelectronics, *STM32F103RB Datasheet*.  
[4] Sensirion, *SHT30 Datasheet*
[5] The Microchip Technology Inc., *24LC256 Datasheet* (memoria EEPROM)
