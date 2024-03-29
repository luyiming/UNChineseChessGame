//#include "stdafx.h"
#include "Define.h"
#include "Square.h"
#include "ClientSocket.h"
#include "UNChineseChess.h"

#include <windows.h>
#include <iostream>
#include <conio.h>
#include <QString>
#include <QList>
#include <QDebug>
#include <QFile>
#include <QTime>
#include <QtNetwork/QTcpSocket>

#define ROWS 4
#define COLS 4
#define ROUNDS 3

const int mr[] = {-1, 0, 1, 0};
const int mc[] = {0, 1, 0, -1};
const int mr2[] = {-2, 0, 2, 0};
const int mc2[] = {0, 2, 0, -2};

QList<int> movemsg;

double factor = 0.68;

Game::Game()
{
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Game::receiveMessage);
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),
            this,SLOT(displayError(QAbstractSocket::SocketError)));
}

void Game::authorize(const char *id, const char *pass) {

    emit statusChanged(QString("Authorize ") + id + "\n");
    char msgBuf[BUFSIZE + 1];
    memset(msgBuf, 0, BUFSIZE + 1);
	msgBuf[0] = 'A';
	memcpy(&msgBuf[1], id, 9);
    memcpy(&msgBuf[10], pass, 6);
    sendMessage(msgBuf);
    b_roundStart = true;
    this->id_ = QString(id);
    this->pwd_ = QString(pass);
}

void Game::gameStart() {
    board.reset();

    ownColor = oppositeColor = -1;
    curRound = 0;

	authorize(ID, PASSWORD);
    emit statusChanged("Game Start\n");
	for (int round = 0; round < ROUNDS; round++) {
		roundStart(round);
		oneRound();
		roundOver(round);
	}
	gameOver();
    close();
}

void Game::gameOver() {
    saveChessBoard(true);

    emit statusChanged("Game Over\n");
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            board[r][c].reset();
        }
    }
    ownColor = oppositeColor = -1;
    curRound = 0;
    record.clear();
}

void Game::roundStart(int round) {
    emit statusChanged(QString("Round ") + QString::number(round) + QString(" Start\n"));

    board.reset();

    ownColor = oppositeColor = -1;

    switch (recvBuf[1]) {
        case 'R':
            ownColor = 0;
            oppositeColor = 1;
            sendMessage("BR");
            break;
        case 'B':
            ownColor = 1;
            oppositeColor = 0;
            sendMessage("BB");
            break;
    }

    curPlayColor = 0;
    emit roundChanged(round, ownColor);

}

void Game::oneRound() {
    switch (ownColor) {
    case 0:
        while (true) {
            step();
            //minimaxStep();
            if (observe() == 1) break;
            saveChessBoard();
            if (observe() == 1) break;
            saveChessBoard();
        }
        break;
    case 1:
        while (true) {
            if (observe() == 1) break;
            saveChessBoard();
            step();
            //minimaxStep();
            if (observe() == 1) break;
            saveChessBoard();
        }
        break;
    default:
        break;
    }
}

void Game::roundOver(int round) {
    emit statusChanged(QString("Round ") + round + QString(" Over\n"));
    board.reset();
    ownColor = oppositeColor = -1;
}

