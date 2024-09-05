//
// Created by Damien Sprockeels on 03/07/2024.
//

#include "../headers/Constraints.hpp"

/***********************************************************************************************************************
 *                                        Linker functions                                                             *
 ***********************************************************************************************************************/

/**
 * Links chord qualities to the degrees
 * The quality of each chord is linked to its degree (V is major/7, I is major,...)
 * formula: majorDegreeQualities[chords[i] * nSupportedQualities + qualities[i]] = 1
 * @param home the problem space
 * @param chords the array of chord degrees
 * @param qualities the array of chord qualities
 */
void link_chords_to_qualities(const Home& home, IntVarArray chords, IntVarArray qualities) {
    for (int i = 0; i < qualities.size(); i++)
        element(home, majorDegreeQualities, expr(home, chords[i] * nSupportedQualities + qualities[i]), 1);
}

/**
 * Links chord states to the degrees
 * The state of each chord is linked to its degree (I can be in fund/1st inversion, VI can be in fund,...)
 * formula: majorDegreeStates[chords[i] * nSupportedStates + states[i]] = 1
 * @param home the problem space
 * @param chords the array of chord degrees
 * @param states the array of chord states
 */
void link_chords_to_states(const Home& home, IntVarArray chords, IntVarArray states) {
    for(int i = 0; i < states.size(); i++)
        element(home, majorDegreeStates, expr(home, chords[i] * nSupportedStates + states[i]), 1);
}

/**
 * Links chord states to qualities
 * The state of each chord is linked to its quality (7th chords can be in 3rd inversion, etc)
 * formula: qualitiesToStates[qualities[i] * nSupportedStates + states[i]] = 1
 * @param home the problem space
 * @param qualities the array of chord qualities
 * @param states the array of chord states
 */
void link_states_to_qualities(const Home& home, IntVarArray qualities, IntVarArray states) {
    for(int i = 0; i < states.size(); i++)
        element(home, qualitiesToStates, expr(home, qualities[i] * nSupportedStates + states[i]), 1);
}

/**
 * Link the chromatic chords array to the chords array, and constraints the number of chromatic chords to be equal to nChromaticChords
 * formula: isChromatic[i] == 1 <=> chords[i] >= FIVE_OF_TWO
 * formula: sum(isChromatic) == nChromaticChords
 * @param home the problem space
 * @param size the number of chords
 * @param chords the array of chord degrees
 * @param isChromatic the array of chromatic chords
 * @param nChromaticChords the number of chromatic chords we want
 */
void chromatic_chords(const Home& home, int size, IntVarArray chords, IntVarArray isChromatic, int nChromaticChords) {
    ///link the chromatic chords
    for (int i = 0; i < size; i++)
        rel(home, expr(home, isChromatic[i] == 1),BOT_EQV,expr(home, chords[i] >= FIVE_OF_TWO), true);
    ///count the number of chromatic chords
    rel(home, sum(isChromatic) == nChromaticChords);
}

/***********************************************************************************************************************
 *                                                   Constraints                                                       *
 ***********************************************************************************************************************/

/**
 * Enforces that chord progression are valid
 * chord[i] -> chord[i+1] is possible (matrix)
 * formula: tonalTransitions[chords[i] * nSupportedChords + chords[i + 1]] = 1
 * @param home the problem space
 * @param size the number of chords
 * @param chords the array of chord degrees
 */
void chord_transitions(const Home& home, int size, IntVarArray chords){
    for(int i = 0; i < size - 1; i++)
        element(home, tonalTransitions, expr(home, chords[i] * nSupportedChords + chords[i + 1]), 1);
}

/**
 * Force the last chord to be diatonic and not the seventh chord
 * The chord progression cannot end on something other than a diatonic chord (also not seventh degree)
 * formula: chords[size - 1] < SEVENTH_DEGREE
 * @param home the problem space
 * @param size the number of chords
 * @param chords the array of chord degrees
 */
void last_chord_cst(const Home& home, int size, const IntVarArray& chords){
    rel(home, chords[size - 1] < SEVENTH_DEGREE);
}

/**
 * Enforces that the fifth degree appogiatura (Vda) is followed by the fifth degree as a major or dominant seventh chord in fundamental state (V5/7+)
 * formula: chords[i] == I => states[i+1] == fund && qualities[i+1] == M/7
 * @param home the problem space
 * @param size the number of chords
 * @param chords the array of chord degrees
 * @param states the array of chord states
 * @param qualities the array of chord qualities
 */
void fifth_degree_appogiatura(const Home& home, int size, IntVarArray chords, IntVarArray states, IntVarArray qualities){
    for(int i = 0; i< size - 1; i++){
        rel(home, expr(home,chords[i] == FIFTH_DEGREE_APPOGIATURA),
            BOT_IMP,
            expr(home, states[i+1] == FUNDAMENTAL_STATE &&
                (qualities[i+1] == MAJOR_CHORD || qualities[i+1] == DOMINANT_SEVENTH_CHORD)), true);
    }
}