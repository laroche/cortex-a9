#include "pl011.h"
#include "pl050.h"
#include "interrupt.h"

typedef volatile struct {
	uint32_t cr;
	uint32_t status;
	uint32_t data;
	uint32_t clk;
	uint32_t ir;
} kmi_t;

#define KMI_KB_BASE 0x10006000U

static kmi_t * const kmi_kb = (kmi_t *) KMI_KB_BASE;

static char kbdus[128] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x00 - 0x0f */
	0,0,0,0,0,'q','1',0,0,0,'z','s','a','w','2',0, /* 0x10 - 0x1f */
	0,'c','x','d','e','4','3',0,0,' ','v','f','t','r','5',0, /* 0x20 - 0x2f */
	0,'n','b','h','g','y','6',0,0,0,'m','j','u','7','8',0, /* 0x30 - 0x3f */
	0,',','k','i','o','0','9',0,0,'.','/','l',';','p','-',0, /* 0x40 - 0x4f */
	0,0,'\'',0,'[','=',0,0,0,0,'\n',']',0,0,0,0, /* 0x50 - 0x5f */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x60 - 0x6f */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 /* 0x70 - 0x7f */
};

static void kb_handler(void)
{
	uint8_t tsc = (uint8_t) kmi_kb->data;

	if (tsc < 128U) {
 		uart_putc(kbdus[tsc]);
 	}
}

void kb_init(void)
{
	kmi_kb->cr = 0x14U;
	kmi_kb->clk = 15U;

	install_isr(KMI0_INTR_IRQn, kb_handler);
	enable_irq(KMI0_INTR_IRQn);
}
