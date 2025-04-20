from library.Device import *

#port = SerialPort("COM11")

def scan_Devices(port:SerialPort):
    id_list = []
    for i in range(255):
        print(i)
        dev = Device(i, port)
        if dev.ping()== True:
            id_list.append(i)
    return id_list


#print(scan_Stewarts(port))