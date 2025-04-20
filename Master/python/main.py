from library.serial_devices import *


port_name = "COM3"
baudrate = 115200
#port = SerialPort(port_name)
water_system = Device(0, port_name, True)


while True:
    print(water_system.ping())
    time.sleep(1)




