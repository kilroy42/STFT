#!/usr/bin/env python
# -*- coding: utf-8 -*-

import cairo
import pango
import pangocairo
import sys
import argparse


def RLE(data):

	shift = 4		# throw away 4 data bits

	mask = (1<<(8-shift))-1

	res = []
	last = None
	cnt = 0

	for c in data:

		if c == last and cnt<mask+1:

			cnt+= 1

		else:

			if last is not None:
				res.append((last>>shift) | ((cnt-1) << (8 - shift)))
			cnt = 1
			last = c

	res.append((c>>shift) | ((cnt-1) << (8 - shift)))

	return res

def reduce2bits(data, shift=4, bits=2):

	res = []

	idx = 0
	e = 0

	for c in data:
		c>>= shift

		e = (e<<shift) | c

		if idx == bits-1:
			res.append(e)
			idx = 0
			e = 0
		else:
			idx+= 1

	if idx != 0:
		res.append(e)

#	print len(data), data
#	print len(res), res

	return res

"""
Fonts/TTF/


AGENCYR.TTF
altgot2n.ttf
arlrdbd.ttf
bengothb.ttf
BROWA.TTF
CNTR65W.TTF
CORDIAUB.TTF


D...Z noch nicht geguckt
"""

parser = argparse.ArgumentParser(description=u'Generate C include files from TTF files', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-s', '--size', type=int, help='Size of font', default=12)
parser.add_argument('-a', '--algorithm', help='Algorithm', default='packed4', choices=('packed4', 'rle', 'unpacked'))
parser.add_argument('-f', '--firstchar', help='First char', default=33, type=int)
parser.add_argument('-l', '--lastchar', help='Last char', default=126, type=int)
parser.add_argument('-e', '--extrachars', help='Extra chars')
parser.add_argument('fontname', metavar='fontname', help='TTF file')
args = parser.parse_args()

start = args.firstchar
end = args.lastchar

surf = cairo.ImageSurface(cairo.FORMAT_ARGB32, 128, 128)
context = cairo.Context(surf)

#draw a background rectangle:
context.rectangle(0,0,320,120)
context.set_source_rgb(1, 1, 1)
context.fill()

#get font families:

font_map = pangocairo.cairo_font_map_get_default()
families = font_map.list_families()

# to see family names:
#print [f.get_name() for f in   font_map.list_families()]

#context.set_antialias(cairo.ANTIALIAS_SUBPIXEL)

# Positions drawing origin so that the text desired top-let corner is at 0,0
context.translate(0, 0)

pangocairo_context = pangocairo.CairoContext(context)
pangocairo_context.set_antialias(cairo.ANTIALIAS_SUBPIXEL)

layout = pangocairo_context.create_layout()
font = pango.FontDescription(args.fontname + ' ' + str(args.size))
layout.set_font_description(font)

name = args.fontname.lower().rstrip('.ttf').capitalize()

allChars = u''.join([chr(c) for c in range(start, end+1)]) + (args.extrachars or u'')

size = 0

offsets = []
font = []
maxHeight = 0
maxWidth = 0
allMinY = 1000

for c in allChars:
	context.rectangle(0,0,128,128)
	context.set_source_rgb(1, 1, 1)
	context.fill()

	layout.set_text(c)
	context.set_source_rgb(0, 0, 0)
	pangocairo_context.update_layout(layout)
	pangocairo_context.show_layout(layout)

	res = layout.get_pixel_extents()
	(width, height) = res[1][2:]

	data = surf.get_data()

	usedX = set()
	usedY = set()

	stream = []

	for y in range(0, height):
		for x in range(0, width):
			r = 255 - ord(data[128*4 * y + x*4])

			if r!=0:
				usedX.add(x)
				usedY.add(y)

			if r == 0:
				p = ' '
			elif r<64:
				p = '.'
			elif r<128:
				p = '-'
			elif r<192:
				p = 'x'
			elif r<256:
				p = 'X'

	offsetX = min(usedX)
	offsetY = min(usedY)
	sizeX = max(usedX) - min(usedX) + 1
	sizeY = max(usedY) - min(usedY) + 1

	maxHeight = max(maxHeight, max(usedY)+1)
	maxWidth = max(maxWidth, sizeX)

	allMinY = min(allMinY, min(usedY))

	for y in range(offsetY, offsetY + sizeY):
		for x in range(offsetX, offsetX + sizeX):
			r = 255 - ord(data[128*4 * y + x*4])

			stream.append(r)

	if args.algorithm == 'packed4':
		stream = reduce2bits(stream, 4, 2)
	elif args.algorithm == 'rle':
		stream = RLE(stream)
	elif args.algorithm == 'unpacked':
		pass

	size+= 2 + 1 + len(stream)

	offsets.append(sum([len(p)-1 for p in font]))
	font.append([c, sizeX, sizeY, offsetY] + stream)

offsets.append(sum([len(p)-1 for p in font]))

print """#define font%(name)s%(size)sSize          %(size)i
#define font%(name)s%(size)sFirstChar     %(start)i
#define font%(name)s%(size)sHeight        %(height)i
#define font%(name)s%(size)sMaxWidth		%(maxWidth)i
""" % {
	'name':				name,
	'size':				args.size,
	'start':			start,
	'height':			maxHeight - allMinY,
	'maxWidth':			maxWidth,
}

print 'uint16_t font'+name+str(args.size)+'Offsets[] PROGMEM = { ' + ', '.join([str(n) for n in offsets]) + ' }; // last value = size of font data'

print 'uint8_t  font'+name+str(args.size)+'Data[]    PROGMEM = {'
for part in font:
	part[3]-= allMinY
	print ', '.join(['%3i' % (n,) for n in part[1:]]) + ',', '/*', part[0], '*/'
print '};'

