#include <iostream>
#include <fstream>
#include <Windows.h>


//! список функций
//{
// получение координат консоли
COORD GetConsoleCursorPosition(HANDLE hConsoleOutput);
// выход с ошибкой
VOID ErrorExit(LPSTR);
// обработка ini файла со списком команд
void init_prj(char f_ini[], HANDLE &hStdin, HANDLE &hStdout, int &pos_X_min);
//}



int main(int argc, char *argv[])
{
  int pos_X_min = 2;          // отступ от начала строки
  COORD position;             // позиция на консоли
  bool flg_loop = true;       // программа работает в цикле до сброса флага
  std::string cmd_str = "";   // строка с введеной командой
  HANDLE hStdin;              // устройство stdin
  HANDLE hStdout;             // устройство stdout
  INPUT_RECORD irInBuf;       // Класс для обработки событий. В данном случае, для обработки событий с клавиатуры
  DWORD cNumRead;             // Сюда будет записано кол-во принятых событий

//  // Инициализация проекта
//  if (argc != 2)
//  {
//    std::cerr << "The file with the list of commands is not specified\n";
//    return 1;
//  }
  // В ручом режиме подставляем файл с примером
  std::string f_ini = argv[0];
  int i = f_ini.rfind("\\bin\\", f_ini.length());
  f_ini.erase(f_ini.begin() + i + 1, f_ini.end());
  f_ini += "\\example\\example.ini";
  //
  init_prj(&f_ini[0], hStdin, hStdout, pos_X_min);




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
    //не обрабатываем пока клавиша нажата. Исключение - нажат shift
    if(irInBuf.Event.KeyEvent.bKeyDown && !(irInBuf.Event.KeyEvent.dwControlKeyState & ( SHIFT_PRESSED) ) )
      continue;
    // обраротка клавиши, на основе чтения таблицы "Virtual-Key Codes"
    switch (irInBuf.Event.KeyEvent.wVirtualKeyCode)
    {
      case VK_UP:
      {
        // TODO: UP log prc
        break;
      }
      case VK_DOWN:
      {
        // TODO: DOWN log prc
        break;
      }
      case VK_LEFT: // перемещение по строке влево
      {
        position   = GetConsoleCursorPosition(hStdout);
        if(position.X > pos_X_min )
        {
          position.X -= 1;
          SetConsoleCursorPosition(hStdout, position);
        }
        break;
      }
      case VK_RIGHT: // перемещение по строке вправо
      {
        position   = GetConsoleCursorPosition(hStdout);
        if (position.X < (cmd_str.length() + pos_X_min))
        {
          position.X += 1;
          SetConsoleCursorPosition(hStdout, position);
        }
        break;
      }
      case VK_ESCAPE: // выход из прошраммы
      {
        flg_loop = false; // выход из цикла
        break;
      }
      case VK_TAB:
      {
        // TODO: tab prc
        break;
      }
      case VK_BACK: // удаляем 1 символ
      {
        position = GetConsoleCursorPosition(hStdout);
        int i = position.X - pos_X_min - 1;
        if (i >= 0)
        {
          cmd_str.erase(cmd_str.begin() + i);
          position.X -= 1;
          SetConsoleCursorPosition(hStdout, position);
          std::cout << &cmd_str[i] << " ";
          SetConsoleCursorPosition(hStdout, position);
        }
        break;
      }
      case VK_DELETE: // удаляем всё строку
      {
        std::string str_clr(cmd_str.length(), ' ');
        position   = GetConsoleCursorPosition(hStdout);
        position.X = pos_X_min;
        SetConsoleCursorPosition(hStdout, position);

        std::cout << str_clr;
        position.X = pos_X_min;
        SetConsoleCursorPosition(hStdout, position);

        cmd_str.clear();

        break;
      }
      case VK_RETURN: // Enter. Обрабатываем введенные данные
      {
        position   = GetConsoleCursorPosition(hStdout);
        position.X = 0;
        position.Y += 1;
        SetConsoleCursorPosition(hStdout, position);

        std::cout << "Yout string: '" << cmd_str << "'" << std::endl;
        cmd_str.clear();

        position   = GetConsoleCursorPosition(hStdout);
        position.X = pos_X_min;
        position.Y += 1;
        SetConsoleCursorPosition(hStdout, position);
        break;
      }
      default: // для нового символа строки
      {
        // проверяем что это текстовый символ (а не служебная клавиша) + отбрасываем кирилицу
        if ( irInBuf.Event.KeyEvent.uChar.AsciiChar >= 32 && irInBuf.Event.KeyEvent.uChar.AsciiChar <= 127)
        {
          position   = GetConsoleCursorPosition(hStdout);
          int i = position.X - pos_X_min;
          if (i < cmd_str.length())
          {
            cmd_str.insert(i, 1, irInBuf.Event.KeyEvent.uChar.AsciiChar);
          }
          else
          {
            cmd_str.push_back(irInBuf.Event.KeyEvent.uChar.AsciiChar);
          }
          // всё что было левее нового символа - остается. Переписываем строку начиная с нового символа и до конца строки
          std::cout << &cmd_str[i];
          // возвращаем курсор с конца строки на место гд пишет пользователь
          position.X = pos_X_min + i + 1;
          SetConsoleCursorPosition(hStdout, position);
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


VOID ErrorExit (char err_msg[])
{
  std::cerr << err_msg;
  exit(1);
}



void init_prj(char f_ini[], HANDLE &hStdin, HANDLE &hStdout, int &pos_X_min)
{
  std::ifstream fid(f_ini, std::ifstream::in);
  if (!fid.is_open())
  {
    std::string str_err = "Error: open ini file: '";
    str_err += f_ini;
    str_err += "'";
    ErrorExit(&str_err[0]);
  }
  // TODO: прочитать файл и составить славарь {имя команды: обрабочик команды}
  fid.close();
  pos_X_min = 2;

  hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  hStdin = GetStdHandle(STD_INPUT_HANDLE);
  if (hStdin == INVALID_HANDLE_VALUE)
    ErrorExit("GetStdHandle");



  std::cout << "Hello! I am cmd" << std::endl;
  std::cout << "  Type Esc to exit" << std::endl;

  COORD position   = GetConsoleCursorPosition(hStdout);
  position.X = pos_X_min;
  SetConsoleCursorPosition(hStdout, position);
}

