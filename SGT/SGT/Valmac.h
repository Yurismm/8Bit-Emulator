
#if !defined(VALMAC_H)
#define VALMAC_H

#pragma once


#define MEMORY_SIZE	(4096)

// added a header to clean up the SGT file provided to me.
class Valmac {


public:
	
	bool drawFlag;

	uint8_t gfx[64 * 32];

	uint8_t key[16];

	bool isRunning{ true };

	void emulateCycle();

	void initialize();

	void load_program(uint8_t* pProgram, size_t bufferSize);

	uint16_t get_program_opcode();

	inline void step_PC();

	void UpdateGraphics();


private:

	uint16_t opcode;

	uint8_t memory[MEMORY_SIZE];

	uint8_t delay_timer;

	uint8_t sound_timer;

	uint16_t I;	

	uint16_t PC;

	uint16_t stack[16];

	uint16_t SP;

	uint8_t V[16];

};

#endif