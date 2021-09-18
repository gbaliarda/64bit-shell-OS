#include "libc.h"

static int strlen(const char *str) {
  int len = 0;
  while (str[len] != 0)
    len++;
  return len;
}

void printf(const char *str) {
  sys_write(1, str, strlen(str));
}

void putChar(char c) {
  sys_write(1, &c, 1);
}

int scanf(char *buffer, int *changed) {
  return sys_read(buffer, -1, changed);
}

int getChar(char *buffer, int *changed) {
  return sys_read(buffer, 1, changed);
}

static uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

void printInt(unsigned int num) {
  char buffer[10];
  int digits = uintToBase(num, buffer, 10);
  sys_write(1, buffer, digits);
}

void printReg(const char *regName, uint64_t regValue)
{
  char buffer[17];
	printf(regName);
	printf(": ");
	int digits = uintToBase(regValue, buffer, 16);
	for (int i = 1; i < 16 - digits; i++)
		printf("0");
	printf(buffer);
	putChar('\n');
}

int compareStrings(char *str1, char *str2){
  int index = 0;
  
  while(str1[index] && str2[index]){
    if(str1[index] != str2[index])
      return 0;
    index++;
  }
  
  if(str1[index] || str2[index]) //Si no tiene la misma long
    return 0;
  return 1;
}

/**
 * hex2int
 * take a hex string and convert it to a 32bit number (max 8 hex digits)
 */
uint64_t hex2int(char *hex, int *ok) {
    uint64_t val = 0;
    while (*hex) {
        // get current character then increment
        uint8_t byte = *hex++;
        
        if (!(byte >= '0' && byte <= '9') && !(byte >= 'a' && byte <= 'f') && !(byte >= 'A' && byte <= 'F')) {
          *ok = 0;
          return 0;
        }
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;    
        // shift 4 to make space for new digit, and add the 4 bits of the new digit 
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}


//[0, +18,446,744,073,709,551,615]
uint64_t atoi(char *str, int *ok){
  uint64_t num = 0;
  
  for (int i = 0; str[i]; ++i) {
    if(str[i] < '0' || str[i] > '9') {
      *ok = 0;
      return 0;
    }
    else
      num = num * 10 + str[i] - '0';
  }
  return num;
}

// Int con signo to string - retorna la cantidad de caracteres escritos
int itos(int value, char* target, int initialIndex){
    int digit; 
    int sign = 1; // 0 negativo, 1 positivo
    int i = -1, j = initialIndex;
    char aux[11];
    if(value < 0){
        sign = 0;
        value *= -1;
    }
    do{
        i++;
        digit = value % 10;
        aux[i] = digit + '0'; // 48 = '0' 
        value /= 10; 
    } while(value > 0);
    if (!sign){ //Si es negativo le agrego el - al principio
        target[j++] = '-';
    } 
    while(i > -1){ //Doy vuelta al aux y lo guardo en target
        target[j++] = aux[i--];
    }
    target[j] = 0;
    return j;
}

void dtos(double num, char *buff) {
  int len = itos((int)num, buff, 0);

  num -= (int)num;

  buff[len++] = ',';


  num *= 1000000000; // 9 decimales
  if (num < 0)
    num *= -1;

  itos((int)num, buff, len);
  return;
}

double strToDouble(char *str, int *ok) {
  int sign = 1, idx = 0, auxIdx = 0;
  if (str[0] == '-') {
    sign = -1;
    str++;
  }
  
  char aux[10];
  while (str[idx] && str[idx] != '.') {
    aux[idx] = str[idx];
    idx++;
  }

  aux[idx] = 0;
  int integerPart = atoi(aux, ok);

  if (!(*ok))
    return -1;

  double decimalPart = 0;

  if (str[idx] == '.') {
    str++;
    while (str[idx])
      aux[auxIdx++] = str[idx++];
    aux[auxIdx] = 0;
    decimalPart = (double) atoi(aux, ok);
  }
  auxIdx = 0;
  while (aux[auxIdx] && aux[auxIdx] == '0')
    auxIdx++;

  if (!(*ok))
    return -1;

  while (decimalPart >= 1)
    decimalPart /= 10;

  while (auxIdx > 0) {
    decimalPart /= 10;
    auxIdx--;
  }

  return ((double) integerPart + decimalPart) * sign;
}

int cpuid(cpuInformation* cpuidData){
  if(cpuidAvailability() == 0)
    return -1;
  
  cpuidRegisters cpuidRegister;
  
  int maxEaxValue = getCpuLeaf();

  if(maxEaxValue < 1)
    return maxEaxValue;

  getCpuProcessorInfo(&cpuidRegister);
  
  // EDX. mmx:23 ,sse: 25, sse2: 26
  cpuidData->mmx = (cpuidRegister.edx & 0x00800000) >> 23; // 0000 0000 1000 0000 0000 0000 0000 0000
  cpuidData->sse = (cpuidRegister.edx & 0x02000000) >> 25; // 0000 0010 0000 0000 0000 0000 0000 0000
  cpuidData->sse2 = (cpuidRegister.edx & 0x04000000) >> 26; // 0100 0000 0000 0000 0000 0000 0000

  //ECX sse3: 0 - pclmulqdq: 1 - vmx: 5, smx: 6 - fma: 12 - sse41: 19 - sse42: 20 - aes: 25 - avx: 28 - f16c: 29
  cpuidData->sse3 = (cpuidRegister.ecx & 0x00000001); // 0001
  cpuidData->pclmulqdq = (cpuidRegister.ecx & 0x00000002) >> 1; // 0010
  cpuidData->vmx = (cpuidRegister.ecx & 0x00000020) >> 5; // 0010 0000
  cpuidData->smx = (cpuidRegister.ecx & 0x00000040) >> 6; // 0100 0000
  cpuidData->fma = (cpuidRegister.ecx & 0x00001000) >> 12; // 0000 0000 0000 0000 0001 0000 0000 0000
  cpuidData->sse41 = (cpuidRegister.ecx & 0x00080000) >> 19; // 1000 0000 0000 0000 0000
  cpuidData->sse42 = (cpuidRegister.ecx & 0x00100000) >> 20; // 0000 0000 0001 0000 0000 0000 0000 0000
  cpuidData->aes = (cpuidRegister.ecx & 0x02000000) >> 25; // 0010 0000 0000 0000 0000 0000 0000
  cpuidData->avx = (cpuidRegister.ecx & 0x10000000) >> 28; // 0001 0000 0000 0000 0000 0000 0000 0000
  cpuidData->f16c = (cpuidRegister.ecx & 0x20000000) >> 29; // 0010 0000 0000 0000 0000 0000 0000 0000

  //EAX = 7

  if(maxEaxValue < 7)
    return maxEaxValue;

  getCpuExtendedFeatures(&cpuidRegister);

  // ECX:  vaes: 9, vpclmulqdq: 10
  cpuidData->vaes = (cpuidRegister.ecx & 0x00000200) >> 9; // 0010 0000 0000
  cpuidData->vpclmulqdq = (cpuidRegister.ecx & 0x00000400) >> 10; // 0100 0000 0000

  cpuidData->avx2 = (cpuidRegister.ebx & 0x00000020) >> 5; // 0010 0000

  return maxEaxValue;
}