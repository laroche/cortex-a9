#include "cortex_config.h"
#if CONFIG_GUI

#include <sys/cdefs.h>
#include "pl111.h"

/* #define PL111_CR_EN		0x001U */
/* #define PL111_CR_PWR		0x800U */
#define PL111_IOBASE		0x10020000U
/* #define PL111_PALBASE	(PL111_IOBASE + 0x200U) */

typedef volatile struct {
	uint32_t tim0;
	uint32_t tim1;
	uint32_t tim2;
	uint32_t tim3;
	uint32_t upbase;
	uint32_t f;
	uint32_t control;
	uint32_t g;
} PL111MMIO;

static UG_GUI gui;
static uint32_t * const fb = (uint32_t *) 0x60110000U;

static __always_inline void draw_pixel (uint32_t x, uint32_t y, uint32_t c)
{
	fb[(y * 800) + x] = c;
}

void pdraw (UG_S16 x, UG_S16 y, UG_COLOR c)
{
	draw_pixel((uint32_t)x, (uint32_t)y, c);
}

void clcd_init (void)
{
	PL111MMIO * const plio = (PL111MMIO *) PL111_IOBASE;
	int x;

	/* 640x480 pixels */
	/* plio->tim0 = 0x3f1f3f9cU; */
	plio->tim0 = 0x3f1f3cc4U;
	/* plio->tim1 = 0x080b61dfU; */
	plio->tim1 = 0x080b6257U;
	plio->upbase = 0x60110000U;
	/* 16-bit color */
	plio->control = 0x192bU;

	for (x = 0; x < (800 * 600); ++x)
		fb[x] = 0U;

	UG_Init (&gui, pdraw, 800, 600);
}
#endif
