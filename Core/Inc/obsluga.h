#ifndef INC_OBSLUGA_H_
#define INC_OBSLUGA_H_

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_tim.h"
#include "pid.h"
#include "bmp2_defs.h"

/**
 * @file obsluga.h
 * @brief Nagłówek zawierający definicje funkcji do obsługi systemu sterowania temperaturą.
 *
 * Zawiera deklaracje funkcji związanych z ustawianiem temperatury, obsługą enkodera,
 * wyświetlaniem temperatury na wyświetlaczu LCD, oraz komunikacją UART.
 *
 * @author Kacper
 * @date 25 styczeń 2025
 */

/**
 * @brief Skaluje temperaturę (0-25°C) do wartości Pulse (0-144000).
 *
 * Funkcja przekształca temperaturę w zakresie od 0 do 25°C na odpowiadającą jej wartość Pulse
 * w zakresie od 0 do 144000, zachowując liniową zależność pomiędzy temperaturą a Pulse.
 *
 * @param temperature Temperatura w stopniach Celsjusza (0-25°C).
 * @return Skala Pulse w zakresie 0-144000 odpowiadająca podanej temperaturze.
 */
int scale_temperature_to_pulse(double temperature);

/**
 * @brief Ustawia temperaturę za pomocą enkodera.
 *
 * Funkcja ta korzysta z układu PWM oraz PID, aby ustawić odpowiednią temperaturę.
 *
 * @param htim Wskaźnik na strukturę timera STM32, który jest używany do generowania sygnałów PWM.
 * @param pid Wskaźnik na strukturę PID, używaną do obliczeń sterujących.
 * @param temp Wskaźnik na zmienną, która przechowuje temperaturę.
 * @param prev_value Wskaźnik na zmienną przechowującą poprzednią wartość PWM.
 */
void set_temperature_via_encoder(TIM_HandleTypeDef *htim, PID *pid, double *temp, int *prev_value);

/**
 * @brief Wyświetla temperatury na wyświetlaczu LCD.
 *
 * Funkcja ta odpowiada za wyświetlenie wartości temperatury oraz zmierzonej temperatury na ekranie LCD.
 *
 * @param temp Temperatura ustawiona przez użytkownika.
 * @param meas_temp Zmierzona temperatura.
 */
void display_on_LCD(double temp, double meas_temp);

/**
 * @brief Ustawia wartość PWM na odpowiednim kanale.
 *
 * Funkcja ustawia wartość PWM na określonym kanale timera.
 *
 * @param htim Wskaźnik na strukturę timera STM32.
 * @param channel Kanał timera, na którym ma być ustawiony PWM.
 * @param value Wartość PWM do ustawienia.
 */
void set_PWM(TIM_HandleTypeDef *htim, uint32_t channel, int value);

/**
 * @brief Wysyła dane przez UART.
 *
 * Funkcja ta wysyła dane zawierające ustawioną temperaturę oraz zmierzoną temperaturę
 * przez interfejs UART.
 *
 * @param set Temperatura ustawiona przez użytkownika.
 * @param measure Zmierzona temperatura.
 * @param huart Wskaźnik na strukturę UART, przez którą będą przesyłane dane.
 */
void send_via_uart(double set, double measure, UART_HandleTypeDef *huart);

/**
 * @brief Odbiera dane przez UART.
 *
 * Funkcja ta odbiera dane z interfejsu UART, które zawierają ustawioną temperaturę
 * i zapisuje je w odpowiednich zmiennych.
 *
 * @param huart Wskaźnik na strukturę UART, z której będą odbierane dane.
 * @param set Wskaźnik na zmienną, w której będzie zapisana odbierana temperatura.
 * @param bufor1 Bufor do przechowywania danych odbieranych przez UART.
 */
void recieve_via_uart(UART_HandleTypeDef *huart, double *set, uint8_t *bufor1);

#endif /* INC_OBSLUGA_H_ */
