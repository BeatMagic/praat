/* praat_FFNet_init.cpp
 *
 * Copyright (C) 1994-2021 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20020408 GPL
 djmw 20020408 added FFNet_help
 djmw 20030701 Removed non-GPL minimizations
 djmw 20040526 Removed bug in FFNet_drawCostHistory interface.
 djmw 20041123 Latest modification
 djmw 20061218 Changed to Melder_information<x> format.
 djmw 20080902 Melder_error<1...>
 djmw 20071011 REQUIRE requires U"".
 djmw 20071024 Use MelderString_append in FFNet_createNameFromTopology
 djmw 20100511 FFNet query outputs
*/

#include "Discriminant.h"
#include "PCA.h"
#include "Minimizers.h"
#include "FFNet_Eigen.h"
#include "FFNet_Matrix.h"
#include "FFNet_PatternList.h"
#include "FFNet_ActivationList_Categories.h"
#include "FFNet_PatternList_ActivationList.h"
#include "FFNet_PatternList_Categories.h"

#include "praat_FFNet.h"

/* Routines to be removed sometime in the future:
20040422, 2.4.04: FFNet_drawWeightsToLayer  use FFNet_drawWeights
20040422, 2.4.04: FFNet_weightsToMatrix use FFNet_extractWeights
*/

static char32 const *QUERY_BUTTON   = U"Query -";
static char32 const *DRAW_BUTTON     = U"Draw -";
static char32 const *MODIFY_BUTTON  = U"Modify -";
static char32 const *EXTRACT_BUTTON = U"Extract -";

/**************** New FFNet ***************************/

FORM (CREATE_ONE__FFNet_create, U"Create FFNet", U"Create FFNet...") {
	WORD (name, U"Name", U"4-3")
	NATURAL (numberOfInputs, U"Number of inputs", U"4")
	NATURAL (numberOfOutputs, U"Number of outputs", U"3")
	INTEGER (numberOfUnits1, U"Number of units in hidden layer 1", U"0")
	INTEGER (numberOfUnits2, U"Number of units in hidden layer 2", U"0")
	OK
DO
	CREATE_ONE
		autoFFNet result = FFNet_create (numberOfInputs, numberOfUnits1, numberOfUnits2, numberOfOutputs, false);
	CREATE_ONE_END (name);
}

FORM (CREATE_MULTIPLE__FFNet_createIrisExample, U"Create iris example", U"Create iris example...") {
	LABEL (U"For the feedforward neural net we need to know the:")
	INTEGER (numberOfUnits1, U"Number of units in hidden layer 1", U"0")
	INTEGER (numberOfUnits2, U"Number of units in hidden layer 2", U"0")
	OK
DO
	CREATE_MULTIPLE
		autoCollection result = FFNet_createIrisExample (numberOfUnits1, numberOfUnits2);
		praat_new (result.move());
	CREATE_MULTIPLE_END
}

FORM (CREATE_ONE__FFNet_create_linearOutputs, U"Create FFNet", U"Create FFNet (linear outputs)...") {
	WORD (name, U"Name", U"4-3")
	NATURAL (numberOfInputs, U"Number of inputs", U"4")
	NATURAL (numberOfOutputs, U"Number of outputs", U"3")
	INTEGER (numberOfUnits1, U"Number of units in hidden layer 1", U"0")
	INTEGER (numberOfUnits2, U"Number of units in hidden layer 2", U"0")
	OK
DO
	CREATE_ONE
		autoFFNet result = FFNet_create (numberOfInputs, numberOfUnits1, numberOfUnits2, numberOfOutputs, true);
	CREATE_ONE_END (name);
}

/**************** New PatternList ***************************/

FORM (CREATE_ONE__PatternList_create, U"Create PatternList", nullptr) {
	WORD (name, U"Name", U"1x1")
	NATURAL (patternDimension, U"Dimension of a pattern", U"1")
	NATURAL (numberOfPatterns, U"Number of patterns", U"1")
	OK
DO
	CREATE_ONE
		autoPatternList result = PatternList_create (numberOfPatterns, patternDimension);
	CREATE_ONE_END (name);
}

/**************** New Categories ***************************/
	
FORM (CREATE_ONE__Categories_create, U"Create Categories", nullptr) {
	WORD (name, U"Name", U"empty")
	OK
DO
	CREATE_ONE
		autoCategories result = Categories_create ();
	CREATE_ONE_END (name);
}

