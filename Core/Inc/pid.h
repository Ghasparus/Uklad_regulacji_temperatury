#ifndef PID_H
#define PID_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Struktura zawierająca parametry algorytmu PID z opóźnieniem transportowym i systemem anty wind-up.
 *
 * Struktura zawiera wzmocnienia PID, wartości błędów, całkowite sumy błędów, punkt zadany
 * oraz indeks próbek, które mają zostać zignorowane przed obliczeniem wyjścia.
 * Opóźnienie transportowe jest modelowane na podstawie liczby próbek do zignorowania,
 * a system anty wind-up zapobiega nadmiernemu wzrostowi sumy błędów, co mogłoby prowadzić do niestabilności.
 */
typedef struct {
    double Kp;                  /**< Wzmocnienie proporcjonalne */
    double Ki;                  /**< Wzmocnienie całkowite */
    double Kd;                  /**< Wzmocnienie różnicowe */
    double prev_error;          /**< Poprzedni błąd */
    double integral;            /**< Całkowita suma błędów */
    double setpoint;            /**< Punkt zadany (wartość docelowa) */

    double prev_input;          /**< Poprzednia próbka wejściowa */
    double prev_output;         /**< Poprzednia próbka wyjściowa */
    uint32_t sample_count;      /**< Licznik próbek */
    uint32_t delay_samples;     /**< Liczba próbek do zignorowania (obliczana na podstawie delay w sekundach) */
    double sampling_time;       /**< Czas próbkowania w sekundach */

    double integral_min;        /**< Minimalna wartość integratora (anty wind-up) */
    double integral_max;        /**< Maksymalna wartość integratora (anty wind-up) */
    double output_min;          /**< Minimalna wartość wyjściowa */
    double output_max;          /**< Maksymalna wartość wyjściowa */
} PID;

/**
 * @brief Inicjalizuje algorytm PID z opóźnieniem transportowym i systemem anty wind-up.
 *
 * Funkcja ta ustawia początkowe wartości wzmocnień PID, punkt zadany, a także inicjalizuje
 * parametry systemu anty wind-up oraz liczbę próbek do zignorowania w zależności od opóźnienia.
 *
 * @param pid Wskaźnik do struktury PID, która ma zostać zainicjalizowana.
 * @param Kp Wzmocnienie proporcjonalne.
 * @param Ki Wzmocnienie całkowite.
 * @param Kd Wzmocnienie różnicowe.
 * @param setpoint Punkt zadany (wartość docelowa).
 * @param delay Opóźnienie w sekundach.
 * @param sampling_time Czas próbkowania w sekundach.
 * @param integral_min Minimalna wartość integratora (zapobiega wind-up).
 * @param integral_max Maksymalna wartość integratora (zapobiega wind-up).
 * @param output_min Minimalna wartość wyjściowa (saturacja).
 * @param output_max Maksymalna wartość wyjściowa (saturacja).
 */
void PID_Init(PID *pid, double Kp, double Ki, double Kd, double setpoint,
              double delay, double sampling_time, double integral_min, double integral_max, double output_min, double output_max);

/**
 * @brief Oblicza wyjście PID z uwzględnieniem opóźnienia transportowego i systemu anty wind-up.
 *
 * Funkcja ta oblicza wartość wyjściową kontrolera PID, biorąc pod uwagę opóźnienie transportowe
 * w systemie oraz zapobiegając przeciążeniu integratora (wind-up) przez ograniczenie jego wartości.
 *
 * @param pid Wskaźnik do struktury PID.
 * @param input Aktualna wartość wejściowa do algorytmu PID.
 * @return Wyjście algorytmu PID z uwzględnieniem opóźnienia transportowego i systemu anty wind-up.
 */
double PID_Compute(PID *pid, double input);

/**
 * @brief Zmienia punkt zadany (setpoint) w algorytmie PID.
 *
 * Funkcja ta pozwala na dynamiczną zmianę punktu zadanego w trakcie pracy algorytmu PID.
 *
 * @param pid Wskaźnik do struktury PID, której punkt zadany ma zostać zmieniony.
 * @param setpoint Nowy punkt zadany (wartość docelowa).
 */
void change_PID_setpoint(PID *pid, double setpoint);

#ifdef __cplusplus
}
#endif

#endif /* PID_H */
