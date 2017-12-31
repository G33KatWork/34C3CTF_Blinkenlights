#taken from sigrok and adapted to my code

from functools import reduce

class Decoder(object):
    def __init__(self, samplerate, put_callback):
        self.samplerate = samplerate
        self.oldpin = None
        self.packet_ss = None
        self.start_samplenum = None
        self.end_samplenum = None
        self.bits = []
        self.inreset = False
        self.put = put_callback

    def put(self, *args, **kwargs):
        print(args, kwargs)

    def handle_bits(self, samplenum):
        if len(self.bits) == 24:
            grb = reduce(lambda a, b: (a << 1) | b, self.bits)
            rgb = (grb & 0xff0000) >> 8 | (grb & 0x00ff00) << 8 | (grb & 0x0000ff)

            self.put(self.packet_ss, samplenum, "COLOR", rgb)

            self.bits = []
            self.packet_ss = None

    def decode(self, data):
        for (samplenum, pin) in data:
            # NOTE: timings
            # https://cpldcpu.wordpress.com/2014/01/14/light_ws2812-library-v2-0-part-i-understanding-the-ws2812/

            if self.oldpin is None:
                self.oldpin = pin
                continue

            # check RESET condition (mfg recommend 50 usec minimal, but real minimum is ~10 usec)
            if not self.inreset and not pin and \
               self.end_samplenum is not None and \
               (samplenum - self.end_samplenum) / self.samplerate > 50e-6:
                # decode last bit value
                tH = (self.end_samplenum - self.start_samplenum) / self.samplerate
                bit_ = True if tH >= 625e-9 else False

                self.bits.append(bit_)
                self.handle_bits(self.end_samplenum)

                self.put(self.start_samplenum, self.end_samplenum, "BIT", bit_)
                self.put(self.end_samplenum, samplenum, "RESET", 0)

                self.inreset = True
                self.bits = []
                self.packet_ss = None
                self.start_samplenum = None

            if not self.oldpin and pin:
                # Rising enge
                if self.start_samplenum and self.end_samplenum:
                    period = samplenum - self.start_samplenum
                    duty = self.end_samplenum - self.start_samplenum
                    # ideal duty for T0H: 33%, T1H: 66%
                    bit_ = (duty / period) > 0.5

                    self.put(self.start_samplenum, samplenum, "BIT", bit_)

                    self.bits.append(bit_)
                    self.handle_bits(samplenum)

                if self.packet_ss is None:
                    self.packet_ss = samplenum

                self.start_samplenum = samplenum

            elif self.oldpin and not pin:
                # Falling edge
                self.inreset = False
                self.end_samplenum = samplenum

            self.oldpin = pin
