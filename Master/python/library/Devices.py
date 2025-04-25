## this is master for connected devices.
import struct
from crccheck.crc import Crc32Mpeg2 as CRC32
import time
import serial
import enum

class SerialPort:
    def __init__(self, port_name, baudrate=921600, timeout=0.1):
        import serial
        self.port_name = port_name
        self.baudrate = baudrate
        self.timeout = timeout
        self.ph = serial.Serial(port=self.port_name, baudrate=self.baudrate, timeout=self.timeout)

    def close_port(self):
        if self.ph and self.ph.is_open:
            self.ph.close()
            print(f"Port '{self.port_name}' kapatıldı.")
        else:
            print(f"Port '{self.port_name}' zaten kapalı.")

    def __del__(self):
        try:
            self.close_port()
        except Exception as e:
            print(f"AcromeDevicesPort yok edilirken bir hata oluştu: {e}")


#Classical Device Indexes
Index_Device_Classical = enum.IntEnum('Index', [
	'Header',
	'DeviceID',
    'DeviceFamily',
	'PackageSize',
	'Command',
    'Status',
	'HardwareVersion',
	'SoftwareVersion',
    'Baudrate',
], start=0)


#Classical_Commands
class Device_Commands(enum.IntEnum):
    PING = 0x00,
    READ = 0x01,
    WRITE = 0x02,
    REBOOT = 0x10,
    EEPROM_WRITE = 0x20,
    FACTORY_RESET = 0x25,
    BL_JUMP = 0x30,
    WRITE_SYNC = 0x40,
    ACK = 0x80,
    WRITE_ACK = 0x80 | 0x02


