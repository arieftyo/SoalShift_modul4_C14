# SoalShift_modul4_C14


Suatu ketika King of YouTube ASEAN, Atta Halilintar, mengunjungi laboratorium AJK karena urusan yang benar-benar genting. Prestasi dan ketenarannya ternyata menarik perhatian hacker yang ingin mencuri video-video berkualitas milik Atta, bahkan ingin mencuri password akun YouTube miliknya. Menurut intel Atta, pemimpin hacker yang berusaha menyerangnya adalah:

Hacker LAPTOP_RUSAK
Hacker tersebut menyusup ke dalam file system yang ada di dalam PC Atta dengan mudah karena ternyata file system tersebut sama sekali tidak memiliki sistem pengamanan. Maka dari itu maksud Atta ke laboratorium AJK karena dia ingin kalian untuk mengembangkan file system yang aman.
Atta menginginkan nama file system yang akan kalian buat adalah “AFSHiaAP”, yang nama kepanjangannya adalah AHHA’s File System: Hierarchical and Advanced Protection. Berikut ini kebutuhan yang harus kalian penuhi:


1. Semua nama file dan folder harus terenkripsi Enkripsi yang Atta inginkan sangat sederhana, yaitu Caesar cipher. Namun, Kusuma mengatakan enkripsi tersebut sangat mudah dipecahkan. Dia menyarankan untuk character list diekspansi,tidak hanya alfabet, dan diacak. Berikut character list yang dipakai:

qE1~ YMUR2"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\8s;g<{3.u*W-0

Misalkan ada file bernama “halo” di dalam folder “INI_FOLDER”, dan key yang dipakai adalah 17, maka:
“INI_FOLDER/halo” saat belum di-mount maka akan bernama “n,nsbZ]wio/QBE#”, saat telah di-mount maka akan otomatis terdekripsi kembali menjadi “INI_FOLDER/halo” .
Perhatian: Karakter ‘/’ adalah karakter ilegal dalam penamaan file atau folder dalam *NIX, maka dari itu dapat diabaikan


### Jawab :


```
void enkripsi(char * enkrip){
    if(strcmp("..", enkrip) ==0 || strcmp(".", enkrip)==0){
        return;
    }
    for(int i=0;i<strlen(enkrip);i++){
        for(int j=0;j<length_cc;j++){
            if(enkrip[i]==cc[j]){
                enkrip[i]= cc[(j+key_en)%length_cc];
                break;
            }
        }
    }
}

void dekripsi(char * dekrip){
    if(strcmp("..", dekrip) ==0 || strcmp(".", dekrip)==0){
        return;
    }
    for(int i=0;i<strlen(dekrip);i++){
        for(int j=0;j<length_cc;j++){
            if(dekrip[i]==cc[j]){
                dekrip[i]= cc[(j+key_dec)%length_cc];
                break;
            }
        }
    }
}


```

Untuk pengenkripsian dan sebaliknya dilakukan di fungsi enkripsi dan dekripsi. Fungsi enkripsi yakni menukar huruf yang ada dengan huruf yang sama pada list karakter yang dipakai dengan menambahkan urutannya dengan 17. Untuk dekripsinya ditambahkan 77 karena jumlah total dari list karakter tersebut adalah 94 sehingga jika hasilnya lebih dari 94 maka akan kembali lagi menuju list karakter yang pertama. Lalu fungsi enkripsi dijalankan di semua fungsi yang berhubungan dengan pembuatan dan pembacaan file atau direktori.


2. Semua file video yang tersimpan secara terpecah-pecah (splitted) harus secara otomatis tergabung (joined) dan diletakkan dalam folder “Videos” Urutan operasi dari kebutuhan ini adalah:
Tepat saat sebelum file system di-mount
Secara otomatis folder “Videos” terbuat di root directory file system
Misal ada sekumpulan file pecahan video bernama “computer.mkv.000”, “computer.mkv.001”, “computer.mkv.002”, dst. Maka secara otomatis file pecahan tersebut akan di-join menjadi file video “computer.mkv”
Untuk mempermudah kalian, dipastikan hanya video file saja yang terpecah menjadi beberapa file. File pecahan tersebut dijamin terletak di root folder fuse
Karena mungkin file video sangat banyak sehingga mungkin saja saat menggabungkan file video, file system akan membutuhkan waktu yang lama untuk sukses ter-mount. Maka pastikan saat akan menggabungkan file pecahan video, file system akan membuat 1 thread/proses(fork) baru yang dikhususkan untuk menggabungkan file video tersebut
Pindahkan seluruh file video yang sudah ter-join ke dalam folder “Videos”
Jangan tampilkan file pecahan di direktori manapun
Tepat saat file system akan di-unmount
Hapus semua file video yang berada di folder “Videos”, tapi jangan hapus file pecahan yang terdapat di root directory file system
Hapus folder “Videos” 

