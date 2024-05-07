from typing import Tuple



# // START USER SECTION

enum = """
typedef enum {
	OSPIM_PIN_DISABLE =		0x00000000,
	// OSPIM_PORT1
	OSPIM_PORT1_IO3_A1 =	0x01090000,		OSPIM_PORT1_NSCK_B12 =	0x0C130000,
	OSPIM_PORT1_IO5_D5 =	0x053A0000,		OSPIM_PORT1_NSS_E11 =	0x0B4B0000,
	OSPIM_PORT1_DQS_A1 =	0x010B0000,		OSPIM_PORT1_IO4_C1 =	0x012A0000,
	OSPIM_PORT1_IO6_D6 =	0x063A0000,		OSPIM_PORT1_IO3_F6 =	0x065A0000,
	OSPIM_PORT1_SCK_A3 =	0x03030000,		OSPIM_PORT1_IO2_C2 =	0x02290000,
	OSPIM_PORT1_IO7_D7 =	0x073A0000,		OSPIM_PORT1_IO2_F7 =	0x075A0000,
	OSPIM_PORT1_IO3_A6 =	0x06060000,		OSPIM_PORT1_IO5_C2 =	0x022B0000,
	OSPIM_PORT1_IO0_D11 =	0x0B390000,		OSPIM_PORT1_IO0_F8 =	0x085A0000,
	OSPIM_PORT1_IO2_A7 =	0x070A0000,		OSPIM_PORT1_IO0_C3 =	0x03290000,
	OSPIM_PORT1_IO1_D12 =	0x0C390000,		OSPIM_PORT1_IO1_F9 =	0x095A0000,
	OSPIM_PORT1_IO1_B0 =	0x001B0000,		OSPIM_PORT1_IO6_C3 =	0x032B0000,
	OSPIM_PORT1_IO3_D13 =	0x0D390000,		OSPIM_PORT1_SCK_F10 =	0x0A590000,
	OSPIM_PORT1_IO0_B1 =	0x011B0000,		OSPIM_PORT1_DQS_C5 =	0x052A0000,
	OSPIM_PORT1_IO2_E2 =	0x02490000,		OSPIM_PORT1_NSCK_F11 =	0x0B590000,
	OSPIM_PORT1_SCK_B2 =	0x02190000,		OSPIM_PORT1_IO0_C9 =	0x09290000,
	OSPIM_PORT1_IO4_E7 =	0x074A0000,		OSPIM_PORT1_NSS_G6 =	0x066A0000,
	OSPIM_PORT1_DQS_B2 =	0x021A0000,		OSPIM_PORT1_IO1_C10 =	0x0A290000,
	OSPIM_PORT1_IO5_E8 =	0x084A0000,		OSPIM_PORT1_IO6_G9 =	0x09690000,
	OSPIM_PORT1_NSS_B6 =	0x061A0000,		OSPIM_PORT1_NSS_C11 =	0x0B290000,
	OSPIM_PORT1_IO6_E9 =	0x094A0000,		OSPIM_PORT1_IO7_G14 =	0x0E690000,
	OSPIM_PORT1_NSS_B10 =	0x0A190000,		OSPIM_PORT1_IO4_D4 =	0x043A0000,
	OSPIM_PORT1_IO7_E10 =	0x0A4A0000,		OSPIM_PORT1_IO4_H2 =	0x02790000,
	OSPIM_PORT1_IO5_H3 =	0x03790000,
	// OSPIM_PORT2
	OSPIM_PORT2_IO0_F0 =	0x00590400,		OSPIM_PORT2_SCK_F4 =	0x04590400,
	OSPIM_PORT2_IO5_G1 =	0x01690400,		OSPIM_PORT2_NSS_G12 =	0x0C630400,
	OSPIM_PORT2_IO1_F1 =	0x01590400,		OSPIM_PORT2_NSCK_F5 =	0x05590400,
	OSPIM_PORT2_DQS_G7 =	0x07690400,		OSPIM_PORT2_DQS_G15 =	0x0F690400,
	OSPIM_PORT2_IO2_F2 =	0x02590400,		OSPIM_PORT2_DQS_F12 =	0x0C590400,
	OSPIM_PORT2_IO6_G10 =	0x0A630400,		OSPIM_PORT2_IO0_I9 =	0x09830400,
	OSPIM_PORT2_IO3_F3 =	0x03590400,		OSPIM_PORT2_IO4_G0 =	0x00690400,
	OSPIM_PORT2_IO7_G11 =	0x0B690400,		OSPIM_PORT2_IO1_I10 =	0x0A830400,
	OSPIM_PORT2_IO2_I11 =	0x0B830400,
} OSPIM_GPIO_t;
"""

