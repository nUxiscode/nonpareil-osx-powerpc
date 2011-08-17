/*
$Id: proc_classic.h 686 2005-05-26 09:06:45Z eric $
Copyright 1995, 2003, 2004, 2005 Eric L. Smith <eric@brouhaha.com>

Nonpareil is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.  Note that I am not
granting permission to redistribute or modify Nonpareil under the
terms of any later version of the General Public License.

Nonpareil is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program (in the file "COPYING"); if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111, USA.
*/

//
// Changes for Mac OS X and iOS by Maciej Bartosiak
// Copyright 2005-2010 Maciej Bartosiak
//

#define WSIZE 14
#define EXPSIZE 3  // two exponent and one exponent sign digit

typedef digit_t reg_t [WSIZE];


#define SSIZE 12
#define EXT_FLAG_SIZE 12

#define MAX_GROUP 2
#define MAX_ROM 8
#define ROM_SIZE 256

#if 1
  // This was the easiest way to use the existing code:
  #define MAX_BANK 1
  #define MAX_PAGE 1
  #define N_PAGE_SIZE (MAX_GROUP * MAX_ROM * ROM_SIZE)
#else
  // This would somewhat more accurately represent the hardware
  // behavior, but the current code won't deal with it correctly:
  #define MAX_BANK (MAX_GROUP * MAX_ROM)
  #define MAX_PAGE 1
  #define N_PAGE_SIZE (ROM_SIZE)
#endif

//struct classic_cpu_reg_t;

typedef struct classic_cpu_reg_t
{
  reg_t a;
  reg_t b;
  reg_t c;
  reg_t d;
  reg_t e;
  reg_t f;
  reg_t m;

  digit_t p;

  bool carry, prev_carry;

  bool s [SSIZE];

  uint8_t pc;
  uint8_t rom;
  uint8_t group;

  uint8_t del_rom;
  uint8_t del_grp;

  uint8_t ret_pc;

  int prev_pc;  /* used to store complete five-digit octal address of instruction */

  bool ext_flag [SSIZE];  /* external flags, e.g., slide switches,
			     magnetic card inserted */

  // keyboard
  bool key_flag;      /* true if a key is down */
  int key_buf;        /* most recently pressed key */

  // display
  bool display_enable;

  int left_scan;
  int right_scan;
  int display_scan_position;   /* word index, left_scan down to right_scan */
  int display_digit_position;  /* character index, 0 to MAX_DIGIT_POSITION-1 */

  void (* display_scan_fn) (struct classic_cpu_reg_t *cpu_reg);

  void (* op_fcn [1024])(struct classic_cpu_reg_t *cpu_reg, int opcode);

  // ROM:
  rom_word_t *ucode;  // name "rom" was already taken
  bool *rom_exists;
  bool *rom_breakpoint;

  // RAM
  uint16_t   max_ram;
  int ram_addr;  /* selected RAM address */
  reg_t *ram;
  
  //sim_t *sim;
  uint64_t cycle_count;
  
  
  // moved from sim_t;
  segment_bitmap_t display_segments [MAX_DIGIT_POSITION];
  segment_bitmap_t *char_gen;
  int display_digits;
  
  //bool need_redraw;
  void *display;
} classic_cpu_reg_t;

classic_cpu_reg_t *classic_new_processor (int ram_size, void *display);

bool classic_read_rom (classic_cpu_reg_t *cpu_reg,
					   uint8_t    bank,
					   addr_t     addr,
					   rom_word_t *val);

bool classic_write_rom (classic_cpu_reg_t *cpu_reg,
						uint8_t    bank,
						addr_t     addr,
						rom_word_t *val);

bool classic_write_rom (classic_cpu_reg_t *cpu_reg,
						uint8_t    bank,
						addr_t     addr,
						rom_word_t *val);

bool classic_execute_instruction (classic_cpu_reg_t *cpu_reg);

void classic_press_key (classic_cpu_reg_t *cpu_reg, int keycode);

void classic_release_key (classic_cpu_reg_t *cpu_reg);

void classic_set_ext_flag (classic_cpu_reg_t *cpu_reg, int flag, bool state);

bool classic_read_object_file (classic_cpu_reg_t *cpu_reg, const char *fn);