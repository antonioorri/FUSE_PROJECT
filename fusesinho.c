/*
DSO 2022
Grupo C

Hecho por Aĺvaro, Antonio y Juan
Team pichasgordas

Version 1.1.2 ❤️😒😊😭😩😍😔😁💕💕💕💕😊😊😊😊
😍😍😍😍😍😍😍😍😍😍😍😍😍😍😍😍😍😍😍😍😍😍😍😍😍😍😍😍

gcc FS.c -o FS pkg-config fuse --cflags --libs
*/
#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include "fusesinho.h"
// gcc FS.c -o FS `pkg-config fuse --cflags --libs`
// ./ FS - f Desktop / OS / mountpoint4





superbloque spbloque;

void initialize_superbloque(){
	memset(spbloque.data_bitmap, '0', 100*sizeof(char));// bytes
	memset(spbloque.inode_bitmap, '0', 100*sizeof(char));//100 bytes
}
filetype * root;
filetype file_array[50];

void initialize_root_directory() {
	spbloque.inode_bitmap[1]=1; //marking it with 0
	root = (filetype *) malloc (sizeof(filetype));
	strcpy(root->path, "/");
	strcpy(root->name, "/");
	root -> children = NULL;
	root -> num_children = 0;
	root -> parent = NULL;
	root -> num_links = 2;
	root -> valid = 1;
	strcpy(root->test, "test");
	//root -> type = malloc(10);
	strcpy(root -> type, "directory");

	root->c_time = time(NULL);
	root->a_time = time(NULL);
	root->m_time = time(NULL);
	root->b_time = time(NULL);

	root -> permissions = S_IFDIR | 0777;

	root -> size = 0;
	root->group_id = getgid();
	root->user_id = getuid();


	root -> number = 2;
	//root -> size = 0;
	root -> blocks = 0;

}




filetype * filetype_from_path(char * path){ //Devuelve el directorio padre del fichero con ruta path
	
	char curr_folder[100];
	char * path_name = malloc(strlen(path) + 2); //Asigno espacio para la ruta

	strcpy(path_name, path); //Indexo ruta del padre

	filetype * curr_node = root; //Valor base es el root

	fflush(stdin);

	if(strcmp(path_name, "/") == 0) //Si path es / es que es root
		return curr_node; //Devuelvo root

	if(path_name[0] != '/'){//Si empieza distinto de / es error
		printf("Path incorrecto\n");
		exit(1);
	}
	else{
		path_name++; //Aumento el puntero
	}

	if(path_name[strlen(path_name)-1] == '/'){ //Si el final se ha puesto con / se quita
		path_name[strlen(path_name)-1] = '\0';
	}

	char * index;
	int flag = 0;

	while(strlen(path_name) != 0){ //Mientras la longitud de la ruta sea distinta de 0
		index = strchr(path_name, '/'); //Busco /
		printf("%s---\n",index);
		
		if(index != NULL){ //Si lo encuentra es un directorio
			printf("index distinto de null\n");
			strncpy(curr_folder, path_name, index - path_name); //Guardo nombre de directorio 
			curr_folder[index-path_name] = '\0'; 
			
			flag = 0;
			for(int i = 0; i < curr_node -> num_children; i++){ //Por cada hijo del nodo actual (primera iteracion es root)
				if(strcmp((curr_node -> children)[i] -> name, curr_folder) == 0){ //Comparo si coincide con el nombre del directorio
					curr_node = (curr_node -> children)[i]; //Asgino el nodo actual al hijo que coincide
					flag = 1; //Lo he encontrado
					break;
				}
			}
			if(flag == 0)
				return NULL;
		}
		else{ //Es fichero
			strcpy(curr_folder, path_name); //Guardo ruta 
			flag = 0;
			for(int i = 0; i < curr_node -> num_children; i++){//Por cada hijo del nodo actual (primera iteracion es root)
				if(strcmp((curr_node -> children)[i] -> name, curr_folder) == 0){//Comparo si coincide con el nombre del fichero
					curr_node = (curr_node -> children)[i];//Asgino el nodo actual al hijo que coincide
					return curr_node; //Devuelvo el nodo actual
				}
			}
			return NULL;
		}
		path_name = index+1;
	}

}

