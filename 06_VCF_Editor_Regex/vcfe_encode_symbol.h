#pragma once

#define VCFE_ENCODE_SYMBOL_H
#ifndef VCFE_ENCODE_SYMBOL_H

#include <string.h>
#include <stdlib.h>

static void encode_symbol(char ss, char* encodedString) {
	switch (ss) {
	case ' ':
		strcpy(encodedString, "=20");
		break;//return encodedString;
	case '!':
		strcpy(encodedString, "=21");
		break;//return encodedString;
	case '"':
		strcpy(encodedString, "=22");
		break;//return encodedString;
	case '#':
		strcpy(encodedString, "=23");
		break;//return encodedString;
	case '$':
		strcpy(encodedString, "=24");
		break;//return encodedString;
	case '%':
		strcpy(encodedString, "=25");
		break;//return encodedString;
	case '&':
		strcpy(encodedString, "=26");
		break;//return encodedString;
	case '\'':
		strcpy(encodedString, "=27");
		break;//return encodedString;
	case '(':
		strcpy(encodedString, "=28");
		break;//return encodedString;
	case ')':
		strcpy(encodedString, "=29");
		break;//return encodedString;
	case '*':
		strcpy(encodedString, "=2A");
		break;//return encodedString;
	case '+':
		strcpy(encodedString, "=2B");
		break;//return encodedString;
	case ',':
		strcpy(encodedString, "=2C");
		break;//return encodedString;
	case '-':
		strcpy(encodedString, "=2D");
		break;//return encodedString;
	case '.':
		strcpy(encodedString, "=2E");
		break;//return encodedString;
	case '/':
		strcpy(encodedString, "=2F");
		break;//return encodedString;
	case '0':
		strcpy(encodedString, "=30");
		break;//return encodedString;
	case '1':
		strcpy(encodedString, "=31");
		break;//return encodedString;
	case '2':
		strcpy(encodedString, "=32");
		break;//return encodedString;
	case '3':
		strcpy(encodedString, "=33");
		break;//return encodedString;
	case '4':
		strcpy(encodedString, "=34");
		break;//return encodedString;
	case '5':
		strcpy(encodedString, "=35");
		break;//return encodedString;
	case '6':
		strcpy(encodedString, "=36");
		break;//return encodedString;
	case '7':
		strcpy(encodedString, "=37");
		break;//return encodedString;
	case '8':
		strcpy(encodedString, "=38");
		break;//return encodedString;
	case '9':
		strcpy(encodedString, "=39");
		break;//return encodedString;
	case ':':
		strcpy(encodedString, "=3A");
		break;//return encodedString;
	case ';':
		strcpy(encodedString, "=3B");
		break;//return encodedString;
	case '<':
		strcpy(encodedString, "=3C");
		break;//return encodedString;
	case '=':
		strcpy(encodedString, "=3D");
		break;//return encodedString;
	case '>':
		strcpy(encodedString, "=3E");
		break;//return encodedString;
	case '?':
		strcpy(encodedString, "=3F");
		break;//return encodedString;
	case '@':
		strcpy(encodedString, "=40");
		break;//return encodedString;
	case 'A':
		strcpy(encodedString, "=41");
		break;//return encodedString;
	case 'B':
		strcpy(encodedString, "=42");
		break;//return encodedString;
	case 'C':
		strcpy(encodedString, "=43");
		break;//return encodedString;
	case 'D':
		strcpy(encodedString, "=44");
		break;//return encodedString;
	case 'E':
		strcpy(encodedString, "=45");
		break;//return encodedString;
	case 'F':
		strcpy(encodedString, "=46");
		break;//return encodedString;
	case 'G':
		strcpy(encodedString, "=47");
		break;//return encodedString;
	case 'H':
		strcpy(encodedString, "=48");
		break;//return encodedString;
	case 'I':
		strcpy(encodedString, "=49");
		break;//return encodedString;
	case 'J':
		strcpy(encodedString, "=4A");
		break;//return encodedString;
	case 'K':
		strcpy(encodedString, "=4B");
		break;//return encodedString;
	case 'L':
		strcpy(encodedString, "=4C");
		break;//return encodedString;
	case 'M':
		strcpy(encodedString, "=4D");
		break;//return encodedString;
	case 'N':
		strcpy(encodedString, "=4E");
		break;//return encodedString;
	case 'O':
		strcpy(encodedString, "=4F");
		break;//return encodedString;
	case 'P':
		strcpy(encodedString, "=50");
		break;//return encodedString;
	case 'Q':
		strcpy(encodedString, "=51");
		break;//return encodedString;
	case 'R':
		strcpy(encodedString, "=52");
		break;//return encodedString;
	case 'S':
		strcpy(encodedString, "=53");
		break;//return encodedString;
	case 'T':
		strcpy(encodedString, "=54");
		break;//return encodedString;
	case 'U':
		strcpy(encodedString, "=55");
		break;//return encodedString;
	case 'V':
		strcpy(encodedString, "=56");
		break;//return encodedString;
	case 'W':
		strcpy(encodedString, "=57");
		break;//return encodedString;
	case 'X':
		strcpy(encodedString, "=58");
		break;//return encodedString;
	case 'Y':
		strcpy(encodedString, "=59");
		break;//return encodedString;
	case 'Z':
		strcpy(encodedString, "=5A");
		break;//return encodedString;
	case '[':
		strcpy(encodedString, "=5B");
		break;//return encodedString;
	case '\\':
		strcpy(encodedString, "=5C");
		break;//return encodedString;
	case ']':
		strcpy(encodedString, "=5D");
		break;//return encodedString;
	case '^':
		strcpy(encodedString, "=5E");
		break;//return encodedString;
	case '_':
		strcpy(encodedString, "=5F");
		break;//return encodedString;
	case '`':
		strcpy(encodedString, "=60");
		break;//return encodedString;
	case 'a':
		strcpy(encodedString, "=61");
		break;//return encodedString;
	case 'b':
		strcpy(encodedString, "=62");
		break;//return encodedString;
	case 'c':
		strcpy(encodedString, "=63");
		break;//return encodedString;
	case 'd':
		strcpy(encodedString, "=64");
		break;//return encodedString;
	case 'e':
		strcpy(encodedString, "=65");
		break;//return encodedString;
	case 'f':
		strcpy(encodedString, "=66");
		break;//return encodedString;
	case 'g':
		strcpy(encodedString, "=67");
		break;//return encodedString;
	case 'h':
		strcpy(encodedString, "=68");
		break;//return encodedString;
	case 'i':
		strcpy(encodedString, "=69");
		break;//return encodedString;
	case 'j':
		strcpy(encodedString, "=6A");
		break;//return encodedString;
	case 'k':
		strcpy(encodedString, "=6B");
		break;//return encodedString;
	case 'l':
		strcpy(encodedString, "=6C");
		break;//return encodedString;
	case 'm':
		strcpy(encodedString, "=6D");
		break;//return encodedString;
	case 'n':
		strcpy(encodedString, "=6E");
		break;//return encodedString;
	case 'o':
		strcpy(encodedString, "=6F");
		break;//return encodedString;
	case 'p':
		strcpy(encodedString, "=70");
		break;//return encodedString;
	case 'q':
		strcpy(encodedString, "=71");
		break;//return encodedString;
	case 'r':
		strcpy(encodedString, "=72");
		break;//return encodedString;
	case 's':
		strcpy(encodedString, "=73");
		break;//return encodedString;
	case 't':
		strcpy(encodedString, "=74");
		break;//return encodedString;
	case 'u':
		strcpy(encodedString, "=75");
		break;//return encodedString;
	case 'v':
		strcpy(encodedString, "=76");
		break;//return encodedString;
	case 'w':
		strcpy(encodedString, "=77");
		break;//return encodedString;
	case 'x':
		strcpy(encodedString, "=78");
		break;//return encodedString;
	case 'y':
		strcpy(encodedString, "=79");
		break;//return encodedString;
	case 'z':
		strcpy(encodedString, "=7A");
		break;//return encodedString;
	case '{':
		strcpy(encodedString, "=7B");
		break;//return encodedString;
	case '|':
		strcpy(encodedString, "=7C");
		break;//return encodedString;
	case '}':
		strcpy(encodedString, "=7D");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=90");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=91");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=92");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=93");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=94");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=95");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=96");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=97");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=98");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=99");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=9A");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=9B");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=9C");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=9D");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=9E");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=9F");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=A0");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=A1");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=A2");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=A3");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=A4");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=A5");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=A6");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=A7");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=A8");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=A9");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=AA");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=AB");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=AC");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=AD");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=AE");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=AF");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=B0");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=B1");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=B2");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=B3");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=B4");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=B5");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=B6");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=B7");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=B8");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=B9");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=BA");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=BB");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=BC");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=BD");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=BE");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=BF");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=80");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=81");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=82");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=83");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=84");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=85");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=86");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=87");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=88");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=89");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=8A");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=8B");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=8C");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=8D");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=8E");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=8F");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D0=01");
		break;//return encodedString;
	case '�':
		strcpy(encodedString, "=D1=91");
		break;//return encodedString;
	default:
		encodedString = NULL;
		break;
		//return NULL;
	}
}

#endif // VCFE_ENCODE_SYMBOL_H