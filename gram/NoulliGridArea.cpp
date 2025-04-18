/* NoulliGridArea.cpp
 *
 * Copyright (C) 2018-2022 Paul Boersma
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

#include "NoulliGridArea.h"
#include "EditorM.h"

Thing_implement (NoulliGridArea, FunctionArea, 0);

void structNoulliGridArea :: v_drawInside () {
	NoulliGrid_paintInside (our noulliGrid(), our graphics(), our startWindow(), our endWindow());
}

/* End of file NoulliGridArea.cpp */
