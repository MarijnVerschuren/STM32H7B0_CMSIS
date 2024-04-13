//
// Created by marijn on 7/19/23.
//

#include "usb/usb.h"

/*!<
 * defines
 * */
#define USB_OTG_HS_WAKEUP_EXTI_LINE	(0x1U << 18)  /*!< USB FS EXTI Line WakeUp Interrupt */


/*!<
 * variables
 * */
uint32_t USB_kernel_frequency = 0;
USB_handle_t USB_handle;


/*!<
 * functions
 * */
void flush_RX_FIFO(USB_OTG_GlobalTypeDef* usb) {
	__IO uint32_t why = 0;  // NOTE: creating a variable IS mandatory????
	while (!(usb->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL));	// wait for AHB master IDLE state
	usb->GRSTCTL = USB_OTG_GRSTCTL_RXFFLSH;					// flush RX FIFO
	while (usb->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH);			// wait until reset is processed
}
void flush_TX_FIFO(USB_OTG_GlobalTypeDef* usb, uint8_t ep) {
	while (!(usb->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL));		// wait for AHB master IDLE state
	usb->GRSTCTL = (
			ep << USB_OTG_GRSTCTL_TXFNUM_Pos		|			// select ep TX FIFO
			0b1UL << USB_OTG_GRSTCTL_TXFFLSH_Pos				// flush TX FIFO
	);
	while (usb->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH);			// wait until reset is processed
}
void flush_TX_FIFOS(USB_OTG_GlobalTypeDef* usb) {
	while (!(usb->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL));		// wait for AHB master IDLE state
	usb->GRSTCTL = (
			0x10UL << USB_OTG_GRSTCTL_TXFNUM_Pos		|		// select all TX FIFOs
			0b1UL << USB_OTG_GRSTCTL_TXFFLSH_Pos				// flush TX FIFOs
	);
	while (usb->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH);			// wait until reset is processed
}


/*!<
 * init
 * */
void config_USB_kernel_clock(USB_CLK_SRC_t src) {
	RCC->CDCCIP2R &= ~RCC_CDCCIP2R_USBSEL;
	RCC->CDCCIP2R |= src << RCC_CDCCIP2R_USBSEL_Pos;
	switch (src) {
	case USB_CLK_SRC_DISABLED:	USB_kernel_frequency = 0; return;
	case USB_CLK_SRC_PLL1_Q:	USB_kernel_frequency = PLL1_Q_clock_frequency; return;
	case USB_CLK_SRC_PLL3_Q:	USB_kernel_frequency = PLL3_Q_clock_frequency; return;
	case USB_CLK_SRC_HSI48:		USB_kernel_frequency = HSI48_clock_frequency; return;
	}
}


