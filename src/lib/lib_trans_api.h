#ifndef WALIA_LIB_TRANS_API_H
#define WALIA_LIB_TRANS_API_H

#include "../core/common.h"
#include "../core/value.h"

// ==========================================
// TRANSCODER LIBRARY API
// ==========================================

/**
 * @brief Initializes the Universal Transcoder Library.
 * 
 * Logic:
 * Registers the following natives:
 * - json_parse()
 * - json_stringify()
 * - wbin_encode()
 * - wbin_decode()
 */
void initTranscoderLibrary();

#endif // WALIA_LIB_TRANS_API_H
