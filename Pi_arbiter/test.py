from datetime import datetime as dt
from pcf8574 import PCF8574

class ArbiterIO:
    def __init__(self):
        self.pcfs = self.__pcf_init()
    def __pcf_init(self):
        pcf_list = []
        for index, pcf_add in enumerate([0x38, 0x39, 0x3A, 0x3C, 0x3D, 0x3E]):
            print(pcf_add)
            pcf_list.append(PCF8574(1, pcf_add))
            for pin in range(0,8):
                pcf_list[index].port[pin] = True
        return pcf_list
    def read_pcf(self, pcf_index):
        pcf = self.pcfs[pcf_index]
        result = 0
        for pin_index in range(0, 8):
            pin = 7 - pin_index
            print(f"pin read {pin}")
            try:
                ret = bool(pcf.port[pin])
                print(ret)
                if not ret:
                    result += (1 << (7 - pin_index))
            except IOError:
                print(f"error reading PCF {pcf_index}")
        print(result)
        return result
    def write_pcf(self, pcf_index, value):
        for pin_index in range(0, 8):
            pin = 7 - pin_index
            pin_value = 1 << (8-pin_index)
            print(f"{pin_value}: {pin}")
            output = not value & pin_value
            print(output)
            try:
                self.pcfs[pcf_index].port[pin] = output
            except IOError:
                print("IOError")
                pass



my_pcfs = ArbiterIO()

def main():
    for no in range(0, 4):
        print(no)
    pcf = 0
    for value in [1,2,4,8]:
        value = 13
        my_pcfs.write_pcf(pcf, value)
        if my_pcfs.read_pcf(1) != value:
            print("failed")
            exit()




main()