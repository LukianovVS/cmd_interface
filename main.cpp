#include "cls_cmd.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <Windows.h>


//! список функций
//{
// получение координат консоли
COORD GetConsoleCursorPosition(HANDLE hConsoleOutput);
// выход с ошибкой
VOID ErrorExit(const char * const);
// Конфигурация проекта
void init_prj(std::string *f_ini, HANDLE &hStdin, HANDLE &hStdout, int &pos_X_min);
// удаление содержимого строки
void line_clc(std::string  &str, HANDLE &hStdout, int &pos_X_min);

//}
//}



int main(int argc, char *argv[])
{
  int pos_X_min = 2;                        // отступ от начала строки
  COORD position;                           // позиция на консоли
  bool flg_loop = true;                     // программа работает в цикле до сброса флага
  std::string cmd_str = "";                 // строка с введеной командой
  HANDLE hStdin;                            // устройство stdin
  HANDLE hStdout;                           // устройство stdout
  INPUT_RECORD irInBuf;                     // Класс для обработки событий. В данном случае, для обработки событий с клавиатуры
  DWORD cNumRead;                           // Сюда будет записано кол-во принятых событий
  bool flag_previous_tab_prc = false;       // флаг обработки на предыдущем шаге клавиши tab
  std::string cmd_str_tab="";               // временная строка, которая создается при обработке tab



  // В ручом режиме подставляем файл с примером
  if (argc != 2)
  {
    std::cerr << "ERROR: configuration file required" << std::endl;
    return 1;
  }

  std::string f_ini(argv[1]);       // сохраняем путь к ini файлу (файл со списком команд)
  // initialization
  init_prj(&f_ini, hStdin, hStdout, pos_X_min);


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

    if (flag_previous_tab_prc)                               // если на предыдущем шаге была результативно обработа клавиша tab
    {
      if (irInBuf.Event.KeyEvent.wVirtualKeyCode != VK_TAB) // при этом текущая клавиша != tab
      {
        cmd_handler.reset_find_ind();                       // обнуляем стартовую позицию поиска команды (слкдующий поиск начнется с нуля, а не продолжится)
        cmd_str = cmd_str_tab;
        flag_previous_tab_prc = false;                      // сбрасываем флаг
      }
    }
    // обраротка клавиши, на основе чтения таблицы "Virtual-Key Codes"
    switch (irInBuf.Event.KeyEvent.wVirtualKeyCode)
    {
      case VK_UP:
      {
        std::string str_copy = cmd_str;
        line_clc(str_copy, hStdout, pos_X_min);
        cmd_log.get_previous(&cmd_str);
        std::cout << cmd_str;
        break;
      }
      case VK_DOWN:
      {
        std::string str_copy = cmd_str;
        line_clc(str_copy, hStdout, pos_X_min);
        cmd_log.get_next(&cmd_str);
        std::cout << cmd_str;
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
        if (cmd_str.length() == 0)
        {
          // если строка пустая, то просто выводим список команд
          cmd_handler.print_cmd_list();
          break;
        }

        if (flag_previous_tab_prc) // если таб был нажат ранее, то предыдущий вариант удаляем
        {
          int p_start = cmd_str.length();
          int p_stop  = cmd_str_tab.length();

          position = GetConsoleCursorPosition(hStdout);
          position.X = pos_X_min + p_start;
          SetConsoleCursorPosition(hStdout, position);

          for (int i = p_start; i < p_stop; i++)
            std::cout << " ";
          SetConsoleCursorPosition(hStdout, position);
        }

        int ind_cmd = -1;
        if ( cmd_handler.findCmd(&ind_cmd, &cmd_str, false, true) )
        {
          cmd_handler.getName(&cmd_str_tab, ind_cmd);
          std::cout << &cmd_str_tab[cmd_str.length()];
          flag_previous_tab_prc = true;
        }
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
        line_clc(cmd_str, hStdout, pos_X_min);
        break;
      }
      case VK_RETURN: // (Enter). Обрабатываем введенные данные
      {
        // добовляем символы в лог
        cmd_log.append(&cmd_str);

        position   = GetConsoleCursorPosition(hStdout);
        position.X = 0;
        position.Y += 1;
        SetConsoleCursorPosition(hStdout, position);

        // введенная строка содержит имя команды и аргументы (опционно).
        // их надо разделить и опбраьатывать отдельно. Конец имени команды совпадает с первым пробелом.
        int i = cmd_str.find_first_of(' ');
        std::string comand_name("");
        std::string comand_args("");
        if (i == std::string::npos)                         // если пробел не найден - то каманда без аргументов
        {
          comand_name.append(cmd_str);
        }
        else                                               // разделяем имя команды и аргументы
        {
          comand_name.append(cmd_str, 0, i);
          // аргументы копируем вместе с пробелом
          comand_args.append(cmd_str, i, cmd_str.length() - i);
        }

        std::cout << "cmd: "<<comand_name<< std::endl;
        std::cout << "arg: "<<comand_args<< std::endl;

        int ind_cmd = -1;
        if (cmd_handler.findCmd(&ind_cmd, &comand_name, true, false))
        {
          cmd_handler.callCmd(ind_cmd, &comand_args);
        }
        else
        {
          // TODO (#5): надо обработь отсуствие команды более серьёзно
          std::cout << "yuar comand don't found: '" << cmd_str << "'" << std::endl;
        }


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


VOID ErrorExit (const char * const err_msg)
{
  std::cerr << err_msg;
  exit(1);
}



void init_prj(std::string *f_ini, HANDLE &hStdin, HANDLE &hStdout, int &pos_X_min)
{
  // читаем ini файл, формируем список команд
  cmd_handler.init(f_ini);

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


// удаление содержимого строки
void line_clc(std::string  &str, HANDLE &hStdout, int &pos_X_min)
{
  std::string str_clr(str.length(), ' ');
  COORD position   = GetConsoleCursorPosition(hStdout);
  position.X = pos_X_min;
  SetConsoleCursorPosition(hStdout, position);

  std::cout << str_clr;
  position.X = pos_X_min;
  SetConsoleCursorPosition(hStdout, position);

  str.clear();
}
