/* A simple server in the internet domain using TCP
 * The port number is passed as an argument
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

class HTTPResponse {
  std::string _header;
  std::string _status;
  std::string _content;

  public:
    HTTPResponse() {
      _status = "200";
    }

    void addHeader(std::string type, std::string value) {
      _header += type;
      _header += ": ";
      _header += value;
      _header += "\r\n";
    }

    void setStatus(std::string code) {
      _status = code;
    }

    void setContent(std::string content) {
      _content = content;
    }

    std::string getHeader() {
      return "HTTP/1.0 " + _status + "\r\n" + _header;
    }

    std::string output() {
      addHeader("Content-Length", std::to_string(_content.size()));
      return getHeader() + "\r\n" + _content;
    }
};

std::string imageFileToString(const char *filePath)
{
  ifstream file(filePath, ios::binary);

  ostringstream ostrm;

  ostrm << file.rdbuf();

  string imageData(ostrm.str());

  return imageData;
}

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[])
{
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  char buffer[256];
  char requestMethod[50];
  char requestPath[50];
  std::stringstream requestStream;
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
        sizeof(serv_addr)) < 0)
    error("ERROR on binding");
  listen(sockfd,5);
  clilen = sizeof(cli_addr);

  while(true) {
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) {
      error("ERROR on accept");
    }
    bzero(buffer,256);
    n = read(newsockfd, buffer, 255);
    if (n < 0) error("ERROR reading from socket");

    requestStream.str(buffer);
    requestStream >> requestMethod;
    requestStream >> requestPath;
    printf("Here is the request path: %s\n", requestPath);

    HTTPResponse resp;
    resp.addHeader("Server", "c-cow v1");
    resp.addHeader("Content-type", "image/jpg");

    std::string imageData = imageFileToString("sea-cow-1.jpg");

    resp.setContent(imageData);

    std::string response = resp.output();

    // Write to the socket, sending the response as a C string
    n = write(newsockfd, response.c_str(), response.size());
    if (n < 0) error("ERROR writing to socket");
    close(newsockfd);
  }
  close(sockfd);

  return 0;
}
