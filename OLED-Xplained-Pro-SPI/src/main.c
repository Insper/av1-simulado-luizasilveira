// Includes:
#include <asf.h>
#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

// Defines:

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

/************************/
/* Handlers             */
/************************/
void button1_handler(void) {
	button1_flag = 1;
}

void button2_handler(void) {
	button2_flag = 1;
}

void button3_handler(void) {
	button3_flag = 1;
}


//init
void io_init(void)
{
	

	// Ativa PIOs necessários
	pmc_enable_periph_clk(LED0_PIO_ID);
	pmc_enable_periph_clk(LED1_PIO_ID);
	pmc_enable_periph_clk(LED2_PIO_ID);
	pmc_enable_periph_clk(LED3_PIO_ID);
	
	pmc_enable_periph_clk(Button1_PIO_ID);
	pmc_enable_periph_clk(Button2_PIO_ID);
	pmc_enable_periph_clk(Button3_PIO_ID);
	
	//pio_configure(LED0_PIO, PIO_OUTPUT_0, LED0_IDX_MASK, PIO_DEFAULT);
	//pio_configure(LED1_PIO, PIO_OUTPUT_0, LED0_IDX_MASK, PIO_DEFAULT);
	//pio_configure(LED2_PIO, PIO_OUTPUT_0, LED0_IDX_MASK, PIO_DEFAULT);
	//pio_configure(LED3_PIO, PIO_OUTPUT_0, LED0_IDX_MASK, PIO_DEFAULT);
	
	// Inicializa LEDs como saída
	pio_set_output(LED1_PIO, LED1_IDX_MASK, 0, 0, 0);
	pio_set_output(LED2_PIO, LED2_IDX_MASK, 0, 0, 0);
	pio_set_output(LED3_PIO, LED3_IDX_MASK, 0, 0, 0);


	// Configura PIO para lidar com o pino do botão como entrada
	// com pull-up
	pio_configure(Button1_PIO, PIO_INPUT, Button1_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_configure(Button2_PIO, PIO_INPUT, Button2_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_configure(Button3_PIO, PIO_INPUT, Button3_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	
	// Set bounce
	//pio_set_debounce_filter(Button1_PIO, Button1_IDX_MASK, 100);
	//pio_set_debounce_filter(Button2_PIO, Button2_IDX_MASK, 100);
	//pio_set_debounce_filter(Button3_PIO, Button2_IDX_MASK, 100);

	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada	
	pio_handler_set(Button1_PIO, Button1_PIO_ID, Button1_IDX_MASK, PIO_IT_RISE_EDGE, selectCallback);
	pio_handler_set(Button2_PIO, Button2_PIO_ID, Button2_IDX_MASK, PIO_IT_RISE_EDGE, playCallback);
	pio_handler_set(Button3_PIO, Button3_PIO_ID, Button3_IDX_MASK, PIO_IT_RISE_EDGE, selectCallback);

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

  // Init OLED
	gfx_mono_ssd1306_init();
  
  // Escreve na tela um circulo e um texto
	gfx_mono_draw_filled_circle(20, 16, 16, GFX_PIXEL_SET, GFX_WHOLE);
  gfx_mono_draw_string("mundo", 50,16, &sysfont);

  /* Insert application code here, after the board has been initialized. */
	while(1) {

	}
}