void inodos_libres(){
	for(int  i = 2; i < strlen(spbloque.inode_bitmap); i++)
		printf("%c ",spbloque.inode_bitmap[i]);
	printf("\n");
}

int find_free_inode(){
	int i;
	
	inodos_libres();
	for ( i = 2; i < strlen(spbloque.inode_bitmap) ; i++){
		if(spbloque.inode_bitmap[i] == '0'){
			spbloque.inode_bitmap[i] = '1';
			return i;
		}

	}
	return i;//da error. pero ahora cuando borremos el archivo hay que poner el inodo a '0'
}

int find_free_db(){
	for (int i = 1; i <strlen(spbloque.inode_bitmap); i++){
		if(spbloque.inode_bitmap[i] == '0'){
			spbloque.inode_bitmap[i] = '1';
			return i;
		}
		
	}
}

void add_child(filetype * parent, filetype * child){ //Añade hijos 
	(parent -> num_children)++; //Incrementando numero de hijos que tiene

	parent -> children = realloc(parent -> children, (parent -> num_children)*sizeof(filetype *)); //Reservando espacio para el nuevo hijo

	(parent -> children)[parent -> num_children - 1] = child; //Agregagandolo a su array de hijos
}

static int mymkdir(const char *path, mode_t mode) {//Para crear carpetas
	printf("My mkdir\n");
	printf("Buscamos un inodo libre\n");
	int index = find_free_inode();//buscamos un inodo libre.
	printf("el inodo que estaba libre es: %d\n",index);
	


	filetype * new_folder = malloc(sizeof(filetype));//para el nuevo directorio

	char * pathname = malloc(strlen(path));
	
	strcpy(pathname, path);//path de la carpeta creada
	printf("path: %s\n",pathname);
	char * rindex = strrchr(pathname, '/');// /antonio ,antnonio //antonio
	printf("nombre del fichero: %s\n",rindex+1);
	//new_folder -> name = malloc(strlen(pathname)+2);
	strcpy(new_folder -> name, rindex+1);
	//new_folder -> path = malloc(strlen(pathname)+2);
	strcpy(new_folder -> path, pathname);// /antonio/alnberjfkdsañ/ hola.c
	*rindex = '\0';
	if(strlen(pathname) == 0){
		//printf("Cuando entro aqui? cuando estamos en / \n");
		strcpy(pathname, "/");
	}

	//--------------------------------------------------------------------------------------//
	new_folder -> children = NULL;
	new_folder -> num_children = 0;
	new_folder -> parent = filetype_from_path(pathname);
	new_folder -> num_links = 2;// . ..
	new_folder -> valid = 1;
	strcpy(new_folder -> test, "test");
	if(new_folder -> parent == NULL)//si no tiene padre error porque tiene que tener si o si
		return -ENOENT;
	add_child(new_folder->parent, new_folder);
	strcpy(new_folder -> type, "directory");
	new_folder->c_time = time(NULL);
	new_folder->a_time = time(NULL);
	new_folder->m_time = time(NULL);
	new_folder->b_time = time(NULL);
	new_folder -> permissions = S_IFDIR | 0755;
	new_folder -> size = 0;
	new_folder->group_id = getgid();
	new_folder->user_id = getuid();
	new_folder -> number = index;
	new_folder -> blocks = 0;
	//--------------------------------------------------------------------------------------//
	return 0;

}