DIRECT (HELP__FFNet_help)  {
	HELP (U"Feedforward neural networks")
}
		

DIRECT (GRAPHICS_EACH__FFNet_drawTopology) {
	GRAPHICS_EACH (FFNet)
		FFNet_drawTopology (me, GRAPHICS);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FFNet_drawWeights, U"FFNet: Draw weights", U"FFNet: Draw weights...") {
	NATURAL (layer, U"Layer number", U"1")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FFNet)
		FFNet_drawWeights (me, GRAPHICS, layer, garnish);
	GRAPHICS_EACH_END
}
	
FORM (GRAPHICS_EACH__FFNet_drawWeightsToLayer, U"FFNet: Draw weights to layer", nullptr) {
	LABEL (U"Warning: Disapproved. Use \"Draw weights..\" instead.")
	NATURAL (layer, U"Layer number", U"1")
	RADIO (scale, U"Scale", 1)
		RADIOBUTTON (U"by maximum of all weights to layer")
		RADIOBUTTON (U"by maximum weight from 'from-unit'")
		RADIOBUTTON (U"by maximum weight to 'to-unit'")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FFNet)
		FFNet_drawWeightsToLayer (me, GRAPHICS, layer, scale, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__FFNet_drawCostHistory, U"FFNet: Draw cost history", U"FFNet: Draw cost history...") {
	INTEGER (fromIteration, U"left Iteration_range", U"0")
	INTEGER (toIteration, U"right Iteration_range", U"0")
	REAL (fromCost, U"left Cost_range", U"0.0")
	REAL (toCost, U"right Cost_range", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FFNet)
		FFNet_drawCostHistory (me, GRAPHICS, fromIteration, toIteration, fromCost, toCost, garnish);
	GRAPHICS_EACH_END
}

DIRECT (QUERY_ONE_FOR_INTEGER__FFNet_getNumberOfLayers) {
	QUERY_ONE_FOR_INTEGER (FFNet)
		integer result = my numberOfLayers;
	QUERY_ONE_FOR_INTEGER_END (U" layer", (my numberOfLayers > 1 ? U"s" : U""))
}

DIRECT (QUERY_ONE_FOR_INTEGER__FFNet_getNumberOfOutputs) {
	QUERY_ONE_FOR_INTEGER (FFNet)
		integer result = my numberOfUnitsInLayer [my numberOfLayers];
	QUERY_ONE_FOR_INTEGER_END (U" units")
}

