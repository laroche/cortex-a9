#include "pl111.h"

/* #define PL111_CR_EN		0x001U */
/* #define PL111_CR_PWR		0x800U */
#define PL111_IOBASE		0x10020000U
/* #define PL111_PALBASE	(PL111_IOBASE + 0x200U) */

typedef unsigned int		uint32;

typedef struct _PL111MMIO {
	uint32		volatile tim0;
	uint32		volatile tim1;
	uint32		volatile tim2;
	uint32		volatile tim3;
	uint32		volatile upbase;
	uint32		volatile f;
	uint32		volatile control;
	uint32		volatile g;
} PL111MMIO;

static UG_GUI gui;
static uint32 *fb = (uint32 *) 0x60110000U;

static inline void draw_pixel(uint32 x, uint32 y, uint32 c)
{
	fb[(y * 800) + x] = c;
}

void pdraw(UG_S16 x, UG_S16 y, UG_COLOR c)
{
	draw_pixel(x, y, c);
}

void clcd_init(void)
{
	PL111MMIO *plio = (PL111MMIO*) PL111_IOBASE;
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
