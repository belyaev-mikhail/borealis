//
// Разыменование неинициализированного или нулевого указателя
//

int main(void) {
    int a[10];
    
    int *p = a;
    int *q = a;
    
    int i;
    
    p += i;    // дефект INI-01 (гарантированный => обрубание)
    *p = -1;   // дефект INI-03
    
    a[0] = 0;
    
    int result = *q;
    return result;
}