### Jawab :
Karena operasi ini dijalankan sebelum file system di-mount maka fungsi FUSE yang digunakan adalah fungsi xmp_init. Lalu untuk mengecek apakah file tersebut berekstensi .000 menggunakan fungsi isVideo. Untuk menggabungkan file file tersebut menggunakan fungsi gabungvideo :

```
void *gabungVideo(void *param){
    DIR *dp;
    struct dirent *de;
    char *namadepan = (char *) param;
    dp = opendir(dirpath);
    char newVideo[1000];
    char encVideo[1000];
    sprintf(encVideo, "%s", namadepan);
    enc(encVideo);
    sprintf(newVideo, "%s/%s", videosPath, encVideo);
    creat(newVideo, 0777);
    printf("gabung video %s\n", newVideo);
    while((de = readdir(dp)) != NULL){
        char temp[1000];
        sprintf(temp, "%s", de->d_name);
        dec(temp);
        printf("part file %s compare %s \n", temp, namadepan);
        if (strstr(temp, namadepan)){
            FILE *fp = fopen(newVideo, "a+");
            printf("part file status ok");
            char namafile[1000];
            sprintf(namafile, "%s/%s", dirpath, de->d_name);
            printf(" opening %s file and begin to copying ", namafile);
            FILE *fp2 = fopen(namafile, "r");
            
            size_t n, m;
            unsigned char buff[8192];
            do {
                    n = fread(buff, 1, sizeof buff, fp2);
                    if (n) m = fwrite(buff, 1, n, fp);
                    else   m = 0;
            } while ((n > 0) && (n == m));
            
            fclose(fp2);
            fclose(fp);
            printf("\nsucces copying %s\n", temp);
        }
    }
    printf("gabung video selesai\n");
}
```



3. Sebelum diterapkannya file system ini, Atta pernah diserang oleh hacker LAPTOP_RUSAK yang menanamkan user bernama “chipset” dan “ic_controller” serta group “rusak” yang tidak bisa dihapus. Karena paranoid, Atta menerapkan aturan pada file system ini untuk menghapus “file bahaya” yang memiliki spesifikasi:
Owner Name 	: ‘chipset’ atau ‘ic_controller’
Group Name	: ‘rusak’
Tidak dapat dibaca
Jika ditemukan file dengan spesifikasi tersebut ketika membuka direktori, Atta akan menyimpan nama file, group ID, owner ID, dan waktu terakhir diakses dalam file “filemiris.txt” (format waktu bebas, namun harus memiliki jam menit detik dan tanggal) lalu menghapus “file bahaya” tersebut untuk mencegah serangan lanjutan dari LAPTOP_RUSAK.


### Jawab :

Untuk nomer 3 ditambahkan pengecekan owner name dan group name dari suatu file yang ada dalam folder. Maka dari pengecekan dilakukan pada fungsi readdir. owner name didapatkan melalui fungsi getpwuid dan untuk groupname didapta melalui fungsi getgrid dan disimpan dalam file bernama grup.

```
if( (strcmp(name->pw_name,"chipset") == 0 || strcmp(name->pw_name,"ic_controller") == 0) 
    && strcmp(grup->gr_name,"rusak")==0 
    && ((tmp.st_mode & S_IRUSR) == 0 || (tmp.st_mode & S_IRGRP) == 0 || (tmp.st_mode & S_IROTH) == 0) )
{
    printf("%s\n",file);
    char root[1000];
    strcpy(root,dirpath);
    char note[10000] = "/filemiris.txt";
    enc(note);
    strcat(root,note);
    FILE * fp;
    fp = fopen (root, "a+");
    char t[1000];
    time_t now = time(NULL);
    strftime(t, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
    char buffer[1000];
    sprintf(buffer,"%s%s-%d-%d-%s",path,temp,name->pw_uid,grup->gr_gid,t);
    fprintf(fp,"%s\n",buffer);
    remove(file);
    fclose(fp);
    chown(root,1000,1000);
}
else{
    memset(&st, 0, sizeof(st));
    st.st_ino = de->d_ino;
    st.st_mode = de->d_type << 12;
    strcpy(temp, de->d_name);
    dekripsi(temp);
    int res = (filler(buf, temp, &st, 0));
    if (res != 0)
    break;
}
```
Jadi jika ada file yang memiliki extensi tersebut maka akan menyimpan nama file, group ID, owner ID, dan waktu terakhir diakses dalam file “filemiris.txt” dan menghapus file tersebut. 


