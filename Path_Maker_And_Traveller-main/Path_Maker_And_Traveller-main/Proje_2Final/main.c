#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>

char* KeywordMu(char string[],int uzunluk);
int Sayimi(char ch);
void commandJumper(char *ptr,int* indeks,int uzunluk);
int tokenSay(char str[]);

//Program sorunsuz bir şekilde çalışıyor. Kontrol printlerimizin çoğunu sildik. Temel olan birkaçını görünürlük açısından bıraktık.
//isterseniz yorum satırına alabilirsiniz.

int main()
{
    char* cwd[PATH_MAX]; // Current working directory alındı ve programın başında kullanıcının bilmesi açısından ekrana yazdırıldı.
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current directory: %s\n", cwd);
    } else {
        perror("getcwd() error");
        return 1;
    }


    // Dosya okuma için kullanıcıdan alacağımız dosya ismi için memory de dynamic memory ayrıdlı..

    char *dosyaAdi;
    dosyaAdi = (char*)(calloc(10,sizeof(char)));

    if (dosyaAdi == NULL)
    {
        printf("Memory not allocated.\n");
        exit(0);
    }
    printf("\n");
    printf("Okunmasini istediginiz dosya adini giriniz\n (myfile.pmk fakat kullanici sadece myfile girmelidir. .pmk yi program otomatik eklemektedir.) : ");
    scanf("%s",dosyaAdi);
    printf("\n");

    //Girilen dosya uzunluğu çok uzun olursa memoryde ona ayrılan yeri arttırmak amacıyla yazıldı.
    int dosyaAdiUzunlugu =0;

    int index =0;
    while(((int)dosyaAdi[index]) != 0)
    {
        dosyaAdiUzunlugu++;
        index++;
    }

    if(dosyaAdi[9] != 0)
    {
        dosyaAdi = realloc(dosyaAdi,sizeof(char)*(dosyaAdiUzunlugu/10));
    }

    //Dosya isminin sonuna .pmk ve .txt ekleri eklendi

    strcat(dosyaAdi,".pmk");
    strcat(dosyaAdi,".txt");

    FILE *dosya; // Dosya okuma işlemi.

    dosya = fopen(dosyaAdi,"r");

    if((dosya = fopen(dosyaAdi,"r"))== NULL)
    {
        printf("Dosya acilamadi. \n");
        exit(0);
    }

    char ch,*ptr;

    int uzunluk =0; // Dosyadayi karakter sayısını bulmak için yazdığımız metot

    while((ch = fgetc(dosya)) != EOF)
    {
        uzunluk++;
    }

    rewind(dosya); // Dosyadaki karakter sayısını bulduktan sonra pointer dosya başına tekrar getirildi.


    ptr = (char*)malloc(uzunluk*sizeof(char));

    if(ptr== NULL)
    {
        printf("Memory tahsis edilemedi. \n");
        exit(0);
    }

    int sayac =0; //Bize malloc ile oluşturduğumuz array'in indeks atamalrı için kullanıldı. Dosyadaki tüm karakterler ptr pointerına dynamic olarak atandı

    while((ch = fgetc(dosya)) != EOF)
    {
        ptr[sayac] = ch;
        sayac++;
    }
    rewind(dosya);

    printf("Kullanicinin olup bitenleri daha rahat gorebilmesi amaciyla bazi printler ekrana yazdirilacaktir.\n");
    printf("\n");

    // ptr deki her bir karakteri tek tek alarak pathmaker programımızda kullandık

    for(int indeks=0; indeks<uzunluk;indeks++)
    {
        //Control structure lar ve commandlar ın hepsi harfle başladığı için ilk harf görüşünde if içerisine girer.

        if((ptr[indeks] >= 'a' && ptr[indeks] <='z') || (ptr[indeks] >= 'A' && ptr[indeks] <= 'Z'))
        {
            /*
                Command larda veya control structure larda hata olduğunda, hangi indeksteki yapıda sorun olduğunu hata mesajı
                yazdıracağımızda kullanmak için indeksi kaydettik.
            */

            int indeksForCommand = indeks;

            char *subString = (char*)malloc(40* sizeof(char)); // 40 karakter yeterli görülmüştür. (if, ifnot, go, make yapıları depolanacak)
            int counter =0; // subStringin kaç harfli olduğunu görmemizi sağlayacak, aynı zamanda indekslemede kullanılacak.
            while(
                  ((ptr[indeks] >= 'a' && ptr[indeks] <='z')) || (ptr[indeks] >= 'A' && ptr[indeks] <= 'Z') ||
                  (Sayimi((char)ptr[indeks])== 1)
                 )
            {

                subString[counter] = ptr[indeks];
                counter++;
                indeks++; // substring'ten bir sonraki karakterde şu anda (if, ifnot, go, make yapılarından herhangi birisinden bir sonraki indekste)
            }


            subString[counter] = '\0'; // Son eleman belirtilerek olası memory hataları önlendi.

            //Directory kurallara uygun mu o kontrol edilecek. < > işaretleri arasındaki tüm stringler burada tutulacak.

            char *directoryName = (char*)malloc(100* sizeof(char));

            if(ptr[indeks]=='<' || ptr[indeks+1] =='<')
            {
                if(ptr[indeks+1] == '<') // Arada boşluk varsa ekstra indeks bir kaydırıldı. Bu sayede boşluklar göz ardı edildi.
                {
                    indeks++;
                }
                indeks++;
                if(ptr[indeks] == '/')
                {
                    printf("%d indeksindeki directory belirtilen < > kisimda / isaretleri yanlis kullanilmis. Baslangicta / isareti kullanilmamalidir.",indeks);
                    break;
                }

                int counter = 0;

                while(ptr[indeks] != '>')
                {
                    if(ptr[indeks] == ' ')// Path te olabilecek boşlukları görmezden gelmek için yazıldı.
                    {
                        indeks++;
                        continue;
                    }
                    if(ptr[indeks] == ';') // Directory satır sonu işareti (;) gelmeden kapatılmadıysa hata verir.
                    {
                        printf("%d indeksindeki Directory satir sonu gelmeden duzgun bir sekilde olusturulmamis.\n",indeks);
                        return 0;
                    }
                    if(ptr[indeks] == '/' && ptr[indeks+1] == '>')
                    {
                        printf("%d indeksindeki directory belirtilen kisimda / isaretleri yanlis kullanilmis Directory bitisinde / isareti kullanilmamalidir.",indeks);
                        return 0;
                    }
                    directoryName[counter] = ptr[indeks];
                    counter++;
                    indeks++;
                }
                directoryName[counter] = '\0';
            }
            else
            {
                printf("Path olusturulmamis");
                return 0;
            }
            char *tokenCheck = (char*)malloc(100* sizeof(char));

            strcpy(tokenCheck,directoryName);
            /*Directory path in ortasında veya sonunda * işareti olamayacağı kontrolünü gerçekleştiriyor.
            Bunun için / işaretlerinden tokenleyip yerlerine bakıyor. Başta bir kez * işaretlerini görürse tokenState i false
            yapıp ileride normal directory isimlerinden sonra * işareti geldiğinde hata mesajı veriyor.
            */
            char* token1;
            token1 = strtok(tokenCheck,"/");
            bool tokenState = true;
            while(token1 != NULL)
            {
                if(strcmp(token1,"*")==0 && tokenState==true)
                {
                    while(token1 != NULL && strcmp(token1,"*")==0)
                    {
                        token1 = strtok(NULL,"/");
                    }
                    tokenState = false;
                }
                else
                {
                    if(strcmp(token1,"*")==0)
                    {
                        printf("Path basindan sonra * isareti olmamalidir (Ornek : <hi/*/there> ).\n");
                        return 0;
                    }
                    else
                    {
                        token1 = strtok(NULL,"/");
                        tokenState = false;
                    }
                }
            }
            //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* İşlem kısmı


            if(KeywordMu(subString,counter) != "-1")
            {
                char keyword[counter+1];
                strcpy(keyword,KeywordMu(subString,counter));
                keyword[counter+1] = '\0';

                //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* Keywordlere göre işlemler

                if(strcmp(keyword,"if") == 0)
                {
                    printf("if kontrol yapisina girdi\n");
                    /*
                    Windows pathleri \ şeklinde veriyor. Bunu chdir, mkdir gibi fonksiyonlarda kullanmak için "\" işaretini "/"
                    işaretiyle değiştirdik. Burada oluşturduğumuz temel prensibi diğer commandler ve control structure ifadeleri için
                    (ifnot, go, make) ufak güncellemelerle kopyaladık ilerideki kısımlarda
                    */

                    char cwdWithSlash[PATH_MAX];
                    char* tokenForCwd;
                    cwdWithSlash[0] = '\0';
                    tokenForCwd = strtok(cwd,"\\");
                    strcat(cwdWithSlash,tokenForCwd);
                    tokenForCwd = strtok(NULL,"\\");
                    while(tokenForCwd != NULL)
                    {
                        strcat(cwdWithSlash,"/");
                        strcat(cwdWithSlash,tokenForCwd);
                        tokenForCwd = strtok(NULL,"\\");
                    }

                    char tempPath2[PATH_MAX]; // cwd ye eşit ve bunu * işareti directory değiştirdiğinde geri dönebilmek için kullanacağız.
                    for(int i=0;i<sizeof(cwdWithSlash);i++)
                    {
                        tempPath2[i] = cwdWithSlash[i];
                    }

                    //cwd yi directory name lerine göre güncellemek için directoryName tokenlerine ayrıldı.
                    printf("< > isaretleri arasindaki Directory : %s\n",directoryName);
                    char *tokenCheck2 = (char*)malloc(100* sizeof(char));
                    strcpy(tokenCheck2,directoryName);

                    char* token2;
                    token2 = strtok(tokenCheck2,"/");
                    int sayacForToken = 0;
                    getcwd(cwd,sizeof(cwd));
                    /*
                    Tokenlemeden önce sürekli cwd alındı. Bu yapılmadığında current cwd C: olarak kaldığı zamanlar oldu.
                        Örneğin chdir("/osman") yaptığımızda C:/osman oldu
                    */
                    while(token2 != NULL)
                    {
                        if(strcmp(token2,"*")==0)
                        {
                            chdir("..");
                            getcwd(cwd,sizeof(cwd));

                        }
                        else
                        {

                            strcat(cwd,"/"); // Tokenlerin başına / koyarak onların path olarak kullanmak için hazırlıyoruz.
                            strcat(cwd,token2);

                        }
                        token2 = strtok(NULL,"/");
                    }
                    if(chdir(cwd) != 0)
                        commandJumper(ptr,&indeks,uzunluk); //directory yoksa komut bloğu ya da satırı atlanmalı

                    //Her chdir çalıştığında current working directory (cwd) değişir ve bu değişimi tekrar cwd ye getcwd metodu ile atandı.
                    getcwd(cwd,sizeof(cwd));

                    /*
                    tempPath2 değişkeni cwd nin modifiye edilmeden önceki halini tutuyordu. if kontrol yapısı cwd yi değiştirmeyeceği
                    için işlemlerimiz bittiğinde cwd yi eski haline getirmek için kullanıldı.
                    */
                    if(chdir(tempPath2) != 0)
                        perror("chdir() error.");

                    getcwd(cwd,sizeof(cwd)); // her chdirden sonra getcwd çağırılıp içerisine cwd değişkeni gönderilmelidir.
                    printf("if kontrol yapisindan cikarken cwd durumu : %s\n",cwd);
                    printf("\n");
                    printf("***********************************************************************");
                    printf("\n");
                }

                else if(strcmp(keyword,"ifnot") == 0)
                {
                    printf("ifnot kontrol yapisina girdi\n");
                    // yukarıda anlatılanlarla aynıları ufak farklılıklarla uygulanmışıtır
                    char cwdWithSlash[PATH_MAX];
                    char* tokenForCwd;
                    cwdWithSlash[0] = '\0';
                    tokenForCwd = strtok(cwd,"\\");
                    strcat(cwdWithSlash,tokenForCwd);
                    tokenForCwd = strtok(NULL,"\\");
                    while(tokenForCwd != NULL)
                    {
                        strcat(cwdWithSlash,"/");
                        strcat(cwdWithSlash,tokenForCwd);
                        tokenForCwd = strtok(NULL,"\\");
                    }

                    char tempPath2[PATH_MAX]; // cwd ye eşit ve bunu * işareti directory değiştirdiğinde geri dönebilmek için kullanacağız.
                    for(int i=0;i<sizeof(cwdWithSlash);i++)
                    {
                        tempPath2[i] = cwdWithSlash[i];
                    }


                    printf("< > isaretleri arasindaki Directory : %s\n",directoryName);
                    char *tokenCheck2 = (char*)malloc(100* sizeof(char));
                    strcpy(tokenCheck2,directoryName);

                    char* token2;
                    token2 = strtok(tokenCheck2,"/");
                    getcwd(cwd,sizeof(cwd));
                    /*
                    Tokenlemeden önce sürekli cwd alındı. Bu yapılmadığında current cwd C: olarak kaldığı zamanlar oldu.
                        Örneğin chdir("/osman") yaptığımızda C:/osman oldu
                    */
                    while(token2 != NULL)
                    {
                        if(strcmp(token2,"*")==0)
                        {
                            chdir("..");
                            getcwd(cwd,sizeof(cwd));

                        }
                        else
                        {

                            strcat(cwd,"/");
                            strcat(cwd,token2);

                        }
                        token2 = strtok(NULL,"/");
                    }
                    if(chdir(cwd) != 0)
                        commandJumper(ptr,&indeks,uzunluk); //directory "varsa" komut bloğu ya da satırı atlanmalı



                    getcwd(cwd,sizeof(cwd));


                    if(chdir(tempPath2) != 0)
                        perror("chdir() error.");

                    getcwd(cwd,sizeof(cwd));
                    printf("ifnot kontrol yapisindan cikarken cwd durumu : %s\n",cwd);
                    printf("\n");
                    printf("***********************************************************************");
                    printf("\n");
                }

                else if(strcmp(keyword,"go") == 0)
                {
                    printf("go command ine girdi\n");
                    /*
                    Go komutu farklı olarak gidilen directory de kalmayı gerektirir bu yüzden ilk durumun tutulmasına gerek yoktur.
                    */
                    printf("< > isaretleri arasindaki Directory : %s\n",directoryName);
                    char *tokenCheck2 = (char*)malloc(100* sizeof(char));
                    strcpy(tokenCheck2,directoryName);
                    char* token2;
                    token2 = strtok(tokenCheck2,"/");
                    getcwd(cwd,sizeof(cwd));
                    /*
                    Tokenlemeden önce sürekli cwd alındı. Bu yapılmadığında current cwd C: olarak kaldığı zamanlar oldu.
                        Örneğin chdir("/osman") yaptığımızda C:/osman oldu
                    */
                    while(token2 != NULL)
                    {
                        if(strcmp(token2,"*")==0)
                        {
                            chdir("..");
                            getcwd(cwd,sizeof(cwd));

                        }
                        else
                        {
                            strcat(cwd,"/");
                            strcat(cwd,token2);

                            if(chdir(cwd) != 0) //directory'e gidilmeli yoksa hata verilmeli
                            {
                                printf("%d. indeksteki go command inde verilen directory'e gidilemiyor. \n",indeksForCommand);
                                return 0;
                            }

                        }
                        token2 = strtok(NULL,"/");
                    }
                    getcwd(cwd,sizeof(cwd));
                    printf("go command inden cikarken cwd durumu : %s\n",cwd);
                    printf("\n");
                    printf("***********************************************************************");
                    printf("\n");
                }

                else if(strcmp(keyword,"make") == 0)
                {
                    printf("make command ine girdi girdi\n");
                    /*
                    Make komutu biraz özel bir komut. Diğerlerinde directorylerde gezerken make komutu ile kendi directory lerimizi
                    yapıyoruz. Burada make komutu current working directory i (cwd) değiştirmediği için şimdiki durumun tutulması gerekiyor
                    */
                    char cwdWithSlash[PATH_MAX];
                    char* tokenForCwd;
                    cwdWithSlash[0] = '\0';
                    tokenForCwd = strtok(cwd,"\\");
                    strcat(cwdWithSlash,tokenForCwd);
                    tokenForCwd = strtok(NULL,"\\");
                    while(tokenForCwd != NULL)
                    {
                        strcat(cwdWithSlash,"/");
                        strcat(cwdWithSlash,tokenForCwd);
                        tokenForCwd = strtok(NULL,"\\");
                    }

                    char tempPath2[PATH_MAX]; // cwd ye eşit ve bunu * işareti directory değiştirdiğinde geri dönebilmek için kullanacağız.
                    for(int i=0;i<sizeof(cwdWithSlash);i++)
                    {
                        tempPath2[i] = cwdWithSlash[i];
                    }

                    printf("< > isaretleri arasindaki Directory : %s\n",directoryName);
                    char *tokenCheck2 = (char*)malloc(100* sizeof(char));
                    strcpy(tokenCheck2,directoryName);

                    char* token2;
                    token2 = strtok(tokenCheck2,"/");
                    int sayacForToken = 0;
                    getcwd(cwd,sizeof(cwd));
                    /*
                    Tokenlemeden önce sürekli cwd alındı. Bu yapılmadığında current cwd C: olarak kaldığı zamanlar oldu.
                        Örneğin chdir("/osman") yaptığımızda C:/osman oldu
                    */

                    bool stateForMake = true;
                    while(token2 != NULL)
                    {
                        if(strcmp(token2,"*")==0)
                        {
                            chdir("..");
                            getcwd(cwd,sizeof(cwd));
                        }
                        else
                        {

                            strcat(cwd,"/");
                            strcat(cwd,token2);
                            //Creating Directory
                            /*
                                Directory halihazırda varsa path i o directorye taşıyor.
                                Yoksa oluşturmaya çalışıyor: Eğer oluşturamazsa directorynin olduğu anlamına geliyor. Burada kullanılan
                                stateForMake in mantığı oluşturmak istediğimiz bütün directoryler varsa (chdir ile kontrol ediyoruz aynı zamanda da
                                path imizi ilerletiyoruz) else durumuna hiç girmeden sonlanacağı için stateForMake bool değişkeni true olarak kalıyor
                                ve biz buradan oluşturmak istediğimiz tüm directorylerin olduğunu anlıyoruz ve hata vermesi amacıyla bilinçli olarak
                                mkdir fonksiyonunu directory oluşturmaya çalışması için tekrar çağırıyoruz. Tabii ki bu çağırma işlemi stateForMake in
                                hangi durumda olduğuna bağlı. Böylece directoryler varsa hata vermiş oluyor. Bir kısmı varsa kalan kısmını oluşturup
                                hata vermeden işlemi bitiriyor.

                            */
                            if(chdir(cwd)==0)
                            {
                                getcwd(cwd,sizeof(cwd));
                            }
                            else
                            {
                                stateForMake = false;
                                int status;
                                status = mkdir(cwd);

                                if(status != 0)
                                {
                                    printf("%d indeksindeki make command inde beklenmedik hata. \n",indeksForCommand);
                                    return 0;
                                }
                            }
                        }
                        token2 = strtok(NULL,"/");
                    }

                    if(stateForMake == true)
                    {
                        int status;
                        status = mkdir(cwd);

                        if(status != 0)
                        {
                            printf("HATA : %d indeksindeki make command i Directory olusturamadi. Ayni isimde directory olmadigindan emin olunuz.\n",indeksForCommand);

                        }
                    }

                    /*
                    Current working directory  cwd ye atandı tekrar. Bu bizim / işaretlerini windwosun path ifade ederken kullandığı
                    \ işaretlerine dönüştürmemizi sağlıyor. Evet burası biraz karışık. chdir mkdir gibi fonksiyonlar / işareti isterken
                    getcwd komutu \ işaretiyle döndürüyor cwd yi. Birbirlerine parametre olarak verirken bizim programımızda
                    yaptığımız değişikliklerin yapılması gerekiyor
                    */
                    getcwd(cwd,sizeof(cwd));


                    // Current working directory nin değişmesi önlendi.
                    if(chdir(tempPath2) != 0)
                        perror("chdir() error.");

                    getcwd(cwd,sizeof(cwd));
                    printf("make command inden cikarken cwd durumu : %s\n",cwd);
                    printf("\n");
                    printf("***********************************************************************");
                    printf("\n");

                }
                else
                {
                    //Dummy
                }

            }
            // (if, go, make, ifnot) tan başka stringler olursa diye ekstra bir kontrol
            else
            {
                printf("Tanimlanamayan string. Lutfen dosyayi kontrol ediniz.\n");
                return 0;
            }
            free(subString);
            free(directoryName);

        }
        else
        {
            //Dummy
        }



    }
    printf("\n");
    printf("PathMaker kritik bir hatayla karsilasmadan calisti. Programimizi kullandiginiz icin tesekkur ederiz :) \n");
    free(dosyaAdi);
    return 0;
}

