#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/file.h>

struct Product{
    int id;
    int valid;
    char name[50];
    int quantity;
    int price;
};


int main(){

  char *ip = "127.0.0.1";
  int port;
  printf("Enter the port number:");
  scanf("%d",&port);

  int server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size;
  char buffer[1024];
  int n;

  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0){
    perror("[-]Socket error");
    exit(1);
  }
  printf("[+]TCP server socket created.\n");

  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (n < 0){
    perror("[-]Bind error");
    exit(1);
  }
  printf("[+]Bind to the port number: %d\n", port);

  listen(server_sock, 5);
  printf("Listening...\n");

  while(1){
    addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
    printf("[+]Client connected.\n");

    bzero(buffer, 1024);
    recv(client_sock, buffer, sizeof(buffer), 0);
    // printf("Client: %s\n", buffer);

    // for admin
    if(!strcmp(buffer,"HELLO, THIS IS ADMIN.")){
        printf("Connected with admin\n");
        bzero(buffer, 1024);
        strcpy(buffer, "HI, THIS IS SERVER.");
        int fd = open("stock.txt", O_RDWR | O_CREAT, 0644);
        struct Product dummy;
        int number=0;
        while(read(fd,&dummy,sizeof(struct Product))){
            number++;
        }
        // printf("Server: %s\n", buffer);
        // printf("Number of unique products=%d\n",number);
        send(client_sock, buffer, strlen(buffer), 0);
        // send(client_sock,&number,sizeof(int),0);

        int choice;
        recv(client_sock,&choice,sizeof(int),0);

        if(choice==1){
          int quan,price;
          char name[50];
          bzero(name,50);
          recv(client_sock,name,sizeof(name),0);
          recv(client_sock,&quan,sizeof(int),0);
          recv(client_sock,&price,sizeof(int),0);
          struct Product *newpro=(struct Product *) malloc (sizeof(struct Product));
          strcpy(newpro->name,name);
          newpro->id=++number;
          newpro->price=price;
          newpro->quantity=quan;
          newpro->valid=1;

          // record locking
          struct flock lock;
          lock.l_type = F_WRLCK;
          lock.l_whence = SEEK_SET;
          lock.l_start = sizeof(struct Product) * (number - 1);
          lock.l_len = sizeof(struct Product);

              // Acquire the lock
          if (fcntl(fd, F_SETLKW, &lock) == -1) {
              perror("fcntl");
              exit(1);
          }

          // printf("%s %d %d %d",newpro->name,newpro->id,newpro->price,newpro->quantity);
          lseek(fd,0,SEEK_END);
          write(fd,newpro, sizeof(struct Product));
          printf("New product has been added successfully");
          // lseek(fd,0,SEEK_SET);
          // struct Product check[13];
          // read(fd,&check,sizeof(struct Product)*13);
          // for(int i=0;i<13;i++){
          //     printf("%d %d %s %d %d\n",check[i].id,check[i].valid,check[i].name,check[i].quantity,check[i].price);
          // }
          sleep(15);
          // Release the lock
          lock.l_type = F_UNLCK;
          if (fcntl(fd, F_SETLK, &lock) == -1) {
              perror("fcntl");
              exit(1);
          }

        }
        else if(choice==2){
          int id;
          recv(client_sock,&id,sizeof(int),0);
          int result;

          if(id>number){
            result=0;
            send(client_sock,&result,sizeof(int),0);
            continue;
          }
          // record locking
          struct flock lock;
          lock.l_type = F_WRLCK;
          lock.l_whence = SEEK_SET;
          lock.l_start = sizeof(struct Product) * (id - 1);
          lock.l_len = sizeof(struct Product);

          // Acquire the lock
          if (fcntl(fd, F_SETLKW, &lock) == -1) {
              perror("fcntl");
              exit(1);
          }

          // Modifying the attribute valid
          lseek(fd, sizeof(struct Product) * (id - 1), SEEK_SET);
          struct Product check;
          read(fd,&check,sizeof(struct Product));
          if(check.valid==1){
            result=1;
            check.valid=0;
          }
          else{
            result=0;
          }
          lseek(fd, sizeof(struct Product) * (id - 1), SEEK_SET);
          write(fd, &check, sizeof(struct Product));

          sleep(15);

          // Release the lock
          lock.l_type = F_UNLCK;
          if (fcntl(fd, F_SETLK, &lock) == -1) {
              perror("fcntl");
              exit(1);
          }
          
          // sending result
          send(client_sock,&result,sizeof(int),0);
          
          

          
        }
        else if (choice==3){
          int id;
          recv(client_sock,&id,sizeof(int),0);
          int new_price;
          recv(client_sock,&new_price,sizeof(int),0);

          int result;

          if(id>number){
            result=0;
            send(client_sock,&result,sizeof(int),0);
            continue;
          }

          // record locking
          struct flock lock;
          lock.l_type = F_WRLCK;
          lock.l_whence = SEEK_SET;
          lock.l_start = sizeof(struct Product) * (id - 1);
          lock.l_len = sizeof(struct Product);

          // Acquire the lock
          if (fcntl(fd, F_SETLKW, &lock) == -1) {
              perror("fcntl");
              exit(1);
          }

          // Modifying the attribute valid
          lseek(fd, sizeof(struct Product) * (id - 1), SEEK_SET);
          struct Product check;
          read(fd,&check,sizeof(struct Product));
          if(check.valid==1){
            result=1;
            check.price=new_price;
          }
          else{
            result=0;
          }
          lseek(fd, sizeof(struct Product) * (id - 1), SEEK_SET);
          write(fd, &check, sizeof(struct Product));

          sleep(15);
          // Release the lock
          lock.l_type = F_UNLCK;
          if (fcntl(fd, F_SETLK, &lock) == -1) {
              perror("fcntl");
              exit(1);
          }
          
          // sending result
          send(client_sock,&result,sizeof(int),0);
          
          

          
        }
        else if (choice==4){
          int id;
          recv(client_sock,&id,sizeof(int),0);
          int new_quan;
          recv(client_sock,&new_quan,sizeof(int),0);

          int result;
          if(id>number){
            result=0;
            send(client_sock,&result,sizeof(int),0);
            continue;
          }
          // record locking
          struct flock lock;
          lock.l_type = F_WRLCK;
          lock.l_whence = SEEK_SET;
          lock.l_start = sizeof(struct Product) * (id - 1);
          lock.l_len = sizeof(struct Product);

          // Acquire the lock
          if (fcntl(fd, F_SETLKW, &lock) == -1) {
              perror("fcntl");
              exit(1);
          }

          // Modifying the attribute valid
          lseek(fd, sizeof(struct Product) * (id - 1), SEEK_SET);
          struct Product check;
          read(fd,&check,sizeof(struct Product));
          if(check.valid==1){
            result=1;
            check.quantity=new_quan;
          }
          else{
            result=0;
          }
          lseek(fd, sizeof(struct Product) * (id - 1), SEEK_SET);
          write(fd, &check, sizeof(struct Product));

          sleep(15);

          // Release the lock
          lock.l_type = F_UNLCK;
          if (fcntl(fd, F_SETLK, &lock) == -1) {
              perror("fcntl");
              exit(1);
          }
          
          // sending result
          send(client_sock,&result,sizeof(int),0);
          
          

          
        }
        
        

        printf("Printing log file\n");
        lseek(fd,0,SEEK_SET);
        for(int i=0;i<number;i++){
          // printf("Sending %d",i);
          struct Product dummy;
          read(fd,&dummy,sizeof(struct Product));
          char name[50];
          bzero(name,50);
          int id,price,quantity,valid;

          strcpy(name,dummy.name);
          id=dummy.id;
          price=dummy.price;
          quantity=dummy.quantity;
          valid=dummy.valid;


          printf("%d %s %d %d %d\n",id,name,price,quantity,valid);


            
        }

      }

    // for customer
    if(!strcmp(buffer,"HELLO, THIS IS CUSTOMER.")){
        printf("Connected with customer\n");
        bzero(buffer, 1024);
        strcpy(buffer, "HI, THIS IS SERVER.");
        int fd = open("stock.txt", O_RDWR | O_CREAT, 0644);
        struct Product dummy;
        int number=0;
        while(read(fd,&dummy,sizeof(struct Product))){
            number++;
        }
        // printf("Server: %s\n", buffer);
        // printf("Number of unique products=%d\n",number);
        send(client_sock, buffer, strlen(buffer), 0);
        // send(client_sock,&number,sizeof(int),0);
        while(1){
          int choice;
          recv(client_sock,&choice,sizeof(int),0);
          if(choice==-1) break;
          else if(choice==1){
            send(client_sock, &number, sizeof(int), 0);
            lseek(fd,0,SEEK_SET);

            struct flock lock;
            // Set the lock to shared (read) mode
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = sizeof(struct Product)*number;

                // Acquire the lock
            if (fcntl(fd, F_SETLKW, &lock) == -1) {
                perror("fcntl");
                exit(1);
            }

            struct Product product;
            while(read(fd,&product,sizeof(struct Product))){
              send(client_sock, &product.id, sizeof(int), 0);
              send(client_sock, &product.price, sizeof(int), 0);
              send(client_sock, &product.quantity, sizeof(int), 0);
              send(client_sock, &product.valid, sizeof(int), 0);
              send(client_sock, &product.name, sizeof(product.name), 0);
              
            }

                // Release the lock
            lock.l_type = F_UNLCK;
            if (fcntl(fd, F_SETLK, &lock) == -1) {
                perror("fcntl");
                exit(1);
            }
          }
          else if(choice==2 || choice==3){
            int id,quan;
            struct flock lock;
            recv(client_sock,&id,sizeof(int),0);
            recv(client_sock,&quan,sizeof(int),0);
            int result=0;

            // Set the lock to shared (read) mode
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = sizeof(struct Product)*(id-1);
            lock.l_len = sizeof(struct Product);

            if (fcntl(fd, F_SETLKW, &lock) == -1) {
                perror("fcntl");
                exit(1);
            }

            if(id<=number){
              lseek(fd,(sizeof(struct Product))*(id-1),SEEK_SET);
              struct Product obj;
              read(fd,&obj,sizeof(struct Product));
              if(obj.valid==1 && obj.quantity>=quan) result=1;
            }

            lock.l_type = F_UNLCK;
            if (fcntl(fd, F_SETLK, &lock) == -1) {
                perror("fcntl");
                exit(1);
            }
            send(client_sock,&result,sizeof(int),0);

          }
          else if(choice==5){
            int bound;
            recv(client_sock,&bound,sizeof(int),0);
            for(int i=0;i<bound;i++){
              int id,quan,result=0;
              recv(client_sock,&id,sizeof(int),0);
              recv(client_sock,&quan,sizeof(int),0); 

              struct flock lock;
              lock.l_type = F_WRLCK;
              lock.l_whence = SEEK_SET;
              lock.l_start = sizeof(struct Product)*(id-1);
              lock.l_len = sizeof(struct Product);

              if (fcntl(fd, F_SETLKW, &lock) == -1) {
                  perror("fcntl");
                  exit(1);
              }

              if(id<=number){
                lseek(fd,(sizeof(struct Product))*(id-1),SEEK_SET);
                struct Product obj;
                read(fd,&obj,sizeof(struct Product));
                if(obj.valid==1 && obj.quantity>=quan){
                  result=1;
                  obj.quantity-=quan;
                  lseek(fd,(sizeof(struct Product))*(id-1),SEEK_SET);
                  write(fd,&obj,sizeof(struct Product));
                }
              }

              lock.l_type = F_UNLCK;
              if (fcntl(fd, F_SETLK, &lock) == -1) {
                  perror("fcntl");
                  exit(1);
              }
              send(client_sock,&result,sizeof(int),0);
            }    

          }

        }
    }
    close(client_sock);
    printf("[+]Client disconnected.\n\n");

  }

  return 0;
}