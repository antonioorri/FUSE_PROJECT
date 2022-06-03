
#include <stdio.h>
#include <stdlib.h>





int main(){
	//estructura  anal_format
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
	
	int bytes_sector = 512 ;
	int reserved_sectors=2;
	fwrite(&bytes_sector,sizeof(int),1,f);
	
	fwrite(&reserved_sectors,sizeof(int),1,f);
	for(int i = 0; i<496;i++){
		fwrite("",sizeof(char),1,f);
		//printf("%d\n",i);
	}
	//---------------------------------------------
	//estructura file info
	
	fclose(f);
	return 0;
}
