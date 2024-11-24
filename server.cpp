// Server side C/C++ program to demonstrate Socket
// programming
#include <winsock2.h>
#include <thread>
#include <mutex>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#include <iostream>
#include <vector>
#include <string>
#include <thread>
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
    char* toChar() {
        char str[11] = {};
        size_t i = 1;
        str[0] = '!';
        for (int l = 0; l < 3; ++l) {
            for (int j = 0; j < 3; ++j) {
                str[i] = matrix[l][j];
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
    XO& operator=(const XO& other)
    {
        if (this != &other)
        {
            matrix = other.matrix;
        }
        return *this;
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
std::mutex my_mutex;
void handleError(bool err, const char* msg)
{
	if (!err)
		return;
	std::cout << msg << WSAGetLastError();
	exit(EXIT_FAILURE);
}
void job1(SOCKET socket1, SOCKET socket2,XO& m) {
    //std::unique_lock<std::mutex> my_lock(my_mutex);
    size_t row = 0;
    size_t col = 0;
    char buffer1[1024] = { 0 };
    if (my_mutex.try_lock()) {
        int valread = recv(socket1, buffer1, 1024, 0);
        row = (int)buffer1[0] - '0';
        col = (int)buffer1[1] - '0';
        m[row][col] = 'x';
        send(socket1, m.toChar(), strlen(m.toChar()), 0);
        send(socket2, m.toChar(), strlen(m.toChar()), 0);
        std::cout << m << '\n';
        my_mutex.unlock();
        
    }
}
void job2(SOCKET socket1,SOCKET socket2,XO& m) {
    //std::unique_lock<std::mutex> my_lock(my_mutex);
    size_t row = 0;
    size_t col = 0;
    char buffer2[1024] = { 0 };
    my_mutex.lock();
    int valread = recv(socket2, buffer2, 1024, 0);
    row = (int)buffer2[0] - '0';
    col = (int)buffer2[1] - '0';
    m[row][col] = 'o';
    send(socket1, m.toChar(), strlen(m.toChar()), 0);
    send(socket2, m.toChar(), strlen(m.toChar()), 0);
    std::cout << m << '\n';
    my_mutex.unlock();
}
void play(SOCKET socket1, SOCKET socket2) {
	XO m;
    bool flag = true;
	while (flag) {
		std::thread t1(job1,socket1,socket2,std::ref(m));
        std::thread t2(job2,socket1,socket2,std::ref(m));
        t1.join();
		t2.join();
        std::cout << m << '\n';
        for (int i = 0; i < 3;++i) {
            for (int j = 0; j < 3;++j) {
                if (m.ItIsWin(i, j, 'x')) {
                    flag = false;
                }
                if (m.ItIsWin(i, j, 'o')) {
                    flag = false;
                }
            }
        }

	}
}
int main(int argc, char const* argv[])
{
	//Initialize winsock
	std::cout << "Initialising Winsock...\n";
	WSADATA wsa;
	int res = WSAStartup(MAKEWORD(2, 2), &wsa);
	handleError(res != 0, "Failed. Error Code : ");

	int valread;
	int opt = 1;
	const char* hello = "hello";
	char buffer[1024] = { 0 };
	SOCKET server_sock = socket(AF_INET, SOCK_STREAM, 0);
	// Creating socket file descriptor
	handleError(server_sock == SOCKET_ERROR, "Could not create socket : ");

	std::cout << "Server socket created.\n";

	struct sockaddr_in address;
	int addrlen = sizeof(address);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 8080
	res = bind(
		server_sock,
		(struct sockaddr*)&address,
		sizeof(address)
	);
	handleError(res == SOCKET_ERROR, "Bind failed with error code : ");
	std::cout << "Bind done\n";

	res = listen(server_sock, 3);
	if (res < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	SOCKET socket1 = accept(
		server_sock,
		(struct sockaddr*)&address,
		&addrlen
	);
	SOCKET socket2 = accept(
		server_sock,
		(struct sockaddr*)&address,
		&addrlen
	);
	handleError(socket1 == SOCKET_ERROR, "Accept failed with error code : ");
	handleError(socket2 == SOCKET_ERROR, "Accept failed with error code : ");
	std::cout << "Accepted\n";
    send(socket1, "you play x\n", strlen("you play x\n"), 0);
    send(socket2, "you play o\n", strlen("you play x\n"), 0);
	play(socket1, socket2);
	// closing the connected socket
	closesocket(socket1);
	closesocket(socket2);
	// closing the listening socket
	closesocket(server_sock);
	WSACleanup();
	return 0;
}

