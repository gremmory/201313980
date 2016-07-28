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

struct Partition{
    char Part_Status[4];//indica si esta activa  o no
    char Part_Type[4];//indica tipo de particion Primario o Extendida p o E
    char Part_Fit[4];//Tipo de ajuste Valores BF (Best) FF(First WF(Worst)
    int Part_Start;//Indica en que Byte Inicia el Disco
    int Part_Size;//Contiene el Tamano total de la particion
    char Part_Name[16];//Nombre de la particion
};

struct MasterBoot{//Mbr
    int Mbr_Tamano;//tamano total del disco en bytes
    //struct tm time;//time //probar sin puntero despues con disco
    char Mbr_Fecha_Creacion[16];// fecha y hora de la creacion del disco
    int Mbr_Disk_Signature;//numero random de forma unica a cada disco
    struct Partition Mbr_Partition_1;//estructura del disco 1
    struct Partition Mbr_Partition_2;//Estructura del disco 2
    struct Partition Mbr_Partition_3;//Estructura del disco 3
    struct Partition Mbr_Partition_4;//estructura del disco 4
};

struct ExtendedBoot{//Ebr
    char Part_Status[4];//indica si esta activa  o no
    char Part_Fit[4];//Tipo de ajuste Valores BF (Best) FF(First WF(Worst)
    int Part_Start;//Indica en que Byte Inicia el Disco
    int Part_Size;//Contiene el Tamano total de la particion
    int Part_Next;//Byte en el que indica el proximo EBR -1 si no hay siguiente
    int Part_Name[16];//Nombre de la particion+ 4 por .disk
};

struct EstMbr{
    int Mbr_Tamano;
    //time tiempo;
    int Mbr_Disk_Assignature;//random


