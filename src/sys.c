//
// Created by marijn on 6/26/23.
//

#include "sys.h"


/*!<
 * static variables
 * */
static SYS_CLK_Config_t sys_config;
static IRQ_callback_t	sys_tick_func =		NULL;
static IRQ_callback_t	clock_fault_func =	NULL;


/*!<
 * constants
 * */
const uint32_t LSI_clock_frequency =		32000;
const uint32_t LSE_clock_frequency =		32768;
const uint32_t CSI_clock_frequency =		4000000;
const uint32_t HSI48_clock_frequency =		48000000;


/*!<
 * variables
 * */
uint32_t HSI_clock_frequency =			64000000;
uint32_t HSE_clock_frequency =			0;			// HSE is within [4, 48] MHz
uint32_t PLL1_P_clock_frequency =		0;
uint32_t PLL1_Q_clock_frequency =		0;
uint32_t PLL1_R_clock_frequency =		0;
uint32_t PLL2_P_clock_frequency =		0;
uint32_t PLL2_Q_clock_frequency =		0;
uint32_t PLL2_R_clock_frequency =		0;
uint32_t PLL3_P_clock_frequency =		0;
uint32_t PLL3_Q_clock_frequency =		0;
uint32_t PLL3_R_clock_frequency =		0;
uint32_t PER_clock_frequency =			0;
uint32_t AHB_clock_frequency =			64000000;
uint32_t APB1_clock_frequency =			64000000;
uint32_t APB2_clock_frequency =			64000000;
uint32_t APB3_clock_frequency =			64000000;
uint32_t APB4_clock_frequency =			64000000;
uint32_t RTC_clock_frequency =			0;
uint32_t SYS_clock_frequency =			64000000;

volatile uint64_t tick = 0;  // updated with sys_tick


/*!<
 * interrupts
 * */
extern void SysTick_Handler(void) { tick++; if (sys_tick_func) { sys_tick_func(); } }
extern void RCC_IRQHandler(void) {
	if (!(RCC->CIFR & (RCC_CIFR_HSECSSF | RCC_CIFR_LSECSSF))) { RCC->CICR = 0x1FFUL; return; }
	if (clock_fault_func) { clock_fault_func(); }
	for(;;);  // halt cpu when clock failure is detected on HSE or LSE
}


/*!<
 * config functions
 * */
void set_PLL_config(
	uint8_t PLL_index,						uint8_t enable,
	uint8_t P_enable,						uint8_t Q_enable,						uint8_t R_enable,
	uint8_t frac_enable,					PLL_IN_t input_range,					PLL_VCO_t VCO_range,
	uint8_t M_factor,						uint8_t P_factor,						uint8_t Q_factor,
	uint8_t R_factor,						uint16_t N_factor,						uint16_t N_fraction
) {
	sys_config.PLL_config[PLL_index].enable =		enable;
	sys_config.PLL_config[PLL_index].P_enable =		P_enable;
	sys_config.PLL_config[PLL_index].Q_enable =		Q_enable;
	sys_config.PLL_config[PLL_index].R_enable =		R_enable;
	sys_config.PLL_config[PLL_index].frac_enable =	frac_enable;
	sys_config.PLL_config[PLL_index].input_range =	input_range;
	sys_config.PLL_config[PLL_index].VCO_range =	VCO_range;
	sys_config.PLL_config[PLL_index].M_factor =		M_factor;
	sys_config.PLL_config[PLL_index].P_factor =		P_factor ? P_factor - 1 : 0b0UL;
	sys_config.PLL_config[PLL_index].Q_factor =		Q_factor ? Q_factor - 1 : 0b0UL;
	sys_config.PLL_config[PLL_index].R_factor =		R_factor ? R_factor - 1 : 0b0UL;
	sys_config.PLL_config[PLL_index].N_factor =		N_factor ? N_factor - 1 : 0b0UL;
	sys_config.PLL_config[PLL_index].N_fraction =	N_fraction;
}

void set_RTC_config(
	uint8_t RTC_enable,						RTC_SRC_t RTC_src,						uint8_t RTC_HSE_prescaler
) {
	sys_config.RTC_enable =			RTC_enable;
	sys_config.RTC_src =			RTC_src;
	sys_config.RTC_HSE_prescaler =	RTC_HSE_prescaler;
}

