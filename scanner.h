#ifndef SYSTEM_SCANNER_H
#define SYSTEM_SCANNER_H

#include "common.h"
#include "token.h"

// ==========================================
// THE SCANNER STATE
// ==========================================
// This struct tracks the current position of the lexer
// as it walks through the source code string.

typedef struct {
  const char* start;   // The beginning of the current token being scanned.
  const char* current; // The current character being processed.
  int line;            // Current line number for error reporting.
} Scanner;

// ==========================================
// SCANNER API
// ==========================================

// Sets up the scanner to process a specific source code string.
void initScanner(const char* source);

// The core engine. 
// Every time this is called, it identifies the NEXT token in the 
// source code and returns it. It does NOT allocate memory; it 
// returns a 'Token' which is a pointer-view into the source.
Token scanToken();

#endif // SYSTEM_SCANNER_H
