# 1 "display7s.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "display7s.c" 2
# 19 "display7s.c"
unsigned char display7s(unsigned char v)
{
  switch(v)
  {
    case 0:
      return 0xEE;
    case 1:
      return 0x28;
    case 2:
      return 0xCD;
    case 3:
      return 0x6D;
    case 4:
      return 0x2B;
    case 5:
      return 0x67;
    case 6:
      return 0xE7;
    case 7:
      return 0x2C;
    case 8:
      return 0xEF;
    case 9:
      return 0x6F;
    case 10:
      return 0xAF;
    case 11:
      return 0xE3;
    case 12:
      return 0xC6;
    case 13:
      return 0xE9;
    case 14:
      return 0xC7;
    case 15:
      return 0x87;
    default:
      return 0;
  }

}