/*
    İf ve ifnot gibi control structure ları şartları sağlanmazsa kod satırının/ bloğunun işleme sokulmadan atlanmasını sağlar.
*/

void commandJumper(char *ptr,int* indeks,int uzunluk) // Şartlar sağlanmazsa kod bloğunun / satırının atlanmasını sağlar.
{
    printf("Jumper calisti\n");
    int temp = *indeks;
    if(ptr[temp+2] == '{' || ptr[temp+3] == '}')
    {
        while(ptr[temp] != '}')
        {
            if(temp >= uzunluk)
            {
                printf("Kod blogu dosya bitiminden once sonlandirilmadi.");
                *indeks = temp;
                return;
            }
            else
            {
                temp++;
            }
        }
        *indeks = temp;
    }
    else
    {
        while(ptr[temp] != '}')
        {
            if(temp >= uzunluk)
            {
                printf("Kod satiri dosya bitiminden once sonlandirilmadi.\n");
                *indeks = temp;
                return;
            }
            else
            {
               temp++;
            }
        }
        *indeks = temp;

    }
}

// < > arasında verilecek olan / işaretiyle ayrılan Directory lerin sayısını bulma amacıyla yazıldı. Kullanılmasına gerek görülmedi.

int tokenSay(char str[])
{
    char* token;
    token = strtok(str,"/");
    int tokenSayisi =0;
    while(token != NULL)
    {
        tokenSayisi++;
        token = strtok(NULL,"/");

    }
    return tokenSayisi;
}



//Okunan stringlerin keyword olup olmadığına baktığımız metot.

char* KeywordMu(char stringToken[],int uzunluk) // Keywordler case sensitive olmalı
{
    char keywords[4][7] =
    {
         {'i','f','\0'}, {'i','f','n','o','t','\0'}, {'m','a','k','e','\0'}, {'g','o','\0'}
    };

    int i;
    for(i=0;i<4;i++)
    {

        if(strncmp(keywords[i],stringToken,uzunluk)==0)
        {
            char *st = (char*)(calloc(7,sizeof(char)));
            strcpy(st,keywords[i]);
            return st;
        }
    }

    return "-1";

}
// bir char ın "rakam" olup olmadığını anladığımız metot.
int Sayimi(char ch)
{
    if(ch == '0' || ch == '1' || ch == '2' || ch == '3' || ch == '4' ||
       ch == '5' || ch == '6' || ch == '7' || ch == '8' ||ch == '9'
       )
    {
        return 1;
    }
    else
    {
        return -1;
    }
}
