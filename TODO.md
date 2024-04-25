# TODO
>### Very High Priority
>* SPI for lcd screen!!! and M95P32 flash (upgrade for 24LC512)

>### High Priority
>* very low power
>* RTC with wake-up

>### Medium Priority
>* QSPI for M95P32 flash speed up
>* Make library components toggleable using defines
>* USB composite device!! (to make it possible to download keyboard passwords as if the keyboard is a flash drive!)

>### Low Priority
>* I2C fast mode
>* I2C slave mode
>* Clean up code (make 0xA0UL and 0b10UL consistent!!)
>  * remove redundant pin device devices (scl_i2c, sda_i2c)
>* clock security on HSE and LSE (+interrupt!!)
>* HRTIM
>* LPTIM


>### Notes
>* Consider making all "internal" interrupts __weak (especially SysTick!!)

>### Features
>* RCC backup domain (RCC->BDCR)
>* STOP mode config (RCC->CFGR:STOPWUCK)
>* clock trimming (RCC->ICSCR, RCC->HSICFGR, RCC->CRRCR, RCC->CSICFGR)

