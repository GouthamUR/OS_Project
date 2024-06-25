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

struct item{
  int id;
  int quantity;
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
    strcpy(buffer, "HELLO, THIS IS CUSTOMER.");
    send(sock, buffer, strlen(buffer), 0);

    bzero(buffer, 1024);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);

    // int number;
    // recv(sock, &number, sizeof(int), 0);
    // printf("Number of unique products=%d\n",number);
    // printf("Please choose the productids accordingly\n");


    struct item cart[1000000];
    int bound=0;
    while(1){
      // choice menu
      printf("Choose an option:\n");
      printf("1.List Products\n");
      printf("2.Add an item to cart\n");
      printf("3.Edit cart\n");
      printf("4.Display cart\n");
      printf("5.Navigate to payment menu and confirm purchase\n");
      printf("-1.exit\n");
      int choice;
      scanf("%d",&choice);
      send(sock,&choice,sizeof(int),0);
      if(choice==-1){
        break;
      }
      else if(choice==1){
        int number;
        recv(sock, &number, sizeof(int), 0);

        for(int i=0;i<number;i++){
          char name[50];
          int id,price,quantity,valid;
          recv(sock, &id, sizeof(int), 0);
          recv(sock, &price, sizeof(int), 0);
          recv(sock, &quantity, sizeof(int), 0);
          recv(sock, &valid, sizeof(int), 0);
          recv(sock, name, sizeof(name), 0);

          if(valid==1)
            printf("%d %s %d %d\n",id,name,price,quantity);
          
        }
      }
      else if(choice==2){
        int id,quan;
        printf("Enter the productID of the product which you wish to add to the cart:");
        scanf("%d",&id);
        printf("Enter the quantity which you wish to add:");
        scanf("%d",&quan);

        send(sock,&id,sizeof(int),0);
        send(sock,&quan,sizeof(int),0);

        // duplicate additions
        int dupl=0;
        for(int i=0;i<bound;i++){
          if(cart[i].id==id){
            dupl=1;
            break;
          }
        }
        if(dupl==1){
          printf("Item already in cart\n");
          printf("Please choose update cart option for any further changes\n");
          continue;
        }
        int result;
        recv(sock, &result, sizeof(int), 0);
        if(result==1){
          struct item i={id,quan};
          cart[bound++]=i;
          printf("Item added to cart successfully\n");
          for(int i=0;i<bound;i++) printf("%d %d\n",cart[i].id,cart[i].quantity);
        }
        else printf("Cannot add item to cart\n");

      }
      else if(choice==3){
        int id,quan;
        printf("Enter the productID of the product which you wish to edit in the cart:");
        scanf("%d",&id);
        printf("Enter the new quantity:");
        scanf("%d",&quan);

        send(sock,&id,sizeof(int),0);
        send(sock,&quan,sizeof(int),0);

        int pres=0;
        for(int i=0;i<bound;i++){
          if(cart[i].id==id){
            pres=1;
            break;
          }
        }

        int result;
        recv(sock, &result, sizeof(int), 0);
        if(pres==0){
          printf("Item absent in the cart\n");
          printf("Please add the item to cart before editing its quantity\n");
        }
        else{
          if(result==1){
            for(int i=0;i<bound;i++){
              if(cart[i].id==id){
                cart[i].quantity=quan;
                printf("Cart updated successfully\n");
                break;
              }
            }
          }
          else printf("Update failed\n");
        }
      }
      else if(choice==4){
        for(int i=0;i<bound;i++){
          printf("%d %d\n",cart[i].id,cart[i].quantity);
        }
      }
      else if(choice==5){
        printf("Press 1 for confirmation:");
        int press;
        scanf("%d",&press);
        int result=1;
        int final=1;
        if(press==1){
          int amount;
          printf("Enter the amount which you are paying:");
          scanf("%d",&amount);
          send(sock,&bound,sizeof(int),0);
          for(int i=0;i<bound;i++){
            send(sock,&cart[i].id,sizeof(int),0);
            send(sock,&cart[i].quantity,sizeof(int),0);
            recv(sock, &result, sizeof(int), 0);
            final=final*result;
          }
          if(final==1){
            printf("Sale successful\n");
          }
          else printf("Sale unsuccessful\n");
        }
      }
    }
}