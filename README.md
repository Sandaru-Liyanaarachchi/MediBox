# MediBox
MediBox is the project that I had done for the module EN2853-Embedded Systems and Applications. This project utilizes the Wokwi platform for simulation and development. Medibox alerts patients when it's time to take their medicine through the buzzer sound, helping ensure timely and accurate medication adherence.
This solution is ideal for patients who struggle with remembering their medication schedules or require multiple daily doses. The project aims to enhance health management and patient well-being with a simple yet effective reminder system.

![medibox](https://github.com/user-attachments/assets/aab55161-7afd-45ef-ab83-ce8d00981114)

### Key Features:
- **Time Zone Setup**: Allows users to set the time zone by taking the offset from UTC as input.
- **Alarm System**: Users can set up to three alarms to remind them when to take their medication. Alarms can also be disabled when needed.
- **Real-Time Clock**: Fetches the current time from an NTP server based on the selected time zone and displays it on an OLED.
- **Environmental Monitoring**: Monitors temperature and humidity, providing warnings when values exceed healthy limits (26°C ≤ Temperature ≤ 32°C and 60% ≤ Humidity ≤ 80%).
- **Alarm Indication**: Alerts the user via buzzer, LED, or OLED display when an alarm is triggered. The alarm can be stopped using a push button.

### Technologies Used:
- ESP32 Microcontroller
- Wokwi Simulation
- NTP Protocol
- OLED Display

This project provides a user-friendly interface to set alarms, monitor environmental conditions, and manage medication reminders efficiently.