int myreaddir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi ){
	printf("READDIR\n");

	filler(buffer, ".", NULL, 0 ); 
	filler(buffer, "..", NULL, 0 ); 

	char * pathname = malloc(strlen(path)+2); 
	strcpy(pathname, path);

	filetype * dir_node = filetype_from_path(pathname); // busca el nodo en el path

	if(dir_node == NULL){
		return -ENOENT;
	}
	else{
		dir_node->a_time=time(NULL); 
		for(int i = 0; i < dir_node->num_children; i++){ // asigna el nodo del directorio hacia el hijo
			printf(":%s:\n", dir_node->children[i]->name); // indica el nodo al hijo
			filler( buffer, dir_node->children[i]->name, NULL, 0 );
		}
	}

	return 0;
}


static int mygetattr(const char *path, struct stat *statit) {
	char *pathname;
	pathname=(char *)malloc(strlen(path));
	strcpy(pathname, path);
	printf("GETATTR %s\n", pathname);
	filetype * file_node = filetype_from_path(pathname);
	if(file_node == NULL)
		return -ENOENT;

	statit->st_uid = file_node -> user_id; // The owner of the file/directory is the user who mounted the filesystem
	statit->st_gid = file_node -> group_id; // The group of the file/directory is the same as the group of the user who mounted the filesystem
	statit->st_atime = file_node -> a_time; // The last "a"ccess of the file/directory is right now
	statit->st_mtime = file_node -> m_time; // The last "m"odification of the file/directory is right now
	statit->st_ctime = file_node -> c_time;
	statit->st_mode = file_node -> permissions;
	statit->st_nlink = file_node -> num_links + file_node -> num_children;
	statit->st_size = file_node -> size;
	statit->st_blocks = file_node -> blocks;
	return 0;
}

int myrmdir(const char * path){
	printf("Se va a borrar un directorio. ;( espero que no fuera importante\n");
	char * pathname = malloc(strlen(path)+2);
	
	strcpy(pathname, path);

	char * rindex = strrchr(pathname, '/');

	char * folder_delete = malloc(strlen(rindex+1)+2);

	strcpy(folder_delete, rindex+1);

	*rindex = '\0';

	if(strlen(pathname) == 0)
		strcpy(pathname, "/");

	filetype * parent = filetype_from_path(pathname);

	if(parent == NULL)
		return -ENOENT;

	if(parent -> num_children == 0)
		return -ENOENT;

	filetype * curr_child = (parent -> children)[0];
	int index = 0;
	while(index < (parent -> num_children)){
		if(strcmp(curr_child -> name, folder_delete) == 0){
			break;
		}
		index++;
		curr_child = (parent -> children)[index];
	}

	if(index < (parent -> num_children)){
		if(((parent -> children)[index] -> num_children) != 0)
			return -ENOTEMPTY;
		for(int i = index+1; i < (parent -> num_children); i++){
			(parent -> children)[i-1] = (parent -> children)[i];
		}
		(parent -> num_children) -= 1;
	}

	else{
		return -ENOENT;
	}
	return 0;

}

int myrm(const char * path){

	char * pathname = malloc(strlen(path)+2);
	strcpy(pathname, path);

	char * rindex = strrchr(pathname, '/'); //busca el path

	char * folder_delete = malloc(strlen(rindex+1)+2); // suma al inodo para borrar el path

	strcpy(folder_delete, rindex+1); 

	*rindex = '\0';

	if(strlen(pathname) == 0)
		strcpy(pathname, "/");

	filetype * parent = filetype_from_path(pathname);

	if(parent == NULL) // si no tiene padre ya está eliminado
		return -ENOENT;

	if(parent -> num_children == 0) // si el hijo está vacío ya está eliminado
		return -ENOENT;

	filetype * curr_child = (parent -> children)[0]; // hace que cuando
	int index = 0;
	while(index < (parent -> num_children)){  // elimines al padre
		if(strcmp(curr_child -> name, folder_delete) == 0){  // elimine al hijo
			break;
		}
		index++;
		curr_child = (parent -> children)[index];
	}

	if(index < (parent -> num_children)){
		if(((parent -> children)[index] -> num_children) != 0) // calcula el numero de archivos dle hijo
			return -ENOTEMPTY;
		for(int i = index+1; i < (parent -> num_children); i++){
			(parent -> children)[i-1] = (parent -> children)[i];
		}
		(parent -> num_children) -= 1;
	}

	else{
		return -ENOENT;
	}

	return 0;

}