int Game::observe() {
    int rtn = 0;
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
                            int clr = board[srcRow][srcCol].color;
                            int pie = board[srcRow][srcCol].piece;
                            board.remainPieces[clr * 10 + pie] = board.remainPieces[clr * 10 + pie] - 1;
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
                case '1':
                    emit statusChanged("Error -1: Msg format error\n");
                    rtn = -1;
                    break;
                case '2':
                    emit statusChanged("Error -2: Coordinate error\n");
                    rtn = -2;
                    break;
                case '3':
                    emit statusChanged("Error -3: Piece color error\n");
                    rtn = -3;
                    break;
                case '4':
                    emit statusChanged("Error -4: Invalid step\n");
                    rtn = -4;
                    break;
                default:
                    emit statusChanged("Error -5: Other error\n");
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

void Game::reversePiece(int row, int col) {
	char msg[6];
	msg[0] = 'S';
	msg[1] = 'R';
	msg[2] = '0' + row;
	msg[3] = '0' + col;
	msg[4] = '0' + row;
	msg[5] = '0' + col;
    //sendMsg(msg);
    sendMessage(msg);
}

void Game::movePiece(int srcRow, int srcCol, int desRow, int desCol) {
	char msg[6];
	msg[0] = 'S';
	msg[1] = 'M';
	msg[2] = '0' + srcRow;
	msg[3] = '0' + srcCol;
	msg[4] = '0' + desRow;
	msg[5] = '0' + desCol;
    //sendMsg(msg);
    sendMessage(msg);
}

void Game::noStep() {
    //sendMsg("SN");
    sendMessage("SN");
}

void Game::saveChessBoard(bool saveToFile) {
    if(curRound != record.size() - 1)
    {
        Record temp;
        temp.ownColor = ownColor;
        record.append(temp);
    }
    record[curRound].boards.append(board);

    if(saveToFile)
    {
        QDateTime time = QDateTime::currentDateTime();
        QString suffix = time.toString("MM-dd hh") + "h" + time.toString("mm") + "m" + time.toString("ss") + "s";

        QFile file(QString("record/" + suffix) + ".dat");
        file.open(QFile::WriteOnly);
        QTextStream fout(&file);
        for(int i = 0; i < ROUNDS; i++)
        {
            fout << i << record[i].ownColor << endl;
            for(auto it = record[i].boards.begin(); it != record[i].boards.end(); ++it)
            {
                for (int r = 0; r < ROWS; r++)
                {
                    for (int c = 0; c < COLS; ++c)
                    {
                        if((*it)[r][c].empty)
                            fout << "--";
                        else if(!(*it)[r][c].valid)
                            fout << "##";
                        else
                            fout << (*it)[r][c].color << (*it)[r][c].piece;
                    }
                }
                fout << endl;
            }
        }
        file.close();
    }
}

void Game::setBoard(Board &b)
{
    if(board_copy.empty())
        board_copy = board;
    this->board = b;
}

void Game::resetBoard()
{
    board = board_copy;
    board_copy.reset();
    if(ownColor == -1)
        board.reset();
}

void Game::step() {
    int srcRow = -1, srcCol = -1, desRow = -1, desCol = -1;
    int score = -1000;
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            if(board[r][c].empty)
                continue;
            if(board[r][c].valid && board[r][c].color == ownColor)
            {
                if(board[r][c].piece == 1)
                {
                    for(int i = 0; i < 4; ++i)
                    {
                        int nr = r + mr2[i];
                        int nc = c + mc2[i];
                        if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                            continue;
                        if(!board[(r+nr)/2][(c+nc)/2].empty && !board[nr][nc].empty && board[nr][nc].valid && board[nr][nc].color != ownColor)
                        {
                            Board nb = board;
                            nb[r][c].moveTo(nb[nr][nc]);
                            int tempScore = evaluate(nb);
                            if(tempScore > score)
                            {
                                score = tempScore;
                                srcRow = r;
                                srcCol = c;
                                desRow = nr;
                                desCol = nc;
                            }
                        }
                    }
                    for(int i = 0; i < 4; ++i)
                    {
                        int nr = r + mr[i];
                        int nc = c + mc[i];
                        if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                            continue;
                        if(!board[nr][nc].empty)
                            continue;
                        Board nb = board;
                        nb[r][c].moveTo(nb[nr][nc]);
                        int tempScore = evaluate(nb);
                        if(tempScore > score)
                        {
                            score = tempScore;
                            srcRow = r;
                            srcCol = c;
                            desRow = nr;
                            desCol = nc;
                        }
                    }
                }
                else
                {
                    for(int i = 0; i < 4; ++i)
                    {
                        int nr = r + mr[i];
                        int nc = c + mc[i];
                        if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                            continue;
                        if(!board[nr][nc].valid || board[nr][nc].color == ownColor)
                            continue;
                        if(board[nr][nc].empty)
                        {
                            Board nb = board;
                            nb[r][c].moveTo(nb[nr][nc]);
                            int tempScore = evaluate(nb);
                            if(tempScore > score)
                            {
                                score = tempScore;
                                srcRow = r;
                                srcCol = c;
                                desRow = nr;
                                desCol = nc;
                            }
                        }
                        else if(board[nr][nc].piece == board[r][c].piece)
                        {
                            Board nb = board;
                            nb[r][c].clear();
                            nb[nr][nc].clear();
                            int tempScore = evaluate(nb);
                            if(tempScore > score)
                            {
                                score = tempScore;
                                srcRow = r;
                                srcCol = c;
                                desRow = nr;
                                desCol = nc;
                            }
                        }
                        else if((board[nr][nc].piece == 6 && board[r][c].piece == 0) || board[nr][nc].piece < board[r][c].piece)
                        {
                            if((board[nr][nc].piece == 0 && board[r][c].piece == 6))
                                continue;
                            Board nb = board;
                            nb[r][c].moveTo(nb[nr][nc]);
                            int tempScore = evaluate(nb);
                            if(tempScore > score)
                            {
                                score = tempScore;
                                srcRow = r;
                                srcCol = c;
                                desRow = nr;
                                desCol = nc;
                            }
                        }

                    }
                }
            }
            else if(!board[r][c].valid)
            {
                for(int i = 0; i < 4; ++i)
                {
                    int nr = r + mr2[i];
                    int nc = c + mc2[i];
                    if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                        continue;
                    if(board[nr][nc].piece == 1 && board[nr][nc].color == ownColor)
                    {
                        int tempScore = evaluate(board) + 2;
                        if(tempScore > score)
                        {
                            score = tempScore;
                            srcRow = r;
                            srcCol = c;
                            desRow = -1;
                            desCol = -1;
                        }
                    }
                }
                int tempScore = evaluate(board) + 1;
                if(tempScore > score)
                {
                    score = tempScore;
                    srcRow = r;
                    srcCol = c;
                    desRow = -1;
                    desCol = -1;
                }
            }
        }
    }
    if(srcRow == -1 || srcCol == -1)
        noStep();
    else if(desRow == -1 || desCol == -1)
        reversePiece(srcRow, srcCol);
    else
        movePiece(srcRow, srcCol, desRow, desCol);
}

