#include <stdio.h>//atoi
#include <stdlib.h>//directorios
#include <dirent.h>//crear directorios
#include <sys/stat.h>//comparacion de cadenas
#include <string.h>//time
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/stat.h>


//******************************** Estructuras del Archivo
//+++++++++++++ struct del Disco *********
struct EstDisk{ //struct del disco
    char nombre[100];
    char tamano[9];
    char particiones[9];
    char pnt[9]; //direccion del mbr -- punteros
};

//******************************** Var. Global
//guarda la ruta de los discos duros del sistema
char Path[250]="";
char Dir[250];//direccion del disco actual

char sistema_reporte[17]="reporte/";
char directorio_reporte[640];
char buffer[1];//bit utilizado para guardar 0's
int ifor=0;//contador de fors
DIR *d;//variable directorio global
struct dirent *di;//para listar discos
char fecha[16];//fecha actual
int login=0;//FAT login (0= no ha iniciado sesion, 1= inicio sesion)

void SetDisk(char nombre [100]){
    sprintf(Dir, "%s%s", Path, nombre);
}

void Directorio(){
    char direc [200];
    strcpy(direc, "mkdir -p ");
    strcat(direc, Path);
    printf(": %s \n:", direc);
    system(direc);
    //mkdir(direc, S_IRUSR | S_IWUSR | S_IXUSR);
    //mkdir(direc, S_IRWXU);
    //Mkdisk -Size::4096 +unit::K -path::"/home/gremory/Escritorio/casas/" -name::"Disco1.dsk"

}

void NewDisk(char name[100], int tam, int unidad, char direc[100]){//crea el disco //crear_disco
    struct EstDisk Ndisck;
    printf("\n");
    printf("************   *********\n");
    printf("Creando Disco...\n");
    printf("************   *********\n");
    //strcpy(Ndisck.nombre, name);
    sprintf(Path, "%s", direc);// cambiamos el lugar donde se almacena
    Directorio();
    sprintf(Ndisck.nombre, "%s", name);

    //libreria sprintf concatena y asigna en forma de arreglo
    sprintf(Ndisck.tamano, "%d", tam);
    sprintf(Ndisck.particiones, "%d", 0);
    sprintf(Ndisck.pnt, "%d", (int)sizeof(Ndisck));
    printf("Creando disco...\n");
    //creando el disco
    SetDisk(Ndisck.nombre);
    printf("%s", Dir);
    FILE *f = fopen (Dir, "w+b");

    if(unidad == 1){// 0 == Mk 1==kb
        for(ifor=0;ifor < tam;ifor++)
            fwrite (buffer, sizeof(buffer), 1024, f);
        //guardando informacion del disco
    }else if(unidad == 0){
        for(ifor=0;ifor < tam*1024;ifor++)
            fwrite (buffer, sizeof(buffer), 1024, f);
        //guardando informacion del disco
    }else{
        for(ifor=0;ifor < tam*1024;ifor++)
            fwrite (buffer, sizeof(buffer), 1024, f);
        //guardando informacion del disco
    }

    rewind(f);
    fwrite(&Ndisck,sizeof(Ndisck),1,f);
    fclose(f);
    printf("Disco %s creado exitosamente\n", Ndisck.nombre);
}

void RmDisk(char direc[100]){
    if(remove(direc)==0) // Eliminamos el archivo
        printf("El Disco fue Eliminado Correctamente\n");
    else
        printf("No se pudo eliminar el Disco Verifique su Inf.\n");
}

//*********************************************************************************************************//
//********************************* Analizador de Entrada ***********************************//