FORM (QUERY_ONE_FOR_INTEGER__FFNet_getNumberOfHiddenUnits, U"FFNet: Get number of hidden units", U"FFNet: Get number of hidden units...") {
	NATURAL (layer, U"Hidden layer number", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (FFNet)
		integer result = layer > 0 && layer <= my numberOfLayers - 1 ? my numberOfUnitsInLayer [layer] : 0;
	QUERY_ONE_FOR_INTEGER_END (U" units")
}

DIRECT (QUERY_ONE_FOR_INTEGER__FFNet_getNumberOfInputs) {
	QUERY_ONE_FOR_INTEGER (FFNet)
		integer result = my numberOfInputs;
	QUERY_ONE_FOR_INTEGER_END (U" units")
}

FORM (QUERY_ONE_FOR_INTEGER__FFNet_getNumberOfHiddenWeights, U"FFNet: Get number of hidden weights", U"FFNet: Get number of hidden weights...") {
	NATURAL (layer, U"Hidden layer number", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (FFNet)
		integer result = 0;
		if (layer <= my numberOfLayers - 1) {
			integer numberOfUnitsInPreviousLayer = ( layer == 1 ? my numberOfInputs : my numberOfUnitsInLayer [layer - 1] );
			result = my numberOfUnitsInLayer [layer] * (numberOfUnitsInPreviousLayer + 1);
		}
	QUERY_ONE_FOR_INTEGER_END (U" weights (including biases)")
}
	
DIRECT (QUERY_ONE_FOR_INTEGER__FFNet_getNumberOfOutputWeights) {
	QUERY_ONE_FOR_INTEGER (FFNet)
		integer numberOfUnitsInPreviousLayer = ( my numberOfLayers == 1 ? my numberOfInputs : my numberOfUnitsInLayer [my numberOfLayers - 1] );
		integer result = my numberOfUnitsInLayer [my numberOfLayers] * (numberOfUnitsInPreviousLayer + 1);
	QUERY_ONE_FOR_INTEGER_END (U" weights");
}

FORM (QUERY_ONE_FOR_STRING__FFNet_getCategoryOfOutputUnit, U"FFNet: Get category of output unit", nullptr) {
	NATURAL (outputUnit, U"Output unit", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (FFNet)
		conststring32 result = FFNet_getCategoryOfOutputUnit (me, outputUnit);
	QUERY_ONE_FOR_STRING_END
}

FORM (QUERY_ONE_FOR_INTEGER__FFNet_getOutputUnitOfCategory, U"FFNet: Get output unit of category", nullptr) {
	SENTENCE (category, U"Category", U"u")
	OK
DO
	QUERY_ONE_FOR_INTEGER (FFNet)
		integer result = FFNet_getOutputUnitOfCategory (me, category);
	QUERY_ONE_FOR_INTEGER_END (U" (output unit)")
}


FORM (QUERY_ONE_FOR_REAL__FFNet_getBias, U"FFNet: Get bias", nullptr) {
	NATURAL (layer, U"Layer", U"1")
	NATURAL (unit, U"Unit", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (FFNet)
		const double result = FFNet_getBias (me, layer, unit);
	QUERY_ONE_FOR_REAL_END (U" (bias)")
}


FORM (QUERY_ONE_FOR_REAL__FFNet_getWeight, U"FFNet: Get weight", nullptr) {
	NATURAL (layer, U"Layer", U"1")
	NATURAL (unitTo, U"Unit", U"1")
	NATURAL (unitFrom, U"Unit from", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (FFNet)
		const double result = FFNet_getWeight (me, layer, unitTo, unitFrom);
	QUERY_ONE_FOR_REAL_END (U" (weight between unit ", unitTo, U" in layer ", layer, U", and unit ", unitFrom, U" in layer ", layer - 1, U")")
}

DIRECT (QUERY_ONE_FOR_REAL__FFNet_getMinimum) {
	QUERY_ONE_FOR_REAL (FFNet)
		const double result = FFNet_getMinimum (me);
	QUERY_ONE_FOR_REAL_END (U" (minimum)");
}

FORM (MODIFY_EACH__FFNet_setBias, U"FFNet: Set bias", nullptr) {
	NATURAL (layer, U"Layer", U"1")
	NATURAL (unit, U"Unit", U"1")
	REAL (value, U"Value", U"0.0")
	OK
DO
	MODIFY_EACH (FFNet)
		FFNet_setBias (me, layer, unit, value);
	MODIFY_EACH_END 
}

FORM (MODIFY_EACH__FFNet_setWeight, U"FFNet: Set weight", nullptr) {
	NATURAL (layer, U"Layer", U"1")
	NATURAL (unitTo, U"Unit", U"1")
	NATURAL (unitFrom, U"Unit (from)", U"1")
	REAL (value, U"Value", U"0.0")
	OK
DO
	MODIFY_EACH (FFNet)
		FFNet_setWeight (me, layer, unitTo, unitFrom, value);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__FFNet_reset, U"FFNet: Reset", U"FFNet: Reset...") {
	LABEL (U"Warning: this command destroys all previous learning.")
	LABEL (U"New weights will be randomly chosen from the interval [-range, +range].")
	POSITIVE (range, U"Range", U"0.1")
	OK
DO
	MODIFY_EACH (FFNet)
		FFNet_reset (me, range);
	MODIFY_EACH_END
}
	
FORM (MODIFY_EACH__FFNet_selectBiasesInLayer, U"FFNet: Select biases", U"FFNet: Select biases...") {
	LABEL (U"WARNING: This command induces very specific behaviour ")
	LABEL (U"during a following learning phase.")
	NATURAL (layer, U"Layer number", U"1")
	OK
DO
	MODIFY_EACH (FFNet)
		FFNet_selectBiasesInLayer (me, layer);
	MODIFY_EACH_END
}
	
DIRECT (MODIFY_EACH__FFNet_selectAllWeights) {
	MODIFY_EACH (FFNet)
		FFNet_selectAllWeights (me);
	MODIFY_EACH_END
}

FORM (NEW_FFNet_extractWeights, U"FFNet: Extract weights", U"FFNet: Extract weights...") {
	NATURAL (layer, U"Layer number", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (FFNet)
		autoTableOfReal result = FFNet_extractWeights (me, layer);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW_FFNet_weightsToMatrix, U"FFNet: Weights to Matrix ", nullptr) {
	LABEL (U"Warning: Use \"Extract weights..\" instead.")
	NATURAL (layer, U"Layer number", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (FFNet)
		autoMatrix result = FFNet_weightsToMatrix (me, layer, false);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (HINT__hint_FFNet_PatternList_classify) {
	INFO_NONE
		Melder_information (U"You can use the FFNet as a classifier by selecting a\n"
			"FFNet and a PatternList together and choosing \"To Categories...\".");
	INFO_NONE_END
}
	
DIRECT (HINT__hint_FFNet_PatternList_Categories_learn) {
	INFO_NONE
		Melder_information (U"You can teach a FFNet to classify by selecting a\n"
			"FFNet, a PatternList and a Categories together and choosing \"Learn...\".");
	INFO_NONE_END
}

/******************* FFNet && ActivationList ******************************************/

FORM (CONVERT_ONE_AND_ONE_TO_ONE__FFNet_ActivationList_to_Categories, U"FFNet & ActivationList: To Categories", 0) {
	RADIO (categorizationgMethod, U"Categorization method", 1)
		RADIOBUTTON (U"winner-takes-all")
		RADIOBUTTON (U"stochastic")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (FFNet, ActivationList)
		autoCategories result = FFNet_ActivationList_to_Categories (me, you, categorizationgMethod);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

/******************* FFNet && Eigen ******************************************/

FORM (GRAPHICS_ONE_AND_ONE__FFNet_Eigen_drawIntersection, U"FFnet & Eigen: Draw intersection", 0) {
	NATURAL (pcx, U"X-component", U"1")
	NATURAL (pcy, U"Y-component", U"2")
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	REAL (ymin, U"Ymin", U"0.0")
	REAL (ymax, U"Ymax", U"0.0")
	OK
DO
	GRAPHICS_ONE_AND_ONE (FFNet, Eigen)
		FFNet_Eigen_drawIntersection (me, you, GRAPHICS, pcx, pcy, xmin, xmax, ymin, ymax);
	GRAPHICS_ONE_AND_ONE_END
} 

/************************* FFNet && Categories **********************************/

DIRECT (CONVERT_ONE_AND_ONE_TO_ONE__FFNet_Categories_to_ActivationList) {
	CONVERT_ONE_AND_ONE_TO_ONE (FFNet, Categories)
		autoActivationList result = FFNet_Categories_to_ActivationList (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get());
}

/************************* FFNet && Matrix **********************************/

FORM (CONVERT_ONE_AND_ONE_TO_ONE__FFNet_weightsFromMatrix, U"Replace weights by values from Matrix", nullptr) {
	NATURAL (layer, U"Layer", U"1")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (FFNet, Matrix)
		autoFFNet result = FFNet_weightsFromMatrix (me, you, layer);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get());
}

/************************* FFNet && PatternList **********************************/

FORM (GRAPHICS_ONE_AND_ONE__FFNet_PatternList_drawActivation, U"Draw an activation", nullptr) {
	NATURAL (row, U"PatternList (row) number", U"1");
	OK
DO
	GRAPHICS_ONE_AND_ONE (FFNet, PatternList)
		FFNet_PatternList_drawActivation (me, you, GRAPHICS, row);
	GRAPHICS_ONE_AND_ONE_END
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__FFNet_PatternList_to_Categories, U"FFNet & PatternList: To Categories", U"FFNet & PatternList: To Categories...") {
	RADIO (categorizationgMethod, U"Categorization method", 1)
		RADIOBUTTON (U"winner-takes-all")
		RADIOBUTTON (U"stochastic")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (FFNet, PatternList)
		autoCategories result = FFNet_PatternList_to_Categories (me, you, categorizationgMethod);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__FFNet_PatternList_to_ActivationList, U"To activations in layer", nullptr) {
	NATURAL (layer, U"Layer", U"1")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (FFNet, PatternList)
		autoActivationList result = FFNet_PatternList_to_ActivationList (me, you, layer);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

/*********** FFNet & PatternList & ActivationList **********************************/

FORM (QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL__FFNet_PatternList_ActivationList_getTotalCosts, U"FFNet & PatternList & ActivationList: Get total costs", U"FFNet & PatternList & ActivationList: Get total costs...") {
	RADIO (costFunctionType, U"Cost function", 1)
		RADIOBUTTON (U"minimum-squared-error")
		RADIOBUTTON (U"minimum-cross-entropy")
	OK
DO
	QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL (FFNet, PatternList, ActivationList)
		const double result = FFNet_PatternList_ActivationList_getCosts_total (me, you, him, costFunctionType);
	QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL_END (U"")
}

FORM (QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL__FFNet_PatternList_ActivationList_getAverageCosts, U"FFNet & PatternList & ActivationList: Get average costs", U"FFNet & PatternList & ActivationList: Get average costs...") {
	RADIO (costFunctionType, U"Cost function", 1)
		RADIOBUTTON (U"minimum-squared-error")
		RADIOBUTTON (U"minimum-cross-entropy")
	OK
DO
	QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL (FFNet, PatternList, ActivationList)
		const double result = FFNet_PatternList_ActivationList_getCosts_average (me, you, him, costFunctionType);
	QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL_END (U"")
}

FORM (MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE__FFNet_PatternList_ActivationList_learn, U"FFNet & PatternList & ActivationList: Learn", nullptr) {
	// NATURAL (U"Layer", U"1")
	NATURAL (maximumNumberOfEpochs, U"Maximum number of epochs", U"100")
	POSITIVE (tolerance, U"Tolerance of minimizer", U"1e-7")
	RADIO (costFunctionType, U"Cost function", 1)
		RADIOBUTTON (U"minimum-squared-error")
		RADIOBUTTON (U"minimum-cross-entropy")
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE (FFNet, PatternList, ActivationList)
		FFNet_PatternList_ActivationList_learnSM (me, you, him, maximumNumberOfEpochs, tolerance, costFunctionType);
	MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE_END	
}
	

FORM (MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE__FFNet_PatternList_ActivationList_learnSlow, U"FFNet & PatternList & ActivationList: Learn slow", nullptr) {
	// NATURAL (U"Layer", U"1")
	NATURAL (maximumNumberOfEpochs, U"Maximum number of epochs", U"100")
	POSITIVE (tolerance, U"Tolerance of minimizer", U"1e-7")
	LABEL (U"Specific for this minimization")
	POSITIVE (learningRate, U"Learning rate", U"0.1")
	REAL (momentum, U"Momentum", U"0.9")
	RADIO (costFunctionType, U"Cost function", 1)
		RADIOBUTTON (U"minimum-squared-error")
		RADIOBUTTON (U"minimum-cross-entropy")
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE (FFNet, PatternList, ActivationList)
		FFNet_PatternList_ActivationList_learnSD (me, you, him, maximumNumberOfEpochs, tolerance, learningRate, momentum, costFunctionType);
	MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE_END	
}

/*********** FFNet & PatternList & Categories **********************************/

FORM (QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL__FFNet_PatternList_Categories_getTotalCosts, U"FFNet & PatternList & Categories: Get total costs", U"FFNet & PatternList & Categories: Get total costs...") {
	RADIO (costFunctionType, U"Cost function", 1)
		RADIOBUTTON (U"minimum-squared-error")
		RADIOBUTTON (U"minimum-cross-entropy")
	OK
DO
	QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL (FFNet, PatternList, Categories)
		const double result = FFNet_PatternList_Categories_getCosts_total (me, you, him, costFunctionType);
	QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL_END (U" (total costs)")
}

FORM (QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL__FFNet_PatternList_Categories_getAverageCosts, U"FFNet & PatternList & Categories: Get average costs", U"FFNet & PatternList & Categories: Get average costs...") {
	RADIO (costFunctionType, U"Cost function", 1)
		RADIOBUTTON (U"minimum-squared-error")
		RADIOBUTTON (U"minimum-cross-entropy")
	OK
DO
	QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL (FFNet, PatternList, Categories)
		const double result = FFNet_PatternList_Categories_getCosts_average (me, you, him, costFunctionType);
	QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL_END (U" (average costs)")
}

FORM (MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE__FFNet_PatternList_Categories_learn, U"FFNet & PatternList & Categories: Learn", U"FFNet & PatternList & Categories: Learn...") {
	NATURAL (maximumNumberOfEpochs, U"Maximum number of epochs", U"100")
	POSITIVE (tolerance, U"Tolerance of minimizer", U"1e-7")
	RADIO (costFunctionType, U"Cost function", 1)
		RADIOBUTTON (U"minimum-squared-error")
		RADIOBUTTON (U"minimum-cross-entropy")
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE (FFNet, PatternList, Categories)
		FFNet_PatternList_Categories_learnSM (me, you, him, maximumNumberOfEpochs, tolerance, costFunctionType);
	MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE_END
}

FORM (MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE__FFNet_PatternList_Categories_learnSlow, U"FFNet & PatternList & Categories: Learn slow", U"FFNet & PatternList & Categories: Learn slow...") {
	NATURAL (maximumNumberOfEpochs, U"Maximum number of epochs", U"100")
	POSITIVE (tolerance, U"Tolerance of minimizer", U"1e-7")
	LABEL (U"Specific for this minimization")
	POSITIVE (learningRate, U"Learning rate", U"0.1")
	REAL (momentum, U"Momentum", U"0.9")
	RADIO (costFunctionType, U"Cost function", 1)
		RADIOBUTTON (U"minimum-squared-error")
		RADIOBUTTON (U"minimum-cross-entropy")
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE (FFNet, PatternList, Categories)
		FFNet_PatternList_Categories_learnSD (me, you, him, maximumNumberOfEpochs,tolerance, learningRate, momentum, costFunctionType);
	MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE_END
}

/*********** FFNet & PCA **********************************/

FORM (GRAPHICS_ONE_AND_ONE__FFNet_PCA_drawDecisionPlaneInEigenspace, U"FFNet & PCA: Draw decision plane", nullptr) {
	NATURAL (unitNumber, U"Unit number", U"1")
	NATURAL (layer, U"Layer number", U"1")
	NATURAL (horizontalEigenvectorNumber, U"Horizontal eigenvector number", U"1")
	NATURAL (verticalEigenvectorNumber, U"Vertical eigenvector number", U"2")
	REAL (xmin, U"left Horizontal range", U"0.0")
	REAL (xmax, U"right Horizontal range", U"0.0")
	REAL (ymin, U"left Vertical range", U"0.0")
	REAL (ymax, U"right Vertical range", U"0.0")
	OK
DO
	GRAPHICS_ONE_AND_ONE (FFNet, PCA)
		FFNet_Eigen_drawDecisionPlaneInEigenspace (me, you, GRAPHICS, unitNumber, layer, horizontalEigenvectorNumber, verticalEigenvectorNumber, xmin, xmax, ymin, ymax);
	GRAPHICS_ONE_AND_ONE_END
}

/*********** PatternList & Categories **********************************/

FORM (CONVERT_ONE_AND_ONE_TO_ONE__PatternList_Categories_to_FFNet, U"PatternList & Categories: To FFNet", U"PatternList & Categories: To FFNet...") {
	INTEGER (numberOfUnitsInHiddenLayer1, U"Number of units in hidden layer 1", U"0")
	INTEGER (numberOfUnitsInHiddenLayer2, U"Number of units in hidden layer 2", U"0")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (PatternList, Categories)
		autoFFNet result = PatternList_Categories_to_FFNet (me, you, numberOfUnitsInHiddenLayer1, numberOfUnitsInHiddenLayer2);
		autostring32 name = result -> name.move();
	CONVERT_ONE_AND_ONE_TO_ONE_END (name.get())
}

void praat_uvafon_FFNet_init () {
	Thing_recognizeClassesByName (classFFNet, nullptr);

	praat_addMenuCommand (U"Objects", U"New", U"Feedforward neural networks", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create FFNet...", nullptr, 1, 
			CREATE_ONE__FFNet_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create iris example...", nullptr, 1, 
			CREATE_MULTIPLE__FFNet_createIrisExample);
	praat_addMenuCommand (U"Objects", U"New", U"Advanced", nullptr, 1, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create FFNet (linear outputs)...", nullptr, 2, 
			CREATE_ONE__FFNet_create_linearOutputs);
	praat_addMenuCommand (U"Objects", U"New", U"Create PatternList... || Create Pattern...",
			nullptr, 2, CREATE_ONE__PatternList_create);   // alternative GuiMenu_DEPRECATED_2015
	praat_addMenuCommand (U"Objects", U"New", U"Create Categories...", nullptr, 2, 
			CREATE_ONE__Categories_create);

	praat_addAction1 (classFFNet, 0, U"FFNet help", nullptr, 0, 
			HELP__FFNet_help);
	praat_addAction1 (classFFNet, 0, DRAW_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classFFNet, 0, U"Draw topology", nullptr, 1, 
			GRAPHICS_EACH__FFNet_drawTopology);
	praat_addAction1 (classFFNet, 0, U"Draw weights...", nullptr, 1, 
			GRAPHICS_EACH__FFNet_drawWeights);
	praat_addAction1 (classFFNet, 0, U"Draw weights to layer...", nullptr, GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			GRAPHICS_EACH__FFNet_drawWeightsToLayer);
	praat_addAction1 (classFFNet, 0, U"Draw cost history...", nullptr, 1, 
			GRAPHICS_EACH__FFNet_drawCostHistory);
	praat_addAction1 (classFFNet, 0, QUERY_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classFFNet, 0, U"Query structure", nullptr, 1, nullptr);
	praat_addAction1 (classFFNet, 1, U"Get number of layers", nullptr, 2, 
			QUERY_ONE_FOR_INTEGER__FFNet_getNumberOfLayers);
	praat_addAction1 (classFFNet, 1, U"Get number of outputs", nullptr, 2, 
			QUERY_ONE_FOR_INTEGER__FFNet_getNumberOfOutputs);
	praat_addAction1 (classFFNet, 1, U"Get number of hidden units...", nullptr, 2, 
			QUERY_ONE_FOR_INTEGER__FFNet_getNumberOfHiddenUnits);
	praat_addAction1 (classFFNet, 1, U"Get number of inputs", nullptr, 2, 
			QUERY_ONE_FOR_INTEGER__FFNet_getNumberOfInputs);
	praat_addAction1 (classFFNet, 1, U"Get number of hidden weights...", nullptr, 2, 
			QUERY_ONE_FOR_INTEGER__FFNet_getNumberOfHiddenWeights);
	praat_addAction1 (classFFNet, 1, U"Get number of output weights", nullptr, 2, 
			QUERY_ONE_FOR_INTEGER__FFNet_getNumberOfOutputWeights);
	praat_addAction1 (classFFNet, 1, U"Get category of output unit...", nullptr, 2, 
			QUERY_ONE_FOR_STRING__FFNet_getCategoryOfOutputUnit);
	praat_addAction1 (classFFNet, 1, U"Get output unit of category...", nullptr, 2, 
			QUERY_ONE_FOR_INTEGER__FFNet_getOutputUnitOfCategory);
	praat_addAction1 (classFFNet, 0, U"-- FFNet weights --", nullptr, 1, nullptr);
	praat_addAction1 (classFFNet, 1, U"Get bias...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__FFNet_getBias);
	praat_addAction1 (classFFNet, 1, U"Get weight...", nullptr, 1, 
			QUERY_ONE_FOR_REAL__FFNet_getWeight);
	praat_addAction1 (classFFNet, 1, U"Get minimum", nullptr, 1, 
			QUERY_ONE_FOR_REAL__FFNet_getMinimum);
	praat_addAction1 (classFFNet, 0, MODIFY_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classFFNet, 0, U"Set bias...", nullptr, 1, 
			MODIFY_EACH__FFNet_setBias);
	praat_addAction1 (classFFNet, 0, U"Set weight...", nullptr, 1, 
			MODIFY_EACH__FFNet_setWeight);
	praat_addAction1 (classFFNet, 0, U"Reset...", nullptr, 1, 
			MODIFY_EACH__FFNet_reset);
	praat_addAction1 (classFFNet, 0, U"Select biases...", nullptr, 1, 
			MODIFY_EACH__FFNet_selectBiasesInLayer);
	praat_addAction1 (classFFNet, 0, U"Select all weights", nullptr, 1, 
			MODIFY_EACH__FFNet_selectAllWeights);
	praat_addAction1 (classFFNet, 0, EXTRACT_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classFFNet, 0, U"Extract weights...", nullptr, 1, 
			NEW_FFNet_extractWeights);
	praat_addAction1 (classFFNet, 0, U"Weights to Matrix...", nullptr, GuiMenu_DEPTH_1 | GuiMenu_HIDDEN, 
			NEW_FFNet_weightsToMatrix);
	praat_addAction1 (classFFNet, 0, U"& PatternList: Classify?", nullptr, 0, 
			HINT__hint_FFNet_PatternList_classify);
	praat_addAction1 (classFFNet, 0, U"& PatternList & Categories: Learn?", nullptr, 0,
			HINT__hint_FFNet_PatternList_Categories_learn);

	praat_addAction2 (classFFNet, 1, classActivationList, 1, U"Analyse", nullptr, 0, nullptr);
	praat_addAction2 (classFFNet, 1, classActivationList, 1, U"To Categories...", nullptr, 0, 
			CONVERT_ONE_AND_ONE_TO_ONE__FFNet_ActivationList_to_Categories);

	praat_addAction2 (classFFNet, 1, classEigen, 1, U"Draw", nullptr, 0, nullptr);
	praat_addAction2 (classFFNet, 1, classEigen, 1, U"Draw hyperplane intersections", nullptr, 0,
			GRAPHICS_ONE_AND_ONE__FFNet_Eigen_drawIntersection);

	praat_addAction2 (classFFNet, 1, classCategories, 1, U"Analyse", nullptr, 0, nullptr);
	praat_addAction2 (classFFNet, 1, classCategories, 1, U"To ActivationList", nullptr, 0, 
			CONVERT_ONE_AND_ONE_TO_ONE__FFNet_Categories_to_ActivationList);

	praat_addAction2 (classFFNet, 1, classMatrix, 1, U"Modify", nullptr, 0, nullptr);
	praat_addAction2 (classFFNet, 1, classMatrix, 1, U"Weights from Matrix...", nullptr, 0, 
			CONVERT_ONE_AND_ONE_TO_ONE__FFNet_weightsFromMatrix);

	praat_addAction2 (classFFNet, 1, classPatternList, 1, U"Draw", nullptr, 0, nullptr);
	praat_addAction2 (classFFNet, 1, classPatternList, 1, U"Draw activation...", nullptr, 0,
			GRAPHICS_ONE_AND_ONE__FFNet_PatternList_drawActivation);
	praat_addAction2 (classFFNet, 1, classPatternList, 1, U"Analyse", nullptr, 0, nullptr);
	praat_addAction2 (classFFNet, 1, classPatternList, 1, U"To Categories...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__FFNet_PatternList_to_Categories);
	praat_addAction2 (classFFNet, 1, classPatternList, 1, U"To ActivationList...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__FFNet_PatternList_to_ActivationList);

	praat_addAction3 (classFFNet, 1, classPatternList, 1, classActivationList, 1, U"Get total costs...", nullptr, 0,
			QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL__FFNet_PatternList_ActivationList_getTotalCosts);
	praat_addAction3 (classFFNet, 1, classPatternList, 1, classActivationList, 1, U"Get average costs...", nullptr, 0,
			QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL__FFNet_PatternList_ActivationList_getAverageCosts);
	praat_addAction3 (classFFNet, 1, classPatternList, 1, classActivationList, 1, U"Learn", nullptr, 0, nullptr);
	praat_addAction3 (classFFNet, 1, classPatternList, 1, classActivationList, 1, U"Learn...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE__FFNet_PatternList_ActivationList_learn);
	praat_addAction3 (classFFNet, 1, classPatternList, 1, classActivationList, 1, U"Learn slow...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE__FFNet_PatternList_ActivationList_learnSlow);

	praat_addAction3 (classFFNet, 1, classPatternList, 1, classCategories, 1, U"Get total costs...", nullptr, 0,
			QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL__FFNet_PatternList_Categories_getTotalCosts);
	praat_addAction3 (classFFNet, 1, classPatternList, 1, classCategories, 1, U"Get average costs...", nullptr, 0,
			QUERY_ONE_AND_ONE_AND_ONE_FOR_REAL__FFNet_PatternList_Categories_getAverageCosts);
	praat_addAction3 (classFFNet, 1, classPatternList, 1, classCategories, 1, U"Learn", nullptr, 0, nullptr);
	praat_addAction3 (classFFNet, 1, classPatternList, 1, classCategories, 1, U"Learn...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE__FFNet_PatternList_Categories_learn);
	praat_addAction3 (classFFNet, 1, classPatternList, 1, classCategories, 1, U"Learn slow...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE_AND_ONE__FFNet_PatternList_Categories_learnSlow);
	
	praat_addAction2 (classFFNet, 1, classPCA, 1, U"Draw decision plane...", nullptr, 0,
			GRAPHICS_ONE_AND_ONE__FFNet_PCA_drawDecisionPlaneInEigenspace);
	
	praat_addAction2 (classPatternList, 1, classCategories, 1, U"To FFNet...", nullptr, 0, 
			CONVERT_ONE_AND_ONE_TO_ONE__PatternList_Categories_to_FFNet);
	
	INCLUDE_MANPAGES (manual_FFNet_init)
}

/* End of file praat_FFnet_init.cpp */
