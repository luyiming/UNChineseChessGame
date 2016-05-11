#pragma once

void authorize(const char *id);

void gameStart();

void gameOver();

void roundStart(int round);

void oneRound();

void roundOver(int round);

int observe();

void reversePiece(int row, int col);

void movePiece(int srcRow, int srcCol, int desRow, int desCol);

void noStep();

void step();

void saveChessBoard();