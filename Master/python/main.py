from library.Device import *

port_name = "COM5"
baudrate = 115200
port = SerialPort(port_name, baudrate, isTest=True)
water_system = Device(0, port)

scan_XXXX_devices(port)

