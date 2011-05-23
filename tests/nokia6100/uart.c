

//turn on the clock
RCC_APB1ENR |= RCC_APB1ENR_USART2EN;

//PA2 (MOSI) to alternate function push-pull
mode = (uint32_t)0x09 << GPIOA_CRL_CNF2_MODE2_BIT;
temp = GPIOA_CRL & ~GPIOA_CRL_CNF2_MODE2_MASK;
GPIOA_CRL = temp | mode;

//PA3 (MISO) to alternate function input floating
mode = (uint32_t)0x04 << GPIOA_CRL_CNF3_MODE3_BIT;
temp = GPIOA_CRL & ~GPIOA_CRL_CNF3_MODE3_MASK;
GPIOA_CRL = temp | mode;

//PA4 (SCK) to alternate function push-pull
mode - (uint32_t)0x09 << GPIOA_CRL_CNF4_MODE4_BIT;
temp = GPIOA_CRL & ~GPIOA_CRL_CNF4_MODE4_MASK;
GPIOA_CRL = temp | mode;

//set clock rate to 6MHz;
USART2_BRR = 36000000L/6000000L; //APB1 bus max speed is 36MHz
USART2_CR1 |= (USART2_CR1_RE | USART2_CR1_TE | USART2_CR1_M); // RX, TX enable, 9-bit
USART2_CR2 |= USART2_CR2_CLKEN; //enable SCK pin CPOL and CPHA are also in this register
USART2_CR1 |= USART2_CR1_UE; // USART enable

//interrupts can also be enabled in the USART2_CR1 register
