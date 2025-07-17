## this file cointains device special commands and parameters.
import enum
import struct
from crccheck.crc import Crc32Mpeg2 as CRC32
import time
import serial
from library.Devices import *

# enter here for extra commands: 
#class Device_ExtraCommands(enum.IntEnum):

Index_Device = enum.IntEnum('Index', [
	'Header',
	'DeviceID',
	'DeviceFamily',
	'PackageSize',
	'Command',
	'Status',
	'HardwareVersion',
	'SoftwareVersion',
	'Baudrate', #'WritableStart' = iBaudrate
	# user parameter start

	# user parameter end
	'CRCValue',
], start=0)


class Device(Embedded_Device):
	DEVICE_FAMILY = 0x01

	def __init__(self, ID, port:SerialPort) -> bool:
		
		self.__ack_size = 0
		if ID > 255 or ID < 0:
			raise ValueError("Device ID can not be higher than 253 or lower than 0!")
		device_special_data = [
			Data_(Index_Device.Header, 'B', False, 0x55),
			Data_(Index_Device.DeviceID, 'B'),
			Data_(Index_Device.DeviceFamily, 'B'),
			Data_(Index_Device.PackageSize, 'B'),
			Data_(Index_Device.Command, 'B'),
			Data_(Index_Device.Status, 'B'),
			Data_(Index_Device.HardwareVersion, 'I'),
			Data_(Index_Device.SoftwareVersion, 'I'),
			Data_(Index_Device.Baudrate, 'I'),
			# user parameter starts

			# user parameter end			
			Data_(Index_Device.CRCValue, 'I'),
		]
		super().__init__(ID, self.DEVICE_FAMILY, device_special_data, port)
		self._vars[Index_Device.DeviceID].value(ID)

	# user start for extra commands.
	#def command(self): 

def scan_XXXX_devices(port:SerialPort):
	device = Device(0, port)
	available_devices = []

	for id in range(0,255):
		device._id = id
		if(device.ping()):
			available_devices.append(id)

	return available_devices
