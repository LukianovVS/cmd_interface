#include <iostream>
#include <Windows.h>


//! список функций
//{
// получение координат консоли
COORD GetConsoleCursorPosition(HANDLE hConsoleOutput);
// выход с ошибкой
VOID ErrorExit(LPSTR);
//}

HANDLE hStdin;
HANDLE hStdout;

int main()
{
  bool flg_loop = true;

  INPUT_RECORD irInBuf;
  DWORD cNumRead;

  hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  hStdin = GetStdHandle(STD_INPUT_HANDLE);
  if (hStdin == INVALID_HANDLE_VALUE)
        ErrorExit("GetStdHandle");



  std::cout << "Hello! I am cmd" << std::endl;
  std::cout << "  Type Esc to exit" << std::endl;
  do
  {

    if (! ReadConsoleInput(
                hStdin,      // input buffer handle
                &irInBuf,     // buffer to read into
                1,           // size of read buffer
                &cNumRead) ) // number of records read
      ErrorExit("ReadConsoleInput");

    // защита. если нет ничего - возвращаеся у чтению
    if(!cNumRead)
      continue;
    // если событие не от клавиатуры  - пропуск события
    if(irInBuf.EventType != KEY_EVENT)
      continue;
    //не обрабатываем пока клавиша нажата
    if(irInBuf.Event.KeyEvent.bKeyDown)
      continue;
    // обраьотка клавиши
    switch (irInBuf.Event.KeyEvent.wVirtualKeyCode)
    {
      case VK_UP:
      {
        COORD position = GetConsoleCursorPosition(hStdout);
        position.Y -= 1;
        SetConsoleCursorPosition(hStdout, position);
        break;
      }
      case VK_DOWN:
      {
        COORD position   = GetConsoleCursorPosition(hStdout);
        position.Y += 1;
        SetConsoleCursorPosition(hStdout, position);
        break;
      }
      case VK_LEFT:
      {
        COORD position   = GetConsoleCursorPosition(hStdout);
        position.X -= 1;
        SetConsoleCursorPosition(hStdout, position);
        break;
      }
      case VK_RIGHT:
      {
        COORD position   = GetConsoleCursorPosition(hStdout);
        position.X += 1;
        SetConsoleCursorPosition(hStdout, position);
        break;
      }
      case VK_ESCAPE:
      {
        flg_loop = false; // выход из цикла
        break;
      }
      default:
      {
        // проверяем что это текстовый символ (а не служебная клавиша) + отбрасываем кирилицу
        if (irInBuf.Event.KeyEvent.uChar.AsciiChar >= 32 &&
            irInBuf.Event.KeyEvent.uChar.AsciiChar <= 127 )
        {
          std::cout << irInBuf.Event.KeyEvent.uChar.AsciiChar;
        }

        break;
      }
    }


  } while(flg_loop);

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


VOID ErrorExit (LPSTR lpszMessage)
{
    fprintf(stderr, "%s\n", lpszMessage);

    ExitProcess(0);
}