double Game::alphaBetaMax(Board& b, int depth, double alpha, double beta)
{
    if (depth == 0)
       return evaluate(b);
    QList<Board> moves;
    QList<QList<int> > msg;
    makeMoves(b, ownColor, moves, msg);
    if(moves.size() != msg.size())
    {
       qDebug() << "not equal";
       exit(1);
    }
    for(int i = 0; i < moves.size(); ++i)
    {
        double score = alphaBetaMin(moves[i], depth - 1, alpha, beta);
        //qDebug() << "score:" << score;
        if(score >= beta)
            return beta;   //beta-cutoff
        if(score > alpha)
        {
            if(depth == 4)
            {
                qDebug() << "move score:" << score;
                movemsg.clear();
                movemsg = msg[i];
                //qDebug() << "score:" << score;
            }
            alpha = score; // alpha acts like max in MiniMax
        }
    }
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            if(b[r][c].empty)
                continue;
            if(!b[r][c].valid)
            {
                double score = evaluateDark(b, r, c);
                if(score >= beta)
                   return beta;
                if(score > alpha)
                {
                    if(depth == 4)
                    {
                        movemsg.clear();
                        movemsg = QList<int>{r,c};
                    }
                    alpha = score;
                }
            }
        }
    }
    if(alpha < -10000.0)
        return evaluate(b);
    return alpha;
}

double Game::alphaBetaMin(Board& b, int depth, double alpha, double beta)
{
   if (depth == 0)
       return -evaluate(b);
   QList<Board> moves;
   QList<QList<int> > msg;
   makeMoves(b, oppositeColor, moves, msg);
   if(moves.size() != msg.size())
   {
       qDebug() << "not equal";
       exit(1);
   }
   for(int i = 0; i < moves.size(); ++i)
   {
      double score = alphaBetaMax(moves[i], depth - 1, alpha, beta);
      if(score <= alpha)
         return alpha; //alpha-cutoff
      if(score < beta)
         beta = score; // beta acts like min in MiniMax
   }
   for (int r = 0; r < ROWS; r++)
   {
       for (int c = 0; c < COLS; c++)
       {
           if(b[r][c].empty)
               continue;
           if(!b[r][c].valid)
           {
               double score = -evaluateDark(b, r, c);
               if(score <= alpha)
                  return alpha; //alpha-cutoff
               if(score < beta)
                  beta = score; // beta acts like min in MiniMax
           }
       }
   }
   if(beta >= 10000.0)
       return -evaluate(b);
   return beta;
}

