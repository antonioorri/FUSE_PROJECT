/*
 * Antonio Ortega Rivas
 *
 *
 */
#define FUSE_USE_VERSION 26
#include "basicFUSE_lib.h"
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
//---------------------------------------//





//---------------------------------------//
static int mi_getattr(const char *path, struct stat *stbuf)
{
    struct estructura_mis_datos* mis_datos = (struct estructura_mis_datos*)fuse_get_context()->private_data;
    int i;
    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 4; 
        stbuf->st_uid = mis_datos->st_uid; 
        stbuf->st_gid = mis_datos->st_gid; 
        stbuf->st_atime = mis_datos->st_atime; 
        stbuf->st_mtime = mis_datos->st_mtime;
        stbuf->st_ctime = mis_datos->st_ctime;
        stbuf->st_size = 1024; 
        stbuf->st_blocks = 2; 
        //int es_archivo(const char * path, struct estructura_mis_datos *mis_datos);

    } else if (es_archivo(path,mis_datos) == 1) {
        stbuf->st_mode = S_IFDIR | 0766;
        stbuf->st_nlink = 1;
        stbuf->st_uid = mis_datos->st_uid;
        stbuf->st_gid = mis_datos->st_gid;
        stbuf->st_atime = mis_datos->st_atime;
        stbuf->st_mtime = mis_datos->st_mtime;
        stbuf->st_ctime = mis_datos->st_ctime;
        stbuf->st_size = MAX_CONTENIDO;
        stbuf->st_blocks = stbuf->st_size/512 + (stbuf->st_size%512)? 1 : 0;
    } else {
        return -ENOENT;
    }
    return 0;
}
static int mi_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi)
{
struct estructura_mis_datos *mis_datos= (struct estructura_mis_datos *) fuse_get_context()->private_data;
    (void)offset;
    (void)fi;
    if (strcmp(path, "/") == 0) {
        if (filler(buf, ".", NULL, 0) != 0) return -ENOMEM;
        if (filler(buf, "..", NULL, 0) != 0) return -ENOMEM;
        for (int i = 0; i <= mis_datos->numero_directorios; i++) {
            if (filler(buf, mis_datos->lista_directorios[i], NULL, 0) != 0) return -ENOMEM;
        }
        for (int i = 0; i <= mis_datos->numero_ficheros; i++) {
            if (filler(buf, mis_datos->lista_ficheros[i], NULL, 0) != 0) return -ENOMEM;
        }
    } else {
		return -ENOMEM;
	}
    return 0;
}
/***********************************
 * */
static int mi_open(const char *path, struct fuse_file_info *fi)
{
	/* completar */
	/*
	 * abre un archivo. como usamos identificadores de archivos,
	 * tambien deve asignar las estructturas necesarias y estableces fi -> fh
	 * Además, fi tiene algunos otros campos que un sistema de archivos
	 * avanzado podría encontrar utiles;
	 */
	 
    struct estructura_mis_datos* mis_datos = (struct estructura_mis_datos*)fuse_get_context()->private_data; //usarla para acceder a la estructura
                
	int i=buscar_fichero(path , mis_datos);
    
    if (i >= 0){
	//printf("nombre: %s\n contenido:\n %s\n", mis_datos->nombre_ficheros[i], mis_datos->contenido_ficheros[i] );
        if ((fi->flags & 3) != O_RDWR) return -EACCES;//escritura y lectuta
        fi->fh=i;//localizado el fichero lo guardamos la posición en la estructura para usarlo después en my_re
        return 0;
    }else        
        return -ENOENT;
}


/***********************************
 * */
static int mi_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	/* completar */
	/* leo los bytes del archivo dado en el buffer.
	 * comenzamos con los bytes del offset del archivo.
	 * devuelvo el número de bytes transferidos, o 0
	 * si e despalzamiento estaba al final del archivo
	 * o si se ha pasado.
	 */
	//printf("\n\nmi_read\n\n");
	size_t len;
	
	struct estructura_mis_datos* mis_datos = (struct estructura_mis_datos*)fuse_get_context()->private_data;
	(void) fi;
	
	int i = fi->fh;//Recuperamos la posición del fichero que buscamos antes en el open.
	
	
//para acceder a mis datos.
	len = strlen(mis_datos -> contenido_archivo[i]) + 1;
	if(offset < len){
		if(offset + size > len) size = len - offset;
		memcpy(buf, mis_datos->contenido_archivo[i] + offset, size);
	}else{
		size = 0;
	}
	return size;
}

static int mi_mkdir(const char * path, mode_t mode){
	
	struct estructura_mis_datos* mis_datos = (struct estructura_mis_datos*)fuse_get_context()->private_data;
	path++;
	anyadir_directorio(path,mis_datos);
	
	
	return 0;
}

static int mi_mknod( const char *path, mode_t mode, dev_t rdev )
{
	struct estructura_mis_datos* mis_datos = (struct estructura_mis_datos*)fuse_get_context()->private_data;
	path++;
	anyadir_archivo(path,mis_datos);
	
	
	return 0;
}
/***********************************
 * operaciones FUSE
 * */
static struct fuse_operations basic_oper = {
	.getattr	= mi_getattr,
	.readdir	= mi_readdir,
	.open		= mi_open,
	.read		= mi_read,
	.mkdir		= mi_mkdir,
	.mknod		= mi_mknod,
	
};


/***********************************
 * */
int main(int argc, char *argv[])
{
	struct estructura_mis_datos *mis_datos;
	
	mis_datos=malloc(sizeof(struct estructura_mis_datos));
	
	// análisis parámetros de entrada
	if ((argc < 3) || (argv[argc-2][0] == '-') || (argv[argc-1][0] == '-')) error_parametros();

	//mis_datos->fichero_inicial = strdup(argv[argc-2]); // fichero donde están los capítulos
    argv[argc-2] = argv[argc-1];
    argv[argc-1] = NULL;
    argc--;
    //leer_fichero(mis_datos);
    int i;
    for(i=0; i<mis_datos->numero_ficheros; i++)
    {
		//printf("----->  %s\n", mis_datos->nombre_ficheros[i]);
		//printf("%s",mis_datos->contenido_ficheros[i]);
	}
	mis_datos->numero_ficheros=-1;
	mis_datos->numero_directorios=-1;
	mis_datos->indice_contenido_archivo=-1;
	//exit(0);
	lineas();
	printf("MAIN\n");	
	lineas();
	//printf("MAIN\n");	
	//printf("MAIN\n");	
	//printf("MAIN\n");	
	return fuse_main(argc, argv, &basic_oper, mis_datos);
}