4. Pada folder YOUTUBER, setiap membuat folder permission foldernya akan otomatis menjadi 750. Juga ketika membuat file permissionnya akan otomatis menjadi 640 dan ekstensi filenya akan bertambah “.iz1”. File berekstensi “.iz1” tidak bisa diubah permissionnya dan memunculkan error bertuliskan “File ekstensi iz1 tidak boleh diubah permissionnya.”

### Jawab :
untuk fungsi mkdir ditambahkan :

```
if(strlen(name)>9 && strncmp(name,"/YOUTUBER",9)==0){
    	enkripsi(name);
    	mode = 0750;
    	sprintf(fpath, "%s%s",dirpath,name);
    	res = mkdir(fpath, mode);
    }
    else{
    	enkripsi(name);
    	sprintf(fpath, "%s%s",dirpath,name);
    	res = mkdir(fpath, mode);
    }
```
Supaya jika membuat direktori didalam direktori Youtuber maka modenya akan menjadi 0750. Namun jika tidak, maka mode nya akan diatur secara default.

lalu untuk fungsi create ditambahkan :

```
if(length != 9 && strncmp(name,"/YOUTUBER",9)==0)
	{
		strcat(name,".iz1");
		mode = 0640;
		enkripsi(name);
		sprintf(fpath, "%s%s",dirpath,name);
    	res = creat(fpath, mode);
	}
	else{
    	enkripsi(name);
    	sprintf(fpath, "%s%s",dirpath,name);
    	res = creat(fpath, mode);
	}
```

supaya pada pembuatan file di direktori Youtuber nama file tersebut akan ditambahkan .iz1 pada belakang nama file.

Dan yang terakhir yakni adanya penambahan pada fungsi chmod :

```
if(strlen(name)>9 && strncmp(name,"/YOUTUBER",9)==0 && strcmp(name+strlen(name)-4,".iz1")==0)
	{
		child=fork();
		if(child==0){
			char *argv[5] = {"zenity", "--error","--text=File ekstensi iz1 tidak boleh diubah permissionnya.","--title=Tidak bisa merubah", NULL};
     		execv("/usr/bin/zenity", argv);
		}
	}
	else{
	 sprintf(fpath, "%s%s",dirpath,name);
	 res = chmod(fpath, mode);
	}
```
Penambahan tersebut ditambahkan untuk membuat file .iz1 tidak dapat diubah permisionnya dan jika diubah akan muncul peringatan.

5. Ketika mengedit suatu file dan melakukan save, maka akan terbuat folder baru bernama Backup kemudian hasil dari save tersebut akan disimpan pada backup dengan nama namafile_[timestamp].ekstensi. Dan ketika file asli dihapus, maka akan dibuat folder bernama RecycleBin, kemudian file yang dihapus beserta semua backup dari file yang dihapus tersebut (jika ada) di zip dengan nama namafile_deleted_[timestamp].zip dan ditaruh ke dalam folder RecycleBin (file asli dan backup terhapus). Dengan format [timestamp] adalah yyyy-MM-dd_HH:mm:ss

Jawab :

Pada soal ini fungsi yang digunakan adalah fungsi unlink dan fungsi write. Fungsi unlink digunakan jika terjadi penghapusan file. Ketika file dihapus maka folder baru akan terbentuk yakni folder RecycleBin. Dan file-file yang terhapus tersebut akan dikompres menjadi .zip dana diletakkan di folder RecycleBin yang telah dibuatkan sebelumnya. Fungsi write digunakan ketika file dietdit maka akan dibuat backupnya dan menambahkan timestamp pada nama filenya dengan aturan yang sama dengan yang ada disoal. Untuk codenya bisa dilihat dicode yang ada.
```

```

Catatan: 
Semua nama file dan folder yang terdapat pada soal harus memenuhi syarat soal no. 1 (terenkripsi saat belum di-mount dan terdekripsi saat di-mount)
Tidak diperkenankan menggunakan system() dan exec*(), kecuali ada pengecualian di butir soal.
Untuk soal nomor 3 diharapkan secara manual membuat user dan group-nya. Pengecekan file dilakukan setiap membuka direktori.
Untuk soal nomor 4 dan 5 diperbolehkan menggunakan exec*().
Pengerjaan hanya dilakukan dalam 1 file program C dengan format nama AFSHiaAP_[Kelompok].c . 
File System AFSHiaAP mengarah ke folder /home/[user]/shift4.
