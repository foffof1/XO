#include<winsock2.h>
#include <iostream>
#include <WS2tcpip.h>
#include <tchar.h>
#include <vector>
#include <string>
class XO {
    std::vector<std::vector<char>> matrix;
public:
    XO() {
        matrix.resize(3);
        for (size_t i = 0; i < 3; ++i) {
            matrix[i].resize(3);
            for (size_t j = 0; j < 3; ++j) {
                matrix[i][j] = '.';
            }
        }
    }
    std::vector<char>& operator[](size_t idx) {
        return matrix[idx];
    }
    const std::vector<char>& operator[](size_t idx) const {
        return matrix[idx];
    }
    char GetMatrix(size_t i, size_t j) {
        return matrix[i][j];
    }
    bool ItIsWin(size_t col, size_t row, char xo) {
        if (fallline(row, col, xo) || upline(row, col, xo) || horizontal(row, col, xo) || vertical(row, col, xo)) {
            return true;
        }
        return false;
    }
    bool fallline(size_t row, size_t col, char xo) {
        size_t col_xo = 0;
        size_t begincol = row - 2;
        size_t beginrow = col - 2;
        while (begincol != row + 3 || beginrow != col + 3) {
            if (ThereIs(begincol, beginrow)) {
                if (xo == matrix[begincol][beginrow]) {
                    ++col_xo;
                }
            }
            ++begincol;
            ++beginrow;
        }
        if (col_xo == 3) {
            return true;
        }
        return false;
    }
    bool upline(size_t row, size_t col, char xo) {
        size_t col_xo = 0;
        size_t begincol = row - 2;
        size_t beginrow = col + 2;
        while (begincol != row + 3 || beginrow != col - 3) {
            if (ThereIs(begincol, beginrow)) {
                if (xo == matrix[begincol][beginrow]) {
                    ++col_xo;
                }
            }
            ++begincol;
            --beginrow;
        }
        if (col_xo == 3) {
            return true;
        }
        return false;
    }
    bool horizontal(size_t row, size_t col, char xo) {
        size_t col_xo = 0;
        size_t begincol = row;
        size_t beginrow = col - 2;
        while (beginrow != col + 3) {
            if (ThereIs(begincol, beginrow)) {
                if (xo == matrix[begincol][beginrow]) {
                    ++col_xo;
                }
            }
            ++beginrow;
        }
        if (col_xo == 3) {
            return true;
        }
        return false;
    }
    bool vertical(size_t row, size_t col, char xo) {
        size_t col_xo = 0;
        size_t begincol = row - 2;
        size_t beginrow = col;
        while (begincol != row + 3) {
            if (ThereIs(begincol, beginrow)) {
                if (xo == matrix[begincol][beginrow]) {
                    ++col_xo;
                }
            }
            ++begincol;
        }
        if (col_xo == 3) {
            return true;
        }
        return false;
    }
    bool ThereIs(size_t row, size_t col) {
        if ((-1 > row && row < 3) && (-1 > col && col < 3)) {
            return true;
        }
        return false;
    }
    char* toChar(XO& matrix) {
        char str[11] = {};
        str[0] = '!';
        size_t i = 1;
        for (int l = 0; l < 3; ++l) {
            for (int j = 0; j < 3; ++j) {
                str[i] = matrix.GetMatrix(l, j);
                ++i;
            }
        }
        return str;
    }
    bool Taken(size_t row, size_t col) {
        if (matrix[col][row] == '.') {
            return true;
        }
        return false;
    }
};
std::ostream& operator<<(std::ostream& out, const XO& m) {
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            out << ' ' << m[i][j];
            if (j == 0 || j == 1) {
                out << " |";
            }
            else if (j == 2) {
                out << '\n';
            }
        }
        if (i == 0 || i == 1) {
            out << "---|---|---\n";
        }
    }
    return out;
}
const int PORT = 2024;
#pragma comment(lib,"ws2_32.lib") //Winsock Library

void handleError(bool err, const char* msg)
{
	if (!err)
		return;
	std::cout << msg << WSAGetLastError();
	exit(EXIT_FAILURE);
}

int main(int argc, char const* argv[])
{
	//Initialize winsock
	std::cout << "Initialising Winsock...\n";
	WSADATA wsa;
	int res = WSAStartup(MAKEWORD(2, 2), &wsa);
	handleError(res != 0, "Failed. Error Code : ");

	//const char* hello = "Hello from client";
	char* h = new char[1024]();

	char buffer[1024] = { 0 };
	SOCKET client_sock = socket(AF_INET, SOCK_STREAM, 0);
	handleError(client_sock == SOCKET_ERROR, "Could not create socket : ");


	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	//servaddr.sin_addr.s_addr = INADDR_ANY;
	InetPton(AF_INET, _T("127.0.0.1"), &servaddr.sin_addr.s_addr);

	int status = connect(
		client_sock,
		(struct sockaddr*)&servaddr,
		sizeof(servaddr)
	);
	handleError(status == SOCKET_ERROR, "Could not create socket : ");
    XO m;
    std::string command = "";
    const char* hello;
    int valread = recv(client_sock, buffer, 1024, 0);
    std::cout << buffer;
	while (command!="quit") {
		std::cin >> command;
		if (command == "add") {
			std::cin >> h;
			hello = h;
            if (m[hello[0]-'0'][hello[1]-'0'] == '.') {
                send(client_sock, hello, strlen(hello), 0);
                //	std::cout << "Hello message sent\n";
                int valread = recv(client_sock, buffer, 1024, 0);
                std::cout << "Message from server: " << buffer << '\n';
                size_t l = 1;
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; ++j) {
                        m[i][j] = buffer[l];
                        ++l;
                    }
                }
                bool flag = true;
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; ++j) {
                        if (m.ItIsWin(i, j, 'x')) {
                            flag = false;
                        }
                        if (m.ItIsWin(i, j, 'o')) {
                            flag = false;
                        }
                    }
                }
                if (!flag) {
                    command="quit";
                }
            }
            else {
                std::cout << "this place is taken\n";
            }
		}
	    if (command == "see") {
            std::cout << m << '\n';
		}
		else if (command == "quit") {
            std::cout << m;
			std::cout << '\n';
			send(client_sock, "quit", strlen("quit"), 0);
			std::cout << "you win\n";
			break;
		}
	}
	delete h;
	// closing the connected socket
	closesocket(client_sock);
	WSACleanup();
	return 0;
}