void Comando(){
    char Cadena [300];
    //Mkdisk -Size::3000 +unit::K -path::"/home/user/"
    //\ -name::"Disco1.dsk"
    //Mkdisk -Size::8 +unit::K -path::"/home/user/" -name::"Disco1.dsk"
    //Mkdisk -Size::4096 +unit::K -path::"/home/gremory/Escritorio/casas/" -name::"Disco1.dsk"
    //rmDisk -path::"/home/gremory/Escritorio/casas/Disco1.dsk"
    // comando
    char comando[100];
    char cmd[20];
    //******** booleanos  de Comandos principales
    bool mkdisk = false;
    bool rmdisk = false;
    int ElimDisk = 2;
    //size
    bool Size = false;
    int TamDisk;
    //unit
    bool UnitB = false;
    int Unit = 0;
    //path
    bool PthDirB = false;
    char PthDir[100];
    //nombre
    char NameDisk[50];
    bool NameDiskB = false;
    //Confirmacion de Eliminacion



    //gets(ps);
    //fflush(stdin);
    //printf("%s \n",ps);
    gets(Cadena);
    //scanf("%s",&Comando);
    fflush(stdin);
    //int tama = strlen(Comando);
    printf(" %s :: %d \n", Cadena, strlen(Cadena));
    int i;

    strcpy(comando, "");
    char *aux;
    for(i=0; i < strlen(Cadena); i++){


        if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' '){
            aux = Cadena[i];
        }else{
            aux = tolower(Cadena[i]);
        }

        if(strcmp(comando, "mkdisk")==0){
            strcpy(cmd, comando);
            strcpy(comando, "");
            mkdisk = true;
        }else if(strcmp(comando, "rmdisk")==0){
            strcpy(cmd, comando);
            strcpy(comando, "");
            rmdisk = true;
            printf("Esta Seguro de Eliminar Disco...\n");
            printf("0.- Cancelar\n");
            printf("1.- Aceptar\n");
            scanf("%d", &ElimDisk);
            getchar();
            while(ElimDisk != 0 && ElimDisk != 1){
                printf("Seleccione Una Opcion Admitida\n");
                scanf("%d", &ElimDisk);
                getchar();
            }
        }else if(Cadena[i] == ' ' || strcmp(comando, " ") == 0){
            strcpy(comando, "");
        }else if(strcmp(comando, "-size::") == 0){
            int j;
            strcpy(comando, "");
            //strcat(comando, &aux);
            for(j = i; j < strlen(Cadena); j++){
                if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' '){
                    aux = Cadena[j];
                }else{
                    aux = tolower(Cadena[j]);
                }
                if(Cadena[j] == ' '){
                    TamDisk = atoi(comando);
                    break;
                }else{
                    strcat(comando, &aux);
                }
                i++;
            }
            Size = true;
            strcpy(comando, "");
        }else if(strcmp(comando, "+unit::")==0){
            int j;
            strcpy(comando, "");
            //strcat(comando, &aux);
            for(j = i; j < strlen(Cadena); j++){
                if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' '){
                    aux = Cadena[j];
                }else{
                    aux = tolower(Cadena[j]);
                }
                if(Cadena[j] == ' '){
                    if(strcpy(comando, "k")){
                        Unit = 1;
                    }else if(strcpy(comando, "m")){
                        Unit = 0;
                    }else{
                        Unit = 0;
                    }
                    break;
                }else{
                    strcat(comando, &aux);
                }
                i++;
            }
            UnitB = true;
            strcpy(comando, "");
        }else if(strcmp(comando, "-path::")==0){
            int comi = 0;
            int j;
            strcpy(comando, "");
            //strcat(comando, &aux);
            for(j = i; j < strlen(Cadena); j++){
                if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' ' || Cadena[i] == '"'){
                    aux = Cadena[j];
                }else{
                    aux = (Cadena[j]);
                }
                if(Cadena[j] == '"'){
                    if(comi != 0){
                        strcpy(PthDir, comando);
                        break;
                    }else{
                        comi = 1;
                    }
                }else{
                    strcat(comando, &aux);
                }
                i++;
            }
            PthDirB = true;
            strcpy(comando, "");
        }else if(strcmp(comando, "-name::")==0){
            char extt[5];
            strcpy(extt, "");
            int comi = 0;
            int ext = 0;
            int j;
            strcpy(comando, "");
            //strcat(comando, &aux);
            for(j = i; j < strlen(Cadena); j++){
                if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' ' || Cadena[i] == '"'){
                    aux = Cadena[j];
                }else if(Cadena[i] == '.'){
                    ext = 1;
                    aux = Cadena[i];
                }else{
                    aux = (Cadena[j]);
                }
                if(Cadena[j] == '"'){
                    if(comi != 0){
                        strcpy(NameDisk, comando);
                        break;
                    }else{
                        comi = 1;
                    }
                }else{
                    if(ext == 1){
                        strcat(extt, &aux);
                    }
                    if(strcmp(extt, ".dsk") == 0){
                        ext = 2;
                    }
                    strcat(comando, &aux);
                }
                i++;
            }
            if(ext == 1 || ext == 0){
                printf("!!Error!! Al Crear Disco No Contiene Extencion Nombre\n");
                NameDiskB = false;
            }else{
                NameDiskB = true;
            }
            strcpy(comando, "");
        }else if(strcmp(comando, "\\") == 0 || Cadena[i] == '\\'){
            gets(Cadena);
            fflush(stdin);
            i = -1;
            strcpy(comando, "");
        }else{
            strcat(comando, &aux);
        }

    }


    if(mkdisk == true){
        if(NameDiskB == true && Size == true && PthDirB == true){
            NewDisk(NameDisk, TamDisk, Unit, PthDir);
        }else{
            printf("Error Al intentar Crear Disco, Verifique sus Datos\n");
        }
    }else if(rmdisk == true){
        if(PthDirB == true && ElimDisk == 1){
            RmDisk(PthDir);
        }else{
            if(ElimDisk != 0){
                printf("Error Al intentar Remover Disco, Verifique sus Datos\n");
            }
        }
    }


    printf("%s|| Comando\n", comando);
    printf("%s \n", cmd);
    printf("%s \n", NameDisk);
    printf("Unididad : %d \n", Unit);
    printf("Tamano : %d \n", TamDisk);
    printf("Paht : %s \n", PthDir);

}



