#include "pid.h"
#include <math.h>

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
              double delay, double sampling_time, double integral_min, double integral_max, double output_min, double output_max)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->prev_error = 0.0;
    pid->integral = 0.0;
    pid->setpoint = setpoint;
    pid->sampling_time = sampling_time;
    pid->integral_min = integral_min;
    pid->integral_max = integral_max;
    pid->output_min = output_min;
    pid->output_max = output_max;

    // Obliczamy liczbę próbek do zignorowania
    pid->delay_samples = (uint32_t)floor(delay/sampling_time);
    pid->sample_count = pid->delay_samples;

    pid->prev_input = 0.0; // Zainicjalizuj poprzednią próbkę wejściową
    pid->prev_output = 0.0; // Zainicjalizuj poprzednią próbkę wyjściową
}

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
double PID_Compute(PID *pid, double input)
{
    pid->sample_count++;

    // Ignorowanie próbek, jeżeli nie osiągnęliśmy jeszcze liczby próbek do opóźnienia
    if (pid->sample_count <= pid->delay_samples) {
        pid->prev_input = input;  // Przechowujemy ostatnią próbkę
        return pid->prev_output;
    }

    // Obliczamy błąd
    double error = pid->setpoint - pid->prev_input;

    // Całkowity błąd (sumowanie błędów)
    pid->integral += error;

    // Ogranicz wartość całkowitego błędu, aby zapobiec wind-up
    if (pid->integral > pid->integral_max) {
        pid->integral = pid->integral_max;
    } else if (pid->integral < pid->integral_min) {
        pid->integral = pid->integral_min;
    }

    // Oblicz pochodną (na podstawie różnicy pomiędzy aktualną próbą a poprzednią)
    double derivative = input - pid->prev_input;

    // Oblicz wyjście PID
    double output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;

    // Ogranicz wyjście PID, aby nie przekroczyło zakresu
    if (output > pid->output_max) {
        output = pid->output_max;
    } else if (output < pid->output_min) {
        output = pid->output_min;
    }

    // Zaktualizuj poprzednią próbkę wejściową
    pid->prev_input = input;

    pid->prev_output = output;

    pid->sample_count = 0;

    return output;
}

/**
 * @brief Zmienia punkt zadany (setpoint) w algorytmie PID.
 *
 * Funkcja ta pozwala na dynamiczną zmianę punktu zadanego w trakcie pracy algorytmu PID.
 *
 * @param pid Wskaźnik do struktury PID, której punkt zadany ma zostać zmieniony.
 * @param setpoint Nowy punkt zadany (wartość docelowa).
 */
void change_PID_setpoint(PID *pid, double setpoint)
{
    pid->setpoint = setpoint;
}
