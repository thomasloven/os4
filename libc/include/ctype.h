#ifndef CTYPE_H
#define CTYPE_H

#define TRUE 1
#define FALSE 0

#define iscntrl(c) (((int)c>=0 && (int)c<=0x1F) || c==0x7F)?TRUE:FALSE	// Control characters
#define isspace(c) (((int)c>=09 && (int)c<=0x0D) || c==0x20)?TRUE:FALSE	// Whitespace characters
#define isupper(c) ((int)c>=0x41 && (int)c<=0x5A)?TRUE:FALSE	// Upper case characters
#define islower(c) ((int)c>=0x61 && (int)c<=0x7A)?TRUE:FALSE	// Lower case characters
#define isalpha(c) (isupper(c) || islower(c))?TRUE:FALSE	// Alpha characters
#define isdigit(c) ((int)c>=0x30 && (int)c<=0x39)?TRUE:FALSE	// Decimal digits
#define isxdigit(c) (isdigit(c) || ((int)c>=0x41 && (int)c<=0x46) ||\
 ((int)c>=0x61 && (int)c<=0x66))?TRUE:FALSE	// Hexadecimal digits
#define isalnum(c) (isdigit(c) || isalpha(c))?TRUE:FALSE	// Alphanumerical characters
#define ispunct(c) (((int)c>=0x21 && (int)c<=0x2F) ||\
 ((int)c>=0x3A && (int)c<=0x40) || ((int)c>=0x5B && (int)c<=0x60) ||\
 ((int)c>=0x7B && (int)c<=0x7E))?TRUE:FALSE	// Punctuation characters
#define isgraph(c) (isalnum(c) || ispunct(c))?TRUE:FALSE	// Graphical characters
#define isprint(c) ((int)c>=0x20 && (int)c<=0x7E)?TRUE:FALSE	// Printable characters

#endif