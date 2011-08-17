/*
$Id: proc_woodstock.h 686 2005-05-26 09:06:45Z eric $
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


#define SSIZE 16
#define STACK_SIZE 2

#define EXT_FLAG_SIZE 16

#define N_PAGE_SIZE 1024
#define MAX_PAGE 4
#define MAX_BANK 2


typedef uint16_t rom_addr_t;


typedef enum
  {
    norm,
    branch,
    selftest
  } inst_state_t;


typedef struct act_reg_t
{
  reg_t a;
  reg_t b;
  reg_t c;
  reg_t y;
  reg_t z;
  reg_t t;
  reg_t m1;
  reg_t m2;

  digit_t f;

  digit_t p;

  bool decimal;

  bool carry, prev_carry;

  bool s [SSIZE];                 // ACT flags (status bits)
  bool ext_flag [EXT_FLAG_SIZE];  // external flags, cause status or CRC
                                     // bits to get set

  uint16_t pc;

  bool del_rom_flag;
  uint8_t del_rom;

  inst_state_t inst_state;

  int sp;  /* stack pointer */
  uint16_t stack [STACK_SIZE];

  int prev_pc;  /* used to store complete five-digit octal address of instruction */

  int crc;

  // keyboard

  bool key_flag;      /* true if a key is down */
  int key_buf;        /* most recently pressed key */

  // display

  bool display_enable;
  bool display_14_digit;  // true after RESET TWF instruction

  int left_scan;
  int right_scan;
  int display_scan_position;   /* word index, left_scan down to right_scan */
  int display_digit_position;  /* character index, 0 to MAX_DIGIT_POSITION-1 */

  void (* display_scan_fn) (struct act_reg_t *act_reg);

  void (* op_fcn [1024])(struct act_reg_t *act_reg, int opcode);

  // ROM:
  uint8_t bank_exists [MAX_PAGE];  // bitmap
  bool bank;                       // only a single global bank bit
  rom_word_t *rom;
  bool *rom_exists;
  bool *rom_breakpoint;

  // RAM:
  int ram_addr;  /* selected RAM address */
  reg_t *ram;
  
  uint16_t   max_ram;
  
  uint64_t cycle_count;
  
  // moved from sim_t;
  segment_bitmap_t display_segments [MAX_DIGIT_POSITION];
  segment_bitmap_t *char_gen;
  int display_digits;
  
  //bool need_redraw;
  void *display;
  
} act_reg_t;

act_reg_t *spice_new_processor (int ram_size, void *display);
act_reg_t *woodstock_new_processor (int ram_size, void *display);

void woodstock_set_ext_flag (act_reg_t *act_reg, int flag, bool state);

bool woodstock_execute_instruction (act_reg_t *act_reg);

bool spice_execute_instruction (act_reg_t *act_reg);

bool woodstock_read_object_file (act_reg_t *act_reg, const char *fn);

void woodstock_press_key (act_reg_t *act_reg, int keycode);

void woodstock_release_key (act_reg_t *act_reg);

void display_callback(struct act_reg_t *ar);