double Game::evaluateDark(const Board &b, int r, int c)
{
    double score = 0;
    int cnt = 0;

    for(int i = 0; i <= 6; i++)
    {
        if(b.remainPieces[i] == 0)
            continue;
        else if(b.remainPieces[i] == 1)
        {
            Board tempb = b;
            tempb[r][c].valid = true;
            tempb[r][c].color = 0;
            tempb[r][c].piece = i;
            score += evaluate(tempb);
            cnt++;
        }
        else if(b.remainPieces[i] == 2)
        {
            Board tempb = b;
            tempb[r][c].valid = true;
            tempb[r][c].color = 0;
            tempb[r][c].piece = i;
            score = score + 2 * evaluate(tempb);
            cnt += 2;
        }
    }
    for(int i = 10; i <= 16; i++)
    {
        if(b.remainPieces[i] == 0)
            continue;
        else if(b.remainPieces[i] == 1)
        {
            Board tempb = b;
            tempb[r][c].valid = true;
            tempb[r][c].color = 1;
            tempb[r][c].piece = i - 10;
            score += evaluate(tempb);
            cnt++;
        }
        else if(b.remainPieces[i] == 2)
        {
            Board tempb = b;
            tempb[r][c].valid = true;
            tempb[r][c].color = 1;
            tempb[r][c].piece = i - 10;
            score = score + 2 * evaluate(tempb);
            cnt += 2;
        }
    }
    score = score / cnt;
    //qDebug() << "evaluate dark:" << score;
    return score;

}

void Game::makeMoves(Board& b, int color, QList<Board> & moves, QList<QList<int> > &msg)
{
    moves.clear();
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            if(b[r][c].empty)
                continue;
            if(b[r][c].valid && b[r][c].color == color)
            {
                if(b[r][c].piece == 1)
                {
                    for(int i = 0; i < 4; ++i)
                    {
                        int nr = r + mr2[i];
                        int nc = c + mc2[i];
                        if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                            continue;
                        if(!b[(r+nr)/2][(c+nc)/2].empty && !b[nr][nc].empty && b[nr][nc].valid && b[nr][nc].color != color)
                        {
                            Board nb = b;
                            nb[r][c].moveTo(nb[nr][nc]);
                            moves.push_back(nb);
                            msg.push_back(QList<int>{r,c,nr,nc});
                        }
                    }
                    for(int i = 0; i < 4; ++i)
                    {
                        int nr = r + mr[i];
                        int nc = c + mc[i];
                        if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                            continue;
                        if(!b[nr][nc].empty)
                            continue;
                        Board nb = b;
                        nb[r][c].moveTo(nb[nr][nc]);
                        moves.push_back(nb);
                        msg.push_back(QList<int>{r,c,nr,nc});
                    }
                }
                else
                {
                    for(int i = 0; i < 4; ++i)
                    {
                        int nr = r + mr[i];
                        int nc = c + mc[i];
                        if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                            continue;
                        if(!b[nr][nc].valid || b[nr][nc].color == color)
                            continue;
                        if(b[nr][nc].empty)
                        {
                            Board nb = b;
                            nb[r][c].moveTo(nb[nr][nc]);
                            moves.push_back(nb);
                            msg.push_back(QList<int>{r,c,nr,nc});
                        }
                        else if(b[nr][nc].piece == b[r][c].piece)
                        {
                            Board nb = b;
                            nb[r][c].clear();
                            nb[nr][nc].clear();
                            moves.push_back(nb);
                            msg.push_back(QList<int>{r,c,nr,nc});
                        }
                        else if((b[nr][nc].piece == 6 && b[r][c].piece == 0) || (b[nr][nc].piece < b[r][c].piece))
                        {
                            if((b[nr][nc].piece == 0 && b[r][c].piece == 6))
                                continue;
                            Board nb = b;
                            nb[r][c].moveTo(nb[nr][nc]);
                            moves.push_back(nb);
                            msg.push_back(QList<int>{r,c,nr,nc});
                        }

                    }
                }
            }
        }
    }
}

void Game::getScore(const Board& b, int &ownScore, int &oppositeScore)
{
    ownScore = 0;
    oppositeScore = 0;
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            if(b[r][c].empty)
                continue;
            if(b[r][c].valid)
            {
                if(b[r][c].color == ownColor)
                    ownScore += (b[r][c].piece + 1);
                else
                    oppositeScore += (b[r][c].piece + 1);
            }
        }
    }
}

double Game::evaluate(const Board& b)
{
    double score = 0.0;

    //piese value
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            if(b[r][c].empty)
                continue;
            if(b[r][c].valid)
            {
                if(b[r][c].color == ownColor)
                    score += (b[r][c].piece + 1);
                else
                    score -= (b[r][c].piece + 1);
            }
        }
    }
    double eatScore = 0;
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            if(board[r][c].empty || !board[r][c].valid)
                continue;

            if(board[r][c].piece == 1)
            {
                for(int i = 0; i < 4; ++i)
                {
                    int nr = r + mr2[i];
                    int nc = c + mc2[i];
                    if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                        continue;
                    if(!board[(r+nr)/2][(c+nc)/2].empty && !board[nr][nc].empty && board[nr][nc].valid)
                    {
                        if(board[r][c].color == ownColor && board[nr][nc].color != ownColor)
                            eatScore += (board[nr][nc].piece + 1);
                        else if(board[r][c].color != ownColor && board[nr][nc].color == ownColor)
                            eatScore -= (board[nr][nc].piece + 1);
                    }
                }
            }
            else
            {
                for(int i = 0; i < 4; ++i)
                {
                    int nr = r + mr[i];
                    int nc = c + mc[i];
                    if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                        continue;
                    if(board[nr][nc].empty || !board[nr][nc].valid)
                        continue;
                    if(board[nr][nc].piece == board[r][c].piece)
                        continue;
                    if((board[nr][nc].piece == 6 && board[r][c].piece == 0) || board[nr][nc].piece < board[r][c].piece)
                    {
                        if((board[nr][nc].piece == 0 && board[r][c].piece == 6))
                            continue;
                        if(board[r][c].color == ownColor && board[nr][nc].color != ownColor)
                            eatScore += (board[nr][nc].piece + 1);
                        else if(board[r][c].color != ownColor && board[nr][nc].color == ownColor)
                            eatScore -= (board[nr][nc].piece + 1);
                    }
                }
            }
        }
    }
    score = score + eatScore * factor;
    return score;
}

int Game::tryMove(int r, int c)
{
    if(curPlayColor != ownColor)
        return 0;
    static int srcRow = -1, srcCol = -1, desRow = -1, desCol = -1;
    if(r == -1 && c == -1)
    {
        isSelected = false;
        return 1;
    }
    if(!isSelected)
    {
        if(!board[r][c].valid)
            reversePiece(r, c);
        else
        {
            isSelected = true;
            srcRow = r;
            srcCol = c;
            return 0;
        }
    }
    else
    {
        desRow = r;
        desCol = c;
        isSelected = false;
        movePiece(srcRow, srcCol, desRow, desCol);
    }
    return 1;
}