    char mbr_byte_particion[10];//Contiene el byte donde inicia la partición.
    char mbr_byte_particion_desactivada[10];//Contiene el byte donde inicia la partición.
    char mbr_tipo_sistema_archivos[7];//Contiene el tipo de sistema de archivos.
    char mbr_tipo_particion[12];//Contiene el tipo de partición.
    char mbr_particion_activa[4];//Indica si la partición es la activa, está será la por defecto que iniciará para realizar operaciones sobre el sistema de archivos.
    char mbr_size_bytes[10];//Contiene el tamaño en bytes del sistema de archivos.
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
//crea direcctorios
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
//crear disco
void NewDisk(char name[100], int tam, int unidad, char direc[100]){//crea el disco //crear_disco
    struct EstDisk Ndisck;
    struct MasterBoot Mbr;
    //struct Partition part = {NULL, NULL, NULL, 0, 0, NULL};

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

    //mbr del disco
    //random
    int rdn = rand();
    timer_t now;
    struct tm *ts;
    char buff[16];
    now = time(0);
    ts = localtime(&now);
    strftime(buff, sizeof(buff), "%d/%m/%y %H:%M", ts);
    Mbr.Mbr_Tamano = tam;
    sprintf(Mbr.Mbr_Fecha_Creacion, "%s", buff);
    Mbr.Mbr_Disk_Signature = rdn;
    Mbr.Mbr_Partition_1;
    //memset(&Mbr.Mbr_Partition_1, 0, sizeof (Mbr.Mbr_Partition_1));
    Mbr.Mbr_Partition_2;
    Mbr.Mbr_Partition_3;
    Mbr.Mbr_Partition_4;

    fwrite(&Mbr,sizeof(Mbr),1,f);


    fclose(f);
    printf("Disco %s creado exitosamente\n", Ndisck.nombre);

}
//remover disco
void RmDisk(char direc[100]){
    if(remove(direc)==0) // Eliminamos el archivo
        printf("El Disco fue Eliminado Correctamente\n");
    else
        printf("No se pudo eliminar el Disco Verifique su Inf.\n");
}
//crea particiones
void FDisk(int Size, char Direc[100], char Name[16], int Unit, int Type, int Fit){

    struct EstDisk EDisk;


    if(Unit == 0){
        Size = Size*1024*1024;//tamano convertido en bytes
    }else if(Unit ==1){
        Size = Size*1024;//tamano convertido en bytes
    }

    char tipo[4];
    if(Type == 0){
        strcpy(tipo, "P");
    }else if(Type == 1){
        strcpy(tipo, "E");
    }else if(Type == 2){
        strcpy(tipo, "L");
        printf("No se puede Crear Particion Logica Como Principal");
    }
    FILE *f = fopen (Direc, "rb+");//Buscamo el Archivo y lo arbimos
    fseek(f,Direc,SEEK_SET);//tomamo los punteros del datos del archivo


    struct Partition Prtion;
    sprintf(Prtion.Part_Status, "%s", "S");//activa si o no
    (Prtion.Part_Type, "%s", tipo);

}

//*********************************************************************************************************//
//********************************* Analizador de Entrada ***********************************//

void Comando(){
    char Cadena [300];
    // comando
    char comando[100];
    char cmd[20];
    //******** booleanos  de Comandos principales
    bool mkdisk = false;
    bool rmdisk = false;
    bool fdisk = false;
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
    //type
    int TypeDisk = 0;
    bool TypeDiskB = false;
    //fit
    int Fit = 2;
    bool FitB = false;
    //delete
    int Delt = 0;
    bool DeltB = false;
    //add pos and neg
    int Pola = 1;
    bool AddS = false;


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
        }else if(strcmp(comando, "fdisk")==0){
            Unit = 1;
            strcpy(cmd, comando);
            strcpy(comando, "");
            fdisk = true;
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
        }else if(strcmp(comando, "+add::") == 0){
            int j;
            strcpy(comando, "");
            for(j = i; j < strlen(Cadena); j++){
                if(Cadena[i] == '+'){// uno positivo
                    Pola = 1;
                }else if( Cadena[i] == '-'){// cero negativo
                    Pola = 0;
                    aux = Cadena[j];
                }else if( Cadena[i] == ' '){
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
            AddS = true;
            strcpy(comando, "");
        }else if(strcmp(comando, "+type::") == 0){
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
                    if(strcmp(comando, "p") == 0){//primaria 0
                        TypeDisk = 0;
                    }else if(strcmp(comando, "e") == 0){// extendida 1
                        TypeDisk = 1;
                    }else if(strcmp(comando, "l") == 0){//logica 2
                        TypeDisk = 2;
                    }else{
                        printf("Tipo de Formato no Existe *****\n");
                    }
                    break;
                }else{
                    strcat(comando, &aux);
                }
                i++;
            }
            TypeDiskB = true;
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
                    if(strcmp(comando, "k") == 0){//kilobyte 1
                        Unit = 1;
                    }else if(strcmp(comando, "m") == 0){//megabyte 0
                        Unit = 0;
                    }else if(strcmp(comando, "b") == 0){//bytes 2
                        Unit = 2;
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
        }else if(strcmp(comando, "+fit::")==0){//fit
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
                    if(strcmp(comando, "bf") == 0){//best fit 1
                        Fit = 0;
                    }else if(strcmp(comando, "ff") == 0){//first fit 0
                        Fit = 1;
                    }else if(strcmp(comando, "wf") == 0){//worst fit 2
                        Fit = 2;
                    }else{
                        Fit = 3;
                    }
                    break;
                }else{
                    strcat(comando, &aux);
                }
                i++;
            }
            FitB = true;
            strcpy(comando, "");
        }else if(strcmp(comando, "+delete::")==0){//delete
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
                    if(strcmp(comando, "fast")  == 0){//fast delete fit 1
                        Delt = 0;
                    }else if(strcmp(comando, "full") == 0){//full delte fit 0
                        Delt = 1;
                    }else{
                        Delt = 2;
                    }
                    break;
                }else{
                    strcat(comando, &aux);
                }
                i++;
            }
            DeltB = true;
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
            if((ext == 1 || ext == 0) && mkdisk == true){
                printf("!!Error!! No Tiene Extension\n");
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



    //Mkdisk -Size::3000 +unit::K -path::"/home/user/"
    //\ -name::"Disco1.dsk"

    //Mkdisk -Size::8 +unit::K -path::"/home/gremory/Escritorio/casas/" -name::"Disco1.dsk"
    //Mkdisk -Size::4096 +unit::K -path::"/home/gremory/Escritorio/casas/" -name::"Disco1.dsk"
    //rmDisk -path::"/home/gremory/Escritorio/casas/Disco1.dsk"

    printf("%s|| Comando\n", comando);
    printf("%s \n", cmd);
    printf("%s \n", NameDisk);
    printf("Unididad : %d \n", Unit);
    printf("Tamano : %d \n", TamDisk);
    printf("Paht : %s \n", PthDir);
    printf("***********************************\n");
    printf("************ Crear Particiones**************\n");
    //fdisk -Size::300 -path::"/home/Disco1.dsk" -name::"Particion1"

    //fdisk +type::E -path::"/home/Disco2.dsk" +Unit::K \\
    //-name::"Particion2" -size::300

    //fdisk -size::1 +type::L +unit::M +fit::BF \\
    //-path::"/mis discos/Disco3.dsk" name::"Particion3"

    //fdisk +type::E -path::"/home/Disco2.dsk" -name::"Part3" \\
    //+Unit::K -size::200

    //fdisk +delete::fast -name::"Particion1"\ -path::"/home/Disco1.dsk"

    //fdisk -name::"Particion1" +delete::full \ -path::"/home/Disco1.dsk"

    //fdisk +add::1 +unit::M -path::"/home/mis discos/Disco4.dsk" \\
    //-name::"Particion 4"

    printf("%d Size:\n", TamDisk);
    printf("%d Unit\n", Unit);
    printf("%s Paht\n", PthDir);
    printf("%d Type\n", TypeDisk);
    printf("%d Fit\n", Fit);
    printf("%d Delete\n", Delt);
    printf("%s Name\n", NameDisk);
    printf("%d %d Add\n", Pola, Size);
}