//*********************************************************************************************************//
//********************************* Menu de Aplicacion **************************************//
int MenuSelect(){
    int SelectOp = 0;
    printf("********************** Particion de Discos *********************\n");
    printf("0.- Salir del Sistema\n");
    printf("1.- Ingreso de Comandos\n");
    printf("\n\n");
    printf("Elige una Opcion\n");
    scanf("%d", &SelectOp);
    getchar();
    return SelectOp;
}

void Menu(){
    char OptionSelect;
    do{
        OptionSelect = MenuSelect();
        switch (OptionSelect) {
        case 1:

            break;
        default:
            break;
        }
    }while(OptionSelect != '0');
}

int main(int argc, char *argv[])
{
    //Menu();
    //NewDisk();
    Comando();
    return 0;
}

/*
 * char *aux;
        if(Cadena[i] == '-' || Cadena[i] == '+'){
            aux = (Cadena[i]);
        }else{
            aux = tolower(Cadena[i]);
        }
        */
//multiples carpetas al mismo tiempo/ mkdir -p path
/*
 *
            while(i < strlen(Cadena)){
                aux = &Cadena[i];
                strcat(cmd, &aux);
                printf("%s \n", cmd);
                if(strcmp(cmd, "-")==0 || strcmp(cmd, "+")==0){
                    aux = Cadena[i];
                }else{
                    aux = tolower(Cadena[i]);

                }

                if(strcmp(cmd, "-size::")==0 && Size == false){
                    strcpy(cmd, "");
                    while(Size == false){
                        aux = tolower(Cadena[i]);
                        if(strcmp(aux, " ")==0 && strcmp(aux, "+")==0 && strcmp(aux, "-")==0){
                            Size = true;
                            TamDisk = atoi(cmd);
                            strcpy(cmd, "");
                        }else{
                            strcat(cmd, &aux);
                        }
                        i++;
                    }
                }else if(strcmp(cmd, "-path::")==0 && PthDirB == false){
                    strcpy(cmd, "");
                    i++;
                    while(PthDirB == false){
                        aux = tolower(Cadena[i]);
                        if(strcmp(aux, "\"")==0){
                            PthDirB = true;
                            strcpy(PthDir, &cmd);
                            strcpy(cmd, "");
                        }else{
                            strcat(cmd, &aux);
                        }
                        i++;
                    }
                }else if(strcmp(cmd, "-name::")==0 && NameDiskB == false){
                    strcpy(cmd, "");
                    i++;
                    while(NameDiskB == false){
                        aux = tolower(Cadena[i]);
                        if(strcmp(aux, "\"")==0){
                            NameDiskB = true;
                            strcpy(NameDisk, &cmd);
                            strcpy(cmd, "");
                        }else{
                            strcat(cmd, &aux);
                        }
                        i++;
                    }
                }else if(strcmp(cmd, "+unit::")==0 && UnitB == false){
                    strcpy(cmd, "");
                    while(UnitB == false){
                        aux = tolower(Cadena[i]);
                        if(strcmp(aux, "m")==0){
                            UnitB = true;
                            strcpy(Unit, "M");
                            strcpy(cmd, "");
                        }else if(strcmp(aux, "k")==0){
                            UnitB = true;
                            strcpy(Unit, "K");
                            strcpy(cmd, "");
                        }
                        i++;
                    }
                }else if(strcmp(cmd, "\\") == 0){
                    gets(Cadena);
                    fflush(stdin);
                    i = 0;
                    strcat(cmd, &aux);// concatena
                }else if(strcmp(cmd, " ") == 0){
                    strcpy(cmd, "");
                }else{
                    strcat(cmd, &aux);
                }
                //printf("%d  %c \n", i, Cadena[i]);
                i++;
            }

            */