int mycreate(const char * path, mode_t mode, struct fuse_file_info *fi) {
	printf("CREATEFILE\n");
	printf("------------------Entramos en mycreate-------------------\n");
//	printf("Vamos ha crear el archivo: ");
	
	int index = find_free_inode(); //Encuentro el inodo libre

	filetype * new_file = malloc(sizeof(filetype)); //Reservo espacio para un nuevo fichero 

	char * pathname = malloc(strlen(path)+2); //Reservo espacio para la ruta
	strcpy(pathname, path); //Añado la ruta del padre

	char * rindex = strrchr(pathname, '/'); //Busco la terminación del directorio

	
	strcpy(new_file -> name, rindex+1); //Le añado el nombre al path
	strcpy(new_file -> path, pathname); //Lo guardo en la ruta del nuevo fichero
//	printf("%s\n",rindex+1);
	*rindex = '\0';

	if(strlen(pathname) == 0)//Si es el raiz
		strcpy(pathname, "/"); //Le añado la /

	//Inicializo los parametros que me quedan
	new_file -> children = NULL; 
	new_file -> num_children = 0;
	new_file -> parent = filetype_from_path(pathname); //Le añado el directorio del padre
	new_file -> num_links = 0;
	new_file -> valid = 1; //Es valido para escribir

	if(new_file -> parent == NULL)
	return -ENOENT;

	add_child(new_file->parent, new_file); //Añade al hijo

	//new_file -> type = malloc(10);
	strcpy(new_file -> type, "file"); //Pongo su tipo

	new_file->c_time = time(NULL);
	new_file->a_time = time(NULL);
	new_file->m_time = time(NULL);
	new_file->b_time = time(NULL);

	new_file -> permissions = S_IFREG | 0755; //Le doy permiso de escritura lectura y ejecución solo para root y lecutura y escritura para los demas

	new_file -> size = 0;
	new_file->group_id = getgid();
	new_file->user_id = getuid();


	new_file -> number = index; //Le doy como id el inodo que habia libre

	for(int i = 0; i < 16; i++){
		(new_file -> datablocks)[i] = find_free_db(); //Asigno bloques de datos libres al nuevo fichero
	}
	new_file -> blocks = 0;
	return 0;
}


int myopen(const char *path, struct fuse_file_info *fi) {
	printf("OPEN\n");

	char * pathname = malloc(sizeof(path)+1); // crea un char con el path
	strcpy(pathname, path); // copia la ruta

	filetype * file = filetype_from_path(pathname); // coge el archivo

	return 0;
}

int myread(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi) {

	printf("READ\n");

	char * pathname = malloc(sizeof(path)+1);
	strcpy(pathname, path);

	filetype * file = filetype_from_path(pathname);
	if(file == NULL)  // si el fichero es nulo, devuelve el error
		return -ENOENT;

	else{
		char * str = malloc(sizeof(char)*1024*(file -> blocks)); // sino introduce el archivo en el bloque

		printf(":%ld:\n", file->size);
		strcpy(str, "");
		int i;
		for(i = 0; i < (file -> blocks) - 1; i++){ // mete el fichero en el bloque
			strncat(str, &spbloque.datablocks[block_size*(file -> datablocks[i])], 1024); // añade el bloque de memoria al otro
			printf("--> %s", str);
		}
		strncat(str, &spbloque.datablocks[block_size*(file -> datablocks[i])], (file -> size)%1024); // lo mismo pero metiendo el fichero en el tamaño
		printf("--> %s", str);
		//strncpy(str, &spbloque.datablocks[block_size*(file -> datablocks[0])], file->size);
		strcpy(buf, str); // copia del buffer
	}
	return file->size;
}

int myaccess(const char * path, int mask){
	return 0;
}


