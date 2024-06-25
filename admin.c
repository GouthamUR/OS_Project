#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <string.h>
#include <arpa/inet.h>

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

  int sock;
  struct sockaddr_in addr;
  socklen_t addr_size;
  char buffer[1024];
  int n;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0){
    perror("[-]Socket error");
    exit(1);
  }
  printf("[+]TCP server socket created.\n");

  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = inet_addr(ip);

  connect(sock, (struct sockaddr*)&addr, sizeof(addr));
  printf("Connected to the server.\n");

    bzero(buffer, 1024);
    strcpy(buffer, "HELLO, THIS IS ADMIN.");
    send(sock, buffer, strlen(buffer), 0);

    bzero(buffer, 1024);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);

    // int number;
    // recv(sock, &number, sizeof(int), 0);
    // printf("Number of unique products=%d\n",number);
    // printf("Please choose the productids accordingly\n");


    // choice menu
    printf("Choose an option:\n");
    printf("1.Add a new product\n");
    printf("2.Delete a product\n");
    printf("3.Update Price for an existing product\n");
    printf("4.Update Quantity for an existing product\n");

    int choice;
    scanf("%d",&choice);
    send(sock,&choice,sizeof(int),0);
    if(choice==1){
        printf("Name of the product:");
        char name[50];
        bzero(name, 50);

        scanf("%s",name);
        send(sock, name, strlen(name), 0);


        printf("Quantity to be added:");
        int quantity;
        scanf("%d",&quantity); 
        send(sock,&quantity,sizeof(int),0);

        printf("Price of the new product:");
        int price;
        scanf("%d",&price);
        send(sock,&price,sizeof(int),0);

        sleep(15);

        printf("Product has been added successfully\n");

    }
    else if(choice==2){
      printf("Enter the productID of the product to be deleted:");
      int id;
      scanf("%d",&id);
      send(sock,&id,sizeof(int),0); 
      
      int result;
      recv(sock, &result, sizeof(int), 0);
      if(result==1) printf("Product successfully deleted\n");
      else printf("Product doesn't exist in the store\n");

    }
    else if (choice==3){
      printf("Enter the productID of the product whose price has to be updated:");
      int id;
      scanf("%d",&id);
      send(sock,&id,sizeof(int),0);

      printf("Enter the new price:");
      int nprice;
      scanf("%d",&nprice);
      send(sock,&nprice,sizeof(int),0);    
      
      int result;
      recv(sock, &result, sizeof(int), 0);
      if(result==1) printf("Price successfully updated\n");
      else printf("Product doesn't exist in the store\n");
    }

    else if (choice==4){
      printf("Enter the productID of the product whose quantity has to be updated:");
      int id;
      scanf("%d",&id);
      send(sock,&id,sizeof(int),0);

      printf("Enter the new quantity:");
      int new_quan;
      scanf("%d",&new_quan);
      send(sock,&new_quan,sizeof(int),0);    
      
      int result;
      recv(sock, &result, sizeof(int), 0);
      if(result==1) printf("Quantity successfully updated\n");
      else printf("Product doesn't exist in the store\n");
    }
    
    // int num;
    // recv(sock,&num,sizeof(int),0);
    // printf("Received number: %d\n",num);
    // for(int j=0;j<num;j++){
    //   char pro_name[50];
    //   int id,price,quantity,valid;
    //   bzero(pro_name,50);
    //   recv(sock, pro_name, sizeof(pro_name), 0);

    //   recv(sock, &id, sizeof(int), 0);
    //   recv(sock, &price, sizeof(int), 0);
    //   recv(sock, &quantity, sizeof(int), 0);
    //   recv(sock, &valid, sizeof(int), 0);

    //   // printf("Received %d\n",id);
    //   // printf("%d\n",valid);
    //   printf("%d %s %d %d\n",id,pro_name,quantity,price);


    //   if(valid==1){
    //     printf("%d %s %d %d\n",id,pro_name,quantity,price);
    //   }



    


 

  close(sock);
  printf("Disconnected from the server.\n");

  return 0;

}