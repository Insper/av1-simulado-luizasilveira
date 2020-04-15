// Includes:
#include <asf.h>
#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

// Defines:

typedef struct
{
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t week;
	uint32_t hour;
	uint32_t minute;
	uint32_t seccond;
} calendar;


// LED placa principal
#define LED0_PIO      PIOC
#define LED0_PIO_ID   ID_PIOC
#define LED0_IDX      8
#define LED0_IDX_MASK (1 << LED0_IDX)

// LED1 OLED
#define LED1_PIO      PIOA 
#define LED1_PIO_ID   ID_PIOA 
#define LED1_IDX      0
#define LED1_IDX_MASK (1 << LED1_IDX)

// LED2 OLED
#define LED2_PIO      PIOC
#define LED2_PIO_ID   ID_PIOC
#define LED2_IDX      30
#define LED2_IDX_MASK (1 << LED2_IDX)

// LED3 OLED
#define LED3_PIO      PIOB
#define LED3_PIO_ID   ID_PIOB
#define LED3_IDX      2
#define LED3_IDX_MASK (1 << LED3_IDX)

// Button 1 do OLED
#define Button1_PIO          PIOD
#define Button1_PIO_ID   ID_PIOD
#define Button1_IDX      28
#define Button1_IDX_MASK (1 << Button1_IDX)

// Button 2 do OLED
#define Button2_PIO      PIOC
#define Button2_PIO_ID   ID_PIOC
#define Button2_IDX		 31
#define Button2_IDX_MASK (1 << Button2_IDX)

// Button 3 do OLED
#define Button3_PIO      PIOA
#define Button3_PIO_ID   ID_PIOA
#define Button3_IDX      19
#define Button3_IDX_MASK (1 << Button3_IDX)

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/
volatile char button1_flag = 0;
volatile char button2_flag = 0;
volatile char button3_flag = 0;

volatile char flag_tc = 0;
volatile char flag_tc3 = 0;

volatile char flag_tc1 = 0;
volatile char flag_tc2 = 0;

volatile Bool f_rtt_alarme = false;

/************************/
/* Handlers             */
/************************/
void button1_callback(void) {
	button1_flag = 1;
}

void button2_callback(void) {
	button2_flag = 1;
}

void button3_callback(void) {
	button3_flag = 1;
}


void TC1_Handler(void){
	volatile uint32_t ul_dummy;

	/****************************************************************
	* Devemos indicar ao TC que a interrupção foi satisfeita.
	******************************************************************/
	ul_dummy = tc_get_status(TC0, 1);

	/* Avoid compiler warning */
	UNUSED(ul_dummy);

	/** Muda o estado do LED */
	flag_tc1 = 1;
}

void TC0_Handler(void){
	volatile uint32_t ul_dummy;

	/****************************************************************
	* Devemos indicar ao TC que a interrupção foi satisfeita.
	******************************************************************/
	ul_dummy = tc_get_status(TC0, 0);

	/* Avoid compiler warning */
	UNUSED(ul_dummy);

	/** Muda o estado do LED */
	flag_tc2 = 1;
}

void TC3_Handler(void){
	volatile uint32_t ul_dummy;

	/****************************************************************
	* Devemos indicar ao TC que a interrupção foi satisfeita.
	******************************************************************/
	ul_dummy = tc_get_status(TC1, 0);

	/* Avoid compiler warning */
	UNUSED(ul_dummy);

	/** Muda o estado do LED */
	flag_tc3 = 1;
}



// Functions:

void io_init(void);
void LED_init(void);
void button1_callback(void);
void button2_callback(void);
void button3_callback(void);

void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq);
void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type);
void pin_toggle(Pio *pio, uint32_t mask);

void pisca_led(int n, int t){
	for (int i=0;i<n;i++){
		pio_clear(LED1_PIO, LED1_IDX_MASK);
		delay_ms(t);
		pio_set(LED1_PIO, LED1_IDX_MASK);
		delay_ms(t);
	}
}

void pin_toggle(Pio *pio, uint32_t mask){
	if (pio_get_output_data_status(pio, mask))
	pio_clear(pio, mask);
	else
	pio_set(pio, mask);
}



//init
/**
* Configura TimerCounter (TC) para gerar uma interrupcao no canal (ID_TC e TC_CHANNEL)
* na taxa de especificada em freq.
*/
void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq){
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();

	/* Configura o PMC */
	/* O TimerCounter é meio confuso
	o uC possui 3 TCs, cada TC possui 3 canais
	TC0 : ID_TC0, ID_TC1, ID_TC2
	TC1 : ID_TC3, ID_TC4, ID_TC5
	TC2 : ID_TC6, ID_TC7, ID_TC8
	*/
	pmc_enable_periph_clk(ID_TC);

	/** Configura o TC para operar em  4Mhz e interrupçcão no RC compare */
	tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
	tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

	/* Configura e ativa interrupçcão no TC canal 0 */
	/* Interrupção no C */
	NVIC_EnableIRQ((IRQn_Type) ID_TC);
	tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);

	/* Inicializa o canal 0 do TC */
	tc_start(TC, TC_CHANNEL);
}
//void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type){
	///* Configura o PMC */
	//pmc_enable_periph_clk(ID_RTC);
//
	///* Default RTC configuration, 24-hour mode */
	//rtc_set_hour_mode(rtc, 0);
//
	///* Configura data e hora manualmente */
	//rtc_set_date(rtc, t.year, t.month, t.day, t.week);
	//rtc_set_time(rtc, t.hour, t.minute, t.seccond);
//
	///* Configure RTC interrupts */
	//NVIC_DisableIRQ(id_rtc);
	//NVIC_ClearPendingIRQ(id_rtc);
	//NVIC_SetPriority(id_rtc, 0);
	//NVIC_EnableIRQ(id_rtc);
