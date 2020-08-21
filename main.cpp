#include <conio.h>
#include <iostream>
#include <Windows.h>


#define KEYBOARD_ARROW -32
#define KEYBOARD_UP     72
#define KEYBOARD_DOWN   80
#define KEYBOARD_LEFT   75
#define KEYBOARD_RIGHT  77



COORD GetConsoleCursorPosition(HANDLE hConsoleOutput);


void prc_arrow();
void prc_ch(char ch);

HANDLE hConsole;

int main()
{
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);	  // ѕолучение дескриптора устройства стандартного вывода

  bool flg_loop = true;
  char ch_keyboard;

  std::cout << "Hello! I am cmd" << std::endl;
  do
  {
    ch_keyboard = _getch();

    switch(ch_keyboard)
    {
      case KEYBOARD_ARROW: prc_arrow(); break;
      default: prc_ch(ch_keyboard);
    }


  }while(flg_loop);


//  position = GetConsoleCursorPosition(hConsole);
//  std::cout << position.X << " " << position.Y << std::endl;

  return 0;
}



COORD GetConsoleCursorPosition(HANDLE hConsoleOutput)
{
    CONSOLE_SCREEN_BUFFER_INFO cbsi;
    if (GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi))
    {
        return cbsi.dwCursorPosition;
    }
    else
    {
        // The function failed. Call GetLastError() for details.
        COORD invalid = { 0, 0 };
        return invalid;
    }
}


void prc_arrow()
{
  char ch_keyboard = _getch();
  COORD position   = GetConsoleCursorPosition(hConsole);

  switch(ch_keyboard)
  {
    case KEYBOARD_UP:   position.Y -= 1; break;
    case KEYBOARD_DOWN: position.Y += 1; break;
    case KEYBOARD_LEFT: position.X -= 1; break;
    case KEYBOARD_RIGHT: position.X += 1; break;
  }
  SetConsoleCursorPosition(hConsole, position);		// ѕеремещение каретки по заданным координатам
}

void prc_ch(char ch)
{
  std::cout<< ch;
}

