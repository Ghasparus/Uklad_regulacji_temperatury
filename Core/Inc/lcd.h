/**
 * @file lcd.h
 * @brief Biblioteka obsługująca wyświetlacz LCD 2x16 w trybie 4-bitowym na mikrokontrolerach STM32.
 *
 * Ten plik zawiera funkcje do obsługi wyświetlacza LCD zgodnego z kontrolerem HD44780
 * w trybie 4-bitowym. Funkcje obejmują inicjalizację, wysyłanie komend, czyszczenie ekranu,
 * ustawianie kursora oraz wyświetlanie tekstu.
 * 
 * @author Twoje Imię
 * @date 2025-01-22
 */

#ifndef LCD_H
#define LCD_H

#include "stm32f7xx_hal.h"

/**
 * @defgroup LCD_Pins Definicje pinów
 * @brief Definicje pinów dla wyświetlacza LCD.
 *
 * Zestawienie pinów GPIO używanych do sterowania wyświetlaczem LCD.
 * Numeracja pinów zaczyna się od 1 w dokumentacji.
 */

/** Pin RS (Rejestr wyboru) */
#define LCD_RS_PIN    GPIO_PIN_0  /**< RS Pin (pin 1) */
/** Pin RW (Rejestr zapisu/odczytu) */
#define LCD_RW_PIN    GPIO_PIN_1  /**< RW Pin (pin 2) */
/** Pin E (Enable) */
#define LCD_E_PIN     GPIO_PIN_7  /**< E Pin (pin 3) */
/** Pin D4 (Dane 4-bitowe) */
#define LCD_D4_PIN    GPIO_PIN_6  /**< D4 Pin (pin 4) */
/** Pin D5 (Dane 4-bitowe) */
#define LCD_D5_PIN    GPIO_PIN_5  /**< D5 Pin (pin 5) */
/** Pin D6 (Dane 4-bitowe) */
#define LCD_D6_PIN    GPIO_PIN_4  /**< D6 Pin (pin 6) */
/** Pin D7 (Dane 4-bitowe) */
#define LCD_D7_PIN    GPIO_PIN_3  /**< D7 Pin (pin 7) */

/** Port GPIO używany do sterowania wyświetlaczem */
#define LCD_GPIO_PORT GPIOD       /**< Port GPIOD */

/**
 * @brief Funkcja opóźnienia.
 *
 * Funkcja ta wykorzystuje funkcję `HAL_Delay` do generowania opóźnienia
 * w milisekundach. Jest używana podczas operacji na wyświetlaczu LCD, aby
 * zapewnić odpowiednią synchronizację z wyświetlaczem.
 *
 * @param delay Czas opóźnienia w milisekundach.
 */
void LCD_Delay(uint32_t delay);

/**
 * @brief Wysyłanie 4-bitowego nibbla do wyświetlacza LCD.
 *
 * Funkcja ta wysyła jeden nibble (4 bity) do wyświetlacza LCD poprzez
 * ustawienie odpowiednich pinów D4-D7.
 *
 * @param nibble Wartość 4-bitowa, która ma zostać wysłana.
 */
void LCD_SendNibble(uint8_t nibble);

/**
 * @brief Wysyłanie komendy do wyświetlacza LCD.
 *
 * Funkcja ta wysyła 8-bitową komendę do wyświetlacza LCD w trybie 4-bitowym.
 * Komenda jest wysyłana po dwóch nibblich (górnym i dolnym).
 * 
 * @param command Wartość 8-bitowej komendy.
 */
void LCD_SendCommand(uint8_t command);

/**
 * @brief Wysyłanie danych do wyświetlacza LCD.
 *
 * Funkcja ta wysyła dane (np. znaki tekstu) do wyświetlacza LCD w trybie 4-bitowym.
 * Dane są przesyłane po dwóch nibblich (górnym i dolnym).
 * 
 * @param data Wartość danych, które mają zostać wysłane (np. znak).
 */
void LCD_SendData(uint8_t data);

/**
 * @brief Inicjalizacja wyświetlacza LCD.
 * 
 * Funkcja ta wykonuje pełną inicjalizację wyświetlacza LCD w trybie 4-bitowym
 * oraz ustawia parametry wyświetlania, takie jak włączenie wyświetlacza,
 * ustawienie kursora, tryb przesuwania itp.
 */
void LCD_Init(void);

/**
 * @brief Czyszczenie ekranu LCD.
 *
 * Funkcja ta wysyła komendę do wyświetlacza, aby wyczyścić ekran.
 * Po jej wykonaniu wyświetlacz zostaje wyczyszczony, a kursor ustawiony
 * na początek.
 */
void LCD_Clear(void);

/**
 * @brief Ustawienie kursora na określonym wierszu i kolumnie.
 *
 * Funkcja ta ustawia kursor na wskazanej pozycji na wyświetlaczu LCD.
 * Pozwala na kontrolowanie, gdzie pojawi się kolejny znak.
 * 
 * @param row Numer wiersza (0 lub 1).
 * @param col Numer kolumny (0-15).
 */
void LCD_SetCursor(uint8_t row, uint8_t col);

/**
 * @brief Wyświetlanie tekstu na LCD.
 *
 * Funkcja ta wyświetla ciąg znaków na wyświetlaczu LCD. Przesyła jeden
 * znak po drugim, aż do końca ciągu.
 * 
 * @param text Wskaźnik na ciąg znaków, który ma zostać wyświetlony.
 */
void LCD_Print(uint8_t* text);

#endif /* LCD_H */
