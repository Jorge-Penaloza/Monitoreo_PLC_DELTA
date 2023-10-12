# Monitoreo PLC Delta

Este proyecto tiene como objetivo principal capturar datos en un archivo CSV a partir de señales analógicas de un PLC Delta. Utiliza un programa en C++ para establecer una comunicación serial con un Arduino, que es el encargado de leer dichas señales. Una vez obtenidos los datos, estos pueden ser utilizados para calcular la Función de Transferencia (FT) del sistema. Posteriormente, se puede sintonizar el controlador PID en el PLC y volver a medirlo.

El propósito detrás de este proyecto es proporcionar a los estudiantes una herramienta práctica para entender y aplicar conceptos relacionados con la sintonización de controladores y la adquisición de datos en tiempo real.

## Modo de Uso

Para utilizar el programa compilado, simplemente ejecute el archivo `.exe` y proporcione el nombre del archivo donde desea guardar los datos, por ejemplo: `datos plc.csv`. Asegúrese de tener conectado el Arduino y de que esté leyendo correctamente las señales del PLC Delta.

## Estructura del Repositorio

- `datos.cpp`: Código fuente en C++ que establece la comunicación serial y guarda los datos en un archivo CSV.
- `datos.exe`: Programa compilado listo para ser ejecutado.
- `pid_delta`: Carpeta que contiene archivos relacionados con el controlador PID.
- `pwm_v1.1`: Carpeta con archivos relacionados con la modulación por ancho de pulso (PWM).

## Nota

Fue desarrollado por Jorge Peñaloza como programador principal.

## Enlace al Repositorio

[Monitoreo PLC Delta en GitHub](https://github.com/Jorge-Penaloza/Monitoreo_PLC_DELTA/)



