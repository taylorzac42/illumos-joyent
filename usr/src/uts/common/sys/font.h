/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright 2006 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _SYS_FONT_H
#define	_SYS_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

enum vfnt_map {
	VFNT_MAP_NORMAL = 0,	/* Normal font. */
	VFNT_MAP_NORMAL_RH,	/* Normal font right hand. */
	VFNT_MAP_BOLD,		/* Bold font. */
	VFNT_MAP_BOLD_RH,	/* Bold font right hand. */
	VFNT_MAPS		/* Number of maps. */
};

struct font_map {
	uint32_t font_src;	/* Source glyph. */
	uint16_t font_dst;	/* Target glyph. */
	uint16_t font_len;	/* The number of glyphs in sequence. */
};

/* Any unknown glyph is mapped as first (offset 0) glyph in bitmap. */
struct font {
	struct font_map	*vf_map[VFNT_MAPS];	/* Mapping tables. */
	uint8_t		*vf_bytes;		/* Font bitmap data. */
	uint32_t	vf_width;		/* Glyph width. */
	uint32_t	vf_height;		/* Glyph height. */
	uint32_t	vf_map_count[VFNT_MAPS];	/* Entries in map */
};

typedef	struct  bitmap_data {
	short		width;
	short		height;
	uint32_t	compressed_size;
	uint32_t	uncompressed_size;
	uint8_t		*compressed_data;
	struct font	*font;
} bitmap_data_t;

struct fontlist {
	bitmap_data_t	*data;
	bitmap_data_t   *(*fontload)(char *);
};

#define	FONT_HEADER_MAGIC	"VFNT0002"
struct font_header {
	uint8_t		fh_magic[8];
	uint8_t		fh_width;
	uint8_t		fh_height;
	uint16_t	fh_pad;
	uint32_t	fh_glyph_count;
	uint32_t	fh_map_count[4];
} __attribute__((__packed__));

extern struct fontlist fonts[];

#define	DEFAULT_FONT_DATA	font_data_10x18
#define	BORDER_PIXELS		10	/* space from screen border */
/*
 * Built in fonts.
extern bitmap_data_t font_data_12x22;
extern bitmap_data_t font_data_8x16;
extern bitmap_data_t font_data_7x14;
extern bitmap_data_t font_data_6x10;
 */
extern bitmap_data_t font_data_10x18;

bitmap_data_t *set_font(short *, short *, short, short);
void font_bit_to_pix4(struct font *, uint8_t *, uint32_t, uint8_t, uint8_t);
void font_bit_to_pix8(struct font *, uint8_t *, uint32_t, uint8_t, uint8_t);
void font_bit_to_pix16(struct font *, uint16_t *, uint32_t, uint16_t, uint16_t);
void font_bit_to_pix24(struct font *, uint8_t *, uint32_t, uint32_t, uint32_t);
void font_bit_to_pix32(struct font *, uint32_t *, uint32_t, uint32_t, uint32_t);

#ifdef __cplusplus
}
#endif

#endif /* !_SYS_FONT_H */
