#include <msp430.h>

#define OLED_ADDR 0x3C 

const unsigned char font_karakterler[25][5] = {
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 10: Boşluk
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // 11: 'N'
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // 12: 'T'
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 13: 'C'
    {0x00, 0x36, 0x36, 0x00, 0x00}, // 14: ':' 
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // 15: 'E'
    {0x7F, 0x02, 0x04, 0x02, 0x7F}, // 16: 'M'
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // 17: 'L'
    {0x7F, 0x41, 0x41, 0x41, 0x3E}, // 18: 'D'
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // 19: 'R'
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 20: 'O' 
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // 21: 'F' 
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // 22: 'P' 
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // 23: 'U' 
    {0x7E, 0x11, 0x11, 0x11, 0x7E}  // 24: 'A' 
};

void UART_init(void) {
    P1SEL  |= BIT1 + BIT2;      
    P1SEL2 |= BIT1 + BIT2;      
    UCA0CTL1 |= UCSSEL_2;       
    UCA0BR0 = 104;              
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0;          
    UCA0CTL1 &= ~UCSWRST;       
}

void UART_Gonder_Metin(char *metin) {
    while (*metin) {
        while (!(IFG2 & UCA0TXIFG)); 
        UCA0TXBUF = *metin++;        
    }
}

void UART_Gonder_Sayi(int sayi) {
    char buffer[6];
    int i = 4;
    buffer[5] = '\0';
    if (sayi < 0) sayi = 0; 
    if (sayi == 0) { UART_Gonder_Metin("0"); return; }
    while (sayi > 0) {
        buffer[i--] = (sayi % 10) + '0';
        sayi /= 10;
    }
    UART_Gonder_Metin(&buffer[i + 1]);
}


void I2C_init(void) {
    P1SEL |= BIT6 + BIT7;
    P1SEL2 |= BIT6 + BIT7;
    UCB0CTL1 |= UCSWRST;
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;
    UCB0CTL1 = UCSSEL_2 + UCSWRST;
    UCB0BR0 = 10;
    UCB0BR1 = 0;
    UCB0I2CSA = OLED_ADDR;
    UCB0CTL1 &= ~UCSWRST;
}

void I2C_write(unsigned char control, unsigned char data) {
    while (UCB0CTL1 & UCTXSTP); 
    UCB0CTL1 |= UCTR + UCTXSTT;
    while (!(IFG2 & UCB0TXIFG));
    UCB0TXBUF = control;
    while (!(IFG2 & UCB0TXIFG));
    UCB0TXBUF = data;
    while (!(IFG2 & UCB0TXIFG));
    UCB0CTL1 |= UCTXSTP; 
}

void OLED_command(unsigned char cmd) { I2C_write(0x00, cmd); }
void OLED_data(unsigned char data)   { I2C_write(0x40, data); }

void OLED_init(void) {
    OLED_command(0xAE); OLED_command(0xD5); OLED_command(0x80); 
    OLED_command(0xA8); OLED_command(0x3F); OLED_command(0xD3); 
    OLED_command(0x00); OLED_command(0x40); OLED_command(0x8D); 
    OLED_command(0x14); OLED_command(0x20); OLED_command(0x00); 
    OLED_command(0xA1); OLED_command(0xC8); OLED_command(0xDA); 
    OLED_command(0x12); OLED_command(0x81); OLED_command(0xCF); 
    OLED_command(0xD9); OLED_command(0xF1); OLED_command(0xDB); 
    OLED_command(0x40); OLED_command(0xA4); OLED_command(0xA6); 
    OLED_command(0xAF); 
}

void OLED_clear(void) {
    int page, col;
    for (page = 0; page < 8; page++) {
        OLED_command(0xB0 + page);
        OLED_command(0x00);
        OLED_command(0x10);
        for (col = 0; col < 128; col++) {
            OLED_data(0x00);
        }
    }
}

void OLED_Imlec(unsigned char satir, unsigned char sutun) {
    OLED_command(0xB0 + satir);           
    OLED_command(sutun & 0x0F);           
    OLED_command(0x10 | (sutun >> 4));    
}

void OLED_Karakter_Yaz(unsigned char indeks) {
    unsigned char i;
    for (i = 0; i < 5; i++) {
        OLED_data(font_karakterler[indeks][i]); 
    }
    OLED_data(0x00); 
}

void OLED_Sayi_Yaz(unsigned int sayi) {
    if(sayi < 1000) OLED_Karakter_Yaz(10); 
    else OLED_Karakter_Yaz(sayi / 1000);       
    if(sayi < 100) OLED_Karakter_Yaz(10);
    else OLED_Karakter_Yaz((sayi / 100) % 10); 
    if(sayi < 10) OLED_Karakter_Yaz(10);
    else OLED_Karakter_Yaz((sayi / 10) % 10);  
    OLED_Karakter_Yaz(sayi % 10);              
}

unsigned int ADC_Oku(unsigned char kanal) {
    ADC10CTL0 &= ~ENC; 
    if (kanal == 3) ADC10CTL1 = INCH_3; 
    if (kanal == 4) ADC10CTL1 = INCH_4; 
    if (kanal == 5) ADC10CTL1 = INCH_5; 
    ADC10CTL0 |= ENC + ADC10SC; 
    while (!(ADC10CTL0 & ADC10IFG)); 
    ADC10CTL0 &= ~ADC10IFG;          
    return ADC10MEM; 
}


int main(void) {
    WDTCTL = WDTPW + WDTHOLD;

    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    P2DIR |= BIT3;    
    P2OUT &= ~BIT3;   

    P2DIR |= BIT4;        
    P2SEL |= BIT4;        
    TA1CCR0 = 1000;       
    TA1CCTL2 = OUTMOD_7;  
    TA1CCR2 = 0;          
    TA1CTL = TASSEL_2 + MC_1; 

    
    UART_init();
    I2C_init();
    __delay_cycles(100000);
    OLED_init();
    OLED_clear();

    ADC10AE0 |= BIT3 + BIT4 + BIT5; 
    ADC10CTL0 = ADC10SHT_3 + ADC10ON; 

    while(1) {
        unsigned int ldr_deger = ADC_Oku(3);
        unsigned int nem_deger = ADC_Oku(4); 
        unsigned int ntc_deger = ADC_Oku(5);

        int nem_fark = nem_deger - 500;
        if (nem_fark < 0) nem_fark = -nem_fark; 
        int nem_puan = 40 - ((nem_fark * 40) / 500);
        if (nem_puan < 0) nem_puan = 0;

        int ntc_fark = ntc_deger - 500;
        if (ntc_fark < 0) ntc_fark = -ntc_fark; 
        int ntc_puan = 40 - ((ntc_fark * 40) / 500);
        if (ntc_puan < 0) ntc_puan = 0;

        int ldr_puan = (ldr_deger * 20) / 1000;
        if (ldr_puan > 20) ldr_puan = 20;

        int kalite_puani = nem_puan + ntc_puan + ldr_puan;
        if (kalite_puani > 100) kalite_puani = 100;
        if (kalite_puani < 0) kalite_puani = 0;

        int pwm_hedef = 1000 - ldr_deger;
        if (pwm_hedef < 0) pwm_hedef = 0;
        if (pwm_hedef > 1000) pwm_hedef = 1000;
        TA1CCR2 = pwm_hedef; 

        if (nem_deger > 700) {
            P2OUT |= BIT3;  
        } else if (nem_deger < 400) {
            P2OUT &= ~BIT3; 
        }

        UART_Gonder_Metin("ISIK: ");  UART_Gonder_Sayi(ldr_deger);
        UART_Gonder_Metin(" | NEM: "); UART_Gonder_Sayi(nem_deger);
        UART_Gonder_Metin(" | ISI: "); UART_Gonder_Sayi(ntc_deger);
        UART_Gonder_Metin(" | PUAN: "); UART_Gonder_Sayi(kalite_puani);

        if (P2OUT & BIT3){

             UART_Gonder_Metin(" | ROLE: ON \r\n");
        }
        else{

               UART_Gonder_Metin(" | ROLE: OFF\r\n");
        }       

        OLED_Imlec(0, 0); // Satır 0
        OLED_Karakter_Yaz(11); OLED_Karakter_Yaz(12); OLED_Karakter_Yaz(13); OLED_Karakter_Yaz(14); // NTC:
        OLED_Karakter_Yaz(10); OLED_Sayi_Yaz(ntc_deger); 

        OLED_Imlec(3, 0); // Satır 3
        OLED_Karakter_Yaz(17); OLED_Karakter_Yaz(18); OLED_Karakter_Yaz(19); OLED_Karakter_Yaz(14); // LDR:
        OLED_Karakter_Yaz(10); OLED_Sayi_Yaz(ldr_deger); 

        OLED_Imlec(6, 0); // Satır 6
        OLED_Karakter_Yaz(11); OLED_Karakter_Yaz(15); OLED_Karakter_Yaz(16); OLED_Karakter_Yaz(14); // NEM:
        OLED_Karakter_Yaz(10); OLED_Sayi_Yaz(nem_deger); 

        OLED_Imlec(0, 64); 
        OLED_Karakter_Yaz(22); OLED_Karakter_Yaz(23); OLED_Karakter_Yaz(24); OLED_Karakter_Yaz(11); // PUAN
        OLED_Karakter_Yaz(14); OLED_Karakter_Yaz(10); OLED_Sayi_Yaz(kalite_puani);

        OLED_Imlec(3, 64); 
        if (P2OUT & BIT3) {
            OLED_Karakter_Yaz(19); OLED_Karakter_Yaz(20); OLED_Karakter_Yaz(17); OLED_Karakter_Yaz(15); // ROLE
            OLED_Karakter_Yaz(14); OLED_Karakter_Yaz(10); 
            OLED_Karakter_Yaz(20); OLED_Karakter_Yaz(11); OLED_Karakter_Yaz(10); // ON(Boşluk)
        } else {
            OLED_Karakter_Yaz(19); OLED_Karakter_Yaz(20); OLED_Karakter_Yaz(17); OLED_Karakter_Yaz(15); // ROLE
            OLED_Karakter_Yaz(14); OLED_Karakter_Yaz(10); 
            OLED_Karakter_Yaz(20); OLED_Karakter_Yaz(21); OLED_Karakter_Yaz(21); // OFF
        }

        __delay_cycles(300000); 
    }
}