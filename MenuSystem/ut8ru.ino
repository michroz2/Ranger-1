/* Функция utf8rus() получает исходную строку, 
символы с кодами 0x00-0xBF пропускает без изменения в выходную строку,
а в оставшихся кодах отбирает русские буквы и перекодирует их.
*/
#define maxString 21
char target[maxString + 1] = "";

char *utf8rus(char *source)
{
  int i,j,k;
  unsigned char n;
  char m[2] = { '0', '\0' };

  strcpy(target, ""); k = strlen(source); i = j = 0;

//  Serial.println("Converse:");

  while (i < k) {
    n = source[i]; i++;

//  Serial.print(n, HEX);

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB8; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
          break;
        }
        case 0xC2: {
          n = source[i]; i++;
          if (n == 0xB0) break;
          n = 0xC2; i--;
          break;
        }
      }
    }

    m[0] = n; strcat(target, m);
    j++; if (j >= maxString) break;

//  Serial.println();

  }
  return target;
}
