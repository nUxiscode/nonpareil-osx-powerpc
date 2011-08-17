/*
 $Id: proc_classic.c 849 2005-06-27 20:33:01Z eric $
 Copyright 2004, 2005 Eric L. Smith <eric@brouhaha.com>
 
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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "arch.h"
#include "macutil.h"
#include "display.h"
#include "proc.h"

//#include "proc_int.h"
#include "digit_ops.h"
#include "proc_classic.h"

static void classic_event_fn (classic_cpu_reg_t *cpu_reg, int event);

bool classic_read_rom (classic_cpu_reg_t *cpu_reg,
					   uint8_t    bank,
					   addr_t     addr,
					   rom_word_t *val)
{
	if ((bank >= MAX_BANK) || (addr >= (MAX_PAGE * N_PAGE_SIZE)))
		return false;
	
	if (! cpu_reg->rom_exists [addr])
		return false;
	
	*val = cpu_reg->ucode [addr];
	return true;
}


bool classic_write_rom (classic_cpu_reg_t *cpu_reg,
						uint8_t    bank,
						addr_t     addr,
						rom_word_t *val)
{
	if ((bank >= MAX_BANK) || (addr > (MAX_PAGE * N_PAGE_SIZE)))
		return false;
	
	cpu_reg->rom_exists [addr] = true;
	cpu_reg->ucode [addr] = *val;
	
	return true;
}


static inline uint8_t arithmetic_base (classic_cpu_reg_t *cpu_reg)
{
	return 10;  // no binary (hex) mode on Classic
}


static void bad_op (classic_cpu_reg_t *cpu_reg, int opcode)
{
	printf ("illegal opcode %04o at %02o%03o\n", opcode,
			cpu_reg->prev_pc >> 8, cpu_reg->prev_pc & 0377);
}


static void op_arith (classic_cpu_reg_t *cpu_reg, int opcode)
{
	uint8_t op, field;
	int first = 0;
	int last = 0;
	
	op = opcode >> 5;
	field = (opcode >> 2) & 7;
	
	switch (field)
    {
		case 0:  /* p  */
			first =  cpu_reg->p; last =  cpu_reg->p;
			if (cpu_reg->p >= WSIZE)
			{
				printf ("Warning! p >= WSIZE at %02o%03o\n",
						cpu_reg->prev_pc >> 8, cpu_reg->prev_pc & 0377);
				last = 0;  /* don't do anything */
			}
				break;
		case 1:  /* m  */  first = EXPSIZE;      last = WSIZE - 2;   break;
		case 2:  /* x  */  first = 0;            last = EXPSIZE - 1; break;
		case 3:  /* w  */  first = 0;            last = WSIZE - 1;   break;
		case 4:  /* wp */
			first =  0; last =  cpu_reg->p;
			if (cpu_reg->p >= WSIZE)
			{
				printf ("Warning! p >= WSIZE at %02o%03o\n",
						cpu_reg->prev_pc >> 8, cpu_reg->prev_pc & 0377);
				last = WSIZE - 1;
			}
				break;
		case 5:  /* ms */  first =  EXPSIZE;     last = WSIZE - 1;   break;
		case 6:  /* xs */  first =  EXPSIZE - 1; last = EXPSIZE - 1; break;
		case 7:  /* s  */  first =  WSIZE - 1;   last = WSIZE - 1;   break;
    }
	
	// Note: carry was set to 0 by classic_execute_instruction before
	// we're called.
	switch (op)
    {
		case 0x00:  /* if b[f] = 0 */
			reg_test_nonequal (cpu_reg->b, NULL, first, last, & cpu_reg->carry);
			break;
		case 0x01:  /* 0 -> b[f] */
			reg_zero (cpu_reg->b, first, last);
			break;
		case 0x02:  /* if a >= c[f] */
			reg_sub (NULL, cpu_reg->a, cpu_reg->c,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x03:  /* if c[f] >= 1 */
			reg_test_equal (cpu_reg->c, NULL, first, last, & cpu_reg->carry);
			break;
		case 0x04:  /* b -> c[f] */
			reg_copy (cpu_reg->c, cpu_reg->b, first, last);
			break;
		case 0x05:  /* 0 - c -> c[f] */
			reg_sub (cpu_reg->c, NULL, cpu_reg->c,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x06:  /* 0 -> c[f] */
			reg_zero (cpu_reg->c, first, last);
			break;
		case 0x07:  /* 0 - c - 1 -> c[f] */
			cpu_reg->carry = 1;
			reg_sub (cpu_reg->c, NULL, cpu_reg->c,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x08:  /* shift left a[f] */
			reg_shift_left (cpu_reg->a, first, last);
			break;
		case 0x09:  /* a -> b[f] */
			reg_copy (cpu_reg->b, cpu_reg->a, first, last);
			break;
		case 0x0a:  /* a - c -> c[f] */
			reg_sub (cpu_reg->c, cpu_reg->a, cpu_reg->c,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x0b:  /* c - 1 -> c[f] */
			cpu_reg->carry = 1;
			reg_sub (cpu_reg->c, cpu_reg->c, NULL,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x0c:  /* c -> a[f] */
			reg_copy (cpu_reg->a, cpu_reg->c, first, last);
			break;
		case 0x0d:  /* if c[f] = 0 */
			reg_test_nonequal (cpu_reg->c, NULL, first, last, & cpu_reg->carry);
			break;
		case 0x0e:  /* a + c -> c[f] */
			reg_add (cpu_reg->c, cpu_reg->a, cpu_reg->c,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x0f:  /* c + 1 -> c[f] */
			cpu_reg->carry = 1;
			reg_add (cpu_reg->c, cpu_reg->c, NULL,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x10:  /* if a >= b[f] */
			reg_sub (NULL, cpu_reg->a, cpu_reg->b,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x11:  /* b exchange c[f] */
			reg_exch (cpu_reg->b, cpu_reg->c, first, last);
			break;
		case 0x12:  /* shift right c[f] */
			reg_shift_right (cpu_reg->c, first, last);
			break;
		case 0x13:  /* if a[f] >= 1 */
			reg_test_equal (cpu_reg->a, NULL, first, last, & cpu_reg->carry);
			break;
		case 0x14:  /* shift right b[f] */
			reg_shift_right (cpu_reg->b, first, last);
			break;
		case 0x15:  /* c + c -> c[f] */
			reg_add (cpu_reg->c, cpu_reg->c, cpu_reg->c,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x16:  /* shift right a[f] */
			reg_shift_right (cpu_reg->a, first, last);
			break;
		case 0x17:  /* 0 -> a[f] */
			reg_zero (cpu_reg->a, first, last);
			break;
		case 0x18:  /* a - b -> a[f] */
			reg_sub (cpu_reg->a, cpu_reg->a, cpu_reg->b,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x19:  /* a exchange b[f] */
			reg_exch (cpu_reg->a, cpu_reg->b, first, last);
			break;
		case 0x1a:  /* a - c -> a[f] */
			reg_sub (cpu_reg->a, cpu_reg->a, cpu_reg->c,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x1b:  /* a - 1 -> a[f] */
			cpu_reg->carry = 1;
			reg_sub (cpu_reg->a, cpu_reg->a, NULL,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x1c:  /* a + b -> a[f] */
			reg_add (cpu_reg->a, cpu_reg->a, cpu_reg->b,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x1d:  /* a exchange c[f] */
			reg_exch (cpu_reg->a, cpu_reg->c, first, last);
			break;
		case 0x1e:  /* a + c -> a[f] */
			reg_add (cpu_reg->a, cpu_reg->a, cpu_reg->c,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
		case 0x1f:  /* a + 1 -> a[f] */
			cpu_reg->carry = 1;
			reg_add (cpu_reg->a, cpu_reg->a, NULL,
					 first, last,
					 & cpu_reg->carry, arithmetic_base (cpu_reg));
			break;
    }
}


static void op_goto (classic_cpu_reg_t *cpu_reg, int opcode)
{
	if (! cpu_reg->prev_carry)
    {
		cpu_reg->pc = opcode >> 2;
		cpu_reg->rom = cpu_reg->del_rom;
		cpu_reg->group = cpu_reg->del_grp;
    }
}


static void op_jsb (classic_cpu_reg_t *cpu_reg, int opcode)
{
	cpu_reg->ret_pc = cpu_reg->pc;
	cpu_reg->pc = opcode >> 2;
	cpu_reg->rom = cpu_reg->del_rom;
	cpu_reg->group = cpu_reg->del_grp;
}


static void op_return (classic_cpu_reg_t *cpu_reg, int opcode)
{
	cpu_reg->pc = cpu_reg->ret_pc;
}


static void op_nop (classic_cpu_reg_t *cpu_reg, int opcode)
{
}


static void op_dec_p (classic_cpu_reg_t *cpu_reg, int opcode)
{
	cpu_reg->p = (cpu_reg->p - 1) & 0xf;
	/* On the ACT (Woodstock) if P=0 before a decrement, it will be
	13 after.  Apparently the CTC (Classic) does not do this. */
}


static void op_inc_p (classic_cpu_reg_t *cpu_reg, int opcode)
{
	cpu_reg->p = (cpu_reg->p + 1) & 0xf;
	/* On the ACT (Woodstock) if P=13 before an increment, it will be
	0 after.  Apparently the CTC (Classic) does not do this. */
}


static void op_clear_s (classic_cpu_reg_t *cpu_reg, int opcode)
{
	int i;
	
	for (i = 0; i < SSIZE; i++)
		cpu_reg->s [i] = 0;
}


static void op_c_exch_m (classic_cpu_reg_t *cpu_reg, int opcode)
{
	int i, t;
	
	for (i = 0; i < WSIZE; i++)
    {
		t = cpu_reg->c [i];
		cpu_reg->c [i] = cpu_reg->m[i];
		cpu_reg->m [i] = t;
    }
}


static void op_m_to_c (classic_cpu_reg_t *cpu_reg, int opcode)
{
	int i;
	
	for (i = 0; i < WSIZE; i++)
		cpu_reg->c [i] = cpu_reg->m [i];
}


static void op_c_to_addr (classic_cpu_reg_t *cpu_reg, int opcode)
{
	if (cpu_reg->max_ram > 10)
		cpu_reg->ram_addr = cpu_reg->c [12] * 10 + cpu_reg->c [11];
	else
		cpu_reg->ram_addr = cpu_reg->c [12];
	if (cpu_reg->ram_addr >= cpu_reg->max_ram)
		printf ("c -> ram addr: address %d out of range\n", cpu_reg->ram_addr);
}


static void op_c_to_data (classic_cpu_reg_t *cpu_reg, int opcode)
{
	int i;
	
	if (cpu_reg->ram_addr >= cpu_reg->max_ram)
    {
		printf ("c -> data: address %d out of range\n", cpu_reg->ram_addr);
		return;
    }
	for (i = 0; i < WSIZE; i++)
		cpu_reg->ram [cpu_reg->ram_addr] [i] = cpu_reg->c [i];
}


static void op_data_to_c (classic_cpu_reg_t *cpu_reg, int opcode)
{
	int i;
	
	if (cpu_reg->ram_addr >= cpu_reg->max_ram)
    {
		printf ("data -> c: address %d out of range, loading 0\n", cpu_reg->ram_addr);
		for (i = 0; i < WSIZE; i++)
			cpu_reg->c [i] = 0;
		return;
    }
	for (i = 0; i < WSIZE; i++)
		cpu_reg->c [i] = cpu_reg->ram [cpu_reg->ram_addr] [i];
}


static void op_c_to_stack (classic_cpu_reg_t *cpu_reg, int opcode)
{
	int i;
	
	for (i = 0; i < WSIZE; i++)
    {
		cpu_reg->f [i] = cpu_reg->e [i];
		cpu_reg->e [i] = cpu_reg->d [i];
		cpu_reg->d [i] = cpu_reg->c [i];
    }
}


static void op_stack_to_a (classic_cpu_reg_t *cpu_reg, int opcode)
{
	int i;
	
	for (i = 0; i < WSIZE; i++)
    {
		cpu_reg->a [i] = cpu_reg->d [i];
		cpu_reg->d [i] = cpu_reg->e [i];
		cpu_reg->e [i] = cpu_reg->f [i];
    }
}


static void op_down_rotate (classic_cpu_reg_t *cpu_reg, int opcode)
{
	int i, t;
	
	for (i = 0; i < WSIZE; i++)
    {
		t = cpu_reg->c [i];
		cpu_reg->c [i] = cpu_reg->d [i];
		cpu_reg->d [i] = cpu_reg->e [i];
		cpu_reg->e [i] = cpu_reg->f [i];
		cpu_reg->f [i] = t;
    }
}


static void op_clear_reg (classic_cpu_reg_t *cpu_reg, int opcode)
{
	int i;
	
	for (i = 0; i < WSIZE; i++)
		cpu_reg->a [i] = cpu_reg->b [i] = cpu_reg->c [i] = cpu_reg->d [i] =
			cpu_reg->e [i] = cpu_reg->f [i] = cpu_reg->m [i] = 0;
}


static void op_load_constant (classic_cpu_reg_t *cpu_reg, int opcode)
{
	if (cpu_reg->p >= WSIZE)
    {
#if 0 /* HP-45 depends on load constant with p > 13 not affecting C */
		printf ("load constant w/ p >= WSIZE at %02o%03o\n",
				cpu_reg->prev_pc >> 8, cpu_reg->prev_pc & 0377)
		;
#endif
    }
	else if ((opcode >> 6) > 9)
		printf ("load constant > 9\n");
	else
		cpu_reg->c [cpu_reg->p] = opcode >> 6;
	
	cpu_reg->p = (cpu_reg->p - 1) & 0xf;
	/* On the ACT (Woodstock) if P=0 before a load constant, it will be
		13 after.  Apparently the CTC (Classic) does not do this. */
}


static void op_set_s (classic_cpu_reg_t *cpu_reg, int opcode)
{
	if ((opcode >> 6) >= SSIZE)
		printf ("stat >= SSIZE at %02o%03o\n",
				cpu_reg->prev_pc >> 8, cpu_reg->prev_pc & 0377);
	else
		cpu_reg->s [opcode >> 6] = 1;
}


static void op_clr_s (classic_cpu_reg_t *cpu_reg, int opcode)
{
	if ((opcode >> 6) >= SSIZE)
		printf ("stat >= SSIZE at %02o%03o\n",
				cpu_reg->prev_pc >> 8, cpu_reg->prev_pc & 0377);
	else
		cpu_reg->s [opcode >> 6] = 0;
}


static void op_test_s (classic_cpu_reg_t *cpu_reg, int opcode)
{
	if ((opcode >> 6) >= SSIZE)
		printf ("stat >= SSIZE at %02o%03o\n",
				cpu_reg->prev_pc >> 8, cpu_reg->prev_pc & 0377);
	else
		cpu_reg->carry = cpu_reg->s [opcode >> 6];
}


static void op_set_p (classic_cpu_reg_t *cpu_reg, int opcode)
{
	cpu_reg->p = opcode >> 6;
}


static void op_test_p (classic_cpu_reg_t *cpu_reg, int opcode)
{
	cpu_reg->carry = (cpu_reg->p == (opcode >> 6));
}


static void op_sel_rom (classic_cpu_reg_t *cpu_reg, int opcode)
{
	cpu_reg->rom = opcode >> 7;
	cpu_reg->group = cpu_reg->del_grp;
	
	cpu_reg->del_rom = cpu_reg->rom;
}


static void op_del_sel_rom (classic_cpu_reg_t *cpu_reg, int opcode)
{
	cpu_reg->del_rom = opcode >> 7;
}


static void op_del_sel_grp (classic_cpu_reg_t *cpu_reg, int opcode)
{
	cpu_reg->del_grp = (opcode >> 7) & 1;
}


static void op_keys_to_rom_addr (classic_cpu_reg_t *cpu_reg, int opcode)
{
	if (cpu_reg->key_buf < 0)
    {
		printf ("keys->rom address with no key pressed\n");
		cpu_reg->pc = 0;
		return;
    }
	cpu_reg->pc = cpu_reg->key_buf;
}


static void op_rom_addr_to_buf (classic_cpu_reg_t *cpu_reg, int opcode)
{
	/* I don't know what this instruction is supposed to do! */
#if 0
	fprintf (stderr, "rom addr to buf!!!!!!!!!!!!\n");
#endif
}


static void op_display_off (classic_cpu_reg_t *cpu_reg, int opcode)
{
	cpu_reg->display_enable = 0;
}


static void op_display_toggle (classic_cpu_reg_t *cpu_reg, int opcode)
{
	cpu_reg->display_enable = ! cpu_reg->display_enable;
}


static void init_ops (classic_cpu_reg_t *cpu_reg)
{
	int i, j;
	
	for (i = 0; i < 1024; i += 4)
    {
		cpu_reg->op_fcn [i + 0] = bad_op;
		cpu_reg->op_fcn [i + 1] = op_jsb;    /* type 1: aaaaaaaa01 */
		cpu_reg->op_fcn [i + 2] = op_arith;  /* type 2: ooooowww10 */
		cpu_reg->op_fcn [i + 3] = op_goto;   /* type 1: aaaaaaaa11 */
    }
	
	/* type 3 instructions: nnnnff0100*/
	for (i = 0; i <= 15; i ++)
    {
		cpu_reg->op_fcn [0x004 + (i << 6)] = op_set_s;
		cpu_reg->op_fcn [0x014 + (i << 6)] = op_test_s;
		cpu_reg->op_fcn [0x024 + (i << 6)] = op_clr_s;
		cpu_reg->op_fcn [0x034 /* + (i << 6) */ ] = op_clear_s;
    }
	
	/* New instructions in HP-55 and maybe HP-65, wedged into the unused
		port of the type 3 instruction space.  On the HP-35 and HP-80 these
		probably cleared all status like 0x034. */
	for (i = 0; i <= 7; i ++)
    {
		cpu_reg->op_fcn [0x074 + (i << 7)] = op_del_sel_rom;
    }
	cpu_reg->op_fcn [0x234] = op_del_sel_grp;
	cpu_reg->op_fcn [0x2b4] = op_del_sel_grp;
	
	/* type 4 instructions: ppppff1100 */
	for (i = 0; i <= 15; i ++)
    {
		cpu_reg->op_fcn [0x00c + (i << 6)] = op_set_p;
		cpu_reg->op_fcn [0x02c + (i << 6)] = op_test_p;
		cpu_reg->op_fcn [0x01c /* + (i << 6) */ ] = op_dec_p;
		cpu_reg->op_fcn [0x03c /* + (i << 6) */ ] = op_inc_p;
    }
	
	/* type 5 instructions: nnnnff1000 */
	for (i = 0; i <= 9; i++)
		cpu_reg->op_fcn [0x018 + (i << 6)] = op_load_constant;
	for (i = 0; i <= 1; i++)
    {
		cpu_reg->op_fcn [0x028 /* + (i << 4) */ ] = op_display_toggle;
		cpu_reg->op_fcn [0x0a8 /* + (i << 4) */ ] = op_c_exch_m;
		cpu_reg->op_fcn [0x128 /* + (i << 4) */ ] = op_c_to_stack;
		cpu_reg->op_fcn [0x1a8 /* + (i << 4) */ ] = op_stack_to_a;
		cpu_reg->op_fcn [0x228 /* + (i << 4) */ ] = op_display_off;
		cpu_reg->op_fcn [0x2a8 /* + (i << 4) */ ] = op_m_to_c;
		cpu_reg->op_fcn [0x328 /* + (i << 4) */ ] = op_down_rotate;
		cpu_reg->op_fcn [0x3a8 /* + (i << 4) */ ] = op_clear_reg;
		for (j = 0; j <= 3; j++)
		{
#if 0
			cpu_reg->op_fcn [0x068 + (j << 8) + (i << 4)] = op_is_to_a;
#endif
			cpu_reg->op_fcn [0x0e8 + (j << 8) + (i << 4)] = op_data_to_c;
			/* BCD->C is nominally 0x2f8 */
		}
    }
	
	/* type 6 instructions: nnnff10000 */
	for (i = 0; i <= 7; i++)
    {
		cpu_reg->op_fcn [0x010 + (i << 7)] = op_sel_rom;
		cpu_reg->op_fcn [0x030 /* + (i << 7) */ ] = op_return;
		if (i & 1)
			cpu_reg->op_fcn [0x050 + 0x080 /* + (i << 7) */ ] = op_keys_to_rom_addr;
#if 0
		else
			cpu_reg->op_fcn [0x050 /* + (i << 7) */ ] = op_external_entry;
#endif
    }
	cpu_reg->op_fcn [0x270] = op_c_to_addr;  /* also 0x370 */
	cpu_reg->op_fcn [0x2f0] = op_c_to_data;
	
	/* no type 7 or type 8 instructions: xxxx100000, xxx1000000 */
	
	/* type 9 and 10 instructions: xxx0000000 */
	cpu_reg->op_fcn [0x200] = op_rom_addr_to_buf;
	cpu_reg->op_fcn [0x000] = op_nop;
}


/*static void classic_disassemble (classic_cpu_reg_t *cpu_reg, int addr, char *buf, int len)
{
	int l;
	
	l = snprintf (buf, len, "%02o%03o: ", addr >> 8, addr & 0377);
	buf += l;
	len -= l;
	if (len <= 0)
		return;
	
	l = snprintf (buf, len, "%04o", cpu_reg->ucode [addr]);
	buf += l;
	len -= l;
	if (len <= 0)
		return;
	
	return;
}*/
void display_callback(classic_cpu_reg_t *cpu_reg);
static void classic_display_scan (classic_cpu_reg_t *cpu_reg)
{
	int a = cpu_reg->a [cpu_reg->display_scan_position];
	int b = cpu_reg->b [cpu_reg->display_scan_position];
	
	if (cpu_reg->display_digit_position < MAX_DIGIT_POSITION)
    {
		cpu_reg->display_segments [cpu_reg->display_digit_position] = 0;  // blank
		
		if (cpu_reg->display_enable && (b <= 7))
		{
			if ((cpu_reg->display_scan_position == 2) ||
				(cpu_reg->display_scan_position == 13))
			{
				if (a >= 8)
				{
					cpu_reg->display_segments [cpu_reg->display_digit_position] = cpu_reg->char_gen ['-'];
				}
			}
			else
			{
				cpu_reg->display_segments [cpu_reg->display_digit_position] = cpu_reg->char_gen ['0' + a];
			}
			if (b == 2)
			{
				if ((cpu_reg->display_digit_position) < MAX_DIGIT_POSITION)
				{
					cpu_reg->display_segments [cpu_reg->display_digit_position] |= cpu_reg->char_gen ['.'];
				}
			}
		}
    }
	
	cpu_reg->display_digit_position++;
	
	if ((--cpu_reg->display_scan_position) < cpu_reg->right_scan)
    {
		while (cpu_reg->display_digit_position < MAX_DIGIT_POSITION)
			cpu_reg->display_segments [cpu_reg->display_digit_position++] = 0;
		
		//print_display(cpu_reg);
		//cpu_reg->need_redraw = true;
		display_callback(cpu_reg);
		
		cpu_reg->display_digit_position = 0;
		cpu_reg->display_scan_position = cpu_reg->left_scan;
    }
}


/*static void print_reg (char *label, reg_t reg)
{
	int i;
	printf ("%s", label);
	for (i = WSIZE - 1; i >= 0; i--)
		printf ("%x", reg [i]);
	printf ("\n");
}*/

/*static void classic_print_state (classic_cpu_reg_t *cpu_reg)
{
	  int i;
	printf ("pc=%05o  p=%d  stat:",
			(cpu_reg->group << 12) + (cpu_reg->rom << 9) + (cpu_reg->pc),
			cpu_reg->p);
	for (i = 0; i < SSIZE; i++)
    if (cpu_reg->s [i])
	printf (" %d", i);
	printf ("\n");
	print_reg ("a: ", cpu_reg->a);
	print_reg ("b: ", cpu_reg->b);
	print_reg ("c: ", cpu_reg->c);
	print_reg ("m: ", cpu_reg->m);
	
	if (cpu_reg->sim->source [cpu_reg->prev_pc])
    printf ("%s\n", cpu_reg->sim->source [cpu_reg->prev_pc]);
	else
{
		char buf [80];
		classic_disassemble (cpu_reg, cpu_reg->prev_pc, buf, sizeof (buf));
		printf ("%s\n", buf);
}
}*/


bool classic_execute_instruction (classic_cpu_reg_t *cpu_reg)
{
	int addr;
	int i;
	int opcode;
	
	addr = (cpu_reg->group << 11) | (cpu_reg->rom << 8) | cpu_reg->pc;
	cpu_reg->prev_pc = addr;
	opcode = cpu_reg->ucode [addr];
	
#ifdef HAS_DEBUGGER
	if (sim->debug_flags & (1 << SIM_DEBUG_KEY_TRACE))
    {
		if (opcode == 00320)  // keys to rom addr
			cpu_reg->sim->debug_flags |= (1 << SIM_DEBUG_TRACE);
		else if (opcode == 00024) // if s0 # 1
			cpu_reg->sim->debug_flags &= ~ (1 << SIM_DEBUG_TRACE);
    }
	
	if (cpu_reg->sim->debug_flags & (1 << SIM_DEBUG_TRACE))
		classic_print_state (cpu_reg->sim);
#endif /* HAS_DEBUGGER */
	
	cpu_reg->prev_carry = cpu_reg->carry;
	cpu_reg->carry = 0;
	
	if (cpu_reg->key_flag)
		cpu_reg->s [0] = 1;
	for (i = 0; i < SSIZE; i++)
		if (cpu_reg->ext_flag [i])
			cpu_reg->s [i] = 1;
	
	cpu_reg->pc++;
	(* cpu_reg->op_fcn [opcode]) (cpu_reg, opcode);
	cpu_reg->cycle_count++;
	
	cpu_reg->display_scan_fn (cpu_reg);
	//classic_display_scan(cpu_reg);
	
	return (true);  /* never sleeps */
}


static bool parse_octal (char *oct, int digits, int *val)
{
	*val = 0;
	
	while (digits--)
    {
		if (((*oct) < '0') || ((*oct) > '7'))
			return (false);
		(*val) = ((*val) << 3) + ((*(oct++)) - '0');
    }
	return (true);
}


bool classic_parse_object_line (char *buf, int *bank, int *addr,
								rom_word_t *opcode)
{
	int a, o;
	
	if (buf [0] == '#')  /* comment? */
		return (false);
		
		if (strlen (buf) != 9)
		return (false);
		
		if (buf [4] != ':')
		{
			fprintf (stderr, "invalid object file format\n");
			return (false);
		}
		
		if (! parse_octal (& buf [0], 4, & a))
		{
			fprintf (stderr, "invalid address %o\n", a);
			return (false);
		}
		
		if (! parse_octal (& buf [5], 4, & o))
		{
			fprintf (stderr, "invalid opcode %o\n", o);
			return (false);
		}
		
		*bank = 0;
		*addr = a;
		*opcode = o;
		return (true);
}


/*static int parse_address (char *oct, int *g, int *r, int *p)
{
	return (sscanf (oct, "%1o%1o%3o", g, r, p) == 3);
}*/


/*static int parse_opcode (char *bin, int *opcode)
{
	int i;
	
	*opcode = 0;
	for (i = 0; i < 10; i++)
    {
		(*opcode) <<= 1;
		if (*bin == '1')
			(*opcode) += 1;
		else if (*bin == '.')
			(*opcode) += 0;
		else
			return (0);
		*bin++;
    }
	return (1);
}*/


/*static bool classic_parse_listing_line (char *buf, int *bank, int *addr,
										rom_word_t *opcode)
{
	int g, r, p, o;
	
	if ((strlen (buf) < 25) || (buf [7] != 'L') || (buf [13] != ':'))
		return (false);
	
	if (! parse_address (& buf [8], &g, &r, &p))
    {
		fprintf (stderr, "bad address format\n");
		return (false);
    }
	
	if ((g >= MAX_GROUP) || (r >= MAX_ROM) || (p >= ROM_SIZE))
    {
		fprintf (stderr, "bad address: group %o rom %o addr %03o\n", g, r, p);
		return (false);
    }
	
	if (! parse_opcode (& buf [16], & o))
    {
		fprintf (stderr, "bad opcode\n");
		return (false);
    }
	
	*bank = 0;
	*addr = (g << 11) + (r << 8) + p;
	*opcode = o;
	return (true);
}*/


void classic_press_key (classic_cpu_reg_t *cpu_reg, int keycode)
{
	cpu_reg->key_buf = keycode;
	cpu_reg->key_flag = true;
}

void classic_release_key (classic_cpu_reg_t *cpu_reg)
{
	cpu_reg->key_flag = false;
}

void classic_set_ext_flag (classic_cpu_reg_t *cpu_reg, int flag, bool state)
{
	cpu_reg->ext_flag [flag] = state;
}


static void classic_reset (classic_cpu_reg_t *cpu_reg)
{
	cpu_reg->cycle_count = 0;
	
	cpu_reg->pc = 0;
	cpu_reg->rom = 0;
	cpu_reg->group = 0;
	cpu_reg->del_rom = 0;
	cpu_reg->del_grp = 0;
	
	op_clear_reg (cpu_reg, 0);
	op_clear_s (cpu_reg, 0);
	cpu_reg->p = 0;
	
	cpu_reg->display_enable = 0;
	cpu_reg->display_digit_position = 0;
	cpu_reg->display_scan_position = cpu_reg->left_scan;
	
	cpu_reg->key_flag = 0;
}


static void classic_clear_memory (classic_cpu_reg_t *cpu_reg)
{
	int addr;
	
	for (addr = 0; addr < cpu_reg->max_ram; addr++)
		reg_zero ((digit_t *)cpu_reg->ram [addr], 0, WSIZE - 1);
}


/*static bool classic_read_ram (classic_cpu_reg_t *cpu_reg, int addr, uint64_t *val)
{
	uint64_t data = 0;
	int i;
	bool status;
	
	if (addr >= cpu_reg->max_ram)
    {
		status = false;
		printf ("classic_read_ram: address %d out of range\n", addr);
    }
	else
    {
		// pack cpu_reg->ram [addr] into data
		for (i = WSIZE - 1; i >= 0; i--)
		{
			data <<= 4;
			data += cpu_reg->ram [addr] [i];
		}
		status = true;
    }
	
	*val = data;
	
	return status;
}*/


/*static bool classic_write_ram (classic_cpu_reg_t *cpu_reg, int addr, uint64_t *val)
{
	uint64_t data;
	int i;
	
	if (addr >= cpu_reg->max_ram)
    {
		printf ("classic_write_ram: address %d out of range\n", addr);
		return false;
    }
	
	data = *val;
	
	// now unpack data into cpu_reg->ram [addr]
	for (i = 0; i <= WSIZE; i++)
    {
		cpu_reg->ram [addr] [i] = data & 0x0f;
		data >>= 4;
    }
	
	return true;
}*/


static void classic_new_rom_addr_space (classic_cpu_reg_t *cpu_reg,
										int max_bank,
										int max_page,
										int page_size)
{
	size_t max_words;
	
	max_words = max_bank * max_page * page_size;
	
	cpu_reg->ucode = alloc (max_words * sizeof (rom_word_t));
	cpu_reg->rom_exists = alloc (max_words * sizeof (bool));
	cpu_reg->rom_breakpoint = alloc (max_words * sizeof (bool));
}


static void classic_new_ram_addr_space (classic_cpu_reg_t *cpu_reg, int max_ram)
{
	cpu_reg->max_ram = max_ram;
	cpu_reg->ram = alloc (max_ram * sizeof (reg_t));
}


classic_cpu_reg_t *classic_new_processor (int ram_size, void *display)
{
	classic_cpu_reg_t *cpu_reg;
	static segment_bitmap_t char_gen[] = 
	{   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  64, 128,   0,
		63,  6,  91,  79, 102, 109, 125,   7, 127, 111,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 };	
	
	cpu_reg = alloc (sizeof (classic_cpu_reg_t));
	
	//  install_chip (sim, & classic_cpu_chip_detail, cpu_reg);
	
	classic_new_rom_addr_space (cpu_reg, MAX_BANK, MAX_PAGE, N_PAGE_SIZE);
	classic_new_ram_addr_space (cpu_reg, ram_size);
	
	// RAM is contiguous starting from address 0.
	cpu_reg->max_ram = ram_size;
	cpu_reg->ram = alloc (ram_size * sizeof (reg_t));
	
	cpu_reg->display_digits = 14; // MAX_DIGIT_POSITION; // Classic calcs use 14 digit
	cpu_reg->display_scan_fn = classic_display_scan;
	cpu_reg->left_scan = WSIZE - 1;
	cpu_reg->right_scan = 0;
	//cpu_reg->need_redraw = false;
	
	cpu_reg->char_gen = char_gen;
	
	cpu_reg->display = display;
	init_ops (cpu_reg);
	
	//chip_event (sim, event_reset, NULL, 0, NULL);
	classic_event_fn(cpu_reg, event_reset);
	
	return cpu_reg;
}


/*static void classic_free_processor (classic_cpu_reg_t *cpu_reg)
{
	//  remove_chip (cpu_reg->sim->first_chip);
}*/


/*static void classic_event_fn (classic_cpu_reg_t *cpu_reg,
chip_t *chip,
int    event,
int    arg,
void   *data){*/
static void classic_event_fn (classic_cpu_reg_t *cpu_reg, int event)
{
	switch (event)
    {
		case event_reset:
			classic_reset (cpu_reg);
			break;
		case event_clear_memory:
			classic_clear_memory (cpu_reg);
			break;
		default:
			// warning ("proc_classic: unknown event %d\n", event);
			break;
    }
}


bool classic_read_object_file (classic_cpu_reg_t *cpu_reg, const char *fn)
{
	FILE *f;
	int bank;
	int addr;  // should change to addr_t, but will have to change
			   // the parse function profiles to match.
	rom_word_t opcode;
	int count = 0;
	char buf [80];
	char magic [4];
	bool eof, error;
	
	f = fopen (fn, "rb");
	if (! f)
    {
		fprintf (stderr, "error opening object file\n");
		return (false);
    }
	
	if (fread_bytes (f, magic, sizeof (magic), & eof, & error) != sizeof (magic))
    {
		fprintf (stderr, "error reading object file\n");
		return (false);
    }
	
	//if (strncmp (magic, "MOD1", sizeof (magic)) == 0)
	//  return sim_read_mod1_file (sim, f);
	
	// switch from binary to text mode, and rewind
	f = freopen (fn, "r", f);
	
	if (! f)
    {
		fprintf (stderr, "error reopening object file\n");
		return (false);
    }
	
	while (fgets (buf, sizeof (buf), f))
    {
		trim_trailing_whitespace (buf);
		if (! buf [0])
			continue;
		if (classic_parse_object_line (buf, & bank, & addr, & opcode))
		{
			if (! classic_write_rom (cpu_reg, bank, addr, & opcode))
				fatal (3, "can't load ROM word at bank %d address %o\n", bank, addr);
			count++;
		}
    }
	
#if 0
	fprintf (stderr, "read %d words from '%s'\n", count, fn);
#endif
	return (true);
}

