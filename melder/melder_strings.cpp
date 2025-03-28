/* melder_strings.cpp
 *
 * Copyright (C) 2006-2012,2014-2022 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "melder.h"
#include "../kar/UnicodeData.h"
#define FREE_THRESHOLD_BYTES 10000LL

static int64 totalNumberOfAllocations = 0, totalNumberOfDeallocations = 0, totalAllocationSize = 0, totalDeallocationSize = 0;

void MelderString16_free (MelderString16 *me) {
	if (! my string) {
		Melder_assert (my bufferSize == 0);   // check class invariant for analyzer
		return;
	}
	Melder_free (my string);
	if (Melder_debug == 34)
		Melder_casual (U"from MelderString_free\t", Melder_pointer (my string), U"\t", my bufferSize, U"\t", sizeof (char16));
	totalNumberOfDeallocations += 1;
	totalDeallocationSize += my bufferSize * (int64) sizeof (char16);
	my bufferSize = 0;
	my length = 0;
}

void MelderString_free (MelderString *me) {
	if (! my string) {
		Melder_assert (my bufferSize == 0);   // check class invariant for analyzer
		return;
	}
	Melder_free (my string);
	if (Melder_debug == 34)
		Melder_casual (U"from MelderString_free\t", Melder_pointer (my string), U"\t", my bufferSize, U"\t", sizeof (char32));
	totalNumberOfDeallocations += 1;
	totalDeallocationSize += my bufferSize * (int64) sizeof (char32);
	my bufferSize = 0;
	my length = 0;
}

template <typename STRING_TYPE, typename CHARACTER_TYPE>
inline static void MelderString_expand_ (STRING_TYPE *me, int64 sizeNeeded) {
	Melder_assert (my bufferSize >= 0);
	Melder_assert (sizeNeeded >= 0);
	sizeNeeded = (int64) (2.0 /*1.618034*/ * sizeNeeded) + 100;
	Melder_assert (sizeNeeded > 0);
	if (my string) {
		totalNumberOfDeallocations += 1;
		totalDeallocationSize += my bufferSize * (int64) sizeof (CHARACTER_TYPE);
	}
	int64 bytesNeeded = sizeNeeded * (integer) sizeof (CHARACTER_TYPE);
	Melder_assert (bytesNeeded > 0);
	try {
		if (Melder_debug == 34)
			Melder_casual (U"from MelderString_expand\t", Melder_pointer (my string), U"\t", sizeNeeded, U"\t", sizeof (CHARACTER_TYPE));
		my string = (CHARACTER_TYPE *) Melder_realloc (my string, bytesNeeded);
		my bufferSize = sizeNeeded;
	} catch (MelderError) {
		// my string is still valid, because realloc doesn't free on error
		// my bufferSize is also still valid
		my length = 0;
		throw;
	}
	totalNumberOfAllocations += 1;
	totalAllocationSize += bytesNeeded;
}

void MelderString_expand (MelderString *me, int64 sizeNeeded) {
	MelderString_expand_ <MelderString, char32> (me, sizeNeeded);
}

void MelderString16_empty (MelderString16 *me) {
	if (my bufferSize * (int64) sizeof (char16) >= FREE_THRESHOLD_BYTES)
		MelderString16_free (me);
	const int64 sizeNeeded = 1;
	if (sizeNeeded > my bufferSize)
		MelderString_expand_ <MelderString16, char16> (me, sizeNeeded);
	my string [0] = u'\0';
	my length = 0;
}

void MelderString_empty (MelderString *me) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES)
		MelderString_free (me);
	const int64 sizeNeeded = 1;
	if (sizeNeeded > my bufferSize)
		MelderString_expand_ <MelderString, char32> (me, sizeNeeded);
	my string [0] = U'\0';
	my length = 0;
}

void MelderString_ncopy (MelderString *me, conststring32 source, int64 n) {
	if (my bufferSize * (int64) sizeof (char32) >= FREE_THRESHOLD_BYTES)
		MelderString_free (me);
	if (! source)
		source = U"";
	int64 length = Melder_length (source);
	if (length > n)
		length = n;
	const int64 sizeNeeded = length + 1;
	Melder_assert (sizeNeeded > 0);
	if (sizeNeeded > my bufferSize)
		MelderString_expand_ <MelderString, char32> (me, sizeNeeded);
	str32ncpy (my string, source, length);
	my string [length] = U'\0';
	my length = length;
}

void MelderString16_appendCharacter (MelderString16 *me, char32 kar) {
	const int64 sizeNeeded = my length + 3;   // make room for character, potential surrogate character, and null character
	if (sizeNeeded > my bufferSize)
		MelderString_expand_ <MelderString16, char16> (me, sizeNeeded);
	if (kar <= 0x00'FFFF) {
		my string [my length] = (char16) kar;   // guarded cast
		my length ++;
	} else if (kar <= 0x10'FFFF) {
		kar -= 0x01'0000;
		my string [my length] = (char16) (0x00'D800 | (kar >> 10));
		my length ++;
		my string [my length] = (char16) (0x00'DC00 | (kar & 0x00'03FF));
		my length ++;
	} else {
		my string [my length] = UNICODE_REPLACEMENT_CHARACTER;
		my length ++;
	}
	my string [my length] = u'\0';
}

void MelderString_appendCharacter (MelderString *me, char32 character) {
	const int64 sizeNeeded = my length + 2;   // make room for character and null character
	if (sizeNeeded > my bufferSize)
		MelderString_expand_ <MelderString, char32> (me, sizeNeeded);
	my string [my length] = character;
	my length ++;
	my string [my length] = U'\0';
}

void MelderString_get (MelderString *me, char32 *destination) {
	if (my string)
		str32cpy (destination, my string);
	else
		destination [0] = U'\0';
}

int64 MelderString_allocationCount () {
	return totalNumberOfAllocations;
}

int64 MelderString_deallocationCount () {
	return totalNumberOfDeallocations;
}

int64 MelderString_allocationSize () {
	return totalAllocationSize;
}

int64 MelderString_deallocationSize () {
	return totalDeallocationSize;
}

MelderString MelderCat::_buffers [MelderCat::_k_NUMBER_OF_BUFFERS] { };
int MelderCat::_bufferNumber = 0;

/* End of file melder_strings.cpp */
