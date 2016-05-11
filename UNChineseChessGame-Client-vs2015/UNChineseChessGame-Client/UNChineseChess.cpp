//#include "stdafx.h"
#include "Define.h"
#include "Square.h"
#include "ClientSocket.h"
#include "UNChineseChess.h"
#include <windows.h>
#include <iostream>
#include <conio.h>

#define ROWS 4
#define COLS 4
#define ROUNDS 3

Square board[ROWS][COLS];
int ownColor = -1, oppositeColor = -1;

void authorize(const char *id, const char *pass) {
	connectServer();
    std::cout << "Authorize " << id << std::endl;
	char msgBuf[BUFSIZE];
	memset(msgBuf, 0, BUFSIZE);
	msgBuf[0] = 'A';
	memcpy(&msgBuf[1], id, 9);
    memcpy(&msgBuf[10], pass, 6);
	sendMsg(msgBuf);
}

void gameStart() {
	authorize(ID, PASSWORD);
    std::cout << "Game Start" << std::endl;
	for (int round = 0; round < ROUNDS; round++) {
		roundStart(round);
		oneRound();
		roundOver(round);
	}
	gameOver();
	close();
}

void gameOver() {
	std::cout << "Game Over" << std::endl;
}

void roundStart(int round) {
	std::cout << "Round " << round << " Start" << std::endl;
    recvMsg();
	switch (recvBuf[1]) {
        case 'R':
            ownColor = 0;
            oppositeColor = 1;
			sendMsg("BR");
            break;
        case 'B':
            ownColor = 1;
            oppositeColor = 0;
			sendMsg("BB");
            break;
    }
}

void oneRound() {
	switch (ownColor) {
	case 0:
		while (true) {
			step();
			if (observe() == 1) break;
            saveChessBoard();
			if (observe() == 1) break;
            saveChessBoard();
		}
		break;
	case 1:
		while (true) {
			step();
			if (observe() == 1) break;
            saveChessBoard();
			if (observe() == 1) break;
            saveChessBoard();
		}
		break;
	default:
		break;
	}
}

void roundOver(int round) {
	std::cout << "Round " << round << " Over" << std::endl;
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            board[r][c].reset();
        }
    }
    ownColor = oppositeColor = -1;
}

int observe() {
    int rtn = 0;
    recvMsg();
    switch (recvBuf[0]) {
        case 'R':
        {
            switch (recvBuf[1]) {
                case '0':
                    switch (recvBuf[2]) {
                        case 'R':
                        {
                            int srcRow = recvBuf[3] - '0', srcCol = recvBuf[4] - '0';
							board[srcRow][srcCol].valid = true;
							board[srcRow][srcCol].color = recvBuf[7] - '0';
							board[srcRow][srcCol].piece = recvBuf[8] - '0';
                            break;
                        }
                        case 'M':
                        {
							int srcRow = recvBuf[3] - '0', srcCol = recvBuf[4] - '0';
							int desRow = recvBuf[5] - '0', desCol = recvBuf[6] - '0';
							if (board[srcRow][srcCol].piece == board[desRow][desCol].piece)
							{
								board[srcRow][srcCol].clear();
								board[desRow][desCol].clear();
							}
							else {
								board[srcRow][srcCol].moveTo(board[desRow][desCol]);
							}
                            break;
                        }
                        case 'N':
                        {
                            break;
                        }
                    }
                    break;
				case 1:
					std::cout << "Error -1: Msg format error";
					rtn = -1;
					break;
				case 2:
					std::cout << "Error -2: Coordinate error";
					rtn = -2;
					break;
				case 3:
					std::cout << "Error -3: Piece color error";
					rtn = -3;
					break;
				case 4:
					std::cout << "Error -4: Invalid step";
					rtn = -4;
					break;
                default:
					std::cout << "Error -5: Other error";
                    rtn = -5;
                    break;
            }
            break;
        }
        case 'E':
        {
			switch (recvBuf[1]) {
                case '0':
                    // game over
                    rtn = 2;
                    break;
                case '1':
                    // round over
                    rtn = 1;
                default:
                    break;
            }
            break;
        }
    }
    return rtn;
}

void reversePiece(int row, int col) {
	char msg[6];
	msg[0] = 'S';
	msg[1] = 'R';
	msg[2] = '0' + row;
	msg[3] = '0' + col;
	msg[4] = '0' + row;
	msg[5] = '0' + col;
	sendMsg(msg);
}

void movePiece(int srcRow, int srcCol, int desRow, int desCol) {
	char msg[6];
	msg[0] = 'S';
	msg[1] = 'M';
	msg[2] = '0' + srcRow;
	msg[3] = '0' + srcCol;
	msg[4] = '0' + desRow;
	msg[5] = '0' + desCol;
	sendMsg(msg);
}

void noStep() {
	sendMsg("SN");
}

void saveChessBoard() {
    
}

void printBoard()
{
	HANDLE hOut;//创建句柄
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	std::cout << "帅->士->象->车>马->炮->兵" << std::endl;
	if (ownColor == 0)
		std::cout << "红色" << std::endl;
	else
		std::cout << "黑色" << std::endl;
	for (int i = 1; i < 5; i++)
		std::cout << "\t" << i;
	std::cout << std::endl;
	for (int r = 0; r < ROWS; r++)
	{
		std::wcout << r + 1 << "\t";
		for (int c = 0; c < COLS; ++c)
		{
			if (board[r][c].empty)
				std::cout << "--\t";
			else if (!board[r][c].valid)
				std::cout << "##\t";
			else
			{
				if(board[r][c].color == 0)
					SetConsoleTextAttribute(hOut, FOREGROUND_INTENSITY | FOREGROUND_RED);
				switch (board[r][c].piece)
				{
				case 0: std::cout << "兵"; break;
				case 1: std::cout << "炮"; break;
				case 2: std::cout << "马"; break;
				case 3: std::cout << "车"; break;
				case 4: std::cout << "象"; break;
				case 5: std::cout << "士"; break;
				case 6: std::cout << "帅"; break;
				}
				std::cout << "\t";
				SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			}
			if (c == COLS - 1)
				std::cout << std::endl;
		}
	}
}
void step() {
	printBoard();
	int r, c;
	std::cout << "input row and colum : ";
	std::cin >> r >> c;
	if (!board[r - 1][c - 1].valid)
	{
		reversePiece(r - 1, c - 1);
		return;
	}
	else
	{
		std::cout << "input destination : ";
		int destr, destc;
		std::cin >> destr >> destc;
		movePiece(r - 1, c - 1, destr - 1, destc - 1);
		return;
	}
	noStep();
}

