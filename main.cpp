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
void init_prj(std::string *f_ini, std::string *cmd_path, int *Num_cmd, HANDLE &hStdin, HANDLE &hStdout, int &pos_X_min);
/* сравнить строку (введенную пользователем) с командой номер i.
 * return значение strncmp, полученное при сравнении первых pstr->length() символов строки и команды номер i
*/
int strcmdncmp(const std::string * const pstr, const int i, const std::string * const pfname);
// Получить кол-во цифр десятичного числа
int nDig(int N)
{
  return (N == 0) ? 1 : log10(abs(N)) + 1;
}

//}
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




  // В ручом режиме подставляем файл с примером
  if (argc != 2)
  {
    std::cerr << "ERROR: configuration file required" << std::endl;
    return 1;
  }

  // TODO (#1) ини файл надо прочесть 1 раз и на его основе создать упорядоченный список команд. Упорядоченный список бужет полезен при обработке клавиши tab
  // Для того ухоодим в ветку девелоп

  std::string f_ini(argv[1]);       // сохраняем путь к ini файлу (файл со списком команд)
  std::string cmd_path("");         // путь, относительно которых заданы команды в ini файле
  int Num_cmd = 0;                  // кол-во команд
  // initialization
  init_prj(&f_ini, &cmd_path, &Num_cmd, hStdin, hStdout, pos_X_min);


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
        // TODO (#6) UP log prc
        break;
      }
      case VK_DOWN:
      {
        // TODO(#6) DOWN log prc
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
        // TODO (#3): Сейчас обработка VK_TAB происходит не очень красиво. Надо:
        // 1. выводить разные варианты комант при нажитии tab несколько раз подряд
        // 2. выводить подсказку если строка пустая
        // 3. Т.к. обработчик очень схож с тем, что происходит при нажатии Enter надо создать общую функцию
        static int i_find = Num_cmd;
        int strLen = cmd_str.length();
        for (int i = 0; i < Num_cmd; i++)
        {
          i_find++;
          if (i_find >= Num_cmd)
            i_find -= Num_cmd;
          std::string cmd_section("cmd ");                  // формируем имя следующей секции для поиска команды
          cmd_section += std::to_string(i_find + 1);
          const int buff_size = 200;                        // инициализируем буфер, в который будем складывать результаты поиска
          char buff[buff_size];
          // дастаем имя команды из cmd X
          GetPrivateProfileString(cmd_section.c_str(), "name", "", buff, buff_size, f_ini.c_str());
          // сравневаем полученное имя с тем, что ввел пользователь
          if ( !strncmp(buff, cmd_str.c_str(), strLen) )
          {
            if (strlen(buff) > strLen)
            {
              cmd_str.append(&buff[strLen]);
              std::cout << &buff[strLen];
              break;
            }
          }
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
      case VK_RETURN: // (Enter). Обрабатываем введенные данные
      {
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

        bool f_cmd_found = false;                           // признак того, что введенная команда найдена
        for (int i = 0; i < Num_cmd; i++)                   // бежим по списку команд
        {
          std::string cmd_section("cmd ");                  // формируем имя следующей секции для поиска команды
          cmd_section += std::to_string(i + 1);
          const int buff_size = 200;                        // инициализируем буфер, в который будем складывать результаты поиска
          char buff[buff_size];
          // дастаем имя команды из cmd X
          GetPrivateProfileString(cmd_section.c_str(), "name", "", buff, buff_size, f_ini.c_str());
          // сравневаем полученное имя с тем, что ввел пользователь
          if ( strlen(buff) == comand_name.length() && !strcmp(buff, comand_name.c_str()) )
          {
            // достаем путь к исполняемому файлу, который вызывает введенная команда
            GetPrivateProfileString(cmd_section.c_str(), "comand", "", buff, buff_size, f_ini.c_str());
            std::string system_cmd(cmd_path);
            std::cout << "cmd: " << system_cmd << std::endl;
            std::cout << "buff: " << buff << std::endl;
            system_cmd.append(buff);
            std::cout << "cmd: " << system_cmd << std::endl;
            system_cmd.append(comand_args);
            std::cout << "cmd: " << system_cmd << std::endl;
            system(system_cmd.c_str());
            f_cmd_found = true;
            break;
          }
        }

        if (f_cmd_found == false)
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



void init_prj(std::string *f_ini, std::string *cmd_path, int *Num_cmd, HANDLE &hStdin, HANDLE &hStdout, int &pos_X_min)
{

  // все команды заданы относительно ini файла (dbg).
  int i = f_ini->find_last_of('/\\');
  cmd_path->append(".\\");
  cmd_path->append(*f_ini, 0, i + 1);

  // Получаем кол-во секций в ini файле.
  const int size_buff = 200;
  char buff[size_buff + 1];             // все секции будут записаны в этот буфер
  int N = GetPrivateProfileSectionNames(buff, size_buff, f_ini->c_str());
  for (int i = 0; i < N; i++)
  {
    if (buff[i] == '\0')        // все секции разделены в буфере символом конца строки. Есть новый символ конца строки -> счетчик кол-ва секций++
      (*Num_cmd)++;
  }

  // первая секция - спецификация
  (*Num_cmd)--;

  cmd_handler.init(f_ini);

  // dbg info:
  std::cout << "f ini: " << f_ini->c_str()    << std::endl;
  std::cout << "path:  " << cmd_path->c_str() << std::endl;
  std::cout << "Num cmd:  " << *Num_cmd << std::endl;



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



/* сравнить строку (введенную пользователем) с командой номер i.
 * return значение strncmp, полученное при сравнении первых pstr->length() символов строки и команды номер i
*/
int strcmdncmp(const std::string * const pstr, const int i, const std::string * const pfname)
{
  const int size_of_arr = 20;  //
  // Проверка на прохождение ограничения на максимальный номер команды. 5 - это на 'c', 'm', 'd', ' ', '\0'.
  if (  nDig(i) <  (size_of_arr - 5) )  // мне кажется, что проверка лишняя. 1e15 - 1 команд вполне хватит для всего... Пусть пока останется.
  {
    ErrorExit("call strcmdncmp");
  }

  char cmd_section[size_of_arr];
  sprintf(cmd_section, "cmd %d", i);  // формируем следующей секции для поиска команды

  const int buff_size = 200;                        // инициализируем буфер, в который будем складывать результаты поиска
  char buff[buff_size];
  // дастаем имя команды из cmd X
  GetPrivateProfileString(cmd_section, "name", "", buff, buff_size, pfname->c_str());

  return strncmp(buff, pstr->c_str(), pstr->length());
}