void set_clock_config(
	uint8_t HSI_enable,						uint8_t HSE_enable,
	uint8_t LSI_enable,						uint8_t LSE_enable,						uint8_t CSI_enable,
	uint8_t HSI48_enable,					uint8_t HSI_enable_stop_mode,			uint8_t CSI_enable_stop_mode,
	uint8_t HSE_CSS_enable,					uint8_t LSE_CSS_enable,					HSI_DIV_t HSI_div,
	uint32_t HSE_freq,						PLL_SRC_t PLL_src
) {
	sys_config.HSI_enable =				HSI_enable;
	sys_config.HSE_enable =				HSE_enable;
	sys_config.LSI_enable =				LSI_enable;
	sys_config.LSE_enable =				LSE_enable;
	sys_config.CSI_enable =				CSI_enable;
	sys_config.HSI48_enable =			HSI48_enable;
	sys_config.HSI_enable_stop_mode =	HSI_enable_stop_mode;
	sys_config.CSI_enable_stop_mode =	CSI_enable_stop_mode;
	sys_config.HSE_CSS_enable =			HSE_CSS_enable;
	sys_config.LSE_CSS_enable =			LSE_CSS_enable;
	sys_config.HSI_div =				HSI_div;
	sys_config.HSE_freq =				HSE_freq;
	sys_config.PLL_src =				PLL_src;
}

void set_SYS_config(
	SYS_CLK_SRC_t SYS_CLK_src,				SYS_CLK_PRE_t SYS_CLK_prescaler,
	CORE_VOS_t CORE_VOS_level,				FLASH_LATENCY_t FLASH_latency
) {
	sys_config.SYS_CLK_src =			SYS_CLK_src;
	sys_config.SYS_CLK_prescaler =		SYS_CLK_prescaler;
	sys_config.CORE_VOS_level =			CORE_VOS_level;
	sys_config.FLASH_latency =			FLASH_latency;
}

void set_domain_config(
	AHB_CLK_PRE_t AHB_prescaler,			APB_CLK_PRE_t APB1_prescaler,			APB_CLK_PRE_t APB2_prescaler,
	APB_CLK_PRE_t APB3_prescaler,			APB_CLK_PRE_t APB4_prescaler
) {
	sys_config.AHB_prescaler =	AHB_prescaler;
	sys_config.APB1_prescaler =	APB1_prescaler;
	sys_config.APB2_prescaler =	APB2_prescaler;
	sys_config.APB3_prescaler =	APB3_prescaler;
	sys_config.APB4_prescaler =	APB4_prescaler;
}

void set_systick_config(
	uint8_t SYSTICK_enable,					uint8_t SYSTICK_IRQ_enable,				SYSTICK_CLK_SRC_t SYSTICK_CLK_src
) {
	sys_config.SYSTICK_enable =		SYSTICK_enable;
	sys_config.SYSTICK_IRQ_enable =	SYSTICK_IRQ_enable;
	sys_config.SYSTICK_CLK_src =	SYSTICK_CLK_src;
}


/*!<
 * init
 * */
void IRQ_callback_init(IRQ_callback_t sys_tick_callback, IRQ_callback_t clock_fault_callback) {
	sys_tick_func =		sys_tick_callback;
	clock_fault_func =	clock_fault_callback;
}

