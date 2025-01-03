import serial
import csv
import time
import os

# Configure serial port 
ser = serial.Serial('COM3', 115200)  
csv_filename_prefix = "D:/handwriting from motion/abhishek/9"
counter = 0
while True:
    csv_filename = f"{csv_filename_prefix}.csv"
    if not os.path.exists(csv_filename):
        break
    counter += 1
csvfile = open(csv_filename, 'w', newline='')
csvwriter = csv.writer(csvfile)

# Write header
csvwriter.writerow(['accelX', 'accelY', 'accelZ', 'gyroX', 'gyroY', 'gyroZ'])
print("Recording...")

try:
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()
            if line == "Recording Stopped":
                print("Done recording.")
                break
            elif line == "Recording Started":
                 print("Started")
            elif line == "Ready":
                 print("Waiting to record...")
            else:
                try:
                    values = line.split(',')
                    values = [float(val) for val in values]
                    csvwriter.writerow(values)
                except ValueError:
                     pass # Skip invalid lines

except KeyboardInterrupt:
    print("\nSaving...")
finally:
    ser.close()
    csvfile.close()