int myrename(const char* from, const char* to) {
	printf("RENAME: %s\n", from);
	printf("RENAME: %s\n", to);

	char * pathname = malloc(strlen(from)+2);
	strcpy(pathname, from);

	char * rindex1 = strrchr(pathname, '/');

	filetype * file = filetype_from_path(pathname);

	*rindex1 = '\0';

	char * pathname2 = malloc(strlen(to)+2);
	strcpy(pathname2, to);

	char * rindex2 = strrchr(pathname2, '/');


	if(file == NULL)
		return -ENOENT;

	//file -> name = realloc(file -> name, strlen(rindex2+1)+2);
	strcpy(file -> name, rindex2+1);
	//file -> path = realloc(file -> path, strlen(to)+2);
	strcpy(file -> path, to);



	printf(":%s:\n", file->name);
	printf(":%s:\n", file->path);

	return 0;
}

int mytruncate(const char *path, off_t size) {
	return 0;
}

int mywrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {

	printf("WRITING\n");

	char * pathname = malloc(sizeof(path)+1);//Reservo espacio para la ruta
	strcpy(pathname, path);//Añado la ruta del padre

	filetype * file = filetype_from_path(pathname);//Busco el directorio padre
	if(file == NULL)//Si da error
		return -ENOENT;

	int indexno = (file->blocks)-1;//Guardo cuantos bloques ocupa el fichero

	if(file -> size == 0){//Si ocupa 0 bytes
		strcpy(&spbloque.datablocks[block_size*((file -> datablocks)[0])], buf);//Copio lo que hay en el buffer en el primer bloque de datos del fichero 
		file -> size = strlen(buf);//Actualizo el tamaño del fichero
		(file -> blocks)++;//Aumento en uno el numero de bloques que ocupa el fichero
	}
	else{//Si ocupa mas espacio
		int currblk = (file->blocks)-1;//Guardao numero de bloques usados
		int len1 = 1024 - (file -> size % 1024);//Calculo tamaño que me queda en el fichero
		if(len1 >= strlen(buf)){//Si cabe
			strcat(&spbloque.datablocks[block_size*((file -> datablocks)[currblk])], buf);//Escribo en el bloque de datos disponible lo que hay en el buffer
			file -> size += strlen(buf);//Actualizo el tamaño del fichero
			printf("---> %s\n", &spbloque.datablocks[block_size*((file -> datablocks)[currblk])]);//
		}
		else{
			char * cpystr = malloc(1024*sizeof(char));//Añadir nuevo bloque 
			strncpy(cpystr, buf, len1-1);//Copio en el nuevo bloque lo que queda del buffer
			strcat(&spbloque.datablocks[block_size*((file -> datablocks)[currblk])], cpystr);//Indexo el puntero del bloque a la ultima posicion de bloques de datos
			strcpy(cpystr, buf);//Copio en el bloque lo que hay en el buffer
			strcpy(&spbloque.datablocks[block_size*((file -> datablocks)[currblk+1])], (cpystr+len1-1));//Meto en el siguiente bloque lo que sobra
			file -> size += strlen(buf);//Actualizo tamaño del fichero
			printf("---> %s\n", &spbloque.datablocks[block_size*((file -> datablocks)[currblk])]);
			(file -> blocks)++;//Actualizo numero de bloques 
		}

	}

	return strlen(buf);
}

static struct fuse_operations operations =
{
	//--------------
	.mkdir=mymkdir,//listo.
	.getattr=mygetattr,//
	.readdir=myreaddir, // listo
	//---------------
	.rmdir=myrmdir,//listo.
	.open=myopen, //listo
	.read=myread, //liste
	//---------------
	.write=mywrite, //Listo
	.create=mycreate,
	.rename=myrename,
	//---------------
	.unlink=myrm,
};

int main( int argc, char *argv[] ) {
	
		initialize_superbloque();
		initialize_root_directory();


	return fuse_main(argc, argv, &operations, NULL);
}