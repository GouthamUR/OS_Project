#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>

struct Product{
    int id;
    int valid;// 1 -> product exists 0 -> product has been deleted
    char name[50];
    int quantity;
    int price;
};
int main(){
    int fd;

    // Open the file in read/write mode
    fd = open("stock.txt", O_RDWR | O_CREAT |O_TRUNC, 0644);

    // Create three records in the file
    struct Product products[3] = {
        {1,1, "Apple", 25, 40},
        {2,1, "Orange", 30, 50},
        {3,1, "Banana", 35, 69}
    };
    write(fd, products, sizeof(products));
    printf("Successfully initialized stock values\n");
    close(fd);
}