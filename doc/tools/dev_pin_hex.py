from argparse import ArgumentParser as parser

parse = parser(
	prog="pin to hex",
	description="convert an STM32 pin for a peripheral to hex"
)
flag =			{"action": "store_true"}
input_default =	{"action": "store", "type": int, "default": None}


if __name__ == "__main__":
	parse.add_argument('-af', **input_default)
	parse.add_argument('-pnum', **input_default)
	
	parse.add_argument('-tim', **flag)
	parse.add_argument('-hrtim', **flag)
	parse.add_argument('-lptim', **flag)
	parse.add_argument('-uart', **flag)
	parse.add_argument('-lpuart', **flag)
	parse.add_argument('-i2c', **flag)
	parse.add_argument('-usb', **flag)
	parse.add_argument('-spi', **flag)
	parse.add_argument('-ospi', **flag)
	
	arg = parse.parse_args()
	arg.tim |= arg.hrtim
	arg.tim |= arg.lptim
	arg.uart |= arg.lpuart
	
	clocks = {
		"APB1": 0, "AHB1": 1,
		"APB2": 2, "AHB2": 3,
		"APB3": 4, "AHB3": 5,
		"APB4": 6, "AHB4": 7,
	}
	ports = {
		"A": 0, "B": 1, "C": 2, "D": 3,
		"E": 4, "F": 5, "G": 6, "H": 7,
		"I": 8, "J": 9, "K": 10
	}
	
	tims = {
		"TIM1": ("APB2", 0),     "TIM2": ("APB1", 0),
		"TIM3": ("APB1", 1),     "TIM4": ("APB1", 2),
		"TIM5": ("APB1", 3),     "TIM6": ("APB1", 4),
		"TIM7": ("APB1", 5),     "TIM8": ("APB2", 1),
		"TIM12": ("APB1", 6),    "TIM13": ("APB1", 7),
		"TIM14": ("APB1", 8),    "TIM15": ("AHB1", 16),
		"TIM16": ("AHB1", 17),   "TIM17": ("AHB1", 18),
		"LPTIM1": ("APB1", 9),	 "LPTIM2": ("APB4", 9),
		"LPTIM3": ("APB4", 10),	 "LPTIM4": ("APB4", 11),
		"LPTIM5": ("APB4", 12)
	}
	uarts = {
		"UART1": ("APB2", 4),    "UART2": ("APB1", 17),
		"UART3": ("APB1", 18),   "UART4": ("APB1", 19),
		"UART5": ("APB1", 20),   "UART6": ("APB2", 5),
		"UART7": ("APB1", 30),   "UART8": ("APB1", 31),
		"LPUART1": ("APB4", 3)
	}
	i2cs = {
		"I2C1": ("APB1", 21),    "I2C2": ("APB1", 22),
		"I2C3": ("APB1", 23),    "I2C4": ("APB4", 7)
	}
	usbs = {
		"USB1": ("AHB1", 25),    "USB2": ("AHB1", 27)
	}
	spis = {
		"SPI1": ("APB2", 12),	"SPI2": ("APB1", 14),
		"SPI3": ("APB1", 15),	"SPI4": ("APB2", 13),
		"SPI5": ("APB2", 20),	"SPI6": ("APB4", 5),
	}
	ospis = {
		"OSPI1": ("AHB3", 14),	"OSPI2": ("AHB3", 19)
	}
	
	while True:
		try:
			sub = 0
			if arg.tim:
				tim = tims["HRTIM"]
				if not arg.hrtim:
					tim = input(f"{'lp' if arg.lptim else ''}tim: ") if not arg.pnum else arg.pnum
					try:    tim = tims[f"LPTIM{int(tim)}" if arg.lptim else f"TIM{int(tim)}"]
					except: tim = tims[tim.upper()]
				clk, dev = tim
				clk = clocks[clk]
				if arg.hrtim:
					hrtim = input("sub_timer: ") if not arg.pnum else arg.pnum
					try:    hrtim = int(hrtim)
					except: hrtim = ports[hrtim.upper()]
					sub |= hrtim << 3
				channel = max((int(input("channel: ")) - 1), 0)
				sub |= channel & 0x7
			elif arg.uart:
				uart = input(f"{'lp' if arg.lpuart else ''}uart: ") if not arg.pnum else arg.pnum
				try:    uart = uarts[f"LPUART{int(uart)}" if arg.lpuart else f"UART{int(uart)}"]
				except: uart = uarts[uart.upper()]
				clk, dev = uart
				clk = clocks[clk]
			elif arg.i2c:
				i2c = input("i2c: ") if not arg.pnum else arg.pnum
				try:    i2c = i2cs[f"I2C{int(i2c)}"]
				except: i2c = i2cs[i2c.upper()]
				clk, dev = i2c
				clk = clocks[clk]
			elif arg.usb:
				usb = input("usb: ") if not arg.pnum else arg.pnum
				ulpi = input("ulpi?") != ""
				print("ulpi " + ("ON" if ulpi else "OFF"))
				try:
					clk, dev = usbs[f"USB{int(usb)}"]
					if ulpi: sub = (0x1 << 5) | ((dev + 1) & 0x1f)  # clock is always AHB1
				except:
					clk, dev = usbs[usb.upper()]
					if ulpi: sub = (0x1 << 5) | ((dev + 1) & 0x1f)  # clock is always AHB1
				clk = clocks[clk]
			elif arg.spi:
				spi = input("spi: ") if not arg.pnum else arg.pnum
				try:    spi = spis[f"SPI{int(spi)}"]
				except: spi = spis[spi.upper()]
				clk, dev = spi
				clk = clocks[clk]
			elif arg.ospi:
				ospi = input("ospi: ") if not arg.pnum else arg.pnum
				try:    ospi = ospis[f"OSPI{int(ospi)}"]
				except: ospi = ospis[ospi.upper()]
				clk, dev = ospi
				clk = clocks[clk]
			else:
				clk = input("clk: ")
				try:    clk = int(clk)
				except: clk = clocks[clk.upper()]
				dev = int(input("offset: "), base=16) >> 10
			alt = int(input("alt: ")) if not arg.af else arg.af
			pin = input("pin: ")
			port = int(ports[pin[0].upper()])
			pin = int(pin[1:])
			res = (
				((pin & 0xf) << 24)     |
				((port & 0xf) << 20)    |
				((alt & 0xf) << 16)     |
				((sub & 0x3f) << 10)    |   # |
				((clk & 0x1f) << 5)     |   # - dev_id
				(dev & 0x1f)                # |
			)
			#print(hex(sub))
			print(f"{res:#0{10}x}".upper().replace("X", "x"), end="\n\n")
		except KeyboardInterrupt:   exit(0)
		except Exception as e:      print(e); pass
