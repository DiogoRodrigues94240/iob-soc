#include "system.h"
#include "periphs.h"
#include "iob-uart.h"
#include "iob-gpio.h"
#include "printf.h"

int fibonacci[100];
char charfibonacci[100];
/*char send_string[200]= "Sending this string as a file to console.\n"
                    "The file is then requested back from console.\n"
                    "The sent file is compared to the received file to confirm " 
                    "correct file transfer via UART using console.\n"
                    "Generating the file in the firmware creates an uniform "
                    "file transfer between pc-emul, simulation and fpga without"
                    " adding extra targets for file generation.\n";*/




void calculate_fibonacci_sequence(int n){
  fibonacci[0] = 0;
  int x = 0, y = 1, z = 0;
  for(int i = 1; i < n; i++){
    x = y;
    y = z;
    z = x + y;
    fibonacci[i] = z;
  }
  int i=0;
  int index = 0;
  for (i=0; i<n; i++)
   index += snprintf(&charfibonacci[index], 128-index, "%d, ", fibonacci[i]);
  return;
}
// copy src to dst
// return number of copied chars (excluding '\0')
int string_copy(char *dst, char *src) {
    if (dst == NULL || src == NULL) {
        return -1;
    }
    int cnt = 0;
    while(src[cnt] != 0){
        dst[cnt] = src[cnt];
        cnt++;
    }
    dst[cnt] = '\0';
    return cnt;
}

// 0: same string
// otherwise: different
int compare_str(char *str1, char *str2, int str_size) {
    int c = 0;
    while(c < str_size) {
        if (str1[c] != str2[c]){
            return str1[c] - str2[c];
        }
        c++;
    }
    return 0;
}

int main()
{
  //init uart
  uart_init(UART_BASE,FREQ/BAUD);

  /*//test puts
  uart_puts("\n\n\nHello world!\n\n\n");

  //test printf with floats 
  printf("Value of Pi = %f\n\n", 3.1415);*/
  
  //test the first 100 numbers of febonacci sequence  
  calculate_fibonacci_sequence(20);
  printf("\n\n\n fibonacci sequence: %s\n\n\n", charfibonacci);
  
  //test file send
  char *sendfile = malloc(1000);
  int send_file_size = 0;
  send_file_size = string_copy(sendfile, /*send_string*/ charfibonacci);
  uart_sendfile("Sendfile.txt", send_file_size, sendfile);

  //test file receive
  char *recvfile = malloc(10000);
  int file_size = 0;
  file_size = uart_recvfile("Sendfile.txt", recvfile);

  //compare files
  if (compare_str(sendfile, recvfile, send_file_size)) {
      printf("FAILURE: Send and received file differ!\n");
  } else {
      printf("SUCCESS: Send and received file match!\n");
  }

  free(sendfile);
  free(recvfile);

  uart_finish();
}
