OS Project
Goutham U R (IMT2021045)

Approach to the problem:
1. gcc stock_initialize.c
2. ./a.out
3. Create a server which will basically act as backend and handle the files
4. Create 2 files for clients(admin and customer) which will work as frontend
5. Used socket programming to communicate between server and clients
6. Used record locking to maintain data consistency

How to run?
1. Create an executable server using gcc sock_test_serv.c -o server for each client
2. Create different clients using different terminals using
    i) gcc admin.c -o admin (for admin)
    ii) gcc customer.c -o customer (for customer)
3. Use the menu shown to test any usecases

Note: To test all the usecases, sleep time of 15 seconds has been added before
      unlocking any record. So, each usecase will take 15 seconds to complete.
