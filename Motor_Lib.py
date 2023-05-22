from time import sleep
MODE1 = 0x00
PRE_SCALE = 0xFE
LED0_OFF_L = 0x06
LED0_OFF_H = 0x07
LED0_OFF_L = 0x08
LED0_OFF_H = 0x09

REG_ALL_ON_L = 0xFA
REG_ALL_ON_H = 0xFB
REG_ALL_OFF_L = 0xFC
REG_ALL_OFF_H = 0xFD

SLEEP = 0x10
RESTART = 0x80

class PWM:
    def __init__(self, bus, address=0x5E): #chip address 0x5e
        self.bus = bus
        self.address = address
        self.writeByte(MODE1, 0x00)

    def set_frequency(self, frequency):
        #print(f"frequency : {frequency}")
        base_mode = self.readByte(MODE1) & 0xFF
        self.writeByte(MODE1, base_mode | SLEEP)

        prescale = (25_000_000.0 / (4_096 * frequency) + 0.5) - 1

        self.writeByte(PRE_SCALE, int(prescale))
        self.writeByte(MODE1, base_mode)

        sleep(0.001)

        self.writeByte(MODE1, base_mode | RESTART)

    def set_duty(self, pin, duty_cycle):
        channel = pin * 4
        duty_off = int(duty_cycle) & 0xFFFF
        self.writeByte(LED0_OFF_L + channel, duty_off & 0xFF)
        self.writeByte(LED0_OFF_H + channel, duty_off >> 8)

    def writeByte(self, register, value):
        try:
            #print(f'vlaue : {value} , type : {type(value)}')
            self.bus.write_byte_data(self.address, register, value)
        except:
            print("Error writing to I2C by writeByte")
            pass

    def readByte(self, register):
        try:
            value = self.bus.read_byte_data(self.address, register)
            #print(f"value : {value} , type : {type(value)}")
            return value
        except:
            print("Error writing to I2C by readByte")
            return None


