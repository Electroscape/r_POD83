try:
    from pcf8574 import PCF8574
except ImportError:
    from PCF_dummy import PCF8574


class ArbiterIO:
    def __init__(self):
        self.pcfs = self.__pcf_init()

    @staticmethod
    def __pcf_init():
        pcf_list = []
        for index, pcf_add in enumerate([0x38, 0x39, 0x3A, 0x3C, 0x3D, 0x3E]):
            print(pcf_add)
            pcf_list.append(PCF8574(1, pcf_add))
            for pin in range(0, 8):
                pcf_list[index].port[pin] = True
        return pcf_list
    def read_pcf(self, pcf_index):
        pcf = self.pcfs[pcf_index]
        result = 0
        for pin_index in range(0, 8):
            pin = 7 - pin_index
            try:
                ret = bool(pcf.port[pin])
                if not ret:
                    result += (1 << (7 - pin_index))
            except IOError:
                print(f"error reading PCF {pcf_index}")
        return result
    def write_pcf(self, pcf_index, value):
        for pin_index in range(0, 8):
            pin = 7 - pin_index
            pin_value = 1 << (7 - pin_index)
            output = not value & pin_value
            try:
                self.pcfs[pcf_index].port[pin] = output
            except IOError:
                print("IOError")
                pass
    def get_inputs(self):
        inputs = []
        for input_pcf in range(3, 6):
            pin_value = self.read_pcf(input_pcf)
            if pin_value > 0:
                inputs.append((input_pcf, pin_value))
        return inputs


