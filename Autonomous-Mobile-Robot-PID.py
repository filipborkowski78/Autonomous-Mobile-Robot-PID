import serial
import time
import sys

BLUETOOTH_PORT = 'COM5'
BAUD_RATE = 9600


def main():
    print(f"Łączenie z robotem na porcie {BLUETOOTH_PORT}...")

    try:
        ser = serial.Serial(BLUETOOTH_PORT, BAUD_RATE, timeout=1)
        time.sleep(2)
        print("\n=== POŁĄCZONO Z ROBOTEM ===")
        print("Dostępne komendy:")
        print(" K       -> Uruchom KALIBRACJĘ (trwa 5 sekund)")
        print(" P       -> START / STOP jazdy")
        print(" Kp 0.1  -> Ustaw Kp")
        print(" Kd 0.8  -> Ustaw Kd")
        print(" Vr 35   -> Ustaw prędkość")
        print(" exit    -> Wyjście z programu")
        print("-" * 40)

    except serial.SerialException as e:
        print(f"Błąd połączenia: {e}")
        return

    try:
        while True:
            # 1. Pobierz komendę
            user_input = input("Komenda >> ")

            if user_input.lower() == 'exit':
                break

            # 2. Wyślij do Arduino
            command_to_send = user_input + '\n'
            ser.write(command_to_send.encode('utf-8'))

            # 3. Odbierz odpowiedź
            time.sleep(0.1)
            while ser.in_waiting > 0:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    print(f"[ROBOT]: {line}")

    except KeyboardInterrupt:
        print("\nPrzerwano.")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
        print("Rozłączono.")


if __name__ == "__main__":
    main()