pin_name_edit =	lambda n, pi, po, a, s, c, d: n
pin_edit =		lambda n, pi, po, a, s, c, d: pi
port_edit =		lambda n, pi, po, a, s, c, d: po
alt_edit =		lambda n, pi, po, a, s, c, d: a
sub_edit =		lambda n, pi, po, a, s, c, d: (any([io in n for io in ["IO4", "IO5", "IO6", "IO7"]]) << 2) | (("IO" in n) << 1) | (s & 0b1)
clk_edit =		lambda n, pi, po, a, s, c, d: 0
dev_edit =		lambda n, pi, po, a, s, c, d: 0

show = False
# \\ END USER SECTION


clocks = ["APB1", "AHB1", "APB2", "AHB2", "APB3", "AHB3", "APB4", "AHB4"]

def edit(
	dev_pin_name: str,
	pin_name_edit: callable,
	dev_pin: int,
	pin_edit: callable,
	port_edit: callable,
	alt_edit: callable,
	sub_edit: callable,
	clk_edit: callable,
	dev_edit: callable,
	show: bool = False
) -> Tuple[str, int]:
	pin = (dev_pin >> 24) & 0xF
	port = (dev_pin >> 20) & 0xF
	alt = (dev_pin >> 16) & 0xF
	sub = (dev_pin >> 10) & 0x3F
	clk = (dev_pin >> 5) & 0x1F
	dev = dev_pin & 0x1F
	
	if (show):
		padding = 30 - len(dev_pin_name)
		print(f"{dev_pin_name}{' ' * padding}-> pin: {chr(65 + port)}{pin}, af: {alt}, sub: {sub}, clk: {clocks[clk]}, dev: {dev}")
	
	dev_pin_name =	pin_name_edit(dev_pin_name, pin, port, alt, sub, clk, dev)
	pin =			pin_edit(dev_pin_name, pin, port, alt, sub, clk, dev)
	port =			port_edit(dev_pin_name, pin, port, alt, sub, clk, dev)
	alt =			alt_edit(dev_pin_name, pin, port, alt, sub, clk, dev)
	sub =			sub_edit(dev_pin_name, pin, port, alt, sub, clk, dev)
	clk =			clk_edit(dev_pin_name, pin, port, alt, sub, clk, dev)
	dev =			dev_edit(dev_pin_name, pin, port, alt, sub, clk, dev)
	
	if (show):
		padding = 30 - len(dev_pin_name)
		print(f"{dev_pin_name}{' ' * padding}-> pin: {chr(65 + port)}{pin}, af: {alt}, sub: {sub}, clk: {clocks[clk]}, dev: {dev}\n")
	
	return (
		dev_pin_name, (
			((pin & 0xf) << 24)     |
			((port & 0xf) << 20)    |
			((alt & 0xf) << 16)     |
			((sub & 0x3f) << 10)    |   # |
			((clk & 0x1f) << 5)     |   # - dev_id
			(dev & 0x1f)                # |
		)
	)


if __name__ == "__main__":
	enum = "".join([e for e in enum.split("\n") if not any([c in e for c in ["{", "}", "/"]])])
	enum = enum.replace(" ", "").replace("\t", "")
	enum = [pair.split("=") for pair in enum.split(",") if pair]
	enum = [(p[0], int(p[1][2:], 16)) for p in enum]
	
	new_enum = "typedef enum {\n"
	for index, (name, dev_pin) in enumerate(enum):
		name, dev_pin = edit(
			name, pin_name_edit,
			dev_pin, pin_edit, port_edit, alt_edit,
			sub_edit, clk_edit, dev_edit, show
		)
		new_enum += f"\t{name} =\t0x{format(dev_pin, '08X')},\n"
	new_enum += "} _;"
	
	print(new_enum)