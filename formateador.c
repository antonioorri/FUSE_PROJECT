
#include <stdio.h>
#include <stdlib.h>

#include "basicFUSE_lib.h"




int main(){
	
	char datos[8]={'a','n','a','l','.','j','u'};
	FILE *f;
	
	f=fopen("analju.dat","wb");
	if(f==NULL)exit(1);
	printf("Formateando modo anal.ju\n");
	
	for(int i = 0; i<=7;i++){
		fwrite(&datos[i],sizeof(char),1,f);
		//printf("%d\n",i);
	}
	//bytes per sector 512 en nuestro caso.
	
	uint16_t a;
	
	
	
	fclose(f);
	return 0;
}