class Embedded_Device():
    _BATCH_ID = 254
    def __init__(self, header, id, device_family, variables, port:SerialPort, _test = False):
        self._test = _test
        if _test == False:
            self.__ph = port.ph
        self._header = header
        self._id = id
        self._device_family = device_family
        self._vars = variables
        self._ack_size = 0

    
    def _write_bus(self, data):
        self.__ph.write(data)

    def _read_bus(self, size) -> bytes:
        self.__ph.flushInput()
        return self.__ph.read(size=size)
    
    def _parse_received(self, data):
        id = data[Index_Device_Classical.DeviceID]
        data = data[4:-4]
        fmt_str = '<'

        i = 0
        while i < len(data):
            fmt_str += 'B' + self._vars[data[i]].type()
            i += self._vars[data[i]].size() + 1

        unpacked = list(struct.unpack(fmt_str, data))
        grouped = zip(*(iter(unpacked),) * 2)
        for group in grouped:
            self._vars[group[0]].value(group[1])
    
    def _read_ack(self) -> bool:
        ret = self._read_bus(self._ack_size)
        print(list(ret))
        if len(ret) == self._ack_size:
            if (CRC32.calc(ret[:-4]) == struct.unpack('<I', ret[-4:])[0]):
                if ret[2] > 8:
                    #print("parse daha yazilmadi.")
                    #print(list(ret))
                    self._parse_received(ret)
                    return True
                else:
                    return True # ping islemi ve WRITE_ACK icin.
            else:
                return False
        else:
            return False
        
    def _read_var_no_timeout(self):
        self.__ph.timeout = 30
        ack_flag = self._read_ack()
        self.__ph.timeout = 0.1
        if ack_flag:
            return True
        else:
            return False 

    def ping(self):
        fmt_str = '<BBBBBB'
        struct_out = list(struct.pack(fmt_str, *[self._header, self._id, self._device_family, 10, Device_Commands.PING, 0]))
        struct_out = bytes(struct_out) + struct.pack('<I', CRC32.calc(struct_out))
        self._ack_size = 10
        #burayi kontrol et.
        if self._test == True:
            print(list(struct_out))
            return True
        
        self._write_bus(struct_out)
        
        if self._read_ack():
            return True
        else:
            return False
    
    def read_var(self, *indexes):
        self._ack_size = 0
        fmt_str = '<BBBB'+'B'*len(indexes)
        struct_out = list(struct.pack(fmt_str, *[self._header, self._id, len(indexes) + 8, Device_Commands.READ, *indexes]))
        struct_out = bytes(struct_out) + struct.pack('<' + 'I', CRC32.calc(struct_out))
        for i in indexes:
            self._ack_size += (self._vars[int(i)].size() + 1)
        self._ack_size += 8

        if self._test == True:
            print(list(struct_out))
            return struct_out

        self._write_bus(struct_out)

        if self._read_ack():
            return [self._vars[index].value() for index in indexes]
        else:
            return [None]

    def write_var(self, *idx_val_pairs):
        # returns : did ACK come?
        fmt_str = '<BBBB'
        var_count = 0
        size = 0
        for one_pair in idx_val_pairs:
            try:
                if len(one_pair) != 2:
                    raise ValueError(f"{one_pair} more than a pair! It is not a pair")
                else:
                    fmt_str += ('B' + self._vars[one_pair[0]].type())
                    var_count+=1
                    size += (1 + self._vars[one_pair[0]].size())
            except:
                raise ValueError(f"{one_pair} is not proper pair")
        
        flattened_list = [item for sublist in idx_val_pairs for item in sublist]

        struct_out = list(struct.pack(fmt_str, *[self._header, self._id, size + 8, Device_Commands.WRITE, *flattened_list]))
        struct_out = bytes(struct_out) + struct.pack('<' + 'I', CRC32.calc(struct_out))
        self._ack_size = 8

        if self._test == True:
            print(list(struct_out))
            return
        
        self._write_bus(struct_out)
        if self._read_ack():
            return True
        else:
            return False
        
    def reboot(self):
        fmt_str = '<BBBB'
        struct_out = list(struct.pack(fmt_str, *[self._header, self._id, 8, Device_Commands.REBOOT]))
        struct_out = bytes(struct_out) + struct.pack('<' + 'I', CRC32.calc(struct_out))    
        try:     
            self._write_bus(struct_out)
        except:
            print("port error.....")
	
    def eeprom_save(self):
        fmt_str = '<BBBB'
        struct_out = list(struct.pack(fmt_str, *[self._header, self._id, 8, Device_Commands.EEPROM_WRITE]))
        struct_out = bytes(struct_out) + struct.pack('<' + 'I', CRC32.calc(struct_out))
        self._write_bus(struct_out)
        try:     
            self._write_bus(struct_out)
            if self._read_ack(id):
                return True
        except:
            print("port error.....")

    def factory_reset(self, ack=False):
        fmt_str = '<BBBB'            
        struct_out = list(struct.pack(fmt_str, *[self._header, self._id, 8, Device_Commands.FACTORY_RESET]))
        struct_out = bytes(struct_out) + struct.pack('<' + 'I', CRC32.calc(struct_out))
        if self._test:
            print(list(struct_out))
            return
        self._write_bus(struct_out)
        if ack:
            try:     
                self._write_bus(struct_out)
                if self._read_ack(id):
                    return True
            except:
                return False


    def _bootloader_jump(self):
        pass

    def get_all_variable(self):
        for i in range(0, len(self._vars), 10):
            j = i
            k = min(i + 9, len(self._vars) - 1)  # Son grupta sınırlamayı sağlar
            index_list = list(range(j, k + 1))
            self.read_var(*index_list) # her birisi maksimum data sayisiymis gibi dusunerek yazarsak 4 byte olur. her bir pakette 10 adet alsin. maksimuma vurmak istemedigimizden dolayi.

class Data_():
    def __init__(self, index, var_type, rw=True, value = 0):
        self.__index = index
        self.__type = var_type
        self.__size  = struct.calcsize(self.__type)
        self.__value = value
        self.__rw = rw

    def value(self, value=None):
        if value is None:
            return self.__value
        elif self.__rw:
            self.__value = struct.unpack('<' + self.__type, struct.pack('<' + self.__type, value))[0]

    def index(self) ->enum.IntEnum:
        return self.__index
    
    def writeable(self) -> bool:
        return self.__rw

    def size(self) -> int:
        return self.__size
	
    def type(self) -> str:
        return self.__type
        