void Game::minimaxStep()
{
    movemsg.clear();
    /*
    int abscore = alphaBetaMax(board, 4, -100000, 100000);
    int score = -100000;
    int srcRow = -1, srcCol = -1;

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            if(board[r][c].empty)
                continue;
            if(!board[r][c].valid)
            {
                for(int i = 0; i < 4; ++i)
                {
                    int nr = r + mr2[i];
                    int nc = c + mc2[i];
                    if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                        continue;
                    if(board[nr][nc].piece == 1 && board[nr][nc].color == ownColor)
                    {
                        int tempScore = evaluate(board) + 1;
                        if(tempScore > score)
                        {
                            score = tempScore;
                            srcRow = r;
                            srcCol = c;
                        }
                    }
                }
                int tempScore = evaluate(board);
                if(tempScore > score)
                {
                    score = tempScore;
                    srcRow = r;
                    srcCol = c;
                }
            }
        }
    }

    if(score > abscore)
    {
        reversePiece(srcRow, srcCol);
        qDebug() << "reverse:" << srcRow << srcCol;
    }
    else if(movemsg.size() != 0)
    {
        movePiece(movemsg.at(0), movemsg.at(1), movemsg.at(2), movemsg.at(3));
        qDebug() << "move:" << movemsg.at(0) << movemsg.at(1) << movemsg.at(2) << movemsg.at(3);
    }
    else
    {
        qDebug() << movemsg.size();
        qDebug() << "nostep";
        noStep();
    }
    */
    alphaBetaMax(board, 4, -100000, 100000);
    if(movemsg.size() == 2)
    {
        reversePiece(movemsg.at(0), movemsg.at(1));
        qDebug() << "reverse:" << movemsg.at(0) << movemsg.at(1);
    }
    else if(movemsg.size() == 4)
    {
        movePiece(movemsg.at(0), movemsg.at(1), movemsg.at(2), movemsg.at(3));
        qDebug() << "move:" << movemsg.at(0) << movemsg.at(1) << movemsg.at(2) << movemsg.at(3);
    }
    else
    {
        qDebug() << "nostep";
        noStep();
    }
}

void Game::receiveMessage()
{
    //qDebug() << "tcp size:" << tcpSocket->size();

    while(tcpSocket->size() != 0)
    {
        memset(recvBuf, 0, BUFSIZE);
        tcpSocket->read(recvBuf, BUFSIZE);
        //qDebug() << "receive msg:" << recvBuf;
        if(b_roundStart)
        {
            roundStart(curRound);
            b_roundStart = false;
        }
        else
        {
            curPlayColor = (curPlayColor + 1) % 2;
            if(observe() > 0)
            {
                int s1 = 0, s2 = 0;
                getScore(board, s1, s2);
                ownScore += s1;
                oppoScore += s2;

                curPlayColor = -1;
                roundOver(curRound);
                if(curRound == 2)
                {
                    gameOver();
                    tcpSocket->close();
                    emit statusChanged("Close socket");
                    curRound = 0;

                    if(b_learn)
                    {
                        if(learn_cnt > 0)
                        {
                            learn_cnt--;
                            if((learn_cnt % 10) == 0)
                            {
                                QFile file(QString("record/score-4.txt"));
                                file.open(QFile::Append);
                                QTextStream fout(&file);
                                fout << factor << "\t" << ownScore/30.0 << "\t" << oppoScore/30.0 << endl;
                                ownScore = 0.0;
                                oppoScore = 0.0;
                                factor += 0.01;
                            }
                            this->connectToServer(this->ip_, this->port_);
                            this->authorize(this->id_.toStdString().data(), this->pwd_.toStdString().data());
                        }
                    }

                    return;
                }
                else
                {
                    ++curRound;
                    b_roundStart = true;
                }
            }
            else
                saveChessBoard();

        }
        emit boardChanged();
        if((curPlayColor == ownColor) && b_aimove)
        {
            //step();
            minimaxStep();
        }
    }

}

int Game::sendMessage(const char* msg)
{
    int len = strlen(msg);
    len = len < BUFSIZE ? len : BUFSIZE;
    memset(sendBuf, 0, BUFSIZE);
    memcpy(sendBuf, msg, len);
    tcpSocket->write(sendBuf, BUFSIZE);
    //qDebug() << "sendmsg:" <<msg;
    return 1;
}

void Game::displayError(QAbstractSocket::SocketError)
{
    qDebug() << tcpSocket->errorString();
}

void Game::connectToServer(QString ip, int port)
{
    emit statusChanged(QString("Connect server: ") + ip + ":" + QString::number(port));
    tcpSocket->abort();
    tcpSocket->connectToHost(ip, port);
    this->ip_ = ip;
    this->port_ = port;
    return;
}

void Game::setAiMode(bool mode)
{
    b_aimove = mode;
}

void Game::learn(int cnt)
{
    b_learn = true;
    learn_cnt = cnt;
    b_aimove = true;
}
