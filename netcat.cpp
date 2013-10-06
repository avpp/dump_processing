#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv)
{
  if (argc < 3 || argc > 4)
  {
    printf("Error! Please run with following arguments:\n %s <host> <port> [<file_name>]\n\n", argv[0]);
    return 1;
  }
  FILE *in = stdin;
  if (argc == 4)
  {
    printf("read from file");
    fflush(stdout);
    in = fopen(argv[3], "rb");
    if (in == 0)
    {
      printf("Error open file.\n");
      return 1;
    }
  }

  int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s == 0)
  {
    printf("Error socket create\n");
    return 1;
  }
  struct hostent* pHostInfo;
  long nHostAddr;
  struct sockaddr_in addr;
  int port;

  sscanf(argv[2], "%d", &port);

  pHostInfo = gethostbyname(argv[1]);
  memcpy(&nHostAddr, pHostInfo->h_addr, pHostInfo->h_length);

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = nHostAddr;

  if (connect(s, (struct sockaddr *) &addr, sizeof(addr)) < 0)
  {
    printf("Connection error.\n");
    return 1;
  }
  int cnt;
  char* d = new char [1001];
  printf("send:\n");
  do
  {
    cnt = fread(d, 1, 1000, in);
    printf("%s", d);
    send(s, d, cnt, 0);
  }
  while (cnt == 1000);
  printf("\n\nrecieve:\n");
  do
  {
    cnt = recv(s, d, 1000, 0);
    if (cnt > 0)
      d[cnt] = 0;
    printf("%s", d);
  }
  while (cnt > 0);

  delete [] d;

  close (s);
  if (argc == 4)
    fclose(in);
  return 0;
}
