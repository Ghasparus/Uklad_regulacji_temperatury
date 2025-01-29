#include <obsluga.h>
#include <math.h>

/**
 * @file obsluga.c
 * @brief Implementacja funkcji do obsługi systemu sterowania temperaturą.
 *
 * Zawiera implementację funkcji związanych z ustawianiem temperatury, obsługą enkodera,
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
int scale_temperature_to_pulse(double temperature)
{
    // Obliczamy wartość Pulse jako liczba zmiennoprzecinkową
    double pulse_double = (temperature / 25.0) * 144000.0;

    // Zaokrąglamy wynik do najbliższej liczby całkowitej
    int pulse = (int)round(pulse_double);

    return pulse;
}

/**
 * @brief Ustawia temperaturę za pomocą enkodera.
 *
 * Funkcja ta śledzi zmiany licznika timera i dostosowuje temperaturę w zależności od zmiany
 * wartości licznika. Wartość temperatury jest następnie przekazywana do regulatora PID.
 *
 * @param htim Wskaźnik na strukturę timera STM32, używanego do odczytu wartości licznika.
 * @param pid Wskaźnik na strukturę PID, używaną do obliczeń sterujących.
 * @param temp Wskaźnik na zmienną, w której przechowywana jest aktualna temperatura.
 * @param prev_value Wskaźnik na zmienną, która przechowuje poprzednią wartość licznika.
 */
void set_temperature_via_encoder(TIM_HandleTypeDef *htim, PID *pid, double *temp, int *prev_value)
{
    if(htim->Instance->CNT > *prev_value)
    {
        *temp = *temp + 0.1;
        *prev_value = htim->Instance->CNT;
    }
    else if(htim->Instance->CNT < *prev_value)
    {
        *temp = *temp - 0.1;
        *prev_value = htim->Instance->CNT;
    }
    change_PID_setpoint(pid, *temp);
}

/**
 * @brief Wyświetla temperatury na wyświetlaczu LCD.
 *
 * Funkcja ta odpowiada za wyświetlenie wartości temperatury ustawionej przez użytkownika
 * oraz zmierzonej temperatury na ekranie LCD.
 *
 * @param temp Temperatura ustawiona przez użytkownika.
 * @param meas_temp Zmierzona temperatura.
 */
void display_on_LCD(double temp, double meas_temp)
{
    uint8_t bufor[16];
    LCD_SetCursor(0, 0);
    sprintf(bufor, "Temp. zad. %.2f", temp);
    LCD_Print(&bufor);
    LCD_SetCursor(1, 0);
    sprintf(bufor, "Temp. akt. %.2f", meas_temp);
    LCD_Print(bufor);
}

/**
 * @brief Ustawia wartość PWM na odpowiednim kanale.
 *
 * Funkcja ta zatrzymuje bieżące generowanie sygnału PWM na kanale, ustawia nową wartość
 * porównania, a następnie uruchamia ponownie sygnał PWM.
 *
 * @param htim Wskaźnik na strukturę timera STM32.
 * @param channel Kanał timera, na którym ma być ustawiony PWM.
 * @param value Wartość PWM do ustawienia.
 */
void set_PWM(TIM_HandleTypeDef *htim, uint32_t channel, int value)
{
    HAL_TIM_PWM_Stop(htim, channel);
    __HAL_TIM_SET_COMPARE(htim, channel, value);
    HAL_TIM_PWM_Start(htim, channel);
}

/**
 * @brief Wysyła dane przez UART.
 *
 * Funkcja ta wysyła przez interfejs UART dane, zawierające temperaturę ustawioną przez użytkownika
 * oraz zmierzoną temperaturę.
 *
 * @param set Temperatura ustawiona przez użytkownika.
 * @param measure Zmierzona temperatura.
 * @param huart Wskaźnik na strukturę UART, przez którą będą przesyłane dane.
 */
void send_via_uart(double set, double measure, UART_HandleTypeDef *huart)
{
    uint8_t bufor[13];
    sprintf(bufor, "Z%.2fA%.2f", set, measure);
    bufor[12] = '\n';
    HAL_UART_Transmit(huart, bufor, 13, 100);
}

/**
 * @brief Odbiera dane przez UART.
 *
 * Funkcja ta odbiera dane zawierające ustawioną temperaturę przez interfejs UART,
 * następnie przetwarza odebrane dane na wartość temperatury.
 *
 * @param huart Wskaźnik na strukturę UART, z której będą odbierane dane.
 * @param set Wskaźnik na zmienną, w której będzie zapisana odbierana temperatura.
 * @param bufor1 Bufor do przechowywania danych odbieranych przez UART.
 */
void recieve_via_uart(UART_HandleTypeDef *huart, double *set, uint8_t *bufor1)
{
    HAL_UART_Receive_IT(huart, bufor1, 7);
    uint8_t bufor2[2];
    double c;
    double pp;
    for (int i = 1; i < 3; i++) {
        bufor2[i - 1] = bufor1[i];
    }
    c = (double)atoi(bufor2);
    for (int i = 4; i < 6; i++) {
        bufor2[i - 4] = bufor1[i];
    }
    pp = (double)(atoi(bufor2) / 100.0);
    *set = c + pp;
}
