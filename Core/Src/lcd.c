#include "lcd.h"

/**
 * @brief Funkcja opóźnienia.
 * 
 * Funkcja wykorzystuje funkcję `HAL_Delay` do wprowadzenia opóźnienia
 * w czasie wykonywania programu.
 * 
 * @param delay Czas opóźnienia w milisekundach.
 */
void LCD_Delay(uint32_t delay)
{
    HAL_Delay(delay);  /**< Funkcja wprowadzająca opóźnienie w milisekundach */
}

/**
 * @brief Wysyła 4 bity na wyświetlacz LCD.
 * 
 * Funkcja ustawia odpowiednie piny GPIO do przesłania 4 bitów (nibbles)
 * na wyświetlacz LCD.
 * 
 * @param nibble 4 bity do wysłania.
 */
void LCD_SendNibble(uint8_t nibble)
{
    if (nibble & 0x08) HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D7_PIN, GPIO_PIN_SET);
    else HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D7_PIN, GPIO_PIN_RESET);
    
    if (nibble & 0x04) HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D6_PIN, GPIO_PIN_SET);
    else HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D6_PIN, GPIO_PIN_RESET);
    
    if (nibble & 0x02) HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D5_PIN, GPIO_PIN_SET);
    else HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D5_PIN, GPIO_PIN_RESET);
    
    if (nibble & 0x01) HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D4_PIN, GPIO_PIN_SET);
    else HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D4_PIN, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_E_PIN, GPIO_PIN_SET);
    LCD_Delay(1);  /**< Krótkie opóźnienie */
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_E_PIN, GPIO_PIN_RESET);
    LCD_Delay(1);  /**< Krótkie opóźnienie */
}

/**
 * @brief Wysyła komendę do wyświetlacza LCD.
 * 
 * Funkcja ustawia linię RS na 0 (komenda) i wysyła 8-bitową komendę
 * poprzez przesłanie jej w dwóch częściach (4 bity na raz).
 * 
 * @param cmd Komenda do wysłania.
 */
void LCD_SendCommand(uint8_t cmd)
{
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_RS_PIN, GPIO_PIN_RESET);  /**< Ustawienie RS na 0 (komenda) */
    
    LCD_SendNibble(cmd >> 4);  /**< Wysyłanie wyższej części komendy */
    LCD_SendNibble(cmd & 0x0F); /**< Wysyłanie niższej części komendy */
    
    LCD_Delay(2); /**< Opóźnienie po wysłaniu komendy */
}

/**
 * @brief Wysyła dane (znak) do wyświetlacza LCD.
 * 
 * Funkcja ustawia linię RS na 1 (dane) i wysyła 8-bitowe dane
 * (znak) na wyświetlacz w dwóch częściach.
 * 
 * @param data Znak do wysłania.
 */
void LCD_SendData(uint8_t data)
{
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_RS_PIN, GPIO_PIN_SET);  /**< Ustawienie RS na 1 (dane) */
    
    LCD_SendNibble(data >> 4);  /**< Wysyłanie wyższej części danych */
    LCD_SendNibble(data & 0x0F); /**< Wysyłanie niższej części danych */
    
    LCD_Delay(1); /**< Opóźnienie po wysłaniu danych */
}

/**
 * @brief Inicjalizuje wyświetlacz LCD.
 * 
 * Funkcja konfiguruje piny GPIO, ustawia wyświetlacz w trybie 4-bitowym,
 * włącza wyświetlacz, ustawia kursor i czyszczenie ekranu.
 */
void LCD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOA_CLK_ENABLE();  /**< Włączenie zegara dla portu GPIOA */
    
    GPIO_InitStruct.Pin = LCD_RS_PIN | LCD_RW_PIN | LCD_E_PIN | LCD_D4_PIN | LCD_D5_PIN | LCD_D6_PIN | LCD_D7_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LCD_GPIO_PORT, &GPIO_InitStruct);
    
    LCD_Delay(50);  /**< Opóźnienie po włączeniu zasilania LCD */
    
    LCD_SendNibble(0x03);  /**< Pierwszy krok inicjalizacji */
    LCD_Delay(5);  /**< Opóźnienie */
    LCD_SendNibble(0x03);  /**< Drugi krok inicjalizacji */
    LCD_Delay(5);  /**< Opóźnienie */
    LCD_SendNibble(0x03);  /**< Trzeci krok inicjalizacji */
    LCD_Delay(5);  /**< Opóźnienie */
    LCD_SendNibble(0x02);  /**< Czwarty krok inicjalizacji */
    
    LCD_SendCommand(0x28);  /**< Ustawienie trybu 4-bitowego (2 linie, czcionka 5x8) */
    LCD_SendCommand(0x0C);  /**< Włączenie wyświetlacza (kursor off) */
    LCD_SendCommand(0x06);  /**< Ustawienie przesuwania kursora */
    LCD_Clear();            /**< Wyczyść ekran */
}

/**
 * @brief Czyści ekran wyświetlacza LCD.
 */
void LCD_Clear(void)
{
    LCD_SendCommand(0x01);  /**< Komenda wyczyszczenia ekranu */
    LCD_Delay(2);           /**< Opóźnienie po wysłaniu komendy */
}

/**
 * @brief Ustawia kursor na wyświetlaczu LCD.
 * 
 * @param row Numer wiersza (0 lub 1).
 * @param col Numer kolumny (0-15).
 */
void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t address;
    
    if (row == 0)
        address = 0x00 + col;  /**< Pierwsza linia */
    else if (row == 1)
        address = 0x40 + col;  /**< Druga linia */
    
    LCD_SendCommand(0x80 | address);  /**< Ustawienie pozycji kursora */
}

/**
 * @brief Wypisuje łańcuch znaków na wyświetlaczu LCD.
 * 
 * @param str Łańcuch znaków do wyświetlenia.
 */
void LCD_Print(uint8_t *str)
{
    while (*str) {
        LCD_SendData(*str++);  /**< Wysyłanie znaków na wyświetlacz */
    }
}
