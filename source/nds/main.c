/*
 *  main.c
 *
 *  Created by Andreas Kröhnke on 10/7/07.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <nds.h>
#include <nds/arm9/console.h>

#include "game.h"

/* TODO: Fix timer, a 0 is at wrong place.
 * Lights seem to work.
 * Save POLY_flags somewhere.
 * Implement quaternions
 */
int main()
{
	struct r_gl_buffer buffer;

	buffer.width = 255;
	buffer.height = 191;

    powerOn(POWER_ALL);
    irqInit();
	irqEnable(IRQ_VBLANK);
    consoleDemoInit();
    
    videoSetMode(MODE_0_3D);
    vramSetBankA(VRAM_A_TEXTURE);
    glInit();
    
    glEnable(GL_ANTIALIAS);
    glClearPolyID(63);

	glMaterialShinyness();

	game_initialize();
    while(1) {
		game_update();
		game_render(&buffer);

		glFlush(0);
		swiWaitForVBlank();
	}
    
    return 0;
}