void Prueba(){
    FILE *f = fopen ("/home/gremory/Escritorio/casas/Disco1.dsk", "rb+");
    struct EstDisk tdisco;
    struct MasterBoot mbr;
    fread(&tdisco,sizeof(tdisco),1,f);
    fread(&mbr, sizeof(mbr), 1,f);

    printf("tamaño: %s MB \n", tdisco.tamano);
    printf("Nombre: %s\n\n", tdisco.nombre);
    printf("Mbr*******:\n");
    printf("tamaño: %d MB \n", mbr.Mbr_Tamano);
    printf("Fecha: %s\n\n", mbr.Mbr_Fecha_Creacion);
    printf("Ramdon: %d\n", mbr.Mbr_Disk_Signature);
    //sprintf(tdisco.nombre, "%s", "puuta");
    struct Partition part;
    part = mbr.Mbr_Partition_1;
    {
        printf(" NO hay ni verga");
    }
    rewind(f);

    /*
    char Part_Status[4];//indica si esta activa  o no
    char Part_Type[4];//indica tipo de particion Primario o Extendida p o E
    char Part_Fit[4];//Tipo de ajuste Valores BF (Best) FF(First WF(Worst)
    int Part_Start;//Indica en que Byte Inicia el Disco
    int Part_Size;//Contiene el Tamano total de la particion
    char Part_Name[16];//Nombre de la particion
    */


    //fseek(f,atoi(tdisco.nombre),SEEK_SET); /pueod usarlo y modifica info
    //fwrite(&tdisco,sizeof(tdisco),1,f);
    fclose(f);
    //Mkdisk -Size::4096 +unit::K -path::"/home/gremory/Escritorio/casas/" -name::"Disco1.dsk"
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
    Prueba();
    //Comando();
    return 0;
}

void ComandosIN(){

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