void sys_clock_init() {
	uint32_t PLL_src_freq = 0;
	uint32_t PLL1_freq, PLL2_freq, PLL3_freq;
	uint32_t clock_ready_mask;
	uint32_t tmp_reg;
	//!< check if current VOS level is stable
	while (!(PWR->SRDCR & PWR_SRDCR_VOSRDY));
	//!< configure CORE voltage scaling
	PWR->CR3 |= PWR_CR3_BYPASS;
	PWR->SRDCR |= (sys_config.CORE_VOS_level << PWR_SRDCR_VOS_Pos);
	while ((PWR->CSR1 & PWR_CSR1_ACTVOS) != (sys_config.CORE_VOS_level << PWR_CSR1_ACTVOS_Pos));  // wait until the power scaling level is applied
	//!< enable and config base clocks
	RCC->CR = (  // HSI is left on
		// enable clocks
		(sys_config.HSE_enable << RCC_CR_HSEON_Pos)						|
		(sys_config.CSI_enable << RCC_CR_CSION_Pos)						|
		(sys_config.HSI48_enable << RCC_CR_HSI48ON_Pos)					|
		RCC_CR_HSION  /* keep HSI on until clock switch */				|
		// other settings
		(sys_config.HSE_CSS_enable << RCC_CR_CSSHSEON_Pos)				|
		(sys_config.HSI_enable_stop_mode << RCC_CR_HSIKERON_Pos)		|
		(sys_config.CSI_enable_stop_mode << RCC_CR_CSIKERON_Pos)
	);
	clock_ready_mask = (
		(sys_config.HSI_enable << RCC_CR_HSIRDY_Pos)					|
		(sys_config.HSE_enable << RCC_CR_HSERDY_Pos)					|
		(sys_config.CSI_enable << RCC_CR_CSIRDY_Pos)					|
		(sys_config.HSI48_enable << RCC_CR_HSI48RDY_Pos)				|
		(sys_config.HSE_CSS_enable << RCC_CR_CSSHSEON_Pos)
	);
	while ((RCC->CR & clock_ready_mask) != clock_ready_mask);	// wait until all enabled basic clocks are ready
	RCC->CR |= sys_config.HSI_div << RCC_CR_HSIDIV_Pos;
	while (!(RCC->CR & RCC_CR_HSIDIVF));						// wait until HSI div is applied
	//!< update base clock frequency variables
	HSE_clock_frequency = sys_config.HSE_freq;
	HSI_clock_frequency = 64000000 / (0b1UL << sys_config.HSI_div);
	//!< switch back to HSI in case of wakeup event
	if ((RCC->CFGR & RCC_CFGR_SWS) >> RCC_CFGR_SWS_Pos == SYS_CLK_SRC_PLL1_P) {
		RCC->CFGR = (
			(sys_config.RTC_HSE_prescaler << RCC_CFGR_RTCPRE_Pos)	|
			(SYS_CLK_SRC_HSI << RCC_CFGR_SW_Pos)  // switch sys clock
		);
		while ((RCC->CFGR & RCC_CFGR_SWS) != (SYS_CLK_SRC_HSI << RCC_CFGR_SWS_Pos));
	}
	//!< disable PLL clocks before configuring
	if (RCC->CR & RCC_CR_PLL3ON) { RCC->CR &= ~RCC_CR_PLL3ON; while (RCC->CR & RCC_CR_PLL3RDY); }
	if (RCC->CR & RCC_CR_PLL2ON) { RCC->CR &= ~RCC_CR_PLL2ON; while (RCC->CR & RCC_CR_PLL2RDY); }
	if (RCC->CR & RCC_CR_PLL1ON) { RCC->CR &= ~RCC_CR_PLL1ON; while (RCC->CR & RCC_CR_PLL1RDY); }
	//!< configure PLL clocks
	RCC->PLLCKSELR = (  // PLL clock source and prescalers (PLL input settings)
		(sys_config.PLL_config[2].M_factor << RCC_PLLCKSELR_DIVM3_Pos)		|
		(sys_config.PLL_config[1].M_factor << RCC_PLLCKSELR_DIVM2_Pos)		|
		(sys_config.PLL_config[0].M_factor << RCC_PLLCKSELR_DIVM1_Pos)		|
		(sys_config.PLL_src << RCC_PLLCKSELR_PLLSRC_Pos)
	);
	RCC->PLLCFGR = (  // PLL enable settings
		// PLL1
		(sys_config.PLL_config[0].R_enable << RCC_PLLCFGR_DIVR1EN_Pos)		|
		(sys_config.PLL_config[0].Q_enable << RCC_PLLCFGR_DIVQ1EN_Pos)		|
		(sys_config.PLL_config[0].P_enable << RCC_PLLCFGR_DIVP1EN_Pos)		|
		(sys_config.PLL_config[0].input_range << RCC_PLLCFGR_PLL1RGE_Pos)	|
		(sys_config.PLL_config[0].VCO_range << RCC_PLLCFGR_PLL1VCOSEL_Pos)	|
		(sys_config.PLL_config[0].frac_enable << RCC_PLLCFGR_PLL1FRACEN_Pos)	|
		// PLL2
		(sys_config.PLL_config[1].R_enable << RCC_PLLCFGR_DIVR2EN_Pos)		|
		(sys_config.PLL_config[1].Q_enable << RCC_PLLCFGR_DIVQ2EN_Pos)		|
		(sys_config.PLL_config[1].P_enable << RCC_PLLCFGR_DIVP2EN_Pos)		|
		(sys_config.PLL_config[1].input_range << RCC_PLLCFGR_PLL2RGE_Pos)	|
		(sys_config.PLL_config[1].VCO_range << RCC_PLLCFGR_PLL2VCOSEL_Pos)	|
		(sys_config.PLL_config[1].frac_enable << RCC_PLLCFGR_PLL2FRACEN_Pos)	|
		// PLL3
		(sys_config.PLL_config[2].R_enable << RCC_PLLCFGR_DIVR3EN_Pos)		|
		(sys_config.PLL_config[2].Q_enable << RCC_PLLCFGR_DIVQ3EN_Pos)		|
		(sys_config.PLL_config[2].P_enable << RCC_PLLCFGR_DIVP3EN_Pos)		|
		(sys_config.PLL_config[2].input_range << RCC_PLLCFGR_PLL3RGE_Pos)	|
		(sys_config.PLL_config[2].VCO_range << RCC_PLLCFGR_PLL3VCOSEL_Pos)	|
		(sys_config.PLL_config[2].frac_enable << RCC_PLLCFGR_PLL3FRACEN_Pos)
	);
	RCC->PLL1DIVR = (
		(sys_config.PLL_config[0].R_factor << RCC_PLL1DIVR_R1_Pos)			|
		(sys_config.PLL_config[0].Q_factor << RCC_PLL1DIVR_Q1_Pos)			|
		(sys_config.PLL_config[0].P_factor << RCC_PLL1DIVR_P1_Pos)			|
		(sys_config.PLL_config[0].N_factor << RCC_PLL1DIVR_N1_Pos)
	);
	RCC->PLL1FRACR = (sys_config.PLL_config[0].N_fraction << RCC_PLL1FRACR_FRACN1_Pos);
	RCC->PLL2DIVR = (
		(sys_config.PLL_config[1].R_factor << RCC_PLL2DIVR_R2_Pos)			|
		(sys_config.PLL_config[1].Q_factor << RCC_PLL2DIVR_Q2_Pos)			|
		(sys_config.PLL_config[1].P_factor << RCC_PLL2DIVR_P2_Pos)			|
		(sys_config.PLL_config[1].N_factor << RCC_PLL2DIVR_N2_Pos)
	);
	RCC->PLL2FRACR = (sys_config.PLL_config[1].N_fraction << RCC_PLL2FRACR_FRACN2_Pos);
	RCC->PLL3DIVR = (
		(sys_config.PLL_config[2].R_factor << RCC_PLL3DIVR_R3_Pos)			|
		(sys_config.PLL_config[2].Q_factor << RCC_PLL3DIVR_Q3_Pos)			|
		(sys_config.PLL_config[2].P_factor << RCC_PLL3DIVR_P3_Pos)			|
		(sys_config.PLL_config[2].N_factor << RCC_PLL3DIVR_N3_Pos)
	);
	RCC->PLL3FRACR = (sys_config.PLL_config[2].N_fraction << RCC_PLL3FRACR_FRACN3_Pos);
	//!< update PLL frequency variables
	switch ((PLL_SRC_t)sys_config.PLL_src) {
		case PLL_SRC_HSI: PLL_src_freq = HSI_clock_frequency / sys_config.PLL_config[0].M_factor; break;
		case PLL_SRC_CSI: PLL_src_freq = CSI_clock_frequency / sys_config.PLL_config[0].M_factor; break;
		case PLL_SRC_HSE: PLL_src_freq = HSE_clock_frequency / sys_config.PLL_config[0].M_factor; break;
	}
	PLL1_freq = PLL_src_freq * (sys_config.PLL_config[0].N_factor + 1);
	PLL2_freq = PLL_src_freq * (sys_config.PLL_config[1].N_factor + 1);
	PLL3_freq = PLL_src_freq * (sys_config.PLL_config[2].N_factor + 1);
	if (sys_config.PLL_config[0].P_enable) { PLL1_P_clock_frequency = PLL1_freq / (sys_config.PLL_config[0].P_factor + 1); }
	if (sys_config.PLL_config[0].Q_enable) { PLL1_Q_clock_frequency = PLL1_freq / (sys_config.PLL_config[0].Q_factor + 1); }
	if (sys_config.PLL_config[0].R_enable) { PLL1_R_clock_frequency = PLL1_freq / (sys_config.PLL_config[0].R_factor + 1); }
	if (sys_config.PLL_config[1].P_enable) { PLL2_P_clock_frequency = PLL2_freq / (sys_config.PLL_config[1].P_factor + 1); }
	if (sys_config.PLL_config[1].Q_enable) { PLL2_Q_clock_frequency = PLL2_freq / (sys_config.PLL_config[1].Q_factor + 1); }
	if (sys_config.PLL_config[1].R_enable) { PLL2_R_clock_frequency = PLL2_freq / (sys_config.PLL_config[1].R_factor + 1); }
	if (sys_config.PLL_config[2].P_enable) { PLL3_P_clock_frequency = PLL3_freq / (sys_config.PLL_config[2].P_factor + 1); }
	if (sys_config.PLL_config[2].Q_enable) { PLL3_Q_clock_frequency = PLL3_freq / (sys_config.PLL_config[2].Q_factor + 1); }
	if (sys_config.PLL_config[2].R_enable) { PLL3_R_clock_frequency = PLL3_freq / (sys_config.PLL_config[2].R_factor + 1); }
	switch ((SYS_CLK_SRC_t)sys_config.SYS_CLK_src) {
		case SYS_CLK_SRC_HSI: SYS_clock_frequency = HSI_clock_frequency; break;
		case SYS_CLK_SRC_CSI: SYS_clock_frequency = CSI_clock_frequency; break;
		case SYS_CLK_SRC_HSE: SYS_clock_frequency = HSE_clock_frequency; break;
		case SYS_CLK_SRC_PLL1_P: SYS_clock_frequency = PLL1_P_clock_frequency; break;
	}
	//!< enable PLL clocks
	RCC->CR |= (
		(sys_config.PLL_config[2].enable << RCC_CR_PLL3ON_Pos)				|
		(sys_config.PLL_config[1].enable << RCC_CR_PLL2ON_Pos)				|
		(sys_config.PLL_config[0].enable << RCC_CR_PLL1ON_Pos)
	);
	clock_ready_mask = (
		(sys_config.PLL_config[2].enable << RCC_CR_PLL3RDY_Pos)				|
		(sys_config.PLL_config[1].enable << RCC_CR_PLL2RDY_Pos)				|
		(sys_config.PLL_config[0].enable << RCC_CR_PLL1RDY_Pos)
	);
	while ((RCC->CR & clock_ready_mask) != clock_ready_mask);	// wait until all enabled PLL clocks are ready
	//!< configure domain pre-scalars
	RCC->CDCFGR1 = (
		(sys_config.SYS_CLK_prescaler << RCC_CDCFGR1_CDCPRE_Pos)			|
		(sys_config.APB3_prescaler << RCC_CDCFGR1_CDPPRE_Pos)				|
		(sys_config.AHB_prescaler << RCC_CDCFGR1_HPRE_Pos)
	);
	RCC->CDCFGR2 = (
		(sys_config.APB2_prescaler << RCC_CDCFGR2_CDPPRE2_Pos)				|
		(sys_config.APB1_prescaler << RCC_CDCFGR2_CDPPRE1_Pos)
	);
	RCC->SRDCFGR = (sys_config.APB4_prescaler << RCC_SRDCFGR_SRDPPRE_Pos);
	//!< update frequency variables
	if (sys_config.SYS_CLK_prescaler & 0x8UL) {
		if (sys_config.SYS_CLK_prescaler & 0x4UL)	{ SYS_clock_frequency /= 0x4UL << (sys_config.SYS_CLK_prescaler & 0x7UL); }
		else									{ SYS_clock_frequency /= 0x2UL << (sys_config.SYS_CLK_prescaler & 0x7UL); }
	} AHB_clock_frequency = SYS_clock_frequency;
	if (sys_config.AHB_prescaler & 0x8UL) {
		if (sys_config.AHB_prescaler & 0x4UL)		{ AHB_clock_frequency = SYS_clock_frequency / (0x4UL << (sys_config.AHB_prescaler & 0x7UL)); }
		else									{ AHB_clock_frequency = SYS_clock_frequency / (0x2UL << (sys_config.AHB_prescaler & 0x7UL)); }
	} APB3_clock_frequency = APB2_clock_frequency = APB1_clock_frequency = APB4_clock_frequency = AHB_clock_frequency;
	if (sys_config.APB3_prescaler & 0x4UL)			{ APB3_clock_frequency = AHB_clock_frequency / (0x2UL << (sys_config.APB3_prescaler & 0x3UL)); }
	if (sys_config.APB2_prescaler & 0x4UL)			{ APB2_clock_frequency = AHB_clock_frequency / (0x2UL << (sys_config.APB2_prescaler & 0x3UL)); }
	if (sys_config.APB1_prescaler & 0x4UL)			{ APB1_clock_frequency = AHB_clock_frequency / (0x2UL << (sys_config.APB1_prescaler & 0x3UL)); }
	if (sys_config.APB4_prescaler & 0x4UL)			{ APB4_clock_frequency = AHB_clock_frequency / (0x2UL << (sys_config.APB4_prescaler & 0x3UL)); }
	//!< configure flash
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |= (sys_config.FLASH_latency << FLASH_ACR_LATENCY_Pos);
	//!< switch sys-clock
	RCC->CFGR = (
		(sys_config.RTC_HSE_prescaler << RCC_CFGR_RTCPRE_Pos)	|
		(sys_config.SYS_CLK_src << RCC_CFGR_SW_Pos)  // switch sys clock
	);
	while ((RCC->CFGR & RCC_CFGR_SWS) != (sys_config.SYS_CLK_src << RCC_CFGR_SWS_Pos));	// wait until the sys clock is switched
	//!< enable/disable HSI
	tmp_reg = RCC->CR; tmp_reg &= ~RCC_CR_HSION;
	tmp_reg |= sys_config.HSI_enable << RCC_CR_HSION_Pos;
	RCC->CR = tmp_reg;
	HSI_clock_frequency *= 1 - sys_config.HSI_enable;
	//!< configure RTC
	RCC->CSR = (sys_config.LSI_enable << RCC_CSR_LSION_Pos);
	while ((RCC->CSR & RCC_CSR_LSIRDY) != (sys_config.LSI_enable << RCC_CSR_LSIRDY_Pos));
	RCC->BDCR = (
		(sys_config.RTC_enable << RCC_BDCR_RTCEN_Pos)						|
		(sys_config.RTC_src << RCC_BDCR_RTCSEL_Pos)						|
		(sys_config.LSE_CSS_enable << RCC_BDCR_LSECSSON_Pos)				|
		(sys_config.LSE_enable << RCC_BDCR_LSEON_Pos)
	);
	while ((RCC->BDCR & RCC_BDCR_LSERDY) != (sys_config.LSE_enable << RCC_BDCR_LSERDY_Pos));
	RCC->CIER = (sys_config.LSE_CSS_enable << RCC_CIER_LSECSSIE_Pos);
	//!< update RTC frequency variables
	switch ((RTC_SRC_t)sys_config.RTC_src) {
		case RTC_SRC_LSE:	RTC_clock_frequency = LSI_clock_frequency; break;
		case RTC_SRC_LSI:	RTC_clock_frequency = LSE_clock_frequency; break;
		case RTC_SRC_HSE:
			if (sys_config.RTC_HSE_prescaler < 2) { break; }
			RTC_clock_frequency = HSE_clock_frequency / sys_config.RTC_HSE_prescaler; break;
		default:			RTC_clock_frequency = 0; break;
	}
	//!< configure sys-tick
	SysTick->LOAD = ((SYS_clock_frequency / (8 - (7 * sys_config.SYSTICK_CLK_src))) / 2000) + 1;
	SysTick->VAL  = 0;
	SysTick->CTRL = (
		(sys_config.SYSTICK_enable << SysTick_CTRL_ENABLE_Pos)				|
		(sys_config.SYSTICK_IRQ_enable << SysTick_CTRL_TICKINT_Pos)		|
		(sys_config.SYSTICK_CLK_src << SysTick_CTRL_CLKSOURCE_Pos)
	);
	if (sys_config.SYSTICK_IRQ_enable) { SCB->SHPR[(SysTick_IRQn & 0xFUL) - 4UL] = 0xF0UL; }	// set SysTick irq priority
	//!< reset peripheral kernel clock config
	RCC->CDCCIPR = sys_config.PER_src << RCC_CDCCIPR_CKPERSEL_Pos;
	RCC->CDCCIP1R =	0;
	RCC->CDCCIP2R =	0;
	RCC->SRDCCIPR =	0;
	//!< update PER frequency variable
	switch ((PER_SRC_t)sys_config.PER_src) {
		case PER_SRC_HSI:		PER_clock_frequency = HSI_clock_frequency; return;
		case PER_SRC_CSI:		PER_clock_frequency = CSI_clock_frequency; return;
		case PER_SRC_HSE:		PER_clock_frequency = HSE_clock_frequency; return;
		case PER_SRC_DISABLED:	PER_clock_frequency = 0; return;
	}
}


/*!<
 * misc
 * */
void delay_ms(uint64_t ms) {
	uint64_t start = tick;
	while ((tick - start) < ms);
}