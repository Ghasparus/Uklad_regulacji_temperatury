import csv
from tkinter import *
import customtkinter
import serial
from PIL import Image, ImageTk
from datetime import datetime
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

# Inicjalizacja portu szeregowego
try:
    ser = serial.Serial(port="COM3", baudrate=9600, timeout=1)
except serial.SerialException as e:
    print(f"Nie udało się otworzyć portu COM3: {e}")
    ser = None

# Globalne zmienne do przechowywania danych dla wykresu
times = []  # Czas
actual_values = []  # Wartości aktualne
desired_value = None  # Wartość zadana

# Funkcja zapisu danych do pliku CSV
def save_to_csv(time, actual_value, desired_value):
    try:
        with open('dane.csv', mode='a', newline='') as file:
            writer = csv.writer(file)
            writer.writerow([time, actual_value, desired_value])
            print(f"Zapisano do pliku CSV: {time}, {actual_value}, {desired_value}")
    except Exception as e:
        print(f"Błąd zapisu do pliku CSV: {e}")

# Funkcja obsługująca kliknięcie przycisku wysyłającego
def button_callback():
    if ser and ser.is_open:
        try:
            value = entry.get()
            if value.isdigit() or value.replace(".", "", 1).isdigit():
                ser.write(f"Z{value}\n".encode())
                print(f"Wiadomość 'Z{value}' wysłana przez port COM3.")
            else:
                print("Proszę wpisać poprawną wartość liczbową.")
        except serial.SerialException as e:
            print(f"Błąd podczas wysyłania wiadomości: {e}")
    else:
        print("Port COM3 nie jest otwarty.")

# Funkcja przetwarzająca dane z portu szeregowego
def process_serial_data(data):
    global desired_value
    try:
        if data.startswith("Z") and "A" in data:
            zadana = float(data.split("A")[0][1:])  # Wartość po 'Z', przed 'A'
            aktualna = float(data.split("A")[1])   # Wartość po 'A'
            desired_value = zadana  # Aktualizacja wartości zadanej

            # Aktualizacja etykiet
            label_zadana.configure(text=f"Wartość zadana: {zadana}")
            label_aktualna.configure(text=f"Wartość aktualna: {aktualna}")

            # Aktualizacja danych dla wykresu
            current_time = datetime.now().strftime("%H:%M:%S")
            times.append(current_time)
            actual_values.append(aktualna)

            # Zapisz dane do pliku CSV
            save_to_csv(current_time, aktualna, desired_value)

            update_plot()
        else:
            print(f"Nieprawidłowy format danych: {data}")
    except Exception as e:
        print(f"Błąd przetwarzania danych: {e}")

# Funkcja do odczytu danych z portu szeregowego z wykorzystaniem after()
def read_serial():
    if ser and ser.is_open:
        try:
            if ser.in_waiting > 0:  # Sprawdzanie, czy są dane na porcie
                data = ser.readline().decode().strip()
                if data:
                    process_serial_data(data)
        except serial.SerialException as e:
            print(f"Błąd podczas odczytu danych: {e}")
        except Exception as e:
            print(f"Nieoczekiwany błąd: {e}")
    app.after(500, read_serial)  # Wywołanie co 500 ms

# Aktualizacja wykresu
def update_plot():
    ax.clear()
    ax.plot(times, actual_values, label="Wartość aktualna", marker='o')
    if desired_value is not None:
        ax.axhline(y=desired_value, color='r', linestyle='--', label=f"Wartość zadana ({desired_value})")
    ax.legend()
    ax.set_title("Wykres wartości aktualnej")
    ax.set_xlabel("Czas")
    ax.set_ylabel("Wartość")
    ax.grid()
    canvas.draw()

# Okno aplikacji
app = customtkinter.CTk()
app.geometry("1000x800")
app.title("Project Manager")

# Logo aplikacji
try:
    logo = PhotoImage(file="PP_logo.png")
    app.iconphoto(True, logo)
    image = Image.open("PP_logo.png")
    image = image.resize((200, 200))
    photo = ImageTk.PhotoImage(image)
    label_image = customtkinter.CTkLabel(app, image=photo, text="")
    label_image.place(relx=0.5, rely=0.15, anchor="center")
except Exception as e:
    print(f"Błąd ładowania obrazu: {e}")

# Wyświetlanie wartości z portu szeregowego
label_zadana = customtkinter.CTkLabel(app, text="Wartość zadana: Brak", font=("Arial", 12))
label_zadana.place(relx=0.1, rely=0.4, anchor="center")

label_aktualna = customtkinter.CTkLabel(app, text="Wartość aktualna: Brak", font=("Arial", 12))
label_aktualna.place(relx=0.1, rely=0.5, anchor="center")

# Przyciski
button = customtkinter.CTkButton(app, text="Wyślij", command=button_callback, width=100)
button.place(relx=0.67, rely=0.5, anchor="center")

label_text = customtkinter.CTkLabel(app, text="Project Manager", font=("Futura", 20))
label_text.place(relx=0.5, rely=0.42, anchor="center")

label_authors = customtkinter.CTkLabel(app, text="Autorzy: Sebastian Nachowiak, Kacper Snopek", font=("Futura", 10))
label_authors.place(relx=0.02, rely=0.98, anchor="sw")

# Aktualizacja czasu
def update_time():
    current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    label_time.configure(text=current_time)
    label_time.after(1000, update_time)

label_time = customtkinter.CTkLabel(app, text="", font=("Arial", 12))
label_time.place(relx=0.02, rely=0.02, anchor="nw")
update_time()

# Pole tekstowe do wpisywania wartości zadanej
entry = customtkinter.CTkEntry(app, placeholder_text="Wpisz wartość zadaną", width=200)
entry.place(relx=0.5, rely=0.5, anchor="center")

# Tworzenie wykresu
fig = Figure(figsize=(8, 4), dpi=100)
ax = fig.add_subplot(111)
canvas = FigureCanvasTkAgg(fig, master=app)
canvas.get_tk_widget().place(relx=0.5, rely=0.8, anchor="center", width=900, height=300)

# Zamknięcie portu szeregowego
def close_port():
    if ser and ser.is_open:
        ser.close()
        print("Port COM zamknięty.")

app.protocol("WM_DELETE_WINDOW", close_port)

# Uruchomienie odczytu z portu szeregowego
if ser and ser.is_open:
    read_serial()

app.mainloop()
