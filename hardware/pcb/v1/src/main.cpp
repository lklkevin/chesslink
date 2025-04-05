#include <Arduino.h>
#include <SquareUnit.h>

#define IR_EMITTER_PIN 2

// Board Configuration
const int BOARD_SIZE = 2;
char board[BOARD_SIZE][BOARD_SIZE];  // e.g., board[rank][file]

// Sensor Pins 
SquareUnit unitA(A1, A6, 10);
SquareUnit unitB(A2, A5, 9);
SquareUnit unitC(A0, A7, 11);
SquareUnit unitD(A3, A4, 12);

SquareUnit* units[] = { &unitA, &unitB, &unitC, &unitD };

void setup() {
  Serial.begin(9600);
  pinMode(IR_EMITTER_PIN, OUTPUT);
  for (auto unit : units) unit->begin();
}

void parseFEN(const char* fen) {
  int rank = 0; // starts at top (rank 8 in real chess)
  int file = 0;

  for (int i = 0; fen[i] != '\0'; i++) {
    char c = fen[i];

    if (c == '/') {
      rank++;
      file = 0;
    } else if (c >= '1' && c <= '8') {
      int empty = c - '0';
      for (int j = 0; j < empty; j++) {
        if (rank < BOARD_SIZE && file < BOARD_SIZE) {
          board[rank][file++] = ' ';
        }
      }
    } else {
      if (rank < BOARD_SIZE && file < BOARD_SIZE) {
        board[rank][file++] = c;
      }
    }
  }
}

void printBoard() {
  Serial.println("Board:");

  for (int r = 0; r < BOARD_SIZE; r++) {
    for (int f = 0; f < BOARD_SIZE; f++) {
      Serial.print(board[r][f]);
      Serial.print(' ');
    }
    Serial.println();
  }
}


void loop() {
  String fen = "";

  // for (auto unit : units) {
  for (int i = 0; i < BOARD_SIZE*2; i++) {
    auto unit = units[i];
    unit->readSensors();
    unit->printStatus();

    char piece = unit->detectPiece();
    Serial.print("FEN Piece: ");
    Serial.println(piece);

    fen += (piece == ' ') ? '1' : piece;

    // Add '/' between ranks (after 2 units)
    if (i == 1) fen += '/';

    // // Optional: visualize result
    // if (unit->isMagnetDetected())
    //   unit->setColor(255, 0, 255);  // Green for magnet
    // else
    //   unit->setColor(255, 255, 255);  // off otherwise, cathodes
  }

  // const char* fen = "r1/1K";  // top-left rook, bottom-right King
  // parseFEN(fen);
  // printBoard();

  delay(1000);
}