//
	///* Ativa interrupcao via alarme */
	//rtc_enable_interrupt(rtc, irq_type);
//}


void LED_init(){
	
		// Ativa PIOs necessários
		pmc_enable_periph_clk(LED0_PIO_ID);
		pmc_enable_periph_clk(LED1_PIO_ID);
		pmc_enable_periph_clk(LED2_PIO_ID);
		pmc_enable_periph_clk(LED3_PIO_ID);
		
		// Inicializa LEDs como saída
		pio_set_output(LED1_PIO, LED1_IDX_MASK, 0, 0, 0);
		pio_set_output(LED2_PIO, LED2_IDX_MASK, 1, 0, 0);
		pio_set_output(LED3_PIO, LED3_IDX_MASK, 0, 0, 0);
		
		//pio_set(LED1_PIO, LED1_IDX_MASK);
		//pio_set(LED2_PIO, LED2_IDX_MASK);
		//pio_set(LED3_PIO, LED3_IDX_MASK);
		
		pio_configure(LED0_PIO, PIO_OUTPUT_0, LED0_IDX_MASK, PIO_DEFAULT);
		pio_configure(LED1_PIO, PIO_OUTPUT_0, LED0_IDX_MASK, PIO_DEFAULT);
		pio_configure(LED2_PIO, PIO_OUTPUT_0, LED0_IDX_MASK, PIO_DEFAULT);
		pio_configure(LED3_PIO, PIO_OUTPUT_0, LED0_IDX_MASK, PIO_DEFAULT);
	
	
	//pmc_enable_periph_clk(LED1_PIO_ID);
	//pio_set_output(LED1_PIO, LED1_IDX_MASK, estado, 0, 0);
	//
	//pmc_enable_periph_clk(LED_PIO_ID);
	//pio_set_output(LED_PIO, LED_IDX_MASK, estado, 0, 0);
	//
	//pmc_enable_periph_clk(LED2_PIO_ID);
	//pio_configure(LED2_PIO, PIO_OUTPUT_0, LED2_IDX_MASK, PIO_DEFAULT);
	//pio_set(LED2_PIO, LED2_IDX_MASK);
};

void io_init(void)
{
	
	pmc_enable_periph_clk(Button1_PIO_ID);
	pmc_enable_periph_clk(Button2_PIO_ID);
	pmc_enable_periph_clk(Button3_PIO_ID);
	
	// Configura PIO para lidar com o pino do botão como entrada
	// com pull-up
	pio_configure(Button1_PIO, PIO_INPUT, Button1_IDX_MASK, PIO_IT_RISE_EDGE | PIO_DEBOUNCE);
	pio_configure(Button2_PIO, PIO_INPUT, Button2_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_configure(Button3_PIO, PIO_INPUT, Button3_IDX_MASK, PIO_IT_RISE_EDGE | PIO_DEBOUNCE);
	
	// Set bounce
	//pio_set_debounce_filter(Button1_PIO, Button1_IDX_MASK, 100);
	//pio_set_debounce_filter(Button2_PIO, Button2_IDX_MASK, 100);
	//pio_set_debounce_filter(Button3_PIO, Button2_IDX_MASK, 100);

	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada	
	pio_handler_set(Button1_PIO, Button1_PIO_ID, Button1_IDX_MASK, PIO_IT_RISE_EDGE, button1_callback);
	pio_handler_set(Button2_PIO, Button2_PIO_ID, Button2_IDX_MASK, PIO_IT_RISE_EDGE, button2_callback);
	pio_handler_set(Button3_PIO, Button3_PIO_ID, Button3_IDX_MASK, PIO_IT_RISE_EDGE, button3_callback);

	// Ativa interrupção
	pio_enable_interrupt(Button1_PIO, Button1_IDX_MASK);
	pio_enable_interrupt(Button2_PIO, Button2_IDX_MASK);
	pio_enable_interrupt(Button3_PIO, Button3_IDX_MASK);
	
	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(Button1_PIO_ID);
	NVIC_SetPriority(Button1_PIO_ID, 4);
	
	NVIC_EnableIRQ(Button2_PIO_ID);
	NVIC_SetPriority(Button2_PIO_ID, 4);
	
	NVIC_EnableIRQ(Button3_PIO_ID);
	NVIC_SetPriority(Button3_PIO_ID, 4);
}



int main (void)
{
	board_init();
	// Initialize the board clock
	sysclk_init();
	delay_init();
	
	/* Disable the watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;

    // Init OLED
	gfx_mono_ssd1306_init();
  
    // Escreve na tela um circulo e um texto
	gfx_mono_draw_filled_circle(20, 16, 16, GFX_PIXEL_SET, GFX_WHOLE);
    gfx_mono_draw_string("mundo", 50,16, &sysfont);
  
    io_init();
	LED_init();
	
	/** Configura timer TC0, canal 1 */
	TC_init(TC0, ID_TC1, 1, 5);
	TC_init(TC0, ID_TC0, 0, 10);
	TC_init(TC1, ID_TC3, 0, 1);
  

  /* Insert application code here, after the board has been initialized. */
	while(1) {
		
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
		
		if(flag_tc1){
			pin_toggle(LED1_PIO, LED1_IDX_MASK);
			flag_tc1 = 0;
		}
		
		if(flag_tc2){
			pin_toggle(LED2_PIO, LED2_IDX_MASK);
			flag_tc2 = 0;
		}
		
		if(flag_tc3){
			pin_toggle(LED3_PIO, LED3_IDX_MASK);
			flag_tc3 = 0;
		}
		
		//if(button1_flag){
			///* ....   */
			//// zera flag
			//pisca_led(1,10);
			//button1_flag = 0;
		//}
	}
}

