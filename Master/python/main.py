from library.serial_devices import *


port_name = "COM5"
baudrate = 115200
port = SerialPort(port_name, baudrate)
water_system = Device(0, port, False)


while True:
    print(water_system.ping())
    time.sleep(5)




