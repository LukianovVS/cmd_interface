#include "cls_cmd.h"
#include <string.h>
#include <windows.h>
#include <algorithm>

//DBG
#include <iostream>



extern VOID ErrorExit (const char * const err_msg);

// Получить кол-во цифр десятичного числа
//int nDig(int N)
//{
//  return (N == 0) ? 1 : log10(abs(N)) + 1;
//}

CMD_HANDLER cmd_handler;


//! функция для упорядовачивания списка команд. чтобы не захломлять класс, вынес её отдельно
bool func_cmptoini(CMD_LIST a, CMD_LIST b)
{
//  char aa[122];
//  char bb[122];
//  strcpy(aa, a.name.c_str());
//  strcpy(bb, b.name.c_str());
//  int i  = strcmp(a.name.c_str(), b.name.c_str());
//  int ii = strcmp(b.name.c_str(), a.name.c_str());
  if ( strcmp(a.name.c_str(), b.name.c_str()) < 0 )
    return true;
  else
    return false;
}


CMD_HANDLER::CMD_HANDLER()
{
  Ncmd = 0;
}

CMD_HANDLER::~CMD_HANDLER()
{
  //dtor
}

/*! поиск строки по маске
 *
 */
bool CMD_HANDLER::findstr(int * const ind, const std::string * const *cmd_mask, const std::string * const  str_arr, int * const i, const int N)
{

}

/*! Поиск команды по маске
 * ind - возврращается номер найденной команды, который удовлетворяет заданной маске
 * cmd_mask - строка, которая содержит маску для поиска команды ( например, если маска = "get", то строка "getXYZ" - подходит под указанную маску)
 * continueFind - продолжить поиск (true), или начать сначала (false)
 * return - true если найдена команда, которая удовлетворяет маске
 */
bool CMD_HANDLER::findCmd(int * const ind, const std::string * const *cmd_mask, bool continueFind)
{

}


/*! Поиск команды по маске среди введеных ранее
 * ind - возврращается номер найденной команды, который удовлетворяет заданной маске
 * cmd_mask - строка, которая содержит маску для поиска команды ( например, если маска = "get", то строка "getXYZ" - подходит под указанную маску)
 * continueFind - продолжить поиск (true), или начать сначала (false)
 * return - true если найдена команда, которая удовлетворяет маске
 */
bool CMD_HANDLER::findToLegend(int * const ind, const std::string * const *cmd_mask, bool continueFind)
{

}

//! Запуск команды c индексом (номером) ind. \Соотвествующий индекс можно получить в findCmd
void CMD_HANDLER::callCmd(const int ind)
{

}

/*! Инициализация полей "name", "cmd", "help"
 */
bool CMD_HANDLER::init(const std::string * const f_ini)
{
  cmd_path = ".\\";

  const int TMP_BUFF_SIZE = 200;
  char tmp_buff[TMP_BUFF_SIZE + 1];             // все секции будут записаны в этот буфер

  // Определеям кол-во команд = кол-во секций - 1 (первая секция - это не команда, а общая конфигурация)
  int N = GetPrivateProfileSectionNames(tmp_buff, TMP_BUFF_SIZE, f_ini->c_str()) - 1;
  for (int i = 0; i < N; i++)
  {
    if (tmp_buff[i] == '\0')        // все секции разделены в буфере символом конца строки. Есть новый символ конца строки -> счетчик кол-ва секций++
      this->Ncmd++;
  }

  int k = 0;
  // читаем подряд команды из ini файла
  for (int i = 0; i < _CFG_NUM_CMD_MAX_; i++)
  {
    char cmd_n[5 + 3 + int(_CFG_NUM_CMD_MAX_/1000) ];
    sprintf(cmd_n, "cmd %d", i + 1);
    GetPrivateProfileString(cmd_n, "name", "", tmp_buff, TMP_BUFF_SIZE, f_ini->c_str());
    if (strlen(tmp_buff) > 0)
    {
      this->cmd[k].name = tmp_buff;
      GetPrivateProfileString(cmd_n, "comand", "", tmp_buff, TMP_BUFF_SIZE, f_ini->c_str());
      this->cmd[k].call = tmp_buff;
      GetPrivateProfileString(cmd_n, "help", "", tmp_buff, TMP_BUFF_SIZE, f_ini->c_str());
      this->cmd[k].help = tmp_buff;
      // DBG
      std::cout << k << ": " << this->cmd[k].name.c_str() << std::endl;
      // end DBG
      k++;
    }
  }

  if (this->Ncmd != k)
    ErrorExit("CMD_LIST Num cmd");

  std::sort(this->cmd, this->cmd + this->Ncmd, func_cmptoini);

  // DBG
  for (int i = 0; i < this->Ncmd; i++)
    std::cout << i << ": " << this->cmd[i].name.c_str() << std::endl;









}