void config_USB(
	USB_OTG_GlobalTypeDef* usb, class_handle_t* class, descriptor_handle_t* desc,
	uint8_t enable_SOF, uint8_t enable_low_power
) {
	uint8_t i;
	USB_OTG_DeviceTypeDef*		device =	(void*)(((uint32_t)usb) + USB_OTG_DEVICE_BASE);
	USB_OTG_INEndpointTypeDef*	in =		(void*)(((uint32_t)usb) + USB_OTG_IN_ENDPOINT_BASE);
	USB_OTG_OUTEndpointTypeDef*	out =		(void*)(((uint32_t)usb) + USB_OTG_OUT_ENDPOINT_BASE);
	__IO uint32_t*				PCGCCTL =	(void*)(((uint32_t)usb) + USB_OTG_PCGCCTL_BASE);

	USB_handle.instance =					usb;
	USB_handle.desc =						desc;
	USB_handle.class =						class;
	USB_handle.dev_state =					DEV_STATE_DEFAULT;
	USB_handle.address =					0U;
	USB_handle.config.dev_endpoints =		9U;
	USB_handle.config.SOF_enable =			enable_SOF;
	USB_handle.config.low_power_enable =	enable_low_power;

	for (i = 0U; i < USB_handle.config.dev_endpoints; i++) {
		USB_handle.IN_ep[i].type =		USB_handle.OUT_ep[i].type =		EP_TYPE_CTRL;
		USB_handle.IN_ep[i].mps =		USB_handle.OUT_ep[i].mps =		0U;
		USB_handle.IN_ep[i].buffer =	USB_handle.OUT_ep[i].buffer =	NULL;
		USB_handle.IN_ep[i].size =		USB_handle.OUT_ep[i].size =		0U;
	}

	fconfig_GPIO(GPIOA, 11, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, 10);
	fconfig_GPIO(GPIOA, 12, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, 10);

	RCC->AHB1ENR |= RCC_AHB1ENR_USB1OTGHSEN;
	RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;

	PWR->CR3 |= PWR_CR3_USB33DEN;

	// TODO !!!
	uint32_t prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(OTG_HS_IRQn, NVIC_EncodePriority(prioritygroup, 0, 0));
	NVIC_EnableIRQ(OTG_HS_IRQn);
	if(USB_handle.config.low_power_enable == 1) {
		EXTI_D1->IMR2 |= USB_OTG_HS_WAKEUP_EXTI_LINE;
		NVIC_SetPriority(OTG_HS_WKUP_IRQn, NVIC_EncodePriority(prioritygroup, 0, 0));
		NVIC_EnableIRQ(OTG_HS_WKUP_IRQn);
	}
	// TODO / !!!

	usb->GAHBCFG &= ~USB_OTG_GAHBCFG_GINT;
	usb->GUSBCFG |= USB_OTG_GUSBCFG_PHYSEL;
	while (!(usb->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL));		// wait for AHB master IDLE state
	usb->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;					// reset the core
	while (usb->GRSTCTL & USB_OTG_GRSTCTL_CSRST);			// wait until reset is processed

	usb->GCCFG |= USB_OTG_GCCFG_PWRDWN;
	usb->GUSBCFG &= ~(USB_OTG_GUSBCFG_FHMOD | USB_OTG_GUSBCFG_FDMOD);
	usb->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;
	while ((usb->GINTSTS) & 0b1U);

	for (i = 0U; i < 15U; i++) { usb->DIEPTXF[i] = 0U; }

	device->DCTL |= USB_OTG_DCTL_SDIS;

	usb->GCCFG &= ~USB_OTG_GCCFG_VBDEN;
	usb->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN;
	usb->GOTGCTL |= USB_OTG_GOTGCTL_BVALOVAL;

	*PCGCCTL = 0U;

	device->DCFG |= USB_OTG_DCFG_DSPD;  // set full speed

	flush_TX_FIFOS(usb);
	flush_RX_FIFO(usb);

	device->DIEPMSK = 0U;
	device->DOEPMSK = 0U;
	device->DAINTMSK = 0U;
	for (i = 0U; i < USB_handle.config.dev_endpoints; i++) {
		if (in[i].DIEPCTL & USB_OTG_DIEPCTL_EPENA) {
			if (!i)	{ in[i].DIEPCTL = USB_OTG_DIEPCTL_SNAK; }
			else	{ in[i].DIEPCTL = USB_OTG_DIEPCTL_EPDIS | USB_OTG_DIEPCTL_SNAK; }
		} else		{ in[i].DIEPCTL = 0U; }
		if (out[i].DOEPCTL & USB_OTG_DOEPCTL_EPENA) {
			if (!i)	{ out[i].DOEPCTL = USB_OTG_DOEPCTL_SNAK; }
			else	{ out[i].DOEPCTL = USB_OTG_DOEPCTL_EPDIS | USB_OTG_DOEPCTL_SNAK; }
		} else		{ out[i].DOEPCTL = 0U; }
		in[i].DIEPTSIZ = out[i].DOEPTSIZ = 0U;
		in[i].DIEPINT  = out[i].DOEPINT  = 0xFB7FU;
	}

	device->DIEPMSK &= ~(USB_OTG_DIEPMSK_TXFURM);
	usb->GINTMSK = 0U;
	usb->GINTSTS = 0xBFFFFFFFU;
	usb->GINTMSK |= (
		USB_OTG_GINTMSK_USBSUSPM		|
		USB_OTG_GINTMSK_USBRST			|
		USB_OTG_GINTMSK_RXFLVLM			|
		USB_OTG_GINTMSK_ENUMDNEM		|
		USB_OTG_GINTMSK_IEPINT			|
		USB_OTG_GINTMSK_OEPINT			|
		USB_OTG_GINTMSK_IISOIXFRM		|
		USB_OTG_GINTMSK_PXFRM_IISOOXFRM	|
		USB_OTG_GINTMSK_WUIM
	);

	if (USB_handle.config.SOF_enable != 0U) { usb->GINTMSK |= USB_OTG_GINTMSK_SOFM; }
	*PCGCCTL &= ~(USB_OTG_PCGCCTL_STOPCLK | USB_OTG_PCGCCTL_GATECLK);
	device->DCTL |= USB_OTG_DCTL_SDIS;

	// TODO!!!
	usb->GRXFSIZ = 0x80;											// TODO: argument
	usb->DIEPTXF0_HNPTXFSIZ = ((uint32_t)0x40 << 16) | 0x80;		// TODO: argument
	usb->DIEPTXF[0] = ((uint32_t)0x80 << 16) | 0xC0;				// TODO: argument + logic to select endpoints
}

void start_USB(USB_OTG_GlobalTypeDef* usb) {
	USB_OTG_DeviceTypeDef*		device =	(void*)(((uint32_t)usb) + USB_OTG_DEVICE_BASE);
	__IO uint32_t*				PCGCCTL =	(void*)(((uint32_t)usb) + USB_OTG_PCGCCTL_BASE);

	usb->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
	*PCGCCTL &= ~(USB_OTG_PCGCCTL_STOPCLK | USB_OTG_PCGCCTL_GATECLK);
	device->DCTL &= ~USB_OTG_DCTL_SDIS;
}