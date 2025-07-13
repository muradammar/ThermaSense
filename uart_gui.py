import serial
import tkinter as tk
from tkinter import StringVar

SERIAL_PORT = '/dev/serial0'  #UARt receive port
BAUD_RATE = 9600  

#store last good readings
last_humidity = None
last_temperature = None

def read_serial(ser, label_var):
    global last_humidity, last_temperature

    if ser.in_waiting:
        try:
            line = ser.readline().decode('utf-8').strip()
            
            #format should match the .ino file
            if line.startswith("Temp:"):
                parts = line.split(',')
                temp_str = parts[0].replace("Temp:", "").replace("°C", "").strip()
                hum_str = parts[1].replace("Humidity:", "").replace("%", "").strip()

                temp = int(temp_str)
                hum = int(hum_str)

                #save valid readings
                last_temperature = temp
                last_humidity = hum

        except Exception:
            pass  # Ignore malformed lines

    #CHAT GPT GENERATED BELOW

    # Update label with last known good values
    if last_humidity is not None and last_temperature is not None:
        label_var.set(f"Humidity: {last_humidity}%\nTemperature: {last_temperature}°C")
    else:
        label_var.set("Waiting for valid data...")

    root.after(1000, read_serial, ser, label_var)

# Setup serial connection
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

# GUI setup
root = tk.Tk()
root.title("Arduino DHT Sensor Readings")

label_var = StringVar()
label = tk.Label(root, textvariable=label_var, font=("Arial", 24), padx=20, pady=20)
label.pack()

# Start reading loop
root.after(100, read_serial, ser, label_var)
root.mainloop()
