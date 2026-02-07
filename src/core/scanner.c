#include <stdio.h>
#include <string.h>
#include <ctype.h> // Fixed: Added for isalpha and isdigit

#include "common.h"
#include "scanner.h"

Scanner scanner;

void initScanner(const char* source) {
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
}

static bool isAtEnd() { return *scanner.current == '\0'; }
static char advance() { scanner.current++; return scanner.current[-1]; }
static char peek() { return *scanner.current; }
static char peekNext() { if (isAtEnd()) return '\0'; return scanner.current[1]; }

static bool match(char expected) {
  if (isAtEnd()) return false;
  if (*scanner.current != expected) return false;
  scanner.current++; return true;
}

static Token makeToken(TokenType type) {
  Token token;
  token.type = type;
  token.start = scanner.start;
  token.length = (int)(scanner.current - scanner.start);
  token.line = scanner.line;
  return token;
}

static Token errorToken(const char* message) {
  Token token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = (int)strlen(message);
  token.line = scanner.line;
  return token;
}

static void skipWhitespace() {
  for (;;) {
    char c = peek();
    switch (c) {
      case ' ': case '\r': case '\t': advance(); break;
      case '\n': scanner.line++; advance(); break;
      case '/':
        if (peekNext() == '/') {
          while (peek() != '\n' && !isAtEnd()) advance();
        } else return;
        break;
      default: return;
    }
  }
}

static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
  if (scanner.current - scanner.start == start + length &&
      memcmp(scanner.start + start, rest, length) == 0) return type;
  return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
  switch (scanner.start[0]) {
    case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
    case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
    case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
    case 'f':
      if (scanner.current - scanner.start > 1) {
        switch (scanner.start[1]) {
          case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
          case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
          case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
        }
      }
      break;
    case 'g': return checkKeyword(1, 3, "ene", TOKEN_GENE); 
    case 'h': return checkKeyword(1, 5, "andle", TOKEN_HANDLE);
    case 'i':
      if (scanner.current - scanner.start > 1) {
        if (scanner.start[1] == 'f') return TOKEN_IF;
        if (scanner.start[1] == 'n') return TOKEN_IN;
      }
      break;
    case 'm': return checkKeyword(1, 4, "atch", TOKEN_MATCH);
    case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
    case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
    case 'p':
      if (scanner.current - scanner.start > 1) {
        switch (scanner.start[1]) {
            case 'r': return checkKeyword(2, 3, "int", TOKEN_PRINT);
            case 'e': return checkKeyword(2, 5, "rform", TOKEN_PERFORM);
        }
      }
      break;
    case 'r':
      if (scanner.current - scanner.start > 1) {
        switch (scanner.start[1]) {
            case 'e':
                if (scanner.current - scanner.start > 2) {
                   if (scanner.start[2] == 't') return checkKeyword(3, 3, "urn", TOKEN_RETURN);
                   if (scanner.start[2] == 's') return checkKeyword(3, 3, "ume", TOKEN_RESUME);
                }
        }
      }
      break;
    case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
    case 't':
      if (scanner.current - scanner.start > 1) {
        switch (scanner.start[1]) {
          case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
          case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
          case 'e': return checkKeyword(2, 2, "st", TOKEN_TEST);
        }
      }
      break;
    case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
    case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
  }
  return TOKEN_IDENTIFIER;
}

static Token identifier() {
  while ((peek() >= 'a' && peek() <= 'z') || (peek() >= 'A' && peek() <= 'Z') || peek() == '_' || (peek() >= '0' && peek() <= '9')) advance();
  return makeToken(identifierType());
}

static Token number() {
  while (peek() >= '0' && peek() <= '9') advance();
  if (peek() == '.' && (peekNext() >= '0' && peekNext() <= '9')) {
    advance(); while (peek() >= '0' && peek() <= '9') advance();
  }
  return makeToken(TOKEN_NUMBER);
}

static Token string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') scanner.line++;
    advance();
  }
  if (isAtEnd()) return errorToken("Unterminated string.");
  advance(); return makeToken(TOKEN_STRING);
}

Token scanToken() {
  skipWhitespace();
  scanner.start = scanner.current;
  if (isAtEnd()) return makeToken(TOKEN_EOF);
  char c = advance();

  if (c == '<' && (isalpha(peek()) || isdigit(peek()) || peek() == '/')) {
      while (peek() != '>' && !isAtEnd()) {
          if (peek() == '\n') scanner.line++;
          advance();
      }
      if (isAtEnd()) return errorToken("Unterminated dimensional unit.");
      advance();
      return makeToken(TOKEN_UNIT);
  }

  if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') return identifier();
  if (c >= '0' && c <= '9') return number();

  switch (c) {
    case '(': return makeToken(TOKEN_LEFT_PAREN);
    case ')': return makeToken(TOKEN_RIGHT_PAREN);
    case '[': return makeToken(TOKEN_LEFT_BRACKET);
    case ']': return makeToken(TOKEN_RIGHT_BRACKET);
    case '{': return makeToken(TOKEN_LEFT_BRACE);
    case '}': return makeToken(TOKEN_RIGHT_BRACE);
    case ';': return makeToken(TOKEN_SEMICOLON);
    case ',': return makeToken(TOKEN_COMMA);
    case '.': return makeToken(match('.') ? TOKEN_RANGE : TOKEN_DOT);
    case '-': return makeToken(TOKEN_MINUS);
    case '+': return makeToken(TOKEN_PLUS);
    case '/': return makeToken(TOKEN_SLASH);
    case '*': return makeToken(TOKEN_STAR);
    case ':': return makeToken(TOKEN_COLON);
    case '&': return makeToken(TOKEN_AMPERSAND);
    case '~': return makeToken(match('=') ? TOKEN_TILDE_EQUAL : TOKEN_TILDE);
    case '|': return makeToken(match('>') ? TOKEN_PIPE : TOKEN_PIPE_CHAR);
    case '!': return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
      if (match('>')) return makeToken(TOKEN_FAT_ARROW);
      return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<': return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>': return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    case '"': return string();
    case '@': return makeToken(TOKEN_AT);
  }
  return errorToken("Unexpected character.